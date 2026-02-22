<!--
title: Project Structure & Documentation
description: Industrial-standard project folder structure, root hygiene, README conventions, and documentation placement rules.
category: Project Organization
-->

# Project Structure & Documentation Skill

> **Skill ID:** `project_structure`
> **Version:** 1.0.0
> **Standard:** [Agent Skills (agentskills.io)](https://agentskills.io)

## Description

Audit and organize any project to follow industrial-standard conventions
for folder structure, root-level file hygiene, documentation placement,
and mandatory project files. This skill classifies every file as either
tool-mandated (fixed location) or author-chosen (movable), then relocates
misplaced artifacts, establishes standard directories, and ensures
`README.md` and `AGENTS.md` follow proven patterns.

Clean project structure eliminates onboarding friction, enables IDE/tool
auto-discovery, and makes the codebase navigable at a glance.

## Prerequisites

| Requirement | Minimum |
|---|---|
| Shell | PowerShell 5.1+ or Bash 4+ |
| VCS | Git (for `git mv` renames) |
| Access | Write access to the project repository |

## When to Apply

Apply this skill when:
- A new project is being initialized from scratch
- A user asks to "organize the project" or "fix the folder structure"
- Design documents, plans, or internal docs are found in the project root
- CI/CD configs for non-active platforms clutter the root
- The project has no `README.md`
- `AGENTS.md` duplicates content from `README.md` or `SKILL.md`
- Root contains files that belong in subdirectories

Do NOT apply when:
- The project is a monorepo with its own established conventions
- A framework mandates a specific structure (e.g., Next.js `app/`, Rails `app/models/`)
- The user explicitly prefers a flat structure

---

## Step-by-Step Procedure

### Step 1 — Audit Root Directory

List every file and directory in the project root and classify each one:

```powershell
Get-ChildItem -Path . -Force | Select-Object Name, Mode |
    Sort-Object Mode, Name
```

```bash
ls -1aF | sort
```

Classify each item against the **Root File Classification Table** (below).
Any file that is NOT in the "belongs in root" list is a candidate for
relocation.

### Step 2 — Establish Standard Directories

Create the standard directory structure. Only create directories that the
project actually needs — do not create empty placeholder dirs.

```
project_root/
├── src/              # Source code (language-specific subtree)
├── docs/             # Design docs, plans, ADRs, internal documentation
├── ci/               # Dormant or reference CI/CD configs (not auto-detected)
├── .agents/          # AI agent skills (agentskills.io standard)
│   └── skills/
│       └── {skill_name}/
│           ├── SKILL.md
│           ├── templates/
│           ├── references/
│           └── examples/
├── .settings/        # IDE-specific settings (Eclipse, IntelliJ via .idea/)
└── test/             # Test code (if not co-located with src/)
```

**Do NOT create:**
- `docs/` if there are no documents to put in it
- `ci/` if all CI configs are in their auto-detect locations
- `test/` if tests live inside `src/` (Maven/Gradle convention)

### Step 3 — Relocate Misplaced Files

Move files that do not belong in root to their correct directory.
Use `git mv` to preserve history.

**Common relocations:**

| File Pattern | From | To | Rationale |
|---|---|---|---|
| `implementation_plan*.md` | root | `docs/` | Internal design documentation |
| `architecture*.md` | root | `docs/` |  Internal design documentation |
| `*-pipelines.yml` (dormant) | root | `ci/` | Not auto-detected; reference only |
| `ADR-*.md` | root | `docs/adr/` | Architecture Decision Records |
| `design_*.md` | root | `docs/` | Design artifacts |

**Ordering:** Move deepest paths first to avoid parent-path invalidation.

**After each move**, search for and update all references:

```powershell
Get-ChildItem -Recurse -File |
    Where-Object { $_.FullName -notmatch '\\(target|\.git)\\' } |
    Select-String -Pattern "old_filename" |
    Format-Table Filename, LineNumber, Line -AutoSize
```

### Step 4 — Audit / Create README.md

Every project MUST have a `README.md` in root. It is the first file
anyone reads — on GitHub, GitLab, or any file browser.

#### Mandatory Sections

| # | Section | Purpose |
|---|---|---|
| 1 | `# Project Name` | H1 title — short, clear |
| 2 | Description paragraph | 2–3 sentences: what it does, who it's for |
| 3 | `## Features` | Bullet list of key capabilities |
| 4 | `## Requirements` | Table of minimum versions (Java, Node, Python, etc.) |
| 5 | `## Quick Start` | Dependency snippet + minimal usage code |
| 6 | `## Configuration` | Key config options (table or brief description) |
| 7 | `## Building from Source` | Exact commands to build/test |
| 8 | `## Project Structure` | Annotated directory tree (only top 2 levels) |
| 9 | `## Documentation` | Table of links to all docs (SKILL.md, API ref, design docs) |
| 10 | `## License` | License name + link |
| 11 | `## Maintainer` | Name and contact (derived from `git config`) |

#### README Rules

- **Zero duplication with AGENTS.md** — README is for humans; AGENTS.md is for AI agents
- **No internal implementation details** — those go in `docs/`
- **Code examples must be copy-pasteable** — complete, not truncated
- **Links use relative paths** — portable across forks and clones
- **Configuration table** references the source code defaults — not invented values

### Step 5 — Audit / Create AGENTS.md

`AGENTS.md` is a **thin bridge** for AI agent discovery. It MUST NOT
duplicate content from `README.md` or `SKILL.md`.

#### Template

```markdown
# AGENTS.md

## Skills

| Skill | Path | When to use |
|---|---|---|
| Skill Name | [`.agents/skills/skill_name/SKILL.md`](.agents/skills/skill_name/SKILL.md) | One-line trigger description |

## Conventions

- Convention 1 (e.g., "Underscore naming for files and directories")
- Convention 2
- See [README.md](README.md) for build commands and project overview
```

#### AGENTS.md Rules

- **Maximum ~15 lines** — it is a routing table, not documentation
- **Skills table** — one row per skill, with relative path and trigger
- **Conventions** — 3–5 bullet points of project-wide rules
- **Defer to README** — link to it for build commands, structure, etc.
- **Defer to SKILL.md** — link to it for procedures, templates, etc.
- **Zero duplicated tables** — if a table exists in SKILL.md or README, do not copy it here

### Step 6 — Audit CI/CD Placement

CI/CD config files have two categories:

| Category | Location | Examples |
|---|---|---|
| **Auto-detected** (active) | Tool-mandated location in root | `.github/workflows/*.yml`, `azure-pipelines.yml`, `.gitlab-ci.yml`, `Jenkinsfile` |
| **Dormant / reference** | `ci/` directory | Pipelines for platforms you're not actively using |

**Decision rule:**
- If the project uses Azure DevOps → `azure-pipelines.yml` stays in root
- If the project pushes to GitHub but has an `azure-pipelines.yml` → move to `ci/`
- If the project has configs for multiple CI platforms → active one in root, others in `ci/`

**After moving**, update any documentation that references the old path.

### Step 7 — Verify

After all changes, verify the root is clean and all links work:

```powershell
# List root — should be only convention files
Get-ChildItem -Path . -Force -Name | Sort-Object

# Check for broken relative links in markdown
Get-ChildItem -Recurse -Filter "*.md" |
    Where-Object { $_.FullName -notmatch '\\(target|\.git)\\' } |
    Select-String -Pattern "\[.*\]\(((?!http)[^)]+)\)" |
    ForEach-Object {
        $link = $_.Matches[0].Groups[1].Value
        $dir = Split-Path $_.Path
        $target = Join-Path $dir $link
        if (-not (Test-Path $target)) {
            [PSCustomObject]@{File=$_.Filename; Line=$_.LineNumber; BrokenLink=$link}
        }
    } | Format-Table -AutoSize
```

**Expected root contents** (typical Java/Maven project):

```
.agents/          # AI agent skills
.classpath        # IDE (if tracked)
.gitignore        # Git
.project          # IDE (if tracked)
.settings/        # IDE (if tracked)
AGENTS.md         # Agent bridge
ci/               # Dormant CI configs (if any)
docs/             # Design documentation
jitpack.yml       # JitPack config (if applicable)
pom.xml           # Maven build
README.md         # Human entry point
src/              # Source code
```

---

## Root File Classification Table (SSOT)

Files that **belong in root** (tool-mandated or universal convention):

| File | Reason | Movable? |
|---|---|---|
| `README.md` | Universal convention — first file read | No |
| `AGENTS.md` | Agent discovery convention | No |
| `LICENSE` | Universal convention | No |
| `CHANGELOG.md` | Universal convention | No |
| `.gitignore` | Git specification | No |
| `.gitmodules` | Git specification | No |
| `pom.xml` | Maven specification | No |
| `build.gradle` / `build.gradle.kts` | Gradle specification | No |
| `package.json` | npm/Node specification | No |
| `Cargo.toml` | Rust specification | No |
| `go.mod` | Go specification | No |
| `Makefile` | Make specification | No |
| `Dockerfile` | Docker specification | No |
| `docker-compose.yml` | Docker Compose specification | No |
| `.classpath`, `.project` | Eclipse IDE (if tracked) | No |
| `azure-pipelines.yml` | Azure DevOps auto-detect (only if active) | `ci/` if dormant |
| `.gitlab-ci.yml` | GitLab auto-detect (only if active) | `ci/` if dormant |
| `Jenkinsfile` | Jenkins auto-detect (only if active) | `ci/` if dormant |
| `jitpack.yml` | JitPack auto-detect | No |

Files that **do NOT belong in root:**

| File Pattern | Move To | Reason |
|---|---|---|
| `implementation_plan*.md` | `docs/` | Internal design artifact |
| `architecture*.md` | `docs/` | Internal design artifact |
| `ADR-*.md` | `docs/adr/` | Architecture Decision Record |
| `design_*.md` | `docs/` | Internal design artifact |
| `*_plan_v*.md` | `docs/` | Versioned plan document |
| Dormant CI config | `ci/` | Not auto-detected by any active platform |

***

## Standard Directory Map (SSOT)

| Directory | Purpose | When to Create |
|---|---|---|
| `src/` | Source code | Always (language-specific subtree inside) |
| `docs/` | Design docs, plans, ADRs | When project has internal documentation |
| `ci/` | Dormant/reference CI configs | When CI config is not in its auto-detect location |
| `.agents/skills/` | AI agent skills | When project has agent skills |
| `.settings/` | IDE settings (Eclipse) | When tracking shared IDE config |
| `.idea/` | IDE settings (IntelliJ) | When tracking shared IDE config |
| `.github/workflows/` | GitHub Actions | When using GitHub Actions (auto-detected) |
| `test/` | Test code (if separate) | Only if tests don't live inside `src/` |

***

## Prohibited Behaviors

The agent is **BLOCKED** from:

- **Creating empty directories** — only create directories that will immediately contain files
- **Duplicating content across README, AGENTS.md, and SKILL.md** — each has a distinct purpose. Zero overlap
- **Moving tool-mandated files** — `pom.xml`, `.gitignore`, `Makefile`, etc. MUST stay in root
- **Moving active CI configs** — only dormant/unused configs go to `ci/`
- **Creating README.md without all mandatory sections** — partial READMEs are worse than none
- **Inventing project details** — derive from `git config`, `pom.xml`, `package.json`, or ask the user
- **Removing files** — this skill relocates and organizes; it never deletes
- **Ignoring reference updates** — every file move MUST include a search for all references to update
- **Using absolute paths in documentation** — all links MUST be relative for portability

## Common Pitfalls

| Pitfall | Solution |
|---|---|
| Moved `azure-pipelines.yml` but it was the active CI | Check if the project actually uses Azure DevOps before moving |
| Created `docs/` with only one file | Still correct — `docs/` signals "documentation lives here" even with one file |
| README duplicates the SKILL.md procedure | README shows Quick Start (3 steps); SKILL.md has the full procedure |
| AGENTS.md grew to 40+ lines | Trim to ~15 lines — it's a routing table. Defer to README and SKILL.md |
| Moved a file but broke markdown links | Always run blast radius search after every move (Step 3) |
| Created `test/` but Maven expects `src/test/` | Respect the build tool's convention — Maven/Gradle use `src/test/` |
| Root still has stray `.md` files after audit | Re-run Step 1 classification — every root file must be justified |
| `ci/` directory confused for source code | The name `ci/` is a well-known convention (Docker, GitLab, many OSS projects) |
