cmake_minimum_required(VERSION 3.26)
project(simcore_package_consumer LANGUAGES CXX)

find_package(aircraft_simulation_core CONFIG REQUIRED)
get_target_property(simcore_features mitre::oss::simcore INTERFACE_COMPILE_FEATURES)
if(NOT "cxx_std_17" IN_LIST simcore_features)
   message(FATAL_ERROR "The installed simcore target must export cxx_std_17")
endif()

add_executable(package_consumer main.cpp)
set_target_properties(package_consumer PROPERTIES
   CXX_STANDARD 17
   CXX_STANDARD_REQUIRED ON
   CXX_EXTENSIONS OFF
)
target_link_libraries(package_consumer PRIVATE mitre::oss::simcore)
target_compile_definitions(package_consumer PRIVATE EXPECTED_VERSION="${EXPECTED_VERSION}")
enable_testing()
add_test(NAME installed_package_consumer COMMAND package_consumer)
