```markdown
# ai-agents Development Patterns

> Auto-generated skill from repository analysis

## Overview

This repository contains AI agent skills, workflows, and documentation following a structured approach to agent development. It includes standardized skill definitions, Git workflow documentation, LeetCode problem-solving patterns, and Azure DevOps integration guides. The codebase emphasizes iterative development, proper documentation, and maintainable workflows for AI agent systems.

## Coding Conventions

### File Naming
- Use `snake_case` for all file names
- Skill files: `.agents/skills/{skill-name}/SKILL.md`
- Workflow files: `.agent/workflows/{workflow-name}.md`
- Versioned files: `{name}_v{version}.md` (e.g., `engagement_plan_oss_v1.0_feature.md`)

### Import Style
- Mixed import patterns accepted
- Consistent within individual files

### Commit Messages
- Follow conventional commit format
- Use prefixes: `chore:`, `docs:`, `feat:`
- Keep messages around 58 characters average
- Example: `feat: add new skill for code analysis`

## Workflows

### Skill Creation
**Trigger:** When someone wants to add a new skill for AI agents to use
**Command:** `/new-skill`

1. Create new directory structure: `.agents/skills/{skill-name}/`
2. Generate `SKILL.md` with standardized format including:
   - Clear description
   - Version information
   - Step-by-step instructions
   - Prohibited behaviors section
3. Add skill entry to main `AGENTS.md` registry
4. Commit with descriptive message following conventional format

### Submodule Synchronization
**Trigger:** When ai-agent-rules repository is updated
**Command:** `/sync-rules`

1. Update submodule pointer to latest commit
2. Check if `.vscode/settings.json` dictionary needs updates
3. Test submodule integration
4. Commit changes with message: `chore: sync ai-agent-rules submodule`
5. Verify submodule is properly tracked

### VS Code Dictionary Management
**Trigger:** When new technical terms need to be added to prevent spell-check errors
**Command:** `/update-dictionary`

1. Open `.vscode/settings.json`
2. Add new terms to `cSpell.userWords` array
3. Sort terms alphabetically
4. Commit with message: `chore: update vscode dictionary`

### Git Workflow Documentation
**Trigger:** When documenting Git procedures and rebase workflows
**Command:** `/new-git-workflow`

1. Create workflow file: `.agent/workflows/{workflow-name}.md`
2. Include comprehensive step-by-step procedures
3. Add safety protocols and verification steps
4. Include troubleshooting section
5. Add examples of commands and expected outputs

### LeetCode Problem Iteration
**Trigger:** When working on LeetCode problem solutions with multiple iterations
**Command:** `/leetcode-iterate`

1. Create initial solution: `leetcode_{problem}_math_wizard_v3.md`
2. Iterate through versions incrementally (v4, v5, ...v34)
3. Each version should improve upon the previous
4. Create final consolidated version: `leetcode_math_wizard_final.md`
5. Document approach evolution and lessons learned

### Engagement Plan Evolution
**Trigger:** When evolving open source engagement strategies
**Command:** `/evolve-engagement`

1. Start with base version: `engagement_plan_oss_v1.0_base.md`
2. Create incremental versions (v1.1, v1.2, ...v1.9)
3. Add new capabilities and approaches in each iteration
4. Document what changed between versions
5. Include implementation strategies

### Azure DevOps Documentation
**Trigger:** When documenting Azure DevOps migration or pipeline consolidation
**Command:** `/azure-docs`

1. Create main documentation file: `Azure-{Topic}.md`
2. Create supporting directory: `Azure-{Topic}/{task,implementation_plan,walkthrough}.md`
3. Include detailed task breakdown
4. Add step-by-step implementation plan
5. Provide comprehensive walkthrough with screenshots/examples

## Testing Patterns

- Test files follow pattern: `*.test.*`
- Framework not specified - use appropriate testing framework for Python
- Include unit tests for core functionality
- Document test scenarios in skill definitions

## Commit Guidelines

### Message Format
```
type: brief description

Optional longer description explaining the change
```

### Common Types
- `feat:` - New features or skills
- `chore:` - Maintenance tasks, dependency updates
- `docs:` - Documentation updates
- `fix:` - Bug fixes

## Commands

| Command | Purpose |
|---------|---------|
| `/new-skill` | Create a new AI agent skill with standardized documentation |
| `/sync-rules` | Synchronize ai-agent-rules submodule with latest changes |
| `/update-dictionary` | Add technical terms to VS Code spell-check dictionary |
| `/new-git-workflow` | Document new Git procedures and workflows |
| `/leetcode-iterate` | Create iterative LeetCode solution development |
| `/evolve-engagement` | Refine open source engagement strategies |
| `/azure-docs` | Create Azure DevOps and CI/CD documentation |
```