---
name: Mise Tool Management
description: Industrial protocols for mise configuration trust, tool version selection,
    and Python package setup. Use whenever a mise.toml is untrusted, a required tool
    is missing, or a Python package needs to be installed into a mise-managed environment.
category: Environment-Management
---

# Mise Tool Management Skill

This skill defines the **layered industrial protocol** for managing `mise`-based development
environments. Each layer is a strict prerequisite for the one above it.

```text
Layer 1: Mise Config Trust              (base — everything depends on this)
└── Layer 2: Mise Tool Selection        (works on top of Layer 1)
    └── Layer 3: Mise Python Setup      (a specialisation of Layer 2 for Python)
        └── Layer 4: Mise Python Package Setup  (works on top of Layer 3)
```

***

## 1. Layer 1 — Mise Configuration Trust Protocol

`mise` blocks any config file it does not explicitly trust. The agent MUST resolve this
before attempting any tool use.

### 1.1 Detection

Run the following command to detect whether any `mise` config in the working directory
is untrusted:

```bash
# Attempt to list tools — mise prints trust errors to stderr
mise ls 2>&1
```

- **Flag**: Output containing `Config files in ... are not trusted` signals an untrusted
  configuration.

### 1.2 Analysis & Presentation

When a trust error is detected, the agent MUST:

