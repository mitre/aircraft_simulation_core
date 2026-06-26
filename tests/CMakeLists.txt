cmake_minimum_required(VERSION 3.14)

CPMAddPackage(
   NAME googletest
   GITHUB_REPOSITORY google/googletest
   VERSION 1.15.2
   OPTIONS
      "INSTALL_GTEST OFF"
      "GTEST_FORCE_SHARED_CRT ON"
      "BUILD_GMOCK ON"
)

set(SIMCORE_PUBLIC_TEST_SOURCE
   src/Public/geolib_tests.cpp
   src/Public/windstack_tests.cpp
   src/Public/utility_tests.cpp
   src/Public/public_tests.cpp
   src/Public/public_atmosphere_tests.cpp
   src/Public/tangent_plane_tests.cpp
   src/Public/wind_blending_tests.cpp
   src/Public/earth_model_tests.cpp
   src/Public/threedof_glider_tests.cpp
)

set(SIMCORE_PUBLIC_TEST_SUPPORT_SOURCE
   src/utils/public/OldCustomMathUtils.cpp
   src/utils/public/PublicUtils.cpp
)

add_executable(simcore_public_tests
   ${SIMCORE_PUBLIC_TEST_SOURCE}
   ${SIMCORE_PUBLIC_TEST_SUPPORT_SOURCE}
   src/main.cpp
)
target_link_libraries(simcore_public_tests
   PRIVATE
      GTest::gtest
      mitre::oss::simcore
)
target_include_directories(simcore_public_tests
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(simcore_public_tests PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests/bin
)

add_test(NAME simcore_public_tests COMMAND simcore_public_tests)
add_custom_target(run_simcore_tests
   ${PROJECT_BINARY_DIR}/tests/bin/simcore_public_tests --gtest_output=xml:simcore_public_test_results.xml
   DEPENDS simcore_public_tests
   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests
)
