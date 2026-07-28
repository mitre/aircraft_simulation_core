.PHONY: debug reldebug release nuke help cppcheck cppcheck-exhaustive cmake-lock cmake-lock-check utest d rd r n h ut cpp cppe cl clc
BUILD_PATH ?= build
LOCK_BUILD_PATH ?= $(BUILD_PATH)-lock
CPPCHECK_JOBS ?= 4
# CPM package-lock output can differ between cold and warm source caches.
# Configure twice so cl/clc use the stable, warmed-cache lock contents.
LOCK_CMAKE_ARGS = $(CMAKE_GENERATOR) -S . -B $(LOCK_BUILD_PATH) -DCMAKE_BUILD_TYPE=RelWithDebInfo -Dsimcore_REGENERATE_CPM_PACKAGE_LOCK=ON

# Determine CMake generator based on availability of build backend
ifneq ($(shell command -v fbuild >/dev/null 2>&1 && echo yes),)
CMAKE_GENERATOR = -G FASTBuild
else ifneq ($(shell command -v ninja >/dev/null 2>&1 && echo yes),)
CMAKE_GENERATOR = -G Ninja
else
CMAKE_GENERATOR =
endif

debug: 
	@echo "Building in debug mode..."
	cmake $(CMAKE_GENERATOR) -S . -B $(BUILD_PATH) -DCMAKE_BUILD_TYPE=Debug -DSIMCORE_BUILD_TESTING=ON
	cmake --build $(BUILD_PATH) --target clean
	cmake --build $(BUILD_PATH) --target simcore -j

reldebug: 
	@echo "Building in release with debug info mode..."
	cmake $(CMAKE_GENERATOR) -S . -B $(BUILD_PATH) -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSIMCORE_BUILD_TESTING=ON
	cmake --build $(BUILD_PATH) --target clean
	cmake --build $(BUILD_PATH) --target simcore -j

release:
	@echo "Building in release mode..."
	cmake $(CMAKE_GENERATOR) -S . -B $(BUILD_PATH) -DCMAKE_BUILD_TYPE=Release -DSIMCORE_BUILD_TESTING=ON
	cmake --build $(BUILD_PATH) --target clean
	cmake --build $(BUILD_PATH) --target simcore -j

utest:
	@echo "Running unit tests..."
	cmake $(CMAKE_GENERATOR) -S . -B $(BUILD_PATH) -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSIMCORE_BUILD_TESTING=ON
	cmake --build $(BUILD_PATH) --target run_simcore_tests -j

cppcheck: nuke release
	@echo "Running cppcheck static analysis..."
	bash .scripts/run_cppcheck.sh --profile reduced --build-dir $(BUILD_PATH) --jobs $(CPPCHECK_JOBS)

cppcheck-exhaustive: nuke release
	@echo "Running exhaustive cppcheck static analysis..."
	bash .scripts/run_cppcheck.sh --profile exhaustive --build-dir $(BUILD_PATH) --jobs $(CPPCHECK_JOBS)

nuke:
	@echo "Removing build directory..."
	rm -rf $(BUILD_PATH)

help:
	@echo "Type less; dev more:"
	@echo "  debug (d)                  : Build in debug mode"
	@echo "  reldebug (rd)              : Build in release with debug info mode"
	@echo "  release (r)                : Build in release mode"
	@echo "  utest (ut)                 : Run unit tests"
	@echo "  cppcheck (cpp)             : Run rapid cppcheck audit"
	@echo "  cppcheck-exhaustive (cppe) : Run deep cppcheck audit"
	@echo "  nuke (n)                   : Delete the build directory"
	@echo "  help (h)                   : Show this help message"

d: debug
rd: reldebug
r: release
n: nuke
h: help
ut: utest
cpp: cppcheck
cppe: cppcheck-exhaustive
