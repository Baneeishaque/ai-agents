---
description: Industrial Hierarchical Rebase & Git Hygiene Workflow
---

# Hierarchical Rebase Workflow

Follow this workflow for complex, multi-branch Git rebases to ensure history integrity and eliminate redundancy.

## 1. Initial Mapping

1. Identify all branches in the chain (`git branch -a`).
2. Create a Mermaid graph of current vs. target hierarchy.
3. Reference: [Git Rebase Standardization Rules](../../ai-agent-rules/git-rebase-standardization-rules.md).

## 2. Commit Action Mapping (CAM)

For each branch in the chain, create a table:

| Action | Commit SHA | Rationale / Literal Payload |
| :--- | :--- | :--- |
| **DROP** | [SHA] | Redundant; logic covered in [Branch Name]. |
| **REWORD** | [SHA] | Upgrade fidelity: "Dig Down" into binary/config changes. |
| **KEEP** | [SHA] | Unique functional logic for this branch. |

## 3. Preparation & Backup

1. **Mandatory Backup**: `git tag backup/pre-rebase-$(date +%s) <branch>`.
2. **Sync Base**: `git fetch origin main`.

## 4. Sequential Execution (The "L-Rebase" Protocol)

For each branch in the hierarchy (bottom-up):

1. **Rebase**: `git rebase origin/main` (or the previous branch in the chain).
2. **Isolate Specific Logic**: `git rebase --onto <new-base> <old-base> <branch>`.
3. **High-Fidelity Reword**:
   - `GIT_SEQUENCE_EDITOR="sed -i '' 's/^pick/edit/'" git rebase -i HEAD^`
   - `git commit --amend -m "[LITERAL_PAYLOAD_FROM_PLAN]"`
   - `git rebase --continue`

## 5. Finalization & Hygiene

1. **Verify Graph**: `git log --oneline --graph -n 10`.
2. **LFS Integrity**: `git lfs status`.
3. **Prune**: `git gc --prune=now`.
4. **Walkthrough**: Update `walkthrough.md` with the new graph.

// turbo-all

## 6. Cleanup

After user approval:

1. `git tag -d <backup-tags>`
2. `git gc --prune=now`
