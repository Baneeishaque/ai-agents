#!/usr/bin/env python3
import os
import sys
from pathlib import Path

def normalize_pattern(pattern, root_dir, sub_repo_dir):
    """
    Adapts a pattern from the root context to the sub-repo context.
    Returns the adapted pattern or None if it's irrelevant.
    """
    pattern = pattern.strip()
    if not pattern or pattern.startswith('#'):
        return pattern

    is_negated = pattern.startswith('!')
    clean_pattern = pattern[1:] if is_negated else pattern
    
    # Handle absolute-like paths (rooted at project root)
    if clean_pattern.startswith('/'):
        # It's a path from root. Check if it falls within sub_repo_dir.
        # Construct full path/glob logic is complex, simpler heuristic:
        # If it starts with "/" + sub_repo_rel_path, strip it.
        # sub_repo_rel_path e.g. "ai-agent-rules"
        
        rel_sub = os.path.relpath(sub_repo_dir, root_dir)
        path_prefix = "/" + rel_sub
        
        if clean_pattern.startswith(path_prefix + "/"):
            # /ai-agent-rules/foo -> /foo
            new_pattern = clean_pattern[len(path_prefix):]
            return "!" + new_pattern if is_negated else new_pattern
        elif clean_pattern == path_prefix:
             # Ignoring the repo itself? Unlikely to be useful inside, but keep as is?
             return None
        else:
            # rooted path outside sub-repo: irrelevant
            return None
    else:
        # Relative pattern (e.g. "node_modules", "build/")
        # Applies recursively or locally. Keep as is.
        # Exceptions: if it refers to a specific file in root that doesn't exist here?
        # Standard .gitignore behavior is recursive unless contained slash?
        # Actually, simpler to just copy generic patterns.
        return pattern

def process_file(file_path, root_dir, sub_repo_dir, source_name):
    lines = []
    if not os.path.exists(file_path):
        return []
        
    lines.append(f"# --- Source: {source_name} ---")
    with open(file_path, 'r') as f:
        for line in f:
            adapted = normalize_pattern(line, root_dir, sub_repo_dir)
            if adapted is not None:
                lines.append(adapted)
    lines.append("")
    return lines

def main():
    if len(sys.argv) < 2:
        print("Usage: generate_ignores.py <sub_repo_relative_path>")
        sys.exit(1)
        
    sub_repo_rel = sys.argv[1]
    root_dir = os.getcwd()
    sub_repo_dir = os.path.join(root_dir, sub_repo_rel)
    
    if not os.path.isdir(sub_repo_dir):
        print(f"Error: {sub_repo_dir} is not a directory")
        sys.exit(1)

    outfile = os.path.join(sub_repo_dir, ".markdownlintignore")
    
    print(f"Generating {outfile}...")
    
    content = []
    content.append("# GENERATED FILE - DO NOT EDIT MANUALLY")
    content.append(f"# Generated at {os.popen('date -u').read().strip()}")
    content.append("")
    
    # 1. Parent .markdownlintignore
    content.extend(process_file(os.path.join(root_dir, ".markdownlintignore"), root_dir, sub_repo_dir, "Parent .markdownlintignore"))
    
    # 2. Parent .gitignore
    content.extend(process_file(os.path.join(root_dir, ".gitignore"), root_dir, sub_repo_dir, "Parent .gitignore"))
    
    # 3. Sub-repo .gitignore
    content.extend(process_file(os.path.join(sub_repo_dir, ".gitignore"), root_dir, sub_repo_dir, "Sub-repo .gitignore"))
    
    # 4. Nested .gitignores (simple scan)
    # Using find to locate nested .gitignores
    # We want paths relative to sub_repo_dir
    import subprocess
    try:
        # find . -name ".gitignore" (inside sub_repo)
        # exclude root .gitignore of sub-repo which we already processed
        cmd = ["find", ".", "-name", ".gitignore", "-not", "-path", "./.gitignore"]
        result = subprocess.run(cmd, cwd=sub_repo_dir, text=True, capture_output=True)
        nested_gitignores = result.stdout.strip().split('\n')
        
        for ng in nested_gitignores:
            if not ng: continue
            # ng is like ./architectures/sync/.gitignore
            # We need to prepend the distinct folder path to the patterns inside
            # e.g. pattern "node_modules" in nested .gitignore becomes "architectures/sync/**/node_modules" ?
            # Or simplified: if nested ignore has "foo", we ignore "path/to/nested/foo".
            
            # This is complex. Git handles this by context. markdownlintignore usually expects paths relative to root of execution.
            # If we run markdownlint at sub-repo root, we need to adjust nested patterns.
            
            ng_path = os.path.join(sub_repo_dir, ng)
            rel_dir = os.path.dirname(ng).lstrip('./')
            
            content.append(f"# --- Source: Nested {ng} ---")
            
            if os.path.exists(ng_path):
                with open(ng_path, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if not line or line.startswith('#'): continue
                        
                        is_neg = line.startswith('!')
                        p = line[1:] if is_neg else line
                        
                        # Prepend relative dir
                        if p.startswith('/'):
                            # Rooted at the nested dir
                            final_p = os.path.join(rel_dir, p[1:]) 
                        else:
                            # Recursive in nested dir? gitignore semantics are tricky.
                            # Usually "node_modules" means "node_modules" in that dir and deeper.
                            # We can approximate by prefixing.
                            final_p = os.path.join(rel_dir, "**", p) # Allow recursive match?
                            # Or just os.path.join(rel_dir, p) if we assume it's local.
                            # Standard gitignore: "foo" matches "foo" in same dir or deeper.
                            # So `path/to/nested/**/foo`
                            # But simply: `path/to/nested/foo` is safer for specific ignores.
                            final_p = os.path.join(rel_dir, p)
                            
                        content.append(("!" if is_neg else "") + final_p)
                content.append("")

    except Exception as e:
        print(f"Warning: parsing nested gitignores failed: {e}")

    with open(outfile, 'w') as f:
        f.write('\n'.join(content))
    
    print("Done.")

if __name__ == "__main__":
    main()
