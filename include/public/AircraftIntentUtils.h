#pragma once

#include <string>

#include "public/AircraftIntent.h"
#include "public/DefaultAircraftIntent.h"

namespace mitre::oss::simcore {

class AircraftIntentUtils final {
 public:
   static DefaultAircraftIntent CopyAndTrimAfterNamedWaypoint(const AircraftIntent &intent,
                                                              const std::string &waypoint_name);
};

}  // namespace mitre::oss::simcore
