# Create `mise-setup-verification-action` Composite Action

I will create a new Git repository in the folder `mise-setup-verification-action`.

## Repository vs. Action Name
- **Repository Name**: `mise-setup-verification-action`
    - *Note*: Users will consume the action via `uses: Baneeishaque/mise-setup-verification-action@<version>`.
- **Action Name (UI/Marketplace)**: `mise-setup-verification`
    - Included in `action.yml` as `name: 'mise-setup-verification'`.

## Rule Compliance Checklist
- **Git-Operation-rules**: Manual "gh" command execution.
- **Git-Repository-rules**:
    - **Folder**: `[repo-path]/mise-setup-verification-action`
    - **License**: MIT (Banee Ishaque K).
- **GitHub-Actions-rules**: Strict input validation.

## Proposed Changes

### New Repository: `mise-setup-verification-action`

#### [NEW] [README.md](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/README.md)
Detailed documentation.
- **Title**: Mise Setup & Verification Action
- **Usage Example**:
```yaml
uses: Baneeishaque/mise-setup-verification-action@main
with:
  mise_version: '2025.12.9'
  working_directory: 'scripts'
  tool_name: 'python'
  version_command: 'python -V'
```

#### [NEW] [action.yml](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/action.yml)
- **Name**: `mise-setup-verification`
- **Inputs**:
  - `mise_version`: **REQUIRED**.
  - `working_directory`: **REQUIRED**.
  - `tool_name`: **REQUIRED**.
  - `version_command`: **REQUIRED**.
  - `install`: (Default: 'true')
  - `cache`: (Default: 'true')
  - `mise_file`: (Default: 'mise.toml')
- **Steps**:
  1. `jdx/mise-action`
  2. Run `src/verify.sh`

#### [NEW] [src/verify.sh](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/src/verify.sh)
- **Refactored**: Moved from `scripts/` to `src/` for better standard compliance.
- Strict argument parsing.

#### [NEW] [tests/mise.toml](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/tests/mise.toml)
- Test configuration file.

#### [NEW] [LICENSE](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/LICENSE)
MIT License (Copyright 2025 Banee Ishaque K).

#### [NEW] [.github/workflows/test.yml](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/.github/workflows/test.yml)
- **Purpose**: Enforce quality (linting) and functionality (self-test).
- **Jobs**:
    1.  **Lint**: Runs `shellcheck` (src) and `actionlint` (workflow).
    2.  **Test**: Runs the action locally (`uses: ./`) to verify success.

#### [NEW] [CONTRIBUTING.md](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/CONTRIBUTING.md)
Standard detailed contributing details.

#### [NEW] [SECURITY.md](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/SECURITY.md)
Standard security policy.

#### [NEW] [CODE_OF_CONDUCT.md](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/CODE_OF_CONDUCT.md)
Contributor Covenant Code of Conduct.

#### [NEW] [.gitignore](https://github.com/Baneeishaque/mise-setup-verification-action/blob/main/.gitignore)
- Generated via API: `windows,linux,macos,visualstudiocode,git`.

## Verification & Publishing Plan

### Automated Steps (Sequence)
1.  **Local Create**: Create folders/files in `mise-setup-verification-action`.
2.  **Git Init & Commit**: `git init -b main`, `git add .`, `git commit -m "feat: Initial repository setup"`.
3.  **GH Repo Create**: `gh repo create Baneeishaque/mise-setup-verification-action --public --source=. --remote=origin --push`.
4.  **GH Metadata**:
    ```bash
    gh repo edit Baneeishaque/mise-setup-verification-action \
      --description "Setup mice and verify tool versions in CI/CD pipelines. Ensures deterministic environments." \
      --add-topic "github-actions" \
      --add-topic "mise" \
      --add-topic "setup" \
      --add-topic "verification" \
      --add-topic "ci-cd" \
      --add-topic "devops" \
      --add-topic "automation" \
      --add-topic "tool-management" \
      --add-topic "workflow" \
      --add-topic "developer-tools"
    ```
5.  **Release & Publish**:
    ```bash
    gh release create v1.0.0 --title "v1.0.0 Initial Release" --notes "Initial release of mise-setup-verification action."
    ```

### Manual Verification
1.  Verify Repo URL: `https://github.com/Baneeishaque/mise-setup-verification-action`
2.  **Marketplace Publication**:
    -   **Research**: Search online/API for valid Marketplace categories (e.g., CI/CD, Utilities).
    -   Go to Release v1.0.0 on GitHub.
    - Click "Edit".
    - Check "Publish this Action to the GitHub Marketplace".
    - Accept Terms of Service if prompted.
    - Verify Action appears in Marketplace search.
