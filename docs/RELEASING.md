# Releases

Developers create release tags manually. Merging an MR never creates a tag or publishes a release.

## Merge requirements

Every MR into `main` must advance the numeric `project(... VERSION ...)` in `CMakeLists.txt`.
Use three components (`major.minor.patch`), even for documentation and CI changes.
Choose the major/minor/patch increase according to the change; CI enforces an increase, not its semantic meaning.
A fourth snapshot component or RC suffix is not accepted in the CMake project version.

The `CI required` check combines the version policy, Cppcheck, and build/package validation.
Builds run in Rocky Linux 8 with GCC 8.5.0 (a RHEL 8 compatibility target), Ubuntu 22.04 with
GCC 11.4.0, and macOS 15 with AppleClang. Each build runs the public tests, installs the package,
relocates it, and builds/runs an independent `find_package(aircraft_simulation_core)` consumer.
The compiler versions are checked explicitly so an image update cannot silently change coverage.

Two concurrent MRs may initially propose the same version. After the first merges, update the
second branch and choose a larger version before merging it.

## Manual stable and candidate tags

With CMake version `2.0.1`, accepted tags are:

- `2.0.1`: stable release; the tagged commit must be reachable from `main`.
- `2.0.1-rc.1`, `2.0.1-rc.2`, etc.: GitHub prereleases; the tagged commit may be on a development branch.

No `v` prefix is used. RC numbers must be positive integers without leading zeros.
Push each release tag individually; GitHub does not emit tag push events when more than three tags
are pushed together. The tagged commit must contain the release workflow.

For example, after committing the version change and code on the candidate branch:

```sh
git tag -a 2.0.1-rc.1 -m 'Release candidate 2.0.1-rc.1'
git push origin 2.0.1-rc.1
```

After the candidate is accepted and its commit is included in `main`:

```sh
git tag -a 2.0.1 <accepted-commit> -m 'Release 2.0.1'
git push origin 2.0.1
```

The stable tag can identify the exact candidate commit if that commit is reachable from `main`.
A squash merge creates a new commit: tag the resulting `main` commit in that case.
RC tags and stable tags are always built and tested independently.

## Published packages

A valid tag triggers the Release workflow. All three builds and installed consumer tests must
pass before the publication job runs. It creates a draft, uploads every asset, then publishes
it as either a stable release or a prerelease. RCs are never marked as the latest stable release.

Assets include:

- A source `.tar.gz`, the primary distribution. Dependency sources are fetched by CPM when building.
- Installed `.tar.gz` packages identifying the target environment, architecture, compiler version,
  and Release configuration. They contain the library, installed dependencies and headers,
  CMake package configuration, license, and `build-metadata.json`.
- `SHA256SUMS` covering every archive.

The release tag becomes `AAESIM_VERSION_STR` in binary packages, including the RC suffix.
CMake's package version remains numeric so `find_package` retains normal version matching.
When building the source archive, preserve that identity with:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSIMCORE_RELEASE_TAG=2.0.1-rc.1
```

Consumers can also pin CPM's `GIT_TAG` to a candidate or stable tag. For an RC build identity,
pass `"SIMCORE_RELEASE_TAG 2.0.1-rc.1"` in CPM's `OPTIONS`.
Binary packages are compiler/platform specific; use the source package when your environment differs.

Release notes are generated from merged MRs. `.github/release.yml` groups `breaking-change`,
`feature`/`enhancement`, `fix`/`bug`, and `documentation` labels, with remaining changes under Other changes.
Branch-only RC changes may have no merged MR in generated notes; maintainers can edit their release notes.

If publication fails, rerun the workflow. Existing drafts may have their assets replaced before publication.
Published releases are never overwritten by the workflow. Fixes require a new version or RC tag.
Do not delete or move published release tags.

## GitHub repository settings

Workflow files define checks, but GitHub repository settings enforce merging and release immutability.
Configure these separately:

1. Protect `main`: require an MR, require `CI required`, and require branches to be up to date before merging.
   Avoid bypass permissions that allow normal contributors to skip these rules.
   The CI workflow also supports `merge_group` events if a merge queue is enabled; set the maximum
   merge group size to one MR so competing version bumps are checked individually.
2. Protect version tags against updates/deletion while allowing maintainers to create tags manually.
3. Enable immutable releases. The draft/upload/publish sequence supports this setting.
4. Allow the Release publication job's `GITHUB_TOKEN` to write repository contents.
   Other jobs use read-only repository permissions.

No personal access token or automatic tagging is required.
