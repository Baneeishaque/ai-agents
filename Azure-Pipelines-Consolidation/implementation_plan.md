# Pipeline Consolidation Plan

Consolidate Azure Pipelines to reduce redundancy by using a shared steps template. This will ensure that the JDK 17
pipeline remains in sync with the primary (Ubuntu) pipeline, with the only difference being the JDK version.

## Proposed Changes

### CI/CD

#### [NEW] azure-pipelines-main-template.yml

- Define `pool` and `variables` at the job level.
- Define a `jobs` section that takes `jdkVersion` as a parameter and calls the steps template.

#### [MODIFY] azure-pipelines-ubuntu.yml

- Include `trigger` and `pr` at the root level.
- Use `extends` to point to the main template.

#### [MODIFY] azure-pipelines-ubuntu-jdk17.yml

- Include `trigger` and `pr` at the root level.
- Use `extends` to point to the main template.

## Verification Plan

### Automated Tests

- Performed a check of the YAML syntax.
- Verified that both files successfully reference the new template file.

### Manual Verification

- Monitor the next pipeline runs in Azure DevOps to ensure they trigger and execute as expected.
