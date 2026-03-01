#!/usr/bin/env python3
"""
Thin wrapper for skill synchronization using the Base Sync engine.
"""

import sys
import os

# Add current directory to path to import base_sync
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from base_sync import SyncManager


def main() -> None:
    """Main entry point for skill synchronization."""
    # Configure for ai-agents root: skills only, no rules.
    manager = SyncManager(
        rules_dir=None,
        skills_dir=".agent/skills",
        templates_dir="templates",
        readme_template="SKILLS.md.template",  # Single output for skills
        index_template=None,  # No index needed for root skills
        readme_output="docs/SKILLS.md",
        index_output=None,
    )

    # Overwrite sync method for custom skill-only behavior if needed
    # But manager.run() will call self.sync() which we can let handle it.
    manager.run()


if __name__ == "__main__":
    main()
