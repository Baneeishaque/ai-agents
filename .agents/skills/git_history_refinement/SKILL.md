<!--
title: Git History Refinement
description: Refine or reconstruct existing commit history using backup
    branches, atomic extraction, tree parity verification,
    and safe remote push reconciliation.
category: Git & Repository Management
-->

# Git History Refinement Skill

> **Skill ID:** `git_history_refinement`
> **Version:** 1.0.0
> **Standard:** [Agent Skills (agentskills.io)](https://agentskills.io)

## Description

Refine or reconstruct existing Git commit history when commits contain
mixed concerns (non-atomic changes) that need to be split, re-ordered,
or cleaned up. This skill covers the full lifecycle: backup creation,
baseline reset, atomic extraction (including JSON manipulation),
metadata preservation, tree parity verification, and safe remote push
reconciliation.

This skill is invoked AFTER commits already exist and need improvement.
For constructing new commits from working-tree changes, use the
[`git_atomic_commit`](../git_atomic_commit/SKILL.md) skill instead.

## Source Rules

| Rule File | Scope Incorporated |
|---|---|
| [`git-history-refinement-rules.md`](../../../ai-agent-rules/git-history-refinement-rules.md) | All sections (primary source) |

For hierarchical multi-branch rebasing, see the
[`git_rebase`](../git_rebase/SKILL.md) skill.

## Prerequisites

| Requirement | Minimum |
|---|---|
| VCS | Git 2.x+ |
| Shell | PowerShell 5.1+ or Bash 4+ |
| Tools | `jq` (for JSON manipulation steps) |
| Access | Write access to the project repository |

## When to Apply

Apply this skill when:
- Existing commits contain mixed concerns that need splitting
- A user asks to "refine history," "split a commit," or "fix non-atomic
  commits"
- A commit mixes formatting + logic + config and needs decomposition
- JSON files (e.g., `.vscode/settings.json`) need atomic key-by-key
  splitting across commits
- The user asks to reconstruct history from a clean baseline

Do NOT apply when:
- Changes are uncommitted (working-tree state) — use
  [`git_atomic_commit`](../git_atomic_commit/SKILL.md) instead
- The task is a multi-branch rebase — use
  [`git_rebase`](../git_rebase/SKILL.md) instead
- The user just wants to amend the most recent commit message

---

## Step-by-Step Procedure

### Step 1 — Backup Protocol (Safety First)

The agent **MUST** create a backup branch before performing any
destructive history operations (`reset --hard`, `rebase`, etc.).

#### 1a — Incremental Branch Naming

Use an incrementing suffix to prevent overwriting existing backups:

1. **Prefix:** `backup/pre-settings-split-` (or descriptive equivalent)
2. **Check existing:**
   ```bash
   git branch --list "backup/pre-settings-split-*"
   ```
3. **Increment:** Select the first integer `n` that does NOT have a
   corresponding branch (e.g., if `-1` and `-2` exist, use `-3`)

#### 1b — Workspace Preservation

Before creating the branch, stage all local changes and create a
temporary "state preservation" commit. This ensures the backup captures
the exact state including uncommitted changes.

#### 1c — Branch Creation

```bash
git branch <backup-branch-name>
```

**CRITICAL:** Do NOT use `-f` to force-move a branch.

---

### Step 2 — Remote Baseline Reconciliation

Before starting any refinement, synchronize with the remote source of
truth to prevent divergence.

1. **Mandatory Fetch:**
   ```bash
   git fetch origin <branch>
   ```
2. **Reconciliation:** If `origin/<branch>` contains commits not present
   locally, identify the latest remote commit as the new "Refinement
   Baseline" and reconstruct atop it.
3. **Submodule Awareness:** This protocol applies recursively to
   submodules. Never begin a submodule refinement without a fetch.

---

### Step 3 — Pre-Execution Analysis (Mandatory)

Commit messages and change descriptions MUST be derived from actual file
analysis, not assumptions.

1. **Read Before Writing:** Before finalizing any commit message:
   - Read the current state: `git show <base-commit>:<file>`
   - Read the target state from the backup branch
   - Perform explicit `git diff` comparisons

2. **Evidence-Based Messages:** Commit messages must reflect ACTUAL
   changes observed, not planned or assumed changes.

3. **No Placeholder Content:** Never use generic descriptions like
   "update metadata" without specifying which fields changed and how.

---

### Step 4 — Baseline Reset (Reconstruction)

Reset the active branch to the last known "clean" commit before the
messy historical segment.

```bash
git reset --hard <clean-commit-hash>
```

#### 4a — Root Commit Refinement

If the refinement involves the first commit (root) of the repository,
use an orphan branch:

```bash
git checkout --orphan temp-master
git rm -rf .
```

This allows reconstructing history from scratch while preserving the
repository's identity.

---

### Step 5 — Atomic Extraction

#### 5a — Standard File Extraction

Fetch the target version of files from the backup branch:

```bash
git show <backup-branch>:<path/to/file> > <path/to/file>
```

#### 5b — Atomic JSON Manipulation (jq)

When a JSON file needs to be split across commits, use `jq` to extract
exactly the relevant keys for the current atomic unit:

1. **Extract original:**
   ```bash
   git show <backup-branch>:<path/to/file.json> > file.json.bak
   ```
2. **Filter with jq:**
   ```bash
   jq '{ "specific.key": .["specific.key"] }' file.json.bak > file.json
   ```
3. **Verify valid JSON:**
   ```bash
   jq . file.json
   ```

#### 5c — Canonical Sorting and Formatting

When manipulating JSON arrays (e.g., `cSpell.words`), preserve or
enforce the project's standard sort order:

1. **Inspect** neighbouring commits to determine expected sort order
   (ASCII vs. case-insensitive natural sort)
2. **Apply consistently** at every stage of reconstruction
3. **Prefer stable sorting tools** (e.g., Python's
   `sorted(key=str.lower)`) over standard `jq` sort if
   case-insensitivity is required

---

### Step 6 — Metadata Preservation

When re-creating commits that originally had specific messages and
timestamps, use the original commit's hash to preserve metadata:

```bash
git commit -C <original-hash>
```

---

### Step 7 — Sequential Integrity (Protocol Files)

When refining history for documents containing numbered phases or
state-dependent protocols, maintain structural validity at **every**
intermediate commit.

- **Valid-Sequence Rule:** Every commit must result in a valid 1-N
  sequence of phases. No gaps, no duplicates, no "broken" intermediate
  states.
- **Micro-Renumbering:** If a commit adds Phases 4 and 5, and the
  original file already had a Phase 4 (now 6), renumber all subsequent
  phases within the same atomic unit.

---

### Step 8 — Link Verification (Mandatory)

For EVERY file rename operation, perform global link verification:

1. **Immediate grep check:**
   ```bash
   grep -r "Old-Filename.md" . --exclude-dir=.git
   ```
2. **Update all references:** Internal documentation links, template
   files, architecture documents, rule cross-references
3. **Exclusion protocol:** Exclude CI/CD-managed files from manual edits
   if they are auto-generated
4. **Final verification:** Re-run grep with `--exclude` flags for
   managed files to confirm cleanup

---

### Step 9 — Cherry-Pick Protocol (Preserving Dependents)

When splitting a commit that has subsequent commits built on top of it,
preserve those dependent commits.

1. **Identify dependents:**
   ```bash
   git log --oneline <split-commit>..HEAD
   ```
2. **Sequential cherry-pick** in chronological order:
   ```bash
   git cherry-pick <commit-hash>
   ```
3. **Conflict resolution:** If conflicts arise:
   - Resolve while preserving the original commit's intent
   - Reference original changes: `git show <original-hash>`
   - Mark resolved: `git add <file>`
   - Continue: `git cherry-pick --continue`

---

### Step 10 — Verification & Tree Parity

#### 10a — Content-Level Verification

After each commit in the reconstruction, confirm only intended changes:

```bash
git show HEAD
```

#### 10b — Tree Parity Check (Mandatory)

After the final commit, compare the current branch to the backup:

```bash
git diff <current-branch> <backup-branch>
```

**Constraint:** The diff MUST be empty. Any discrepancy indicates a
regression introduced during refinement.

---

### Step 11 — Post-Refinement Remote Push

If the remote has diverged after refinement, reconcile before pushing.

#### 11a — Pre-Push Remote Backup (Mandatory)

```bash
git branch backup/pre-force-push-<n> origin/<branch>
```

Use incremental integers. Verify:

```bash
git branch --list "backup/pre-force-push-*"
```

#### 11b — Remote Divergence Analysis

```bash
git fetch origin <branch>
git log <refinement-baseline>..origin/<branch> --oneline --graph --decorate
```

#### 11c — Commit Categorization

Assign every remote commit to exactly one category:

| Category | Definition | Action |
|---|---|---|
| **New** | Truly unique logic/content created after refinement started | MUST cherry-pick |
| **Covered** | Changes already present in refined history | Skip (user approval required) |
| **Regenerative** | Auto-generated files synced by CI | Skip (user approval required) |

**Identifying Regenerative Files:** Check `.github/workflows/` or
relevant scripts to see if the file is a target of automated generation.

#### 11d — Reconciliation

1. Present categorized list to the user
2. Obtain mandatory approval for skipping "Covered" or "Regenerative"
3. Cherry-pick only approved "New" commits onto refined HEAD

#### 11e — Force Push Safety

1. Request explicit user approval: "I understand that `origin/<branch>`
   will be replaced. Proceed?"
2. Use the lease flag:
   ```bash
   git push --force-with-lease origin <branch>
   ```

#### 11f — Remote Rollback Procedure

If remote state is incorrect after push:

```bash
git push --force-with-lease origin backup/pre-force-push-<n>:<branch>
```

#### 11g — Backup Cleanup

Once the user has manually verified the remote state, the backup branch
SHOULD be deleted. The agent is **PROHIBITED** from executing this
automatically — it MUST be a manual instruction or require separate
explicit authorization.

---

### Step 12 — Finalization

The agent is **BLOCKED** from deleting backup branches automatically.

1. Provide a walkthrough of the refined history
2. Request explicit user confirmation before deleting any backup branches

---

## Prohibited Behaviors

The agent is **BLOCKED** from:

- **Deleting backup branches automatically** — Requires explicit user
  authorization
- **Skipping the backup step** — Mandatory before any destructive
  operation
- **Force-moving branches with `-f`** — Use incremental naming instead
- **Using generic commit messages** — Messages must reflect actual
  observed changes
- **Skipping tree parity check** — The final diff against backup MUST
  be empty
- **Skipping empty commits without user confirmation** — During rebase
- **Force-pushing without `--force-with-lease`** — To prevent
  overwriting unexpected remote changes
- **Auto-deleting backup branches after force push** — Must remain until
  user verifies remote state

## Common Pitfalls

| Pitfall | Solution |
|---|---|
| Backup branch overwritten by force | Use incremental naming, never `-f` |
| Uncommitted changes lost during reset | Create state-preservation commit before backup |
| JSON key order changed between commits | Enforce canonical sorting consistently across all stages |
| Protocol file has broken phase numbering mid-history | Apply micro-renumbering within the same atomic unit |
| Cherry-pick conflicts during dependent preservation | Reference `git show <original-hash>` to preserve intent |
| Tree parity check shows differences | Regression during refinement — re-examine extraction steps |
| Remote diverged during long refinement | Fetch and categorize remote commits before force-push |
| Generated file manually edited in refinement | Check for auto-generation markers first |
| Commit message says "update metadata" without specifics | Read actual diffs before writing messages |
