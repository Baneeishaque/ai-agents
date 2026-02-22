<!--
title: Azure DevOps to GitHub Actions Migration Agent
description: Operational playbook for migrating Gradle/Java pipelines from Azure DevOps to GitHub Actions.
category: DevOps & CI/CD
-->

# Azure DevOps to GitHub Actions Migration Agent (Gradle/Java)

This document defines the **Agent Persona and Playbook** for migrating Azure DevOps pipelines (specifically
Gradle/Java) to GitHub Actions. It serves as a complete instruction set for an AI agent to execute this specific
migration task with high precision.

***

## 1. Workflow Structure

- **Reference**: See `GitHub-Actions-rules.md` > **Workflow Structure and Configuration** for naming conventions,
    triggers, and permissions.
- **Key Actions**:
    - **Naming**: Rename generically named files (e.g., `ci.yml`) to descriptive names (e.g., `gradle-build.yml`).
    - **Permissions**: Ensure `contents: write` and `pull-requests: write` are set.

## 2. Runner & Environment

- **Reference**: See `GitHub-Actions-rules.md` > **Java & Gradle Workflow Specifics** > **Runner & Environment**.
- **Strategy**:
    - **Identify Version**: Extract JDK version from Azure pipeline (e.g., `JavaToolInstaller`).
    - **Dynamic Setup**: Use the **Conditional Java Setup** strategy defined in the rules. This ensures robustness
    and saves time by using pre-installed versions where possible.
    - **Scripts**: Use standalone Bash scripts for version checks as defined in the rules.

## 3. Gradle Configuration

- **Reference**: See `GitHub-Actions-rules.md` > **Java & Gradle Workflow Specifics** > **Gradle Configuration**.
- **Key Actions**:
    - **Action**: Use `gradle/actions/setup-gradle`.
    - **Caching**: Implement rigorous caching (cleanup, includes, encryption) as detailed in the rules.
    - **Reporting**: Enable Build Scans, Dependency Graphs, and PR Summaries.
    - **Artifacts**: Upload standard Gradle reports.

## 4. Verification & Secret Management

- **Reference**: See `GitHub-Actions-rules.md` > **Migration, Verification & Secrets**.
- **Key Actions**:
    - **Secrets**: Generate secrets non-interactively using `openssl` as shown in the rules.
    - **Monitoring**: Use `gh run list` and `gh run watch` to monitor the migration.
    - **Protocol**: Commit and push *before* verifying to ensure remote consistency.
