#!/usr/bin/env python3
"""Write a compilation database containing only this project's source files."""

import argparse
import json
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Filter a CMake compilation database to repository-owned source files."
    )
    parser.add_argument("--input", type=Path, required=True, help="Input compile_commands.json file.")
    parser.add_argument("--output", type=Path, required=True, help="Filtered compilation database path.")
    parser.add_argument("--source-dir", type=Path, required=True, help="Repository root directory.")
    parser.add_argument("--exclude-dir", type=Path, required=True, help="Build directory to exclude.")
    return parser.parse_args()


def is_within(path: Path, directory: Path) -> bool:
    try:
        path.relative_to(directory)
    except ValueError:
        return False
    return True


def main() -> None:
    args = parse_args()
    source_dir = args.source_dir.resolve()
    exclude_dir = args.exclude_dir.resolve()
    commands = json.loads(args.input.read_text(encoding="utf-8"))

    filtered_commands = []
    for command in commands:
        source_file = Path(command["file"])
        if not source_file.is_absolute():
            source_file = Path(command["directory"]) / source_file
        source_file = source_file.resolve()

        if is_within(source_file, source_dir) and not is_within(source_file, exclude_dir):
            filtered_commands.append(command)

    if not filtered_commands:
        raise SystemExit("No repository-owned translation units were found in the compilation database.")

    args.output.write_text(json.dumps(filtered_commands, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
