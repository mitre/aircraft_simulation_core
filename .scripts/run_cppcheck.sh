#!/usr/bin/env bash
set -euo pipefail

usage() {
   cat <<'EOF'
Usage: .scripts/run_cppcheck.sh [options]

Options:
  --profile reduced|exhaustive  reduced checks CI signal; exhaustive is the deeper local audit.
  --build-dir DIR               CMake build directory. Defaults to $BUILD_DIR or build.
  --jobs N                      Number of cppcheck jobs. Defaults to $CMAKE_BUILD_PARALLEL_LEVEL or 4.
  --generator NAME              Optional CMake generator name, for example Ninja.
  --xml FILE                    XML output path. Defaults to cppcheck.xml.
  --report-dir DIR              HTML report output directory. Defaults to cppcheck-output.
  -h, --help                    Show this help.
EOF
}

profile="reduced"
build_dir="${BUILD_DIR:-build}"
jobs="${CMAKE_BUILD_PARALLEL_LEVEL:-4}"
generator=""
xml_file="cppcheck.xml"
report_dir="cppcheck-output"

while [[ $# -gt 0 ]]; do
   case "$1" in
      --profile)
         profile="$2"
         shift 2
         ;;
      --build-dir)
         build_dir="$2"
         shift 2
         ;;
      --jobs)
         jobs="$2"
         shift 2
         ;;
      --generator)
         generator="$2"
         shift 2
         ;;
      --xml)
         xml_file="$2"
         shift 2
         ;;
      --report-dir)
         report_dir="$2"
         shift 2
         ;;
      -h|--help)
         usage
         exit 0
         ;;
      *)
         echo "Unknown option: $1" >&2
         usage >&2
         exit 2
         ;;
   esac
done

case "$profile" in
   reduced)
      check_level="reduced"
      enabled_checks="warning,performance,portability"
      ;;
   exhaustive)
      check_level="exhaustive"
      enabled_checks="all"
      ;;
   *)
      echo "Unknown cppcheck profile: $profile" >&2
      usage >&2
      exit 2
      ;;
esac

command -v cmake >/dev/null 2>&1 || { echo "cmake not installed." >&2; exit 1; }
command -v cppcheck >/dev/null 2>&1 || { echo "cppcheck not installed." >&2; exit 1; }
command -v uv >/dev/null 2>&1 || { echo "uv not installed." >&2; exit 1; }

if [[ "$check_level" == "reduced" && "$(cppcheck --help)" != *"reduced:"* ]]; then
   echo "Cppcheck does not support the reduced check level; using normal." >&2
   check_level="normal"
fi

cmake_args=(-S . -B "$build_dir" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
if [[ -n "$generator" ]]; then
   cmake_args=(-G "$generator" "${cmake_args[@]}")
fi

cmake "${cmake_args[@]}"

cppcheck_args=(
   cppcheck
   "-j${jobs}"
   --report-progress
   "--project=${build_dir}/compile_commands.json"
   "--cppcheck-build-dir=${build_dir}"
   "--check-level=${check_level}"
   "--enable=${enabled_checks}"
   --error-exitcode=1
   --std=c++20
   --language=c++
   --platform=unix64
   --xml
   --xml-version=2
   --inline-suppr
   '--suppress=*:unittest*'
   '--suppress=*:Loader/*'
   '--suppress=*:*/Loader/*'
   '--suppress=*:include/loader/*'
   '--suppress=*:*/include/loader/*'
   '--suppress=normalCheckLevelMaxBranches:*'
   '--suppress=missingIncludeSystem:*'
   -i unittest
   -i Loader
   -i include/loader
   -I include
)

cpm_source_cache="${CPM_SOURCE_CACHE:-${HOME}/.cpm}"
if [[ -n "$cpm_source_cache" ]]; then
   cppcheck_args+=("--suppress=*:${cpm_source_cache}*" -i "$cpm_source_cache")
fi

cppcheck_status=0
set +e
"${cppcheck_args[@]}" 2> "$xml_file"
cppcheck_status=$?
set -e

if [[ -s "$xml_file" ]]; then
   uv run cppcheck-htmlreport --file="$xml_file" --report-dir="$report_dir" --source-dir=. --title=AAESim
else
   echo "cppcheck XML output was not created or is empty: $xml_file" >&2
fi

exit "$cppcheck_status"
