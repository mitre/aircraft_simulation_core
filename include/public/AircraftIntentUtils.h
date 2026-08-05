#pragma once

#include <string>

#include "public/AircraftIntent.h"
#include "public/DefaultAircraftIntent.h"

namespace aaesim::open_source {

class AircraftIntentUtils final {
 public:
   static DefaultAircraftIntent CopyAndTrimAfterNamedWaypoint(const AircraftIntent &intent,
                                                              const std::string &waypoint_name);
};

}  // namespace aaesim::open_source
