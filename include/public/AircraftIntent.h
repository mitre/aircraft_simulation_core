// ****************************************************************************
// NOTICE
//
// This work was produced for the U.S. Government under Contract 693KA8-22-C-00001
// and is subject to Federal Aviation Administration Acquisition Management System
// Clause 3.5-13, Rights In Data-General, Alt. III and Alt. IV (Oct. 1996).
//
// The contents of this document reflect the views of the author and The MITRE
// Corporation and do not necessarily reflect the views of the Federal Aviation
// Administration (FAA) or the Department of Transportation (DOT). Neither the FAA
// nor the DOT makes any warranty or guarantee, expressed or implied, concerning
// the content or accuracy of these views.
//
// For further information, please contact The MITRE Corporation, Contracts Management
// Office, 7515 Colshire Drive, McLean, VA 22102-7539, (703) 983-6000.
//
// (c) 2026 The MITRE Corporation. All Rights Reserved.
// ****************************************************************************

#pragma once

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <scalar/Speed.h>

#include <algorithm>
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"
#include "public/TangentPlaneSequence.h"
#include "public/Waypoint.h"
#include "utility/BoundedValue.h"
#include "utility/UtilityConstants.h"

namespace aaesim::open_source
{

   struct AircraftIntent
   {
      enum WaypointPhaseOfFlight
      {
         ASCENT,
         CRUISE,
         DESCENT
      };

      enum Arinc424LegType
      {
         UNSET = -1,
         IF,
         VI,
         TF,
         RF,
         CF,
         VA,
         CA
      };

      struct RouteData final
      {
         std::vector<std::string> m_name{};
         std::vector<WaypointPhaseOfFlight> m_waypoint_phase_of_flight{};
         std::vector<Units::MetersLength> m_x{};
         std::vector<Units::MetersLength> m_y{};
         std::vector<Units::MetersLength> m_z{};
         std::vector<Units::MetersLength> m_nominal_altitude{};
         std::vector<Units::RadiansAngle> m_latitude{};
         std::vector<Units::RadiansAngle> m_longitude{};
         std::vector<Units::FeetPerSecondSpeed> m_nominal_ias{};
         std::vector<Units::MetersLength> m_high_altitude_constraint{};
         std::vector<Units::MetersLength> m_low_altitude_constraint{};
         std::vector<Units::MetersPerSecondSpeed> m_high_speed_constraint{};
         std::vector<Units::MetersPerSecondSpeed> m_low_speed_constraint{};
         std::vector<Arinc424LegType> m_leg_type{};
         std::vector<Units::MetersLength> m_x_rf_center{};
         std::vector<Units::MetersLength> m_y_rf_center{};
         std::vector<Units::MetersLength> m_rf_radius{};
         std::vector<Units::RadiansAngle> m_rf_latitude{};
         std::vector<Units::RadiansAngle> m_rf_longitude{};
      };

      AircraftIntent() = default;

      virtual ~AircraftIntent() = default;

      virtual void LoadWaypointsFromList(const std::list<Waypoint> &ascent_waypoints,
                                         const std::list<Waypoint> &cruise_waypoints,
                                         const std::list<Waypoint> &descent_waypoints) = 0; // FIXME Stuart should not be public...remove list usage

      virtual void UpdateXYZFromLatLonWgs84() = 0; // FIXME Stuart should not be public, refactor name...only needed by FIM?

      virtual void UpdateWaypoint(const Waypoint &waypoint) = 0; // FIXME Stuart is this used? Replace any uses with builder pattern?

      virtual std::list<Waypoint> GetWaypointList() const = 0; // FIXME Stuart should return vector only, refactor to GetWaypoints

      virtual void GetLatLonFromXYZ(const Units::Length &xMeters, const Units::Length &yMeters, const Units::Length &zMeters,
                                    Units::Angle &lat, Units::Angle &lon) const = 0; // FIXME Stuart should not exist

      virtual void SetNumberOfWaypoints(unsigned int n) = 0; // FIXME Stuart replace any uses with builder pattern, remove this method

      virtual const Waypoint &GetWaypoint(unsigned int i) const = 0;

      virtual const std::string &GetWaypointName(unsigned int i) const = 0; // FIXME Stuart remove this method

      virtual Units::MetersLength GetWaypointX(unsigned int i) const = 0; // FIXME Stuart remove this method

      virtual Units::MetersLength GetWaypointY(unsigned int i) const = 0; // FIXME Stuart remove this method

      virtual Units::MetersLength GetPlannedCruiseAltitude() const = 0;

      virtual void SetPlannedCruiseAltitude(Units::Length altitude) = 0; // FIXME Stuart replace any uses with builder pattern, remove this method

      virtual const RouteData &GetRouteData() const = 0;

      virtual int GetWaypointIndexByName(const std::string &waypoint_name) const = 0;

      virtual std::pair<int, int> FindCommonWaypoint(const AircraftIntent &intent) const = 0; // FIXME Stuart rethink this design

      virtual void InsertPairAtIndex(const std::string &wpname, const Units::Length &x, const Units::Length &y, const int index) = 0; // FIXME Stuart remove this method

      virtual void InsertWaypointAtIndex(const Waypoint &waypoint, const int index) = 0; // FIXME Stuart remove this method

      virtual void ClearWaypoints() = 0; // FIXME Stuart replace any uses with builder pattern, remove this method

      virtual unsigned int GetNumberOfWaypoints() const = 0;

      virtual bool IsLoaded() const = 0; // FIXME Stuart remove this method

      virtual bool ContainsAscentWaypoints() const = 0;

      virtual const std::vector<Waypoint> &GetAscentWaypoints() const = 0;

      virtual bool ContainsCruiseWaypoints() const = 0;

      virtual const std::vector<Waypoint> &GetCruiseWaypoints() const = 0;

      virtual bool ContainsDescentWaypoints() const = 0;

      virtual const std::vector<Waypoint> &GetDescentWaypoints() const = 0;

      virtual double GetPlannedCruiseMach() const = 0;

      virtual void SetPlannedCruiseMach(BoundedValue<double, 0, 1> mach_number) = 0; // FIXME Stuart replace any uses with builder pattern, remove this method

      virtual bool ContainsWaypointName(const std::string &waypoint_name) const = 0;

   };

} // namespace aaesim::open_source