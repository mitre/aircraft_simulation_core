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

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "public/Waypoint.h"

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

      virtual ~AircraftIntent() = default;

      virtual std::optional<Waypoint> GetWaypoint(unsigned int i) const = 0;
      virtual const std::vector<Waypoint> &GetWaypoints() const = 0;
      virtual std::optional<std::string> GetWaypointName(unsigned int i) const = 0;

      virtual Units::MetersLength GetPlannedCruiseAltitude() const = 0;
      virtual const RouteData &GetRouteData() const = 0;
      virtual std::optional<unsigned int> GetWaypointIndexByName(const std::string &waypoint_name) const = 0;
      virtual std::pair<int, int> FindCommonWaypoint(const AircraftIntent &intent) const = 0;
      virtual unsigned int GetNumberOfWaypoints() const = 0;
      virtual const std::vector<Waypoint> &GetAscentWaypoints() const = 0;
      virtual const std::vector<Waypoint> &GetCruiseWaypoints() const = 0;
      virtual const std::vector<Waypoint> &GetDescentWaypoints() const = 0;
      virtual double GetPlannedCruiseMach() const = 0;
      virtual bool ContainsWaypointName(const std::string &waypoint_name) const = 0;

   };

} // namespace aaesim::open_source

using AircraftIntent = aaesim::open_source::AircraftIntent;
