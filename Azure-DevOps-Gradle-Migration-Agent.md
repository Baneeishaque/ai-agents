# Azure DevOps to GitHub Actions Migration Agent (Gradle/Java)

This document defines the **Agent Persona and Playbook** for migrating Azure DevOps pipelines (specifically Gradle/Java) to GitHub Actions. It serves as a complete instruction set for an AI agent to execute this specific migration task with high precision.

***

### 1. Workflow Structure
- **Naming**: Rename generically named files (e.g., `ci.yml`) to descriptive names like `gradle-build.yml`, reflecting the specific technology and purpose.
- **Permissions**:
    - `contents: write` (Required for dependency submission).
    - `pull-requests: write` (Required for PR comments).
- **Triggers**:
    - `push`: Trigger on main branches (e.g., `master`, `main`).
    - `workflow_dispatch`: Always include for manual runs.
    - **No Redundant PR Triggers**: Do NOT enable `pull_request` trigger if `push` configuration already covers PR branches/commits to avoid double execution, unless specific PR-only logic is needed.

### 2. Runner & Environment
- **Runner**: Use specific LTS versions (e.g., `ubuntu-24.04`) instead of `ubuntu-latest`.
- **Java Setup (Conditional)**:
    - **Identify Version**: Extract the required JDK version from the source Azure pipeline (e.g., `JavaToolInstaller` step).
    - **Research Runners**: Always verify specific GitHub runner capabilities (e.g., Ubuntu 24.04) online to identify pre-installed JDKs and default `JAVA_HOME`. Do NOT rely on static or historical information.
    - **Strategy**: Use dynamic checks for the *specific* identified version environment variable. This approach offers significant advantages by ensuring robustness against runner environment changes and pre-installed JDK path variations. It provides time profit by reducing manual maintenance (especially when Java is already pre-installed, avoiding unnecessary installation time), preventing pipeline failures due to incorrect `JAVA_HOME` settings, and accelerating debugging of Java setup issues.
    - **Scripting**: Use **standalone Bash scripts** (must use `.bash` extension) instead of inline YAML logic for complex checks. This improves readability and testability.
    - **Distribution**: Prefer `oracle` as the **primary choice**. Fall back to `temurin` ONLY if `oracle` does not support the required version. Always verify availability in `actions/setup-java` documentation.
    - **Example Logic** (e.g., if JDK 21 is required but not default):
    ```yaml
    - name: Ensure Java <VERSION>
      id: java-setup
      run: ./.github/scripts/ensure-java.bash
      env:
         REQUIRED_VERSION: '<VERSION>'
      shell: bash

    - name: Install Java <VERSION>
      if: steps.java-setup.outputs.skipped == 'false'
      uses: actions/setup-java@v4
      with:
        distribution: 'oracle' # First choice. Use 'temurin' only if Oracle misses the version.
        java-version: '<VERSION>'
        java-package: 'jdk'
    ```
    *Example `ensure-java.bash`:*
    ```bash
    #!/bin/bash
    # Verify variable name online first! (e.g., JAVA_HOME_21_X64)
    VAR_NAME="JAVA_HOME_${REQUIRED_VERSION}_X64"
    if [ -n "${!VAR_NAME}" ]; then
      echo "Java found at ${!VAR_NAME}. Setting JAVA_HOME."
      echo "JAVA_HOME=${!VAR_NAME}" >> $GITHUB_ENV
      echo "skipped=true" >> $GITHUB_OUTPUT
    else
      echo "Java ${REQUIRED_VERSION} not found. Will install."
      echo "skipped=false" >> $GITHUB_OUTPUT
    fi
    ```

### 3. Gradle Configuration
- **Action**: Use `gradle/actions/setup-gradle`.
- **Versioning**: Explicitly set `gradle-version: 'wrapper'`.
- **Caching**:
    - `cache-cleanup`: Set to `'on-success'`.
    - `gradle-home-cache-includes`: Explicitly list `'caches,notifications,wrapper'`.
    - **Configuration Cache**:
        - Enable in `gradle.properties`: `org.gradle.configuration-cache=true`.
        - **Encryption**: MUST use a GitHub Secret for `cache-encryption-key`.
        - Input: `cache-encryption-key: ${{ secrets.GRADLE_CACHE_ENCRYPTION_KEY }}`.
- **Reporting & Scans**:
    - Build Scans:
        - `build-scan-publish: true`
        - `build-scan-terms-of-use-url: 'https://gradle.com/terms-of-service'`
        - `build-scan-terms-of-use-agree: 'yes'`
    - Dependency Graph:
        - `dependency-graph: generate-and-submit`
        - **Failure Handling**: Use `continue-on-error: ${{ github.event_name == 'pull_request' }}` to prevent PR blocks on submission failure.
    - PR Summaries: `add-job-summary-as-pr-comment: 'always'`.
- **Artifacts**: Upload build reports (e.g., `**/build/reports/**`, `**/build/test-results/**`, `**/build/jacoco/**`).

### 4. Verification & Secret Management
- **Commit & Push Protocol**:
    - **Commit Message**: MUST follow the strict rules defined in `Git-Commit-Message-rules.md` (e.g., `ci(workflow): migrate Azure pipeline...`).
    - **Action**: Commit and push changes *before* running verification to ensure the remote state matches the workflow being tested.
- **Secret Creation (Non-Interactive)**:
    - Use `openssl` and `gh secret set` with redirection to avoid interactive prompts and history leaks.
    ```bash
    openssl rand -base64 16 > key.txt
    gh secret set GRADLE_CACHE_ENCRYPTION_KEY < key.txt
    rm key.txt
    ```
- **Automated Monitoring**:
    - Use `gh run list` with JSON output to dynamically grab the Run ID and watch execution.
    ```bash
    # Trigger
    gh workflow run <WORKFLOW_FILE>
    
    # Monitor
    RUN_ID=$(gh run list --workflow <WORKFLOW_FILE> -L 1 --json databaseId -q '.[0].databaseId')
    gh run watch $RUN_ID
    ```
