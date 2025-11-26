## Plan: Reword Git Commit Non-Interactively

Reword a git commit using a fully automated, non-interactive git rebase process on macOS with comprehensive error handling, verification steps, and rollback capability.

### Pre-Flight Checks

1. **Check for existing rebase state** - Verify no orphaned rebase state exists in `.git/rebase-merge/` or `.git/rebase-apply/`; if found and no active rebase, safely delete these directories

2. **Verify working directory is clean** - Run `git status` to ensure no uncommitted changes in working directory or staging area; commit or stash changes if needed

3. **Verify Git LFS availability** - Run `git lfs version` to confirm Git LFS is installed and functional; this repository has LFS hooks that will **abort rebase with exit code 2** if git-lfs is not available

### Main Steps

4. **Create and verify backup tag** - Execute `TAG_NAME=backup-before-reword-$(date +%s)` to store tag name, then `git tag "$TAG_NAME"` to create tag, then verify with `git rev-parse "$TAG_NAME"` matches `git rev-parse HEAD`

5. **Start non-interactive rebase** - Run `GIT_SEQUENCE_EDITOR="sed -i '' 's/^pick <commit-hash>/reword <commit-hash>/'" GIT_EDITOR=true git rebase -i <commit-hash>^` to automatically change "pick" to "reword" and skip the editor pause (GIT_EDITOR=true makes git use /bin/true which exits immediately)

6. **Complete rebase with new message** - Execute `git commit --amend -m "<proper commit message>"` to set the new commit message without opening editor

7. **Verify rebase success** - Run `git log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit -5` to confirm commit message changed and verify new commit hashes

8. **Clean up backup tag** - After confirming success, delete the backup tag with `git tag -d "$TAG_NAME"`

### Error Handling

**If Git LFS not installed:**
- **Impact:** Repository has PDF files tracked by LFS; hooks will fail during rebase
- **Action:** Install Git LFS before proceeding, or accept that rebase will abort with "git-lfs was not found on your path" error
- **Note:** Git LFS is **required** for this repository due to configured hooks and tracked LFS objects

**If "swap file already exists" error:**
- **Check:** Run `git status` to see if rebase is actually in progress
- **If no active rebase:** Safely delete `.git/rebase-merge/` directory and restart
- **If active rebase:** Abort with `git rebase --abort` and investigate concurrent git processes

**If rebase initiation fails:**
- **Action:** Check error message; if LFS-related, verify `command -v git-lfs` succeeds
- **Cleanup:** Run `git rebase --abort` to clean up any partial state

### Rollback Strategy

1. **If rebase fails during process** - Execute `git rebase --abort` to return to original state before rebase started

2. **If rebase completes but result is unsatisfactory** - Run `git reset --hard "$TAG_NAME"` to restore to exact state before rebase

3. **Verify rollback success** - Execute `git rev-parse HEAD` and compare with `git rev-parse "$TAG_NAME"` to confirm they match, then run `git log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit -1` to display current commit details

4. **Verify clean state** - Run `git diff --quiet` and `git diff --cached --quiet` to confirm working directory and staging area are clean

### Notes

- **Fully automated rebase:** Setting `GIT_EDITOR=true` uses `/bin/true` (a command that does nothing and exits with success) as the editor, preventing git from pausing and waiting for user input when reword is triggered
- **git lg alias:** This repository has `git lg` configured as an alias for the pretty-formatted log command used in verification steps
- **Git LFS:** This is NOT optional for all repositories; this specific repository requires it due to 5 PDF files tracked by LFS and hooks that enforce its presence
- **Swap files:** Created in `.git/rebase-merge/` during interactive rebase; safe to delete only when no active rebase is in progress
- **Tag verification:** Always verify tags point to expected commits using `git rev-parse` before relying on them for rollback
