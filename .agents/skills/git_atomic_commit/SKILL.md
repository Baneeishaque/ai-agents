<!--
title: Git Atomic Commit Construction
description: Analyze, group, and arrange working-tree changes into logical,
    independent atomic commits — with hunk-based staging, formatting
    isolation, and mandatory user authorization.
category: Git & Repository Management
-->

# Git Atomic Commit Construction Skill

> **Skill ID:** `git_atomic_commit`
> **Version:** 1.0.0
> **Standard:** [Agent Skills (agentskills.io)](https://agentskills.io)

## Description

Construct high-quality, atomic Git commits from a set of working-tree
changes. This skill covers the full lifecycle: environment validation,
change analysis, logical grouping, hunk-based staging, formatting
isolation, commit message quality, execution with user authorization,
and post-commit verification.

Every commit produced by this skill is independent, logically coherent,
and buildable. Mixed concerns are never committed together. Formatting
is separated from logic. Configuration is coupled to its functional
change. The user retains full control via mandatory preview and explicit
"start" authorization.

## Source Rules

This skill distills and operationalizes the following rule files:

| Rule File | Scope Incorporated |
|---|---|
| [`git-atomic-commit-construction-rules.md`](../../../ai-agent-rules/git-atomic-commit-construction-rules.md) | All 15 phases (primary source) |
| [`git-operation-rules.md`](../../../ai-agent-rules/git-operation-rules.md) | Phase -1, 0, 1 (environment, repo context, change detection) and Sections 2–4 (commit/push/stash protocols) |

For history refinement (splitting existing commits), see the
[`git_history_refinement`](../git_history_refinement/SKILL.md) skill.
For complex multi-branch rebasing, see the
[`git_rebase`](../git_rebase/SKILL.md) skill.

## Prerequisites

| Requirement | Minimum |
|---|---|
| VCS | Git 2.x+ |
| Shell | PowerShell 5.1+ or Bash 4+ |
| Access | Write access to the project repository |
| Auth | GitHub CLI authenticated (if pushing to GitHub) |

## When to Apply

Apply this skill when:
- A user asks to "commit changes," "arrange commits," or "stage and commit"
- `git status` shows staged, unstaged, or untracked modifications
- Multiple unrelated changes exist in the working tree and need separation
- A user asks to review what should be committed

Do NOT apply when:
- The user asks to refine or split **existing** commits — use
  [`git_history_refinement`](../git_history_refinement/SKILL.md) instead
- The user asks to rebase branches — use
  [`git_rebase`](../git_rebase/SKILL.md) instead
- The request is a simple single-file, single-concern commit with no
  mixed changes (a lightweight commit suffices without the full protocol)

---

## Step-by-Step Procedure

### Step 0 — Environment & Repository Context

Before any Git commands, validate the environment and establish context.

#### 0a — Authenticate Services

Verify authentication for required services:

```bash
gh auth status        # GitHub CLI
```

If authentication is missing, guide the user through login.

#### 0b — Identify the Target Repository

Determine the correct repository from the user's request and file paths.

- **Nested repositories:** If changes are in a sub-directory that is its
  own Git project, `cd` into it before executing any `git` commands.
- **Ambiguity:** If multiple repositories exist in the workspace and the
  target is unclear, ask the user for clarification.

#### 0c — Verify Build Tool Permissions

Ensure build tools have execute permissions:

```bash
chmod +x gradlew      # Gradle wrapper example
```

---

### Step 1 — Deep Change Analysis

Perform a dependency analysis of ALL modifications before staging
anything.

#### 1a — Detect All Changes

Use `git status` to discover staged, unstaged, and untracked changes:

```powershell
git status
```

**Complete Scope (Critical):** The analysis MUST cover ALL three change
categories — **staged**, **unstaged**, AND **untracked** — as a single
unified inventory from the very first step. Untracked files are
first-class members of the change scope, not a secondary check.
Failing to include untracked files in the initial analysis leads to
incomplete commit plans and files discovered only after execution.

**Untracked files:** Any untracked file not excluded by `.gitignore` is
a candidate for version control. The agent **MUST NOT** stage untracked
files without explicit user confirmation to avoid committing credentials,
large binaries, or environment-specific files.

#### 1b — Use `git ls-files` as Source of Truth

For rename or restructuring operations, `git ls-files` is the
authoritative list of tracked files — not `Get-ChildItem` or `find`,
which include git-ignored content:

```powershell
git ls-files
```

#### 1c — Read `.gitignore` for Tracked vs Ignored

Read `.gitignore` carefully, paying special attention to **negation
patterns** (`!`) that re-include specific files inside ignored
directories:

```gitignore
# Example: directory ignored, but .zip files are tracked
pevers/*
!pevers/*.zip
```

#### 1d — Analyze Change Dependencies

- **Shared Identifiers:** Group changes that modify the same functions,
  classes, or constants across different files.
- **Cross-File References:** If file A depends on a change in file B
  (e.g., an import, a link, a `.gitignore` pattern), they MUST be in
  the same atomic commit.
- **Categorical Alignment:** Group changes by architectural layer (UI,
  Logic, Docs) unless they are functionally coupled.

#### 1e — Workflow-First Priority

If changes involve CI/CD workflows (GitHub Actions, scripts), the agent
**MUST** fix, test, and verify workflow functionality **BEFORE** arranging
or executing commits. Pipeline stability takes precedence over
documentation or stylistic refinements.

#### 1f — Present Complete Inventory

List **ALL changes** — staged, unstaged, AND untracked (not just
violations or modifications to already-tracked files) — with their
status. This gives the user full visibility and ensures no file is
analyzed as an afterthought:

| # | File | Status | Action |
|---|---|---|---|
| 1 | `.gitignore` | Modified | 🔄 Update |
| 2 | `src/main.java` | Modified | 🔄 Stage |
| 3 | `README.md` | Untracked | ❓ Confirm with user |

---

### Step 2 — Logical Grouping (Arrangement)

Arrange detected changes into a proposed sequence of commits.

#### 2a — Independence Principle

Each commit must stand alone. If the repository were checked out at
that commit, it should still build/function (or be logically coherent).

#### 2b — Atomic Principle

Never commit half of a logical change. If a file contains two unrelated
changes, use **hunk-based staging** (Step 3).

#### 2c — Buildable State Priority

While atomicity is the goal, maintaining a buildable repository takes
precedence. If a core infrastructure change (e.g., a signature change in
a shared helper) breaks all consumers, the refactor and the resulting
fixes in consumer files MUST be consolidated into a single commit.

#### 2d — The Commit Preview (Mandatory Verbose Display)

Present the proposed "Arranged Commits" using a structured format with
**maximum detail**. For files with mixed concerns requiring hunk-based
staging, the preview **MUST** include the specific git hunks:

````markdown
## Arranged Commits Preview

### Commit 1: [type](scope): [title]
- **Files**: [file1.md], [file2.md]
- **Message**:
  ```
  [type](scope): [title]

  [Body line 1]
  [Body line 2]
  ```
- **Hunks/Preview**:
  ```diff
  [Show actual hunks for this commit]
  ```

### Commit 2: [type](scope): [title]
- **Files**: [file3.md]
- **Message**:
  ```
  [type](scope): [title]

  [Body line 1]
  ```
---
Please say "start" to begin the sequential execution of these atomic
commits.
````

#### 2e — Commit Authorization

The agent **MUST NOT** proceed with any commit execution until the user
explicitly says **"start"**. Other triggers like "commit" or "go" are
insufficient.

---

### Step 3 — Interactive Hunk-Based Staging

When a file contains mixed concerns, use interactive staging to
partition changes.

#### 3a — Command

```bash
git add -p <file>
```

#### 3b — Hunk-by-Hunk Evaluation

During interactive staging, evaluate and respond to each hunk
individually (`y`, `n`, `s`, etc.). Do NOT batch responses. Every
modified line must be evaluated: "Does this line belong to the
*current* atomic goal?"

#### 3c — Granular Hygiene

If a grammatical fix is discovered while implementing a feature, it
MUST be staged and committed separately unless it is part of the same
logical chunk.

#### 3d — Verification After Staging

After staging each chunk, verify strictly atomic contents:

```bash
git diff --cached
```

#### 3e — Discard Rejected Noise

After accepting the desired hunks and rejecting noise, discard the
rejected changes from the working tree if they are unintentional:

```bash
git checkout -- <file>
```

#### 3f — Mixed-Concern Noise Handling Workflow

When a file contains both functional changes AND unrelated noise
(invisible characters, spurious whitespace, trailing `\r` differences),
follow this workflow:

1. **Attempt to fix the noise in the editor** — remove the spurious
   whitespace or extra blank lines directly. This may resolve it.
2. **Re-check the diff** — run `git diff <file>`. If the noise persists
   (e.g., invisible character differences that the editor cannot show),
   fall back to hunk-based staging.
3. **Stage only functional hunks** — run `git add -p <file>`, accepting
   (`y`) only the hunks that belong to the current atomic goal and
   rejecting (`n`) the noise hunks.
4. **Discard the remaining noise** — run `git checkout -- <file>` to
   revert the rejected noise from the working tree. This preserves the
   staged functional changes.
5. **Verify staged state is clean** — run `git diff --cached <file>` to
   confirm only functional changes are staged, then run `git diff` to
   confirm no unstaged changes remain.

**PowerShell caveat:** Piping input to `git add -p` is unreliable in
PowerShell (standard pipe methods like `echo`, `Write-Output`, and
string joins often fail to register). Preferred workaround:
- Accept the functional hunks manually or in a sequence where piping
  works, then use `git checkout -- <file>` to discard whatever noise
  remains unstaged.

#### 3g — IDE Artifact Bulk Discard

IDE tooling (VS Code Java Language Server, Eclipse, IntelliJ) often
auto-modifies project metadata files across **many** sub-projects at
once — for example, adding `<filteredResources>` blocks to every
Eclipse `.project` file. These changes **may** be noise — but some
projects intentionally track IDE metadata for reproducible workspace
setup. The agent **MUST NOT** assume these are discardable.

**Detection pattern:**

- `git diff --stat` shows a large number of identical-looking changes
  (e.g., 50+ `.project` files each with exactly +11 lines)
- The diff content is the same boilerplate repeated per file
- The change was not initiated by the developer

**Common IDE artifact files to watch for:**

| Pattern | Source |
|---|---|
| `**/.project` | Eclipse / VS Code Java Language Server |
| `**/.classpath` | Eclipse JDT |
| `**/.settings/**` | Eclipse workspace preferences |
| `**/*.iml` | IntelliJ IDEA module files |
| `**/.idea/**` | IntelliJ IDEA project files |

**Tracked vs Untracked Pre-Check (Critical):**

Before discarding anything, the agent **MUST** distinguish between
**tracked** (version-controlled) and **untracked** (new/generated)
files in the affected area. This is critical because directories
like `.settings/` often contain a **mix** of tracked files (e.g.,
`org.eclipse.jdt.core.prefs` committed by the team) and untracked
files (e.g., `org.eclipse.m2e.core.prefs` auto-generated by the
JDT Language Server).

```powershell
# List tracked files under .settings/
git ls-files .settings/

# List untracked files under .settings/
git ls-files --others --exclude-standard .settings/

# For modified tracked files, show what changed
git diff --stat HEAD -- .settings/
```

**⚠️ Never bulk-delete a directory that contains tracked files.**
Using `Remove-Item ".settings" -Recurse -Force` when the directory
contains tracked files will cause those files to appear as deleted
in `git status`, requiring immediate restoration via
`git checkout -- <file>`. Instead, remove only the specific
untracked files.

**JDT Language Server + m2e Auto-Injection:**

When the JDT Language Server detects a `pom.xml`, it automatically
imports the project as Maven-managed and injects:
- `org.eclipse.m2e.core.maven2Builder` into `.project` `<buildSpec>`
- `org.eclipse.m2e.core.maven2Nature` into `.project` `<natures>`
- `.settings/org.eclipse.m2e.core.prefs` (untracked)
- `.settings/org.eclipse.core.resources.prefs` (untracked)

These are **not** from the VS Code Maven extension
(`vscjava.vscode-maven`) — that extension provides the UI only.
The `.project` modifications come from the **Eclipse JDT Language
Server** (`eclipse.jdt.ls`) which bundles **m2e** internally.

**Mandatory User Confirmation Workflow:**

The agent **MUST** present suspected noise to the user and obtain
explicit confirmation before discarding. Never silently discard
changes to IDE metadata files — the project may rely on them.

1. **Present the suspected noise** — Show the user a categorized
   summary separating modified tracked files from untracked files,
   and include the proposed discard steps:

   ````markdown
   ## Suspected IDE Artifact Noise

   ### Modified Tracked Files
   | File | Change | Source |
   |---|---|---|
   | `.project` | +17 lines (Maven builder/nature + filteredResources) | JDT LS / m2e auto-import |

   ### Untracked Files (IDE-generated)
   | File | Content | Source |
   |---|---|---|
   | `.settings/org.eclipse.m2e.core.prefs` | m2e workspace config | JDT LS m2e import |
   | `.settings/org.eclipse.core.resources.prefs` | Encoding `Cp1252` | Eclipse workspace |
   | `.gitignore` | `/bin/` | Possibly auto-generated |

   ### Already-Tracked Files (will NOT be touched)
   | File | Status |
   |---|---|
   | `.settings/org.eclipse.jdt.core.prefs` | ✅ Tracked, unchanged — preserved |

   **Proposed discard steps:**
   ```powershell
   # 1. Revert modified tracked file
   git checkout -- .project

   # 2. Remove specific untracked files (NOT the whole directory)
   Remove-Item ".settings/org.eclipse.m2e.core.prefs" -Force
   Remove-Item ".settings/org.eclipse.core.resources.prefs" -Force
   Remove-Item ".gitignore" -Force

   # 3. Verify
   git status --short
   ```

   **⚠️ Warning:** `.settings/org.eclipse.jdt.core.prefs` is tracked
   and will be preserved. The discard targets only IDE-generated noise.

   Should I discard these changes? (yes / no / inspect further)
   ````

2. **Act on user feedback:**
   - **"yes" / "discard"** — Execute the proposed discard steps
     **exactly as presented**, then verify with `git status --short`.
   - **"no" / "keep"** — Leave the changes in the working tree.
     They may be staged as a separate commit (e.g.,
     `chore: update Eclipse project metadata`) or left for later.
   - **"inspect further"** — Show full diffs for additional files
     so the user can distinguish intentional changes from noise.
   - **Partial discard** — If the user identifies some files as
     intentional and others as noise, discard only the confirmed
     noise files individually.

3. **Post-discard verification:**
   ```powershell
   git status --short
   git diff --stat HEAD
   ```
   If any tracked file appears as deleted (accidentally removed),
   restore it immediately:
   ```powershell
   git checkout -- <accidentally-deleted-file>
   ```

**Prevention:** Add IDE artifact patterns to `.gitignore` if the
project does not require IDE metadata to be version-controlled. If
the project *does* track them, coordinate with the team on which
metadata files are shared vs personal before discarding.

---

### Step 4 — Formatting & Structural Partitioning

Stylistic and structural changes MUST be explicitly separated from
functional commits.

#### 4a — Formatting & Stylistic Consolidation

**Target:** Purely aesthetic changes — indentation, whitespace,
Markdown header-level corrections.

**Rule:** If multiple files require these adjustments, club them into a
single dedicated commit. Commit type: `style`.

#### 4b — Structural Refactor Isolation

**Target:** Functional-preserving reorganizations — alphabetical
reordering of methods, variables, or constants.

**Rule:** Isolate into dedicated commits. Commit type: `refactor`.
Large structural reorders should be committed per-file or
per-logical-group for clear "move" history.

#### 4c — Zero Mixture

Never mix formatting (4a) with structural refactors (4b) or functional
logic (Step 2). Use `git add -p` or Intermediate State Synthesis
(Step 11) to ensure absolute partitioning.

---

### Step 5 — Configuration Coupling

Tool configurations and metadata MUST be atomically linked to the code
they support.

- **Functional Pairing:** Updates to `.vscode/settings.json` (e.g.,
  cSpell words), `.lintrc`, or other config files MUST be staged and
  committed alongside the functional changes that necessitate them.
- **IDE Project Files:** Shared IDE config files (`.idea/` core XMLs,
  `.vscode/` shared settings) that establish project structure MUST be
  tracked. Personal settings (e.g., `workspace.xml`) MUST remain ignored.
- **Example:** If adding a new rule file introduces technical terms, the
  cSpell update for those terms MUST be part of the same atomic unit.

---

### Step 6 — Submodule Synchronization

When managing submodules, the main repository's history must remain
descriptive and clear.

- **Synchronized Commits:** Every functional update in a submodule
  requiring a pointer update in the main repo MUST be coupled with its
  relevant main-repo configuration changes.
- **Descriptive Titles:** Main repo sync commits MUST NOT use generic
  titles like `sync submodule`. They MUST summarize the modular
  improvements (e.g., `docs: sync rules submodule and update markdown
  generation standards`).
- **Dangling Pointer Check:** Before pushing, verify the referenced
  submodule commit exists in the remote submodule repository.
- **Canonical Ancestry:** Ensure the new submodule pointer is a
  descendant of the previous pointer if linear history is expected.

---

### Step 7 — Generated vs Custom File Splitting

When a file contains both standard API-generated content (e.g., from
gitignore.io) and user-defined custom rules, split into separate commits.

- **Commit A (Foundation):** Commit only the standard, API-generated
  portion first. Back up the full file, overwrite with the exact API
  content, and commit. This establishes a clean, reproducible baseline.
- **Commit B (Customization):** Commit the user-defined sections in a
  subsequent commit. This clearly distinguishes "standard boilerplate"
  from "project-specific logic."
- **User Modifications:** If the user has altered the API-generated
  portion, separate those alterations from the raw API import if
  possible, or document clearly as user-patches.

---

### Step 8 — Commit Message Quality Standards

Every commit message MUST meet these quality requirements:

| Requirement | Detail |
|---|---|
| **Specificity** | Avoid generic titles. List specific components (e.g., `add linux, macos, and windows gitignore rules` not `os-specific`) |
| **Anti-Repetition** | The body MUST NOT merely rephrase the title |
| **Context Enrichment** | Explain the 'Why' — especially for architectural or security decisions |
| **Atomic Rationale** | The body MUST state WHY these specific changes are grouped together. If multiple files, explain their functional coupling |
| **Constraint Documentation** | Mention constraints or external dependencies that influenced grouping |
| **Contextual Accuracy** | Use precise terms (e.g., "Supabase project-specific" not generic "project-specific") |
| **Body/Diff Congruence** | The message body MUST be a complete, accurate summary of ALL changes in the staged hunks. Any discrepancy requires an immediate corrected preview |

---

### Step 9 — Execution & Verification

#### 9a — Step-by-Step Execution

Execute commits one-by-one according to the approved arrangement.

#### 9b — Recovery

If a mistake is made during staging:
- **Unstage:** `git reset <file>`
- **Selective discard:** `git checkout -p`
- **WARNING:** Never use `git reset --hard` for synchronization.
  Always prefer `git pull`.

#### 9c — Pull Before Push

Always `git pull` (or `git pull --rebase` upon explicit approval) before
pushing to incorporate latest remote changes.

#### 9d — Opaque Content Analysis

For files flagged as binary or large assets (LFS), verify internal
consistency by inspecting file contents (e.g., `cat -v` or hex dump) to
ensure the commit message accurately reflects the data being stored.

#### 9e — History Refinement Delegation

If existing commits need to be split or refined (e.g., to fix non-atomic
changes), delegate to the
[`git_history_refinement`](../git_history_refinement/SKILL.md) skill.

#### 9f — Stash Workflow for Rebase

If rebase fails due to unstaged changes:

```bash
git stash push -m "Descriptive message"
git pull --rebase origin <branch>
git stash pop
```

If `git stash pop` creates conflicts, resolve manually, then:

```bash
git add <resolved-files>
git stash drop
```

#### 9g — Corrupted Rebase State Recovery

If `git rebase --continue` or `git rebase --abort` fails with
`warning: could not read '.git/rebase-merge/head-name'`, the
`.git/rebase-merge` directory is likely empty or corrupted.

**Diagnostic:**

```powershell
Test-Path ".git/rebase-merge"           # True = directory exists
Get-ChildItem ".git/rebase-merge"       # Empty = corrupted state
```

**Resolution:**

1. **Verify staged changes are intact** — run `git diff --cached` to
   confirm your staged work is preserved.
2. **Remove the corrupted directory:**
   ```powershell
   Remove-Item ".git/rebase-merge" -Recurse -Force
   ```
3. **Verify clean state** — run `git status` to confirm the rebase
   state indicator is gone.
4. **Commit directly** — since the rebase state is cleared, use a
   normal `git commit` with the planned message instead of
   `git rebase --continue`.

---

### Step 10 — Logic-Documentation Compass

Visualize the commit history as a compass where each direction is a
logical area:

| Direction | Area |
|---|---|
| **North** | Architectural / Schema changes |
| **East** | Logic / Feature implementation |
| **West** | Testing / Verification |
| **South** | Documentation / Refinement |

A high-quality commit history moves clearly through these directions
without "spinning" (mixing logic and documentation in one commit).

#### External Tool Asset Granularity

When versioning assets for external tools (Postman, Insomnia, DBeaver),
maximize granularity by separating:

- **Environments:** Endpoints, variables, credentials
- **Collections:** Logical groupings of requests, tests, scripts
- **Data Tables:** CSV/JSON templates for bulk-run or validation

Never group these into a single generic `test(tooling)` commit if they
serve distinct purposes.

---

### Step 11 — Source Logic & Generated Files

#### 11a — Update the Source, Not the Output

Never manually edit generated files. Always update the source logic
(templates, scripts, CI/CD workflows) that produces them:

- `README.md` from `templates/README.md.template` → edit the template
- Build artifacts, compiled code → edit source code or configuration

#### 11b — Identify Synchronization Mechanisms

Before making changes:

1. **Detect Generation:** Check for `<!-- AUTO-GENERATED -->` comments,
   build scripts, or CI/CD workflows that regenerate files.
2. **Locate Source:** Find the template, script, or configuration that
   produces the generated file.
3. **Document Sync:** Note in commit messages if manual synchronization
   is required (e.g., "Run `npm run build` to regenerate").

#### 11c — CI/CD Managed File Exclusion

Files managed by CI/CD automation MUST be excluded from manual edits.

- Maintain an explicit exclusion list (e.g., `README.md`,
  `agent-rules.md`)
- When verifying link updates, use `--exclude` flags:

```bash
grep -r "old-name.md" . --exclude-dir=.git --exclude=README.md
```

- Before committing, verify no CI/CD managed files are staged unless
  the commit explicitly targets the source logic that generates them.

---

### Step 12 — Intermediate State Synthesis

When a file contains interleaved changes or massive structural reorders
(50+ lines moved) mixed with functional fixes, hunk-based staging may
become unreliable.

**The Synthesis Strategy:**

1. **De-construct:** Manually edit the file (or use selective
   undo/revert) to match the current atomic goal BEFORE staging.
2. **Stage & Commit:** Stage the "synthesized" intermediate version that
   contains ONLY the intended logical unit.
3. **Iterate:** Repeat for remaining changes until the working directory
   is clean.

This guarantees that even high-entropy working states can be refactored
into pristine, industrial-grade commit history.

---

### Step 13 — User-Requested Coupling & Deviations

If the user explicitly requests coupling unrelated changes or deviating
from atomic rules:

1. **Warn First:** Explicitly warn: "This coupling technically violates
   Rule [X] because [reason]."
2. **Explicit Override:** Accept the coupling ONLY if the user
   re-confirms after the warning.
3. **Documentation:** Document the deviation rationale in the commit
   message body (e.g., "Coupled with IDE updates per user request for
   atomic convenience").

---

### Step 14 — Push Protocol

After commits are complete, follow the push protocol:

- **Explicit Request Required:** Do NOT execute `git push` unless the
  user explicitly requests it.
- **Offer, Don't Execute:** After commits, OFFER to push. Wait for
  explicit "yes" or "push" command.
- **Status Check:** Always run `git status` before push.
- **Discover Default Branch:** Do NOT assume the default branch name.
  Discover it programmatically:

```bash
git branch -r
```

---

### Step 15 — Guardrail Against Predictive Planning

The agent must never "commit" in a plan to what will be changed in the
future. Commit construction is a **Real-Time Analysis** task. The plan
serves only as a roadmap for the **Protocol** of commitment, not the
**Content** of the commits themselves. Logic for commit construction must
be synthesized from real-time analysis, never mocked in a plan.

---

## Scope Coverage

| Category | Convention |
|---|---|
| Functional changes | One logical unit per commit |
| Formatting / style | Dedicated `style` commit |
| Structural refactors | Dedicated `refactor` commit |
| Config updates | Coupled with their functional change |
| Submodule pointer updates | Descriptive title, coupled with main-repo config |
| Generated vs custom content | Split into Foundation + Customization commits |
| CI/CD managed files | Excluded from manual edits |

---

## Prohibited Behaviors

The agent is **BLOCKED** from:

- **Auto-committing** — Never commit without explicit user "start"
  authorization
- **Auto-pushing** — Never push without explicit user request
- **Mixing concerns** — Never combine formatting + logic + refactor in
  one commit
- **Staging untracked files without confirmation** — Especially in repos
  with minimal `.gitignore`
- **Using `git reset --hard` for synchronization** — Use `git pull`
  instead
- **Skipping the commit preview** — The verbose arranged commits display
  is mandatory
- **Excluding untracked files from initial analysis** — Untracked files
  MUST be included in the Step 1 inventory alongside staged and unstaged
  changes; discovering them post-commit is a protocol violation
- **Predicting commit content in plans** — Commits are built from
  real-time analysis only
- **Editing generated files directly** — Update the source logic instead
- **Using generic commit messages** — Every message must be specific and
  non-repetitive
- **Batching hunk responses** — Each hunk must be evaluated individually
- **Skipping empty commits without user confirmation** — During rebase
  operations
- **Bulk-deleting directories with mixed tracked/untracked files** —
  Never `Remove-Item` a directory that contains tracked files; remove
  only specific untracked files to avoid accidental deletions

## Common Pitfalls

| Pitfall | Solution |
|---|---|
| Scanned with `Get-ChildItem` and got false positives | Use `git ls-files` as source of truth for tracked files |
| `.gitignore` negation patterns missed | Read `.gitignore` carefully — `!dir/*.zip` means those zips ARE tracked |
| `.gitignore` references to renamed directories not updated | `.gitignore` is a critical blast-radius target — update patterns or tracked files become untracked |
| `git mv` failed on empty directory | Empty dirs aren't tracked by Git — use `Rename-Item` or `mv` instead |
| Noise from unrelated hunks leaked into functional commit | Use `git add -p` and verify with `git diff --cached` after staging |
| Formatting fix discovered during feature work | Stage and commit separately via hunk-based staging |
| Committed half a logical change across two commits | If file A depends on file B's change, they MUST be in the same commit |
| Untracked files discovered only after committing staged/unstaged changes | Include ALL three categories (staged + unstaged + untracked) in the initial Step 1 inventory — untracked files are first-class scope members |
| CI/CD managed file manually edited | Check for auto-generation markers and CI workflows before editing |
| Piped input to `git add -p` didn't register in PowerShell | Use file-based input or discard noise via `git checkout --` after accepting desired hunks |
| Corrupted rebase state (`rebase-merge` dir empty) | Remove empty `.git/rebase-merge` directory to clear the broken state |
| Commit message body just rephrases the title | Body must add WHY, not repeat WHAT |
| Submodule pointer updated without verifying remote | Always verify the referenced commit exists in the remote submodule repo |
| IDE auto-modified 50+ `.project` / `.classpath` files with boilerplate | Present suspected noise to user with sample diff and proposed discard command; never auto-discard — project may intentionally track IDE metadata |
| Bulk-deleted `.settings/` directory and a tracked file disappeared | Use `git ls-files .settings/` to identify tracked files first; remove only specific untracked files, never the whole directory |
| Assumed Maven nature/builder in `.project` came from `vscjava.vscode-maven` | The `.project` modifications come from **JDT Language Server** (embedded m2e), not the Maven UI extension; attribute correctly when presenting to user |
