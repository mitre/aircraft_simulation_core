#!/usr/bin/env python3
"""Archive a tested installation with compiler and commit metadata."""

import argparse
import json
from pathlib import Path
import platform
import subprocess
import tarfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--prefix', type=Path, required=True)
parser.add_argument('--tag', required=True)
parser.add_argument('--platform', required=True)
parser.add_argument('--output', type=Path, default=Path('dist'))
args = parser.parse_args()


def command(*argv):
    return subprocess.check_output(argv, text=True).strip()


compiler = 'gcc' if args.platform.startswith(('rockylinux', 'ubuntu')) else 'appleclang'
version_flag = '-dumpfullversion' if compiler == 'gcc' else '-dumpversion'

metadata = {
    'tag': args.tag,
    'commit': command('git', 'rev-parse', 'HEAD'),
    'platform': args.platform,
    'architecture': platform.machine(),
    'compiler': command('c++', '--version'),
    'compiler_version': command('c++', version_flag),
    'cmake': command('cmake', '--version'),
    'build_type': 'Release',
    'cxx_standard': 17,
}
(args.prefix / 'build-metadata.json').write_text(json.dumps(metadata, indent=2) + '\n')
args.output.mkdir(parents=True, exist_ok=True)
name = f'aircraft_simulation_core-{args.tag}-{args.platform}-{platform.machine()}-{compiler}-{metadata["compiler_version"]}-Release'
with tarfile.open(args.output / f'{name}.tar.gz', 'w:gz') as archive:
    archive.add(args.prefix, arcname=name)
