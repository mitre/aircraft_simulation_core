#!/usr/bin/env python3
"""Validate CMake version bumps and manually created stable/RC release tags."""

import argparse
import os
from pathlib import Path
import re
import subprocess
import sys

NUMBER = r"(?:0|[1-9][0-9]*)"
VERSION = rf"{NUMBER}\.{NUMBER}\.{NUMBER}"
TAG = re.compile(rf"(?P<version>{VERSION})(?P<rc>-rc\.[1-9][0-9]*)?")


def cmake_version(text):
    # Ignore comments so a commented-out project declaration cannot satisfy the check.
    text = re.sub(r"#\[\[.*?\]\]", "", text, flags=re.S)
    text = re.sub(r"#[^\n]*", "", text)
    declarations = re.findall(
        r"\bproject\s*\(\s*aircraft_simulation_core\s+VERSION\s+([^\s)]+)", text, re.I
    )
    if len(declarations) != 1 or not re.fullmatch(VERSION, declarations[0]):
        raise ValueError("CMake project version must be one numeric major.minor.patch version")
    return declarations[0]


def require_bump(current, base):
    if tuple(map(int, current.split('.'))) <= tuple(map(int, base.split('.'))):
        raise ValueError(f"CMake version {current} must advance the target branch version {base}")


def validate_tag(tag, version):
    match = TAG.fullmatch(tag)
    if not match or match['version'] != version:
        raise ValueError(f"Tag must be {version} or {version}-rc.N (N is a positive integer)")
    return bool(match['rc'])


def git(*args):
    return subprocess.check_output(['git', *args], text=True).strip()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base', help='Target branch commit for an MR version comparison')
    parser.add_argument('--tag', help='Manual release tag to validate')
    parser.add_argument('--main-ref', default='origin/main')
    args = parser.parse_args()
    version = cmake_version(Path('CMakeLists.txt').read_text())
    if args.base:
        require_bump(version, cmake_version(git('show', f'{args.base}:CMakeLists.txt')))
    prerelease = False
    if args.tag:
        prerelease = validate_tag(args.tag, version)
        tagged_commit = git('rev-parse', f'refs/tags/{args.tag}^{{commit}}')
        if tagged_commit != git('rev-parse', 'HEAD'):
            raise ValueError('The checkout must be the tagged commit')
        if not prerelease:
            result = subprocess.run(['git', 'merge-base', '--is-ancestor', tagged_commit, args.main_ref])
            if result.returncode:
                raise ValueError('Stable release commit must be reachable from main')
    print(f'Validated version {version}' + (f', tag {args.tag}' if args.tag else ''))
    if output := os.environ.get('GITHUB_OUTPUT'):
        with open(output, 'a') as stream:
            stream.write(f'version={version}\nprerelease={str(prerelease).lower()}\n')


if __name__ == '__main__':
    try:
        main()
    except (ValueError, subprocess.CalledProcessError) as error:
        print(f'::error::{error}', file=sys.stderr)
        sys.exit(1)
