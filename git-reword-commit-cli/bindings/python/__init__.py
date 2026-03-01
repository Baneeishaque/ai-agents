"""
git_reword - Python bindings for git-reword-commit-cli

A cross-platform library for non-interactive git commit rewording.
"""

try:
    from .git_reword import *
except ImportError:
    # Native module not built, provide fallback
    pass

__version__ = "1.0.0"
__author__ = "Baneeishaque K"
