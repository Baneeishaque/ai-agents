<!--
title: Underscore Naming Convention
description: Enforce underscore_based naming for all project files, directories, and identifiers — with industry-standard exemptions.
category: Naming & Conventions
-->

# Underscore Naming Convention Skill

> **Skill ID:** `underscore_naming`
> **Version:** 1.0.0
> **Standard:** [Agent Skills (agentskills.io)](https://agentskills.io)

## Description

Enforce a consistent `underscore_based` naming convention across all
project files, directories, artifact identifiers, and internal references.
This skill detects violations, classifies exemptions, traces the full
blast radius of each rename, executes renames, updates all cross-references,
and verifies zero stale references remain.

Consistency in naming eliminates cognitive overhead, prevents build-tool
edge cases with special characters, and ensures grep/search reliability
across the entire codebase.

## Prerequisites

| Requirement | Minimum |
|---|---|
| Shell | PowerShell 5.1+ or Bash 4+ |
| VCS | Git (for `git mv` renames) |
| Access | Write access to the project repository |

## When to Apply

Apply this skill when:
- A new project is being initialized and naming conventions must be set
- A user asks to "enforce underscore naming" or "rename to underscore convention"
- A file, directory, or identifier with hyphens, camelCase, or mixed casing is detected
- An artifact ID, module name, or config key uses hyphens instead of underscores
- Pre-commit audit is requested to ensure naming consistency

Do NOT apply when:
- The file is an **industry-standard exempt name** (see Exemptions table below)
- The project explicitly uses a different convention (e.g., kebab-case for npm packages)
- Renaming would break external contracts (published API URLs, package registry names)

---

## Step-by-Step Procedure

### Step 1 — Scan for Violations

Detect all files and directories whose names contain hyphens or other
non-underscore separators.

**PowerShell:**
```powershell
Get-ChildItem -Recurse | Where-Object {
    $_.FullName -notmatch '\\(target|\.git|node_modules|dist|build)\\' -and
    $_.Name -match '-'
} | ForEach-Object { $_.FullName }
```

**Bash:**
```bash
find . -not -path '*/target/*' -not -path '*/.git/*' \
       -not -path '*/node_modules/*' -name '*-*' | sort
```

### Step 2 — Classify Exemptions

Not every hyphenated name is a violation. The following are
**industry-standard names** that MUST keep their hyphens:

| File / Pattern | Reason |
|---|---|
| `azure-pipelines.yml` | Azure DevOps mandates this exact filename |
| `.gitignore`, `.gitmodules` | Git specification |
| `docker-compose.yml` | Docker Compose specification |
| `package-lock.json` | npm specification |
| `.eslintrc.cjs`, `.prettierrc` | Tool-specific dotfiles |
| `tsconfig.json` | TypeScript specification |
| `pom.xml`, `build.gradle` | Build tool specifications |
| `README.md`, `LICENSE`, `AGENTS.md` | Universal conventions (no hyphens — already compliant) |
| `Makefile`, `Dockerfile` | Tool specifications |
| `.env.*` files | Environment config convention |
| `-rules.md` files in `ai-agent-rules/` | Per `ai-rule-standardization-rules.md` — kebab-case mandated |

**Decision rule:** If a tool, platform, or specification **mandates** the
exact filename, it is exempt. If the name is author-chosen, it MUST use
underscores.

### Step 3 — Trace the Blast Radius

Before renaming anything, find **every reference** to the old name across
the entire codebase. This prevents orphaned links, broken imports, and
stale documentation.

**PowerShell:**
```powershell
Get-ChildItem -Recurse -File |
    Where-Object { $_.FullName -notmatch '\\(target|\.git)\\' } |
    Select-String -Pattern "old-name" |
    Format-Table Filename, LineNumber, Line -AutoSize
```

**Bash:**
```bash
grep -rn "old-name" --include='*' \
    --exclude-dir='{target,.git,node_modules}' .
```

Document every match. This is the **blast radius** — every file in this
list must be updated after the rename.

### Step 4 — Rename (Deepest-First)

Rename files and directories using `git mv` to preserve history.
Always rename **deepest paths first** to avoid parent-path invalidation.

```bash
# Directory rename (deepest first)
git mv path/to/old-name path/to/old_name

# File rename
git mv path/to/old-file.ext path/to/old_file.ext
```

**Ordering rule:** If renaming both a directory and files inside it,
rename the directory first (Git tracks the content, not the path).

#### Fallback — Locked Directories

If the directory cannot be renamed because it is locked by VS Code,
OneDrive, or another process, use the **mirror-and-remove** strategy:

1. **Remove from VS Code workspace** — if the directory is a workspace
   root folder, remove it first to release file watchers:
   - **File → Remove Folder from Workspace**, or
   - Run VS Code command: `workbench.action.removeRootFolder`
2. **Mirror** the directory to the new name:
   ```powershell
   robocopy "path/to/old-name" "path/to/old_name" /MIR /R:1 /W:1
   ```
3. **Verify** the new directory has identical contents.
4. **Remove contents** of the old directory:
   ```powershell
   Get-ChildItem "path/to/old-name" -Recurse -Force -File |
       Remove-Item -Force -ErrorAction SilentlyContinue
   Get-ChildItem "path/to/old-name" -Recurse -Force -Directory |
       Sort-Object { $_.FullName.Length } -Descending |
       Remove-Item -Force -ErrorAction SilentlyContinue
   ```
5. **Delete the empty shell** — if still locked, instruct the user to
   close VS Code and run manually:
   ```powershell
   rmdir "path/to/old-name"
   ```
6. **Re-add to workspace** — if the directory was a VS Code workspace
   folder, remind the user to re-add the renamed folder via
   **File → Add Folder to Workspace…** or:
   ```powershell
   code --add "path/to/old_name"
   ```

### Step 5 — Update All Cross-References

Using the blast radius from Step 3, update every reference to the old
name in all affected files:

- **Documentation** (`.md` files): Links, titles, inline references
- **Build configs** (`pom.xml`, `build.gradle`, `package.json`): Artifact IDs, module names
- **CI/CD** (`azure-pipelines.yml`, `.github/workflows/`): Artifact references, paths
- **IDE configs** (`.project`, `.classpath`, `.idea/`): Project names, module paths
- **Source code**: Import statements, string literals, constants
- **Agent skills** (`SKILL.md`, `AGENTS.md`): Skill IDs, references, templates

**Critical:** Artifact identifiers (Maven `<artifactId>`, npm package `name`,
Gradle `rootProject.name`) are first-class rename targets — not just filenames.

### Step 6 — Verify Zero Stale References

After all renames and updates, verify that **zero** references to the old
name remain:

**PowerShell:**
```powershell
Get-ChildItem -Recurse -File |
    Where-Object { $_.FullName -notmatch '\\(target|\.git)\\' } |
    Select-String -Pattern "old-name" |
    Format-Table Filename, LineNumber, Line -AutoSize
```

**Expected result:** No output. If matches appear, return to Step 5.

**Exception:** The workspace folder name itself (e.g., `my-project`
as a parent directory) is NOT a violation — it exists outside the project's
controlled scope.

---

## Scope Coverage

The naming convention applies to these categories:

| Category | Convention | Example |
|---|---|---|
| Files (author-chosen) | `snake_case` | `tul_api_reference.md` |
| Directories (author-chosen) | `snake_case` | `tul_logging/` |
| Maven `<artifactId>` | `snake_case` | `tul_logging` |
| Gradle `rootProject.name` | `snake_case` | `tul_logging` |
| Skill IDs | `snake_case` | `underscore_naming` |
| Template files | `snake_case` + `.tpl` | `integration_snippet.java.tpl` |
| Config keys (author-chosen) | `snake_case` or `dot.notation` | `tul.server.url` |

---

## Prohibited Behaviors

The agent is **BLOCKED** from:

- **Renaming industry-standard files** — `azure-pipelines.yml`, `docker-compose.yml`, `package-lock.json`, etc. (see Exemptions table)
- **Renaming without tracing blast radius** — Every rename MUST be preceded by a full-codebase grep
- **Partial updates** — If a rename has 10 references, ALL 10 must be updated atomically. Leaving stale references is forbidden
- **Renaming external contract names** — Published package names, public API URLs, or registry-registered identifiers without explicit user approval
- **Renaming files in `ai-agent-rules/`** — These follow kebab-case per `ai-rule-standardization-rules.md`
- **Guessing references** — If the blast radius search returns unexpected results, the agent MUST pause and confirm with the user before proceeding
- **Skipping verification** — Step 6 (zero stale references) is mandatory. Never assume the rename is complete without running the verification command

## Common Pitfalls

| Pitfall | Solution |
|---|---|
| Renamed file but forgot Maven `<artifactId>` | Artifact IDs are first-class targets — always include in blast radius search |
| Renamed directory but child paths broke | Rename deepest-first; for `git mv`, rename the directory in one operation |
| Eclipse `.project` still has old name | IDE configs (`.project`, `.classpath`, `.idea/`) must be updated |
| CI/CD pipeline references old artifact glob | Search CI configs (`*-pipelines.yml`, `*.yml` in `.github/`) for old name |
| Documentation links broken after rename | Run blast radius search on the **new** name after rename to verify link targets exist |
| Renamed a tool-mandated filename | Check the Exemptions table first — some hyphens are mandatory |
| `ai-agent-rules/` files renamed to underscores | These files follow kebab-case per their own standardization rules — exempt |
| Directory locked by VS Code / OneDrive | Use `robocopy /MIR` fallback: mirror → remove contents → user deletes empty shell after closing VS Code |
| Renamed folder disappeared from VS Code workspace | After renaming a workspace root folder, re-add it via **File → Add Folder to Workspace…** or `code --add` |
