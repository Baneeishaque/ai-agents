<!--
title: Git Commit Edit
description: Edit existing commits in-place via interactive rebase —
    remove files, add files, amend content, or fix mixed concerns
    without full history reconstruction.
category: Git & Repository Management
-->

# Git Commit Edit Skill

> **Skill ID:** `commit_edit`
> **Version:** 1.0.0
> **Standard:** [Agent Skills (agentskills.io)](https://agentskills.io)

## Description

Edit an existing Git commit in-place using interactive rebase with the
`edit` action. This skill covers targeted, surgical modifications to a
single commit — removing unwanted files, adding missing files, amending
content, or fixing mixed concerns — while preserving all descendant
commits and the working tree state.

Unlike [`git_history_refinement`](../git_history_refinement/SKILL.md)
(which reconstructs history from a clean baseline), this skill performs
**minimal, targeted edits** to a specific commit. It is the "scalpel"
approach vs the "rebuild" approach.

Unlike [`git_atomic_commit`](../git_atomic_commit/SKILL.md) (which
constructs new commits from working-tree changes), this skill modifies
**already-committed** history.

## Source Rules

| Rule File | Scope Incorporated |
|---|---|
| [`git-operation-rules.md`](../../../ai-agent-rules/git-operation-rules.md) | Sections 2–4 (commit/push/stash protocols) |
| [`git-atomic-commit-construction-rules.md`](../../../ai-agent-rules/git-atomic-commit-construction-rules.md) | Phase 9 (execution & verification), Phase 14 (push protocol) |

## Prerequisites

| Requirement | Minimum |
|---|---|
| VCS | Git 2.x+ |
| Shell | PowerShell 5.1+ or Bash 4+ |
| Access | Write access to the project repository |
| State | Clean working tree (or willingness to stash) |

## When to Apply

Apply this skill when:
- A user asks to "edit a commit," "remove files from a commit," or
  "fix a commit"
- A specific commit contains unwanted files (noise, binaries,
  IDE artifacts) that should be removed
- A commit is missing files that should have been included
- A commit's content needs amendment without splitting into multiple
  commits
- The user identifies a specific commit hash and wants targeted changes

Do NOT apply when:
- The user wants to split a commit into multiple atomic commits — use
  [`git_history_refinement`](../git_history_refinement/SKILL.md) instead
- Changes are uncommitted (working-tree state) — use
  [`git_atomic_commit`](../git_atomic_commit/SKILL.md) instead
- The user wants to rebase branches — use
  [`git_rebase`](../git_rebase/SKILL.md) instead
- The commit is the most recent and only needs a message change — use
  `git commit --amend -m "..."` directly (no rebase needed)

---

## Step-by-Step Procedure

### Step 0 — Pre-Edit Analysis

Before any rebase, fully understand the target commit and its context.

#### 0a — Identify the Target Commit

Confirm the exact commit hash, its position in history, and the current
branch:

```powershell
git log --oneline -20
git branch --show-current
```

#### 0b — Inspect the Target Commit

Show the full stat and optionally the diff to understand ALL changes
in the commit:

```powershell
git show --stat <commit-hash>
```

For detailed content inspection:

```powershell
git show <commit-hash> -- <specific-file>
```

#### 0c — Count Descendant Commits

Determine how many commits sit on top of the target. These will be
replayed after the edit:

```powershell
git log --oneline <commit-hash>..HEAD
```

#### 0d — Check Remote Divergence

If the branch has been pushed, warn the user that editing will require
a force push:

```powershell
git log --oneline <commit-hash>..origin/<branch>
```

#### 0e — Present the Edit Plan

The agent **MUST** present the following to the user before proceeding:

````markdown
## Commit Edit Plan

**Target commit:** `<short-hash>` — `<commit message>`
**Descendant commits to replay:** <count>
**Branch:** `<branch-name>`

### Changes to make:
- Remove: <list of files/patterns to remove>
- Add: <list of files to add>
- Modify: <list of files to amend>

### Proposed steps:
1. Stash uncommitted changes (if any)
2. Interactive rebase, mark `<short-hash>` as `edit`
3. <specific edit actions>
4. Amend the commit
5. Continue rebase (replay <count> descendants)
6. Restore stashed changes

**⚠️ Warning:** This rewrites history. Force push required if
branch was previously pushed to remote.

Proceed? (yes / no)
````

**The agent MUST NOT begin the rebase until the user confirms.**

---

### Step 1 — Stash Uncommitted Work

If the working tree has uncommitted changes, stash them with a
descriptive message:

```powershell
git stash push -m "Pre-edit stash: <description of pending work>"
```

Verify the stash was created:

```powershell
git stash list
```

---

### Step 2 — Start Interactive Rebase

#### 2a — Create Sequence Editor Script

Create a temporary script that automatically marks the target commit
as `edit` in the rebase todo list:

**PowerShell:**

```powershell
$script = @'
param($file)
(Get-Content $file) -replace '^pick <short-hash>', 'edit <short-hash>' | Set-Content $file
'@
Set-Content -Path "_rebase_editor.ps1" -Value $script
```

#### 2b — Launch the Rebase

```powershell
$env:GIT_SEQUENCE_EDITOR = 'powershell -ExecutionPolicy Bypass -File _rebase_editor.ps1'
git rebase -i <commit-hash>~1
```

**Expected output:** Git stops at the target commit with the message
`Stopped at <hash>... <message>`.

#### 2c — Verify Rebase State

Confirm the rebase paused at the correct commit:

```powershell
git log --oneline -1
```

The output should show the target commit hash and message.

---

### Step 3 — Perform the Edit

Execute the specific edit actions. Common operations:

#### 3a — Remove Files from the Commit

Restore files to their parent's version (effectively removing them
from this commit's diff):

```powershell
# Remove specific files
git checkout HEAD~1 -- <file1> <file2>

# Remove files matching a pattern
git checkout HEAD~1 -- $(git diff --name-only HEAD~1 HEAD -- "*.project")
```

Verify the removal:

```powershell
git diff --cached --stat -- "*.project"
```

#### 3b — Add Files to the Commit

Stage new files that should have been part of this commit:

```powershell
git add <file1> <file2>
```

#### 3c — Modify Existing Files

Edit the file content as needed, then stage:

```powershell
# Make edits to the file...
git add <modified-file>
```

#### 3d — Remove Binary Files

For binary files that should not have been committed:

```powershell
git rm --cached <binary-file>
```

---

### Step 4 — Amend the Commit

After all edits are staged, amend the commit:

```powershell
# Keep the original message
git commit --amend --no-edit

# Or update the message
git commit --amend -m "<new message>"
```

#### 4a — Verify the Amended Commit

Confirm the commit now contains only the intended changes:

```powershell
git show --stat HEAD
```

**Count check:** Compare the file count before and after. The agent
MUST report the delta:

```
Before: 70 files changed, +1,024 / −21
After:  16 files changed, +686 / −13
Removed: 54 noise files
```

---

### Step 5 — Continue Rebase

Replay the descendant commits on top of the amended commit:

```powershell
git rebase --continue
```

**Expected output:** `Successfully rebased and updated refs/heads/<branch>.`

#### 5a — Handle Conflicts

If a descendant commit conflicts with the edit:

1. **Inspect the conflict:**
   ```powershell
   git status
   git diff
   ```

2. **Resolve the conflict** — Edit the conflicting files, then:
   ```powershell
   git add <resolved-files>
   git rebase --continue
   ```

3. **If the conflict makes the descendant commit empty** (e.g., the
   descendant also touched a removed file):
   ```powershell
   git rebase --skip
   ```
   **⚠️ Only skip after confirming with the user** that the now-empty
   commit is expected.

#### 5b — Handle Corrupted Rebase State

If `git rebase --continue` fails with
`warning: could not read '.git/rebase-merge/head-name'`:

```powershell
Test-Path ".git/rebase-merge"
Get-ChildItem ".git/rebase-merge"
```

If the directory is empty/corrupted:

```powershell
Remove-Item ".git/rebase-merge" -Recurse -Force
git status
```

Then commit directly instead of using `git rebase --continue`.

---

### Step 6 — Restore Stashed Work

If changes were stashed in Step 1:

```powershell
git stash pop
```

Verify restored state:

```powershell
git status --short
```

If `git stash pop` creates conflicts, resolve manually, then:

```powershell
git add <resolved-files>
git stash drop
```

---

### Step 7 — Cleanup

Remove any temporary files created during the rebase:

```powershell
Remove-Item "_rebase_editor.ps1" -Force -ErrorAction SilentlyContinue
```

#### 7a — Final Verification

Run a comprehensive status check:

```powershell
git log --oneline -10
git status --short
```

#### 7b — Report Divergence

If the branch was previously pushed, inform the user:

```
⚠️ Branch has diverged from origin/<branch>.
Force push required: git push --force-with-lease origin <branch>
```

**The agent MUST NOT force-push without explicit user request.**

---

## Scope Coverage

| Category | Convention |
|---|---|
| File removal from commit | Restore from parent via `git checkout HEAD~1 --` |
| File addition to commit | Stage and amend |
| Content modification | Edit, stage, and amend |
| Binary removal | `git rm --cached` and amend |
| Message-only edit | `git commit --amend -m` |
| Descendant preservation | Automatic replay via `git rebase --continue` |

---

## Prohibited Behaviors

The agent is **BLOCKED** from:

- **Starting rebase without user confirmation** — The edit plan MUST
  be presented and approved first
- **Force-pushing without explicit request** — Always inform, never
  auto-push
- **Editing without stashing first** — If the working tree is dirty,
  stash MUST precede rebase
- **Skipping conflicted descendants without confirmation** — Empty
  commits after edit require user approval to skip
- **Leaving temporary files** — Cleanup is mandatory (`_rebase_editor.ps1`,
  etc.)
- **Assuming commit position** — Always verify the commit hash and its
  position with `git log` before proceeding
- **Using `git reset --hard`** — Use `git checkout HEAD~1 -- <files>`
  for targeted file restoration, never hard reset

## Common Pitfalls

| Pitfall | Solution |
|---|---|
| Dirty working tree prevents rebase | Stash first with descriptive message; restore after rebase completes |
| Rebase editor script not picked up | Verify `GIT_SEQUENCE_EDITOR` env var is set correctly; use absolute path on Windows |
| Wrong commit marked as `edit` | Verify with `git log --oneline -1` after rebase stops; abort with `git rebase --abort` if wrong |
| Descendant commit conflicts after file removal | Resolve conflict or skip if the descendant's changes to removed files are also noise |
| Forgot to clean up temp script | Always run `Remove-Item` for `_rebase_editor.ps1` after rebase completes or aborts |
| Stash pop conflicts with replayed commits | Resolve manually, `git add`, then `git stash drop` |
| Amended commit has unexpected file count | Compare `git show --stat HEAD` against the pre-edit plan; re-amend if needed |
| `Deletion of directory failed` during rebase | Answer `n` to retry prompt — Git will proceed; the directory is cleaned up later |
| Force push overwrites teammate's work | Always use `--force-with-lease` instead of `--force` to prevent overwriting unknown remote commits |
