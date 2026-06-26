cmake_minimum_required(VERSION 3.14)

project(aircraft_simulation_core VERSION 5.3.1.0 LANGUAGES CXX)

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

if(NOT TARGET mitre::cppuom)
   CPMAddPackage(
      NAME cppuom
      GIT_REPOSITORY https://github.com/mitre/cppuom.git
      GIT_TAG 2.2
      OPTIONS
         "BUILD_EXAMPLE OFF"
         "BUILD_TESTING OFF"
   )
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

if(NOT TARGET geolib)
   CPMAddPackage(
      NAME geolib_idealab
      GIT_REPOSITORY https://github.com/mitre/geodetic_library.git
      GIT_TAG v3.2.9
   )
endif()

if(NOT TARGET nlohmann_json::nlohmann_json)
   CPMAddPackage(
      NAME nlohmann_json
      GITHUB_REPOSITORY nlohmann/json
      VERSION 3.11.3
   )
endif()

CPMAddPackage(
   NAME minicsv
   GIT_REPOSITORY https://github.com/shaovoon/minicsv.git
   GIT_TAG v1.8.7
   DOWNLOAD_ONLY TRUE
)
if(NOT TARGET minicsv::minicsv)
   add_library(simcore_minicsv INTERFACE)
   add_library(minicsv::minicsv ALIAS simcore_minicsv)
   target_include_directories(simcore_minicsv INTERFACE
      $<BUILD_INTERFACE:${minicsv_SOURCE_DIR}>
   )
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

set_target_properties(simcore PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(simcore PUBLIC cxx_std_20)
target_include_directories(simcore
   PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${geolib_idealab_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
)
target_link_libraries(simcore
   PUBLIC
      mitre::cppuom
      geolib
      log4cplus::log4cplus
      minicsv::minicsv
      nlohmann_json::nlohmann_json
      "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.1>>:-lstdc++fs>"
)
if(SIMCORE_USE_STATIC_LOG4CPLUS)
   target_compile_definitions(simcore PUBLIC LOG4CPLUS_STATIC)
endif()

if(SIMCORE_BUILD_TESTING)
   enable_testing()
   add_subdirectory(tests)
endif()
