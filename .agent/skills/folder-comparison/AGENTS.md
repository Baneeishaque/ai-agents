# Folder Comparison Agent

This agent is specialized in comparing two directories for data duplicity and integrity.

## Single Source of Truth

The core instructions and protocol for this agent are defined in the **Skill SSOT**:
[SKILL.md](./SKILL.md)

## Passive Context

- This agent prefers `rclone` for hash verification and `diff` for structural verification.
- It automatically excludes `.DS_Store` files.
- It provides literal, non-summarized reports of file deltas.
