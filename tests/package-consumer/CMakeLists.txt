cmake_minimum_required(VERSION 3.26)
project(simcore_package_consumer LANGUAGES CXX)

find_package(aircraft_simulation_core CONFIG REQUIRED)
add_executable(package_consumer main.cpp)
target_link_libraries(package_consumer PRIVATE mitre::oss::simcore)
target_compile_definitions(package_consumer PRIVATE EXPECTED_VERSION="${EXPECTED_VERSION}")
enable_testing()
add_test(NAME installed_package_consumer COMMAND package_consumer)
