<!--
title: Non-Interactive Git Commit Reword Plan
description: Automated protocol for rewording git commits with verification and rollback strategies.
category: DevOps & Workflow
-->

# Plan: Reword Git Commit Non-Interactively

Reword a git commit using a fully automated, non-interactive git rebase process on macOS with comprehensive error
handling, verification steps, and rollback capability.

## Pre-Flight Checks

1. **Check for existing rebase state** - Verify no orphaned rebase state exists in `.git/rebase-merge/` or
   `.git/rebase-apply/`; if found and no active rebase, safely delete these directories

2. **Verify working directory is clean** - Run `git status` to ensure no uncommitted changes in working directory or
   staging area; commit or stash changes if needed

3. **Verify Git LFS availability** - Run `git lfs version` to confirm Git LFS is installed and functional; this
   repository has LFS hooks that will **abort rebase with exit code 2** if git-lfs is not available

## Main Steps

1. **Draft new commit message** - Inspect the commit using `git show <commit-hash>` and prepare a new message
    compliant with:
    - [git-commit-message-rules.md](./ai-agent-rules/git-commit-message-rules.md) (Standard formatting)
    - [git-atomic-commit-construction-rules.md](./ai-agent-rules/git-atomic-commit-construction-rules.md)
      (Rationale documentation)
    - **Dig Down Principle**: If the commit involves binary or encoded files, use `cat -v` or similar to verify the
      payload before drafting.

2. **Create and verify backup tag** - Execute `TAG_NAME=backup-before-reword-$(date +%s)` to store tag name, then
   `git tag "$TAG_NAME"` to create tag, then verify with `git rev-parse "$TAG_NAME"` matches `git rev-parse HEAD`

3. **Start non-interactive rebase** - Run `GIT_SEQUENCE_EDITOR="sed -i '' 's/^pick <commit-hash>/edit <commit-hash>/'"`
and `git rebase -i <commit-hash>^` to automatically change "pick" to "edit" in the todo list;
   this stops rebase at the target commit without opening an editor

4. **Complete rebase with new message** - Execute `git commit --amend -m "<proper commit message>"`
   to set the new commit message, then `git rebase --continue` to finish rebase

5. **Verify rebase success** - Run `git log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s
%Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit -5`
   to confirm commit message changed and verify new commit hashes

6. **Clean up backup tag** - After confirming success, delete the backup tag with `git tag -d "$TAG_NAME"` and run
   `git gc --prune=now` to clean up unreachable objects.

## Error Handling

**If Git LFS not installed:**

- **Impact:** Repository has PDF files tracked by LFS; hooks will fail during rebase
- **Action:** Install Git LFS before proceeding, or accept that rebase will abort with
    "git-lfs was not found on your path" error
- **Note:** Git LFS is **required** for this repository due to configured hooks and tracked LFS objects

**If "swap file already exists" error:**

- **Check:** Run `git status` to see if rebase is actually in progress
- **If no active rebase:** Safely delete `.git/rebase-merge/` directory and restart
- **If active rebase:** Abort with `git rebase --abort` and investigate concurrent git processes

**If rebase initiation fails:**

- **Action:** Check error message; if LFS-related, verify `command -v git-lfs` succeeds
- **Cleanup:** Run `git rebase --abort` to clean up any partial state

## Rollback Strategy

1. **If rebase fails during process** - Execute `git rebase --abort` to return to original state before rebase started

2. **If rebase completes but result is unsatisfactory** - Run `git reset --hard "$TAG_NAME"` to restore to exact state
   before rebase

3. **Verify rollback success** - Execute `git rev-parse HEAD` and compare with `git rev-parse "$TAG_NAME"` to confirm
   they match, then run `git log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s`
   `%Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit -1` to display current commit details

4. **Verify clean state** - Run `git diff --quiet` and `git diff --cached --quiet` to confirm working directory and
   staging area are clean

## Feature Branch Recovery Procedure

When the `main` branch history is rewritten (e.g., via rebase/reword), feature branches based on the old history
must be carefully rebased onto the new `main`.

### 1. Identification & Analysis

Identify all remote branches that contain the old, now-rewritten commits.

```bash
# Find branches containing the old commit hash (before rewrite)
git branch -r --contains <old-commit-hash>

```

For each identified branch, analyze its divergence:

- **Find Merge Base:** `git merge-base <branch> <old-main-tip>`
- **List Unique Commits:** `git log <branch> ^<old-main-tip> --oneline`
- **Verify Divergence:** Ensure the branch actually stems from the old history and not an unrelated point.

### 2. User Confirmation Loop

**CRITICAL:** Do not batch process. Present each branch individually to the user for confirmation.

**Presentation Format:**

- **Branch Name:** `origin/feature/branch-name`
- **Divergence Point:** Commit hash and message where it split from old master.
- **Commits to Replay:** List of unique commits on the feature branch that need to be moved.
- **Commits to Drop:** List of commits that exist in the new master (if any) to avoid duplicates.

**Action:** Ask: *"Do you want to proceed with rebasing this branch?"*

### 3. Sequential Execution

If confirmed, perform the following steps for the branch:

1. **Checkout & Track:**

    ```bash
    git checkout -b <local-branch-name> origin/<remote-branch-name>
    ```

2. **Rebase onto New Main:**

    ```bash
    git rebase main
    ```

    *Note: Git is usually smart enough to skip commits that were already applied (the ones we reworded), but watch for
    conflicts.*
3. **Force Push:**

    ```bash
    git push origin <local-branch-name> --force
    ```

4. **Cleanup:**
    Delete the local temporary branch to keep the workspace clean.

    ```bash
    git checkout main
    git branch -D <local-branch-name>
    ```

### 4. Final Verification

After processing all branches, verify the remote state to ensure all feature branches are now compatible with the
new `main` tip.

## Notes

- **git lg alias:** This repository has `git lg` configured as an alias for the pretty-formatted log command used in
    verification steps
- **Git LFS:** This is NOT optional for all repositories; this specific repository requires it due to 5 PDF files
    tracked by LFS and hooks that enforce its presence
- **Swap files:** Created in `.git/rebase-merge/` during interactive rebase; safe to delete only when no active
    rebase is in progress
- **Tag verification:** Always verify tags point to expected commits using `git rev-parse` before relying on them for rollback
