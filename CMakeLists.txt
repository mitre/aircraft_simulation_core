cmake_minimum_required(VERSION 3.15)

project(aircraft_simulation_core VERSION 1.0.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Debug")
endif()
message("${PROJECT_NAME}: Build type set to ${CMAKE_BUILD_TYPE}")

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
   set(SIMCORE_TOP_LEVEL ON)
else()
   set(SIMCORE_TOP_LEVEL OFF)
endif()
option(SIMCORE_BUILD_TESTING "Build aircraft_simulation_core tests" ${SIMCORE_TOP_LEVEL})

include(${PROJECT_SOURCE_DIR}/cmake/get_cpm.cmake)

set(SIMCORE_INSTALL_CPPUOM_COMPAT OFF)
if(NOT TARGET mitre::cppuom)
   CPMAddPackage(
      NAME cppuom
      GIT_REPOSITORY https://github.com/mitre/cppuom.git
      GIT_TAG 2.2
      OPTIONS
         "BUILD_EXAMPLE OFF"
         "BUILD_TESTING OFF"
   )
   set(SIMCORE_INSTALL_CPPUOM_COMPAT ON)
endif()

set(SIMCORE_USE_STATIC_LOG4CPLUS OFF)
if(NOT TARGET log4cplus::log4cplus)
   set(SIMCORE_USE_STATIC_LOG4CPLUS ON)
   CPMAddPackage(
      NAME log4cplus
      GITHUB_REPOSITORY log4cplus/log4cplus
      GIT_TAG REL_2_1_2
      OPTIONS
         "BUILD_SHARED_LIBS FALSE"
         "LOG4CPLUS_BUILD_TESTING FALSE"
         "LOG4CPLUS_BUILD_LOGGINGSERVER FALSE"
         "WITH_UNIT_TESTS FALSE"
   )
endif()

set(SIMCORE_INSTALL_GEOLIB_COMPAT OFF)
if(NOT TARGET geolib)
   CPMAddPackage(
      NAME geolib_idealab
      GIT_REPOSITORY https://github.com/mitre/geodetic_library.git
      GIT_TAG v3.2.9
   )
   set(SIMCORE_INSTALL_GEOLIB_COMPAT ON)
endif()
if(TARGET testGeolib)
   set_target_properties(testGeolib PROPERTIES EXCLUDE_FROM_ALL ON)
endif()
if(TARGET geolib)
   set_target_properties(geolib PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
   )
endif()

set(SIMCORE_INSTALL_NLOHMANN_JSON_COMPAT OFF)
if(NOT TARGET nlohmann_json::nlohmann_json)
   CPMAddPackage(
      NAME nlohmann_json
      GITHUB_REPOSITORY nlohmann/json
      VERSION 3.11.3
   )
   set(SIMCORE_INSTALL_NLOHMANN_JSON_COMPAT ON)
endif()

set(SIMCORE_INSTALL_MINICSV_COMPAT OFF)
if(NOT TARGET minicsv::minicsv)
   CPMAddPackage(
      NAME minicsv
      GIT_REPOSITORY https://github.com/shaovoon/minicsv.git
      GIT_TAG v1.8.7
      DOWNLOAD_ONLY TRUE
   )
   add_library(simcore_minicsv INTERFACE)
   add_library(minicsv::minicsv ALIAS simcore_minicsv)
   target_include_directories(simcore_minicsv INTERFACE
      $<BUILD_INTERFACE:${minicsv_SOURCE_DIR}>
   )
   set(SIMCORE_INSTALL_MINICSV_COMPAT ON)
endif()

if(PROJECT_VERSION_TWEAK)
   set(SIMCORE_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-SNAPSHOT)
else()
   set(SIMCORE_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
endif()
message("-- Generating build for ${PROJECT_NAME} version ${SIMCORE_VERSION}")
configure_file(
   ${PROJECT_SOURCE_DIR}/cmake/build_info.h.in
   ${PROJECT_BINARY_DIR}/include/aaesim/build_info.h
   @ONLY
)

set(SIMCORE_SOURCE_FILES
   src/ADSBSVReport.cpp
   src/AircraftCalculations.cpp
   src/AircraftControl.cpp
   src/AircraftIntent.cpp
   src/AircraftSpeed.cpp
   src/AircraftState.cpp
   src/AlongPathDistanceCalculator.cpp
   src/ArcOnEllipsoid.cpp
   src/Atmosphere.cpp
   src/BlendWindsVerticallyByAltitude.cpp
   src/CalcWindGradControl.cpp
   src/ClimbPhaseVerticalController.cpp
   src/ConfigurationFileReader.cpp
   src/ControlCommands.cpp
   src/CoreUtils.cpp
   src/CustomMath.cpp
   src/DMatrix.cpp
   src/DVector.cpp
   src/DataReader.cpp
   src/DefaultLateralController.cpp
   src/DirectionOfFlightCourseCalculator.cpp
   src/EarthModel.cpp
   src/EllipsoidalEarthModel.cpp
   src/Environment.cpp
   src/EuclideanTrajectoryPredictor.cpp
   src/EuclideanWaypointMonitor.cpp
   src/FlightEnvelopeSpeedLimiter.cpp
   src/FullWindTrueWeatherOperator.cpp
   src/GeolibUtils.cpp
   src/HorizontalPath.cpp
   src/HorizontalPathTracker.cpp
   src/HorizontalTurnPath.cpp
   src/InvalidIndexException.cpp
   src/KinematicDescent4DPredictor.cpp
   src/KinematicTrajectoryPredictor.cpp
   src/LatitudeLongitudePoint.cpp
   src/LegacyPositionEstimator.cpp
   src/LineOnEllipsoid.cpp
   src/LocalTangentPlane.cpp
   src/NullSpeedLimiter.cpp
   src/NullWindEvaluator.cpp
   src/PassThroughAssap.cpp
   src/PositionCalculator.cpp
   src/PrecalcConstraint.cpp
   src/PrecalcWaypoint.cpp
   src/RandomGenerator.cpp
   src/ScenarioUtils.cpp
   src/SingleTangentPlaneSequence.cpp
   src/SpeedOnPitchControl.cpp
   src/SpeedOnThrustControl.cpp
   src/StatisticalPilotDelay.cpp
   src/StereographicProjection.cpp
   src/TangentPlaneSequence.cpp
   src/ThreeDOFDynamics.cpp
   src/TvReader.cpp
   src/USStandardAtmosphere1976.cpp
   src/VectorDifferenceWindEvaluator.cpp
   src/VerticalPath.cpp
   src/VerticalPredictor.cpp
   src/Waypoint.cpp
   src/WeatherEstimate.cpp
   src/WeatherPrediction.cpp
   src/WeatherTruth.cpp
   src/Wgs84PrecalcWaypoint.cpp
   src/Wind.cpp
   src/WindStack.cpp
   src/WindZero.cpp
   src/ZeroWindTrueWeatherOperator.cpp
)

add_library(simcore STATIC ${SIMCORE_SOURCE_FILES})
add_library(mitre::oss::simcore ALIAS simcore)

set_target_properties(simcore PROPERTIES
   EXPORT_NAME simcore
   POSITION_INDEPENDENT_CODE ON
)
target_compile_features(simcore PUBLIC cxx_std_20)
target_include_directories(simcore
   PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
      "$<BUILD_INTERFACE:$<TARGET_PROPERTY:mitre::cppuom,INTERFACE_INCLUDE_DIRECTORIES>>"
      "$<BUILD_INTERFACE:$<TARGET_PROPERTY:minicsv::minicsv,INTERFACE_INCLUDE_DIRECTORIES>>"
      "$<BUILD_INTERFACE:$<TARGET_PROPERTY:nlohmann_json::nlohmann_json,INTERFACE_INCLUDE_DIRECTORIES>>"
      $<INSTALL_INTERFACE:include>
)
if(DEFINED geolib_idealab_SOURCE_DIR)
   target_include_directories(simcore
      PUBLIC
         $<BUILD_INTERFACE:${geolib_idealab_SOURCE_DIR}/include>
   )
endif()
target_link_libraries(simcore
   PUBLIC
      log4cplus::log4cplus
      $<BUILD_INTERFACE:$<TARGET_FILE:geolib>>
      "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.1>>:-lstdc++fs>"
)
if(SIMCORE_USE_STATIC_LOG4CPLUS)
   target_compile_definitions(simcore PUBLIC LOG4CPLUS_STATIC)
endif()

if(SIMCORE_BUILD_TESTING)
   enable_testing()
   add_subdirectory(tests)
endif()

install(
   TARGETS simcore
   EXPORT aircraft_simulation_core-targets
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
   DIRECTORY ${PROJECT_SOURCE_DIR}/include/
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   REGEX "/public/minicsv\\.h$" EXCLUDE
)

install(
   DIRECTORY ${PROJECT_BINARY_DIR}/include/
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(SIMCORE_INSTALL_CPPUOM_COMPAT)
   install(
      DIRECTORY ${cppuom_SOURCE_DIR}/include/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
endif()

if(SIMCORE_INSTALL_GEOLIB_COMPAT)
   install(
      TARGETS geolib
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   )
   install(
      DIRECTORY ${geolib_idealab_SOURCE_DIR}/include/geolib
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
endif()

if(SIMCORE_INSTALL_MINICSV_COMPAT)
   install(
      DIRECTORY ${minicsv_SOURCE_DIR}/MiniCSV/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MiniCSV
      FILES_MATCHING PATTERN "*.h"
   )
endif()

if(SIMCORE_INSTALL_NLOHMANN_JSON_COMPAT)
   install(
      DIRECTORY ${nlohmann_json_SOURCE_DIR}/include/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
endif()

install(
   EXPORT aircraft_simulation_core-targets
   FILE aircraft_simulation_core-targets.cmake
   NAMESPACE mitre::oss::
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aircraft_simulation_core
)

configure_package_config_file(
   ${PROJECT_SOURCE_DIR}/cmake/aircraft_simulation_core-config.cmake.in
   ${PROJECT_BINARY_DIR}/aircraft_simulation_core-config.cmake
   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aircraft_simulation_core
)

write_basic_package_version_file(
   ${PROJECT_BINARY_DIR}/aircraft_simulation_core-config-version.cmake
   VERSION ${PROJECT_VERSION}
   COMPATIBILITY SameMajorVersion
)

install(
   FILES
      ${PROJECT_BINARY_DIR}/aircraft_simulation_core-config.cmake
      ${PROJECT_BINARY_DIR}/aircraft_simulation_core-config-version.cmake
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aircraft_simulation_core
)
