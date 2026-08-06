#include "public/AircraftIntentUtils.h"

#include <algorithm>

namespace mitre::oss::simcore {

DefaultAircraftIntent AircraftIntentUtils::CopyAndTrimAfterNamedWaypoint(const AircraftIntent &intent,
                                                                          const std::string &waypoint_name) {
   auto copy = *DefaultAircraftIntent::Builder(intent).Build();
   const auto &waypoints = intent.GetWaypoints();
   if (!intent.ContainsWaypointName(waypoint_name) || waypoints.empty() || waypoints.back().GetName() == waypoint_name) {
      return copy;
   }
   const auto matches = [&waypoint_name](const Waypoint &waypoint) { return waypoint.GetName() == waypoint_name; };
   const auto trim = [&matches](const std::vector<Waypoint> &waypoints) {
      std::vector<Waypoint> result;
      for (const auto &waypoint : waypoints) {
         result.push_back(waypoint);
         if (matches(waypoint)) break;
      }
      return result;
   };
   auto ascent = intent.GetAscentWaypoints();
   auto cruise = intent.GetCruiseWaypoints();
   auto descent = intent.GetDescentWaypoints();
   if (std::any_of(ascent.rbegin(), ascent.rend(), matches)) {
      ascent = trim(ascent);
      cruise.clear();
      descent.clear();
   } else if (std::any_of(cruise.rbegin(), cruise.rend(), matches)) {
      cruise = trim(cruise);
      descent.clear();
   } else {
      descent = trim(descent);
   }
   return *DefaultAircraftIntent::Builder().SetAscentWaypoints(ascent).SetCruiseWaypoints(cruise)
                  .SetDescentWaypoints(descent).SetPlannedCruiseMach(
                        BoundedValue<double, 0, 1>(intent.GetPlannedCruiseMach()))
                  .SetPlannedCruiseAltitude(intent.GetPlannedCruiseAltitude()).Build();
}

}  // namespace mitre::oss::simcore