1. **Read** the untrusted `mise.toml` (or `.mise.toml`) file.
2. **Present** the full file content to the USER in a fenced code block.
3. **Analyse** each declared tool: what it is, what version is pinned, and why it exists.
4. **Recommend** explicitly (e.g. "This file only pins `python = "3.11"`. It does not
   run scripts and poses no security risk. I recommend trusting it.").

### 1.3 User Decision Gate

The agent MUST ask:

> "Do you want to trust this `mise.toml`? (yes / no)"

- **Yes** → run `mise trust` for the directory, then continue to Layer 2.
- **No** → halt. Document the decision. Do **not** attempt to use any `mise`-managed tool.

### 1.4 Trust Command

```bash
# Trust the config for the given directory
# Pass the config file path directly as the argument
mise trust /absolute/path/to/directory/mise.toml
```

- The config file path is passed as a positional argument, not via `--path`.
- Note: mise prints trust-error noise during parsing even as it processes the command—
  this is expected. Confirm success by checking for `mise trusted <path>` in the output.

***

## 2. Layer 2 — Mise Tool Selection Protocol

Applies when a tool required by `mise.toml` is missing or could be satisfied by an
already-installed version. This layer applies to **any** `mise`-managed tool (Python,
Node, Go, etc.).

### 2.1 Inventory

```bash
# List every installed version of a tool (replace `python` with the tool name)
mise ls python --json
```

Each JSON element has:

- `version` — installed version string
- `install_path` — filesystem path
- `source.path` — which `mise.toml` declared it
- `active` — whether it is the currently active version

### 2.2 Required Version Freshness Check

Before comparison, check whether the version pinned in `mise.toml` is itself the
latest release of the tool:

```bash
# Example: check latest Python available in mise
mise ls-remote python | tail -5
```

Present the result to the USER:

```text
mise.toml pins  : python 3.11
Latest available: python 3.13.2

Recommendation: The pinned version is not the latest.
Offer to update mise.toml to 3.13.2, but only if the USER agrees.
```

- **If user wants latest** → install latest AND update `mise.toml` pin.
- **If user wants required** → install required version; do NOT alter `mise.toml`.
- **If user is undecided about conf update** → install chosen version; do NOT alter
  `mise.toml` even if the user chose the latest.

### 2.3 Comparison Logic

Let `required` = version stated in the project `mise.toml`.
Let `installed` = all versions returned by `mise ls <tool> --json`.

| Scenario | Action |
| :--- | :--- |
| **No installed versions** | Run freshness check (§2.2); offer required or latest |
| **Installed version older than required** | Offer: install required OR use existing (with warning) |
| **Installed version exactly equals required** | Recommend using it as-is; no `mise.toml` change needed |
| **Installed version strictly greater than required** | Present analysis; ask user to use it AND offer to update `mise.toml` pin |
| **Multiple versions installed** | Present all with analysis; USER selects one |

> **Greater-version rule**: If the installed version is strictly greater than the pinned
> `required` version, the agent MUST ask the USER two questions:
>
> 1. "Use the installed `<tool>@<installed>` instead of `<tool>@<required>`?"
> 2. "Update `mise.toml` to pin `<tool> = "<installed>"`?" (only if user said yes to 1)
>
> The `mise.toml` MUST NOT be updated unless the USER explicitly approves question 2.

### 2.4 Presentation to User

The agent MUST present a full decision table, for example:

```text
Required by mise.toml : python 3.11
Latest available      : python 3.13.2
Installed under mise  :
  ① python 3.11.9  (active, from ~/.config/mise/config.toml)
                    GREATER than required (3.11.9 > 3.11) ← RECOMMENDED

Recommendation:
  Use python 3.11.9 — already installed, satisfies requirement.
  Offer to update mise.toml pin from "3.11" → "3.11.9"? Ask user.

mise.toml freshness : OUTDATED (3.11 vs latest 3.13.2)
  Separately: offer to bump to 3.13.2? Ask user.
```

### 2.5 User Decision Gate

1. **Which version to use?** User selects from presented options.
2. **Update `mise.toml` pin?** Only if user explicitly confirms.

```bash
# Use an existing installed version — scoped to the config file, not globally
mise use --path /absolute/path/to/project python@<chosen-version>

# Install and use a new version — scoped to config
mise install python@<chosen-version>
mise use --path /absolute/path/to/project python@<chosen-version>
```

- `--path` — Ensures the `use` command writes to the **project-local** `mise.toml`,
  not the global `~/.config/mise/config.toml`. This prevents polluting the global env.

***

## 3. Layer 3 — Mise Python Environment Setup

A specialisation of Layer 2 for the Python tool. Python requires extra steps to verify
the interpreter and `pip` are available through `mise exec`, preventing accidental use
of the system Python.

### 3.1 Verify Python via `mise exec`

Always use `mise exec` to invoke Python, scoped to the correct configuration file:

```bash
# Verify the exact Python version (not system Python)
mise exec --cd /absolute/path/to/project python@<version> -- python --version

# Verify pip is bundled with mise-managed Python
mise exec --cd /absolute/path/to/project python@<version> -- python -m pip --version
```

- `mise exec` — Runs the command inside the mise environment, bypassing system PATH.
- `python@<version>` — Pins the exact version, preventing accidental tool resolution.
- `--cd /absolute/path/to/project` — Ensures the correct local `mise.toml` is loaded,
  NOT the global config.
- `--` — Separator between `mise exec` arguments and the actual command to run.

### 3.2 Repair pip if Missing

```bash
mise exec --cd /absolute/path/to/project python@<version> -- python -m ensurepip --upgrade
```

Both verification commands MUST succeed before proceeding to Layer 4.

***

## 4. Layer 4 — Mise Python Package Setup Protocol

Applies when a Python package (e.g. `pylint`, `black`, `ruff`) is required but not
installed. Requires Layer 3 to be complete. Uses `jq` for JSON parsing — ensure
`jq` is installed via the [System-Wide Tool Management Skill](../system-wide-tool-management/SKILL.md)
before running §4.3.

### 4.1 Requirements File Detection

Check whether a `requirements.txt` exists in the project scripts directory:

```bash
# Use absolute path to avoid any working-directory ambiguity
ls /absolute/path/to/project/requirements.txt 2>/dev/null && echo "EXISTS" || echo "MISSING"
```

### 4.2 Package Entry Check

If `requirements.txt` exists, check whether the target package is listed:

```bash
# Replace `pylint` with the package name; use absolute path
grep -i "^pylint" /absolute/path/to/project/requirements.txt
```

### 4.3 Version Freshness Check via PyPI (using `jq`)

If the package is listed, compare the pinned version against the latest on PyPI:

```bash
# Get the latest published version from PyPI — requires jq (see system-wide-tool-management skill)
curl -s https://pypi.org/pypi/pylint/json | jq -r '.info.version'
```

- `curl -s` — Silent mode; suppresses progress output.
- `https://pypi.org/pypi/<package>/json` — PyPI JSON API endpoint for any package.
- `jq -r '.info.version'` — Extracts the `version` field from `info` as raw text.
  The `-r` flag removes surrounding quotes.

### 4.4 Decision Table

| State | Action |
| :--- | :--- |
| `requirements.txt` missing | Create it; add `<package>==<latest>` |
| Package not in `requirements.txt` | Add `<package>==<latest>` |
| Package pinned to outdated version | Present analysis; offer to update pin to latest |
| Package pinned to latest | Proceed directly to installation |

If pinned version is **not** the latest:

1. Present comparison to user (pinned vs latest).
2. Ask: "Update pin to latest and install?"
3. **User says yes to latest** → update pin, install.
4. **User says no to latest** → install the pinned version as-is. Do NOT alter
   `requirements.txt`.

### 4.5 Presentation to User

The agent MUST present its analysis, for example:

```text
requirements.txt  : /absolute/path/to/project/requirements.txt — EXISTS
pylint entry      : MISSING
Latest on PyPI    : 3.3.4

Recommendation:
  Add `pylint==3.3.4` to requirements.txt, then install via:
  mise exec python@3.11.9 -- python -m pip install -r /absolute/path/to/project/requirements.txt
```

### 4.6 User Decision Gate

> "Approve adding `pylint==<version>` to `requirements.txt` and installing? (yes / no)"

### 4.7 Installation

Upon approval, always use `mise exec` with an absolute path to `requirements.txt`:

```bash
# Install all packages declared in requirements.txt into the active mise python env
mise exec --cd /absolute/path/to/project python@<version> -- \
  python -m pip install -r /absolute/path/to/project/requirements.txt
```

- `mise exec --cd ... python@<version>` — Targets the exact mise-managed Python,
  not the system interpreter. This ensures packages land in the correct isolated environment.
- `python -m pip` — Uses the pip bundled with the `mise`-managed Python.
- `-r /absolute/path/to/project/requirements.txt` — Reads the pinned dependency file for reproducible installs.
  Absolute path eliminates any working-directory ambiguity.

### 4.8 Execution Verification

After installation, verify the tool works:

```bash
# Example: verify pylint is reachable and show its version
mise exec --cd /absolute/path/to/project python@<version> -- python -m pylint --version
```

***

## 5. Full Worked Example — Pylint Setup for `sync-rules.py`

This section demonstrates all four layers against the real scenario.

### 5.1 Layer 1: Trust Check

```bash
mise ls 2>&1
# → mise ERROR: Config files in .../scripts/mise.toml are not trusted.
```

Present `mise.toml`:

```toml
[tools]
python = "3.11"
```

**Analysis**: Only pins `python = "3.11"`. No scripts or hooks. Safe to trust.

**Ask user**: "Trust this `mise.toml`? (yes/no)"

```bash
mise trust /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/mise.toml
# → mise trusted /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts
```

### 5.2 Layer 2: Python Version Selection

```bash
mise ls python --json
mise ls-remote python | tail -5
```

```text
Required by mise.toml : python 3.11
Latest available      : python 3.13.2
Installed under mise  :
  ① python 3.11.9  (active, from ~/.config/mise/config.toml)
    — Satisfies requirement (3.11.9 >= 3.11). RECOMMENDED.

mise.toml is OUTDATED (3.11 vs 3.13.2).
Offer to update? Ask user.
```

**User decision**: Use `3.11.9`, do not update `mise.toml`.

```bash
mise use --path /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts python@3.11.9
```

### 5.3 Layer 3: Python Verification

```bash
mise exec --cd /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts python@3.11.9 -- python --version
# → Python 3.11.9
mise exec --cd /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts python@3.11.9 -- python -m pip --version
# → pip 24.x
```

### 5.4 Layer 4: Pylint Setup

```bash
grep -i "^pylint" /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/requirements.txt
# → (no match)

curl -s https://pypi.org/pypi/pylint/json | jq -r '.info.version'
# → 3.3.4
```

**Analysis**: `pylint` absent from `requirements.txt`. Latest is `3.3.4`.

**Recommendation**: Add `pylint==3.3.4` to `requirements.txt` and install.

```bash
# After user approval:
echo "pylint==3.3.4" >> /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/requirements.txt

mise exec --cd /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts python@3.11.9 -- \
  python -m pip install -r /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/requirements.txt

mise exec --cd /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts python@3.11.9 -- \
  python -m pylint --version
```

***

## 6. Post-Edit File Validation Protocol

After **any** edit to a project file, the agent MUST validate the file using its
industrial-standard tool before proceeding. This catches formatting errors (e.g. stray
indentation) introduced by automated edits.

> The validation tools listed here are **system-wide tools**. Ensure they are installed
> using the [System-Wide Tool Management Skill](../system-wide-tool-management/SKILL.md)
> before running these commands.

### 6.1 Validation Commands by File Type

| File | Tool | Syntax Check | Format Check & Fix |
| :--- | :--- | :--- | :--- |
| `mise.toml` | `taplo` | `taplo check <absolute_path>` | `taplo fmt --check <absolute_path>` / `taplo fmt <absolute_path>` |
| `requirements.txt` | `pip` | `pip install --dry-run -r <absolute_path>` | N/A (no formatter) |
| `*.md` Markdown | `markdownlint-cli2` | `markdownlint-cli2 <absolute_path>` | `markdownlint-cli2 --fix <absolute_path>` |
| `*.py` Python | `pylint` via `mise exec` | `mise exec --cd <project_dir> python@<ver> -- python -m pylint <absolute_path>` | Manual fix per error |

> **Note on Python Errors & Formatting:**
>
> - `pylint` is a strict, comprehensive linter. It highlights semantic and structural
>   errors (e.g., missing docstrings, generic exceptions, complexity limits) which
>   **must be fixed manually** via code edits.
> - **DO NOT manually fix Pylint errors** before exhausting auto-fixers. You MUST use
>   specialized formatters and auto-fixers first to resolve stylistic and structural
>   complaints automatically.
> - **Primary Industrial Standard**: Use **`ruff check --fix`** and **`ruff format`** first.
>   It is drastically faster and can auto-fix many structural and code-style issues.
> - **Secondary Standard (Fallback)**: If `ruff` cannot be used, youMUST use **`black`**.
>   `black` is the definitive, uncompromising Python code formatter and was the
>   industry standard before `ruff`. (Only use `autopep8` as a last-resort legacy fallback).
> - Only after running these auto-fixers should you manually fix any remaining semantic
>   Pylint errors (e.g., missing docstrings, complex logic).
> - Because these are Python tools, they MUST follow Layer 4: add them to
>   `requirements.txt` and run them via `mise exec`.

### 6.2 `mise.toml` Validation & Formatting (`taplo`)

> **Important distinction:**
>
> - `taplo check` — validates TOML **syntax** (parse errors, duplicate keys). A file can
>   pass `check` but still be incorrectly formatted.
> - `taplo fmt --check` — verifies **formatting** (indentation, spacing, alignment).
> - `taplo fmt` — **auto-fixes** formatting in place; run after `check` passes.

Always run **both** steps in order:

```bash
# taplo must be installed — use system-wide-tool-management skill if missing
# Step 1 — Syntax check (exits 1 if invalid TOML)
taplo check /absolute/path/to/project/mise.toml

# Step 2 — Formatting check (exits 1 if not properly formatted)
taplo fmt --check /absolute/path/to/project/mise.toml

# Step 3 — Auto-format if Step 2 fails (modifies file in place)
taplo fmt /absolute/path/to/project/mise.toml

# Step 4 — Re-run formatting check to confirm clean
taplo fmt --check /absolute/path/to/project/mise.toml
```

- ✅ `taplo check`: exit 0, output `found files total=1` with no ERROR lines
- ✅ `taplo fmt --check`: exit 0 (no output on success)
- ❌ `taplo fmt --check` exit 1: `ERROR ... the file is not properly formatted` — run
  `taplo fmt` to auto-fix

Common TOML formatting mistakes:

- Leading indentation on key-value pairs under a table header.
    - ❌ Incorrect: `python = "3.11"` (4 spaces before `python`)
    - ✅ Correct: `python = "3.11"` (Starts securely at the beginning of the line)
- Missing blank line separating table sections
- Inconsistent quote style

### 6.3 `requirements.txt` Validation (`pip`)

```bash
pip install --dry-run -r /absolute/path/to/project/requirements.txt 2>&1 \
  | grep -E "^(Collecting|Requirement already|ERROR|WARNING)"
```

- ✅ Success: only `Collecting` and `Requirement already satisfied` lines; exit 0
- ❌ Failure: any `ERROR` line — fix the package specifier before proceeding
- No auto-formatter for `requirements.txt`; fix manually.

### 6.4 Worked Example — Validation After This Session's Edits

```bash
# Step 1 — Syntax check
taplo check /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/mise.toml
# → INFO taplo: found files total=1 ... (no ERROR) ✅

# Step 2 — Formatting check (detected issue: 4-space indent on python key)
taplo fmt --check /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/mise.toml
# → ERROR ... the file is not properly formatted ❌

# Step 3 — Auto-fix
taplo fmt /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/mise.toml
# → (reformats in place)

# Step 4 — Confirm clean
taplo fmt --check /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/mise.toml
# → (no output, exit 0) ✅

# Validate requirements.txt
pip install --dry-run -r /Users/dk/lab-data/ai-agents/ai-agent-rules/scripts/requirements.txt \
  2>&1 | grep -E "^(Collecting|Requirement already)"
# → Collecting google-generativeai==0.8.5 ... ✅
# → Collecting pylint==4.0.5 ...              ✅
```

***

## 7. Prohibited Actions

The agent is FORBIDDEN from:

- Running `mise use`, `mise install`, or `pip install` without explicit user approval.
- Modifying `requirements.txt` before presenting the full analysis to the USER.
- Modifying `mise.toml` (version pin) without explicit user approval, even if the user
  chose a newer tool version for installation.
- Trusting a `mise.toml` without first reading and presenting its full content.
- Invoking `python` or `pip` directly from PATH — always use `mise exec` with an
  explicit version and `--cd` to guarantee the correct environment.
- Running `mise use` without a project-scoped config target — always scope to the
  project directory, not globally.
- **Skipping post-edit file validation** — every edited file MUST be validated with its
  industrial-standard tool (§6) before proceeding to the next step.
- **Adding inline disable comments (e.g. `# pylint: disable=...`)** without asking the
  user first. The agent MUST present the error (e.g., `invalid-name` for a file name)
  and ask the user how they want to resolve it (e.g., rename the file vs disable the check).
