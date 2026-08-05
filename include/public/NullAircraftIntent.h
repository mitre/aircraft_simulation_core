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

#include "public/AircraftIntent.h"

namespace aaesim::open_source {

struct NullAircraftIntent final : AircraftIntent {
   std::optional<Waypoint> GetWaypoint(unsigned int) const override { return std::nullopt; }
   const std::vector<Waypoint> &GetWaypoints() const override { return EMPTY_WAYPOINTS; }
   std::optional<std::string> GetWaypointName(unsigned int) const override { return std::nullopt; }
   Units::MetersLength GetPlannedCruiseAltitude() const override { return Units::MetersLength{}; }
   const RouteData &GetRouteData() const override { return EMPTY_ROUTE_DATA; }
   std::optional<unsigned int> GetWaypointIndexByName(const std::string &) const override { return std::nullopt; }
   std::pair<int, int> FindCommonWaypoint(const AircraftIntent &) const override { return {-1, -1}; }
   unsigned int GetNumberOfWaypoints() const override { return 0; }
   const std::vector<Waypoint> &GetAscentWaypoints() const override { return EMPTY_WAYPOINTS; }
   const std::vector<Waypoint> &GetCruiseWaypoints() const override { return EMPTY_WAYPOINTS; }
   const std::vector<Waypoint> &GetDescentWaypoints() const override { return EMPTY_WAYPOINTS; }
   double GetPlannedCruiseMach() const override { return 0.0; }
   bool ContainsWaypointName(const std::string &) const override { return false; }

 private:
   inline static const std::vector<Waypoint> EMPTY_WAYPOINTS{};
   inline static const RouteData EMPTY_ROUTE_DATA{};
};

}  // namespace aaesim::open_source

using NullAircraftIntent = aaesim::open_source::NullAircraftIntent;
