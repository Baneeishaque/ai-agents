# Fully Consolidated Azure Pipelines

I have further consolidated the Azure Pipelines by moving all remaining boilerplate (triggers, PR settings, pool, and variables) into a shared main template.

## Changes Made
- **[NEW] azure-pipelines-main-template.yml**: This template now serves as the single source of truth for the job-level configuration, including the pool and variables.
- **[NEW] azure-pipelines-steps-template.yml**: Contains the actual build steps (API injection, Gradle build, etc.).
- **[MODIFY] azure-pipelines-ubuntu.yml**: Minimal file that defines its own triggers and extends the main template with JDK 21.
- **[MODIFY] azure-pipelines-ubuntu-jdk17.yml**: Minimal file that defines its own triggers and extends the main template with JDK 17 on Ubuntu.

## Structural Requirements
- **Triggers**: Must remain in the root entry pipeline file.
- **Pool/Variables**: Moved into the job definition within the template to avoid "Unexpected value" errors during extension.

## Benefits
- **DRY (Don't Repeat Yourself)**: Shared logic reduces maintenance effort.
- **Consistency**: Guaranteed identical environments and steps for different JDK versions.
- **Readability**: Primary pipeline files are now extremely clean.
