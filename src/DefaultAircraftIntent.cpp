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

#include "public/DefaultAircraftIntent.h"

#include <algorithm>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "public/CoreUtils.h"
#include "public/InvalidIndexException.h"
#include "public/SingleTangentPlaneSequence.h"

using namespace aaesim::open_source;

DefaultAircraftIntent::Builder::Builder(const AircraftIntent &aircraft_intent) {
   if (aircraft_intent.ContainsAscentWaypoints()) {
      ascent_waypoints_ = aircraft_intent.GetAscentWaypoints();
   }
   if (aircraft_intent.ContainsCruiseWaypoints()) {
      cruise_waypoints_ = aircraft_intent.GetCruiseWaypoints();
   }
   if (aircraft_intent.ContainsDescentWaypoints()) {
      descent_waypoints_ = aircraft_intent.GetDescentWaypoints();
   }
   planned_cruise_mach_ = aircraft_intent.GetPlannedCruiseMach();
   planned_cruise_altitude_ = aircraft_intent.GetPlannedCruiseAltitude();
}

DefaultAircraftIntent::Builder &DefaultAircraftIntent::Builder::SetAscentWaypoints(const std::vector<Waypoint> &ascent_waypoints) {
   ascent_waypoints_ = ascent_waypoints;
   return *this;
}

DefaultAircraftIntent::Builder &DefaultAircraftIntent::Builder::SetCruiseWaypoints(const std::vector<Waypoint> &cruise_waypoints) {
   cruise_waypoints_ = cruise_waypoints;
   return *this;
}

DefaultAircraftIntent::Builder &DefaultAircraftIntent::Builder::SetDescentWaypoints(const std::vector<Waypoint> &descent_waypoints) {
   descent_waypoints_ = descent_waypoints;
   return *this;
}

DefaultAircraftIntent::Builder &DefaultAircraftIntent::Builder::SetPlannedCruiseMach(BoundedValue<double, 0, 1> planned_cruise_mach) {
   planned_cruise_mach_ = planned_cruise_mach;
   return *this;
}

DefaultAircraftIntent::Builder &DefaultAircraftIntent::Builder::SetPlannedCruiseAltitude(Units::Length planned_cruise_altitude) {
   planned_cruise_altitude_ = planned_cruise_altitude;
   return *this;
}

std::shared_ptr<DefaultAircraftIntent> DefaultAircraftIntent::Builder::Build() const {
   std::shared_ptr<DefaultAircraftIntent> aircraft_intent = std::make_shared<DefaultAircraftIntent>();
   aircraft_intent->SetPlannedCruiseMach(planned_cruise_mach_);
   aircraft_intent->SetPlannedCruiseAltitude(planned_cruise_altitude_);
   aircraft_intent->LoadWaypointsFromList(ConvertVectorToList(ascent_waypoints_), ConvertVectorToList(cruise_waypoints_),
                                          ConvertVectorToList(descent_waypoints_));
   return aircraft_intent;
}

DefaultAircraftIntent::DefaultAircraftIntent(const DefaultAircraftIntent &in) {
   Copy(in);
}

DefaultAircraftIntent &DefaultAircraftIntent::operator=(const DefaultAircraftIntent &in) {
   Copy(in);
   return *this;
}

void DefaultAircraftIntent::DeleteRouteDataContent() {
   route_data_.m_name.clear();
   route_data_.m_x.clear();
   route_data_.m_y.clear();
   route_data_.m_z.clear();
   route_data_.m_nominal_altitude.clear();
   route_data_.m_latitude.clear();
   route_data_.m_longitude.clear();
   route_data_.m_nominal_ias.clear();
   route_data_.m_waypoint_phase_of_flight.clear();
   route_data_.m_leg_type.clear();
   route_data_.m_high_altitude_constraint.clear();
   route_data_.m_low_altitude_constraint.clear();
   route_data_.m_high_speed_constraint.clear();
   route_data_.m_low_speed_constraint.clear();
   route_data_.m_rf_latitude.clear();
   route_data_.m_rf_longitude.clear();
   route_data_.m_rf_radius.clear();
   route_data_.m_rf_latitude.clear();
   route_data_.m_rf_longitude.clear();
   route_data_.m_y_rf_center.clear();
   route_data_.m_x_rf_center.clear();
}

void DefaultAircraftIntent::ClearAndResetRouteDataContent(const std::vector<Waypoint> &ascent_waypoints,
                                                   const std::vector<Waypoint> &cruise_waypoints,
                                                   const std::vector<Waypoint> &descent_waypoints) {
   m_ascent_waypoints = ascent_waypoints;
   m_cruise_waypoints = cruise_waypoints;
   m_descent_waypoints = descent_waypoints;
   m_ordered_waypoints.clear();

   DeleteRouteDataContent();
   AddWaypointsToRouteDataVectors(m_ascent_waypoints, ASCENT);
   AddWaypointsToRouteDataVectors(m_cruise_waypoints, CRUISE);

   auto vector_current_index = m_ordered_waypoints.size();
   auto waypoint_itr = m_descent_waypoints.begin();
   while (waypoint_itr != m_descent_waypoints.end()) {
      m_ordered_waypoints.push_back(*waypoint_itr);
      route_data_.m_name.push_back(waypoint_itr->GetName());
      route_data_.m_waypoint_phase_of_flight.push_back(DESCENT);
      route_data_.m_nominal_altitude.emplace_back(waypoint_itr->GetAltitude());
      route_data_.m_latitude.emplace_back(waypoint_itr->GetLatitude());
      route_data_.m_longitude.emplace_back(waypoint_itr->GetLongitude());
      route_data_.m_nominal_ias.emplace_back(waypoint_itr->GetNominalIas());
      route_data_.m_leg_type.push_back(m_arinc424_dictionary[waypoint_itr->GetArinc424LegType()]);
      route_data_.m_high_altitude_constraint.emplace_back(waypoint_itr->GetAltitudeConstraintHigh());
      route_data_.m_low_altitude_constraint.emplace_back(waypoint_itr->GetAltitudeConstraintLow());
      route_data_.m_high_speed_constraint.emplace_back(waypoint_itr->GetSpeedConstraintHigh());
      route_data_.m_low_speed_constraint.emplace_back(waypoint_itr->GetSpeedConstraintLow());
      route_data_.m_rf_latitude.emplace_back(waypoint_itr->GetRfTurnCenterLatitude());
      route_data_.m_rf_longitude.emplace_back(waypoint_itr->GetRfTurnCenterLongitude());
      route_data_.m_rf_radius.emplace_back(waypoint_itr->GetRfTurnArcRadius());

      const Units::Speed nominal_ias = waypoint_itr->GetNominalIas();

      if (vector_current_index == m_ordered_waypoints.size()) {
         if (planned_cruise_mach_ != 0) {
            // cruise mach specified; use it
            route_data_.m_nominal_ias[vector_current_index] = nominal_ias;
         } else {
            route_data_.m_nominal_ias[vector_current_index] = nominal_ias;
         }
      } else {
         if (nominal_ias == Units::zero()) {
            // use previous mach because ias is unspecified
            route_data_.m_nominal_ias[vector_current_index] = Units::ZERO_SPEED;
         } else if (nominal_ias != Units::zero()) {
            // use ias because it is specified
            route_data_.m_nominal_ias[vector_current_index] = nominal_ias;
         } else if (route_data_.m_nominal_ias[vector_current_index - 1].value() != 0 && nominal_ias == Units::zero()) {
            // use previous ias
            route_data_.m_nominal_ias[vector_current_index] = route_data_.m_nominal_ias[vector_current_index - 1];
         } else {
            route_data_.m_nominal_ias[vector_current_index] = nominal_ias;
         }
      }

      ++waypoint_itr;
      ++vector_current_index;
   }
}

void DefaultAircraftIntent::LoadWaypointsFromList(const std::list<Waypoint> &ascent_waypoints,
                                           const std::list<Waypoint> &cruise_waypoints,
                                           const std::list<Waypoint> &descent_waypoints) {
   const bool has_ascent = !ascent_waypoints.empty();
   const bool has_cruise = !cruise_waypoints.empty();
   const bool has_descent = !descent_waypoints.empty();
   std::list<Waypoint> cruise_waypoints_with_connection_added, descent_waypoints_with_connection_added;
   if (has_ascent) {
      if (has_cruise) {
         cruise_waypoints_with_connection_added = AddConnectingLeg(ascent_waypoints, cruise_waypoints);
         if (has_descent)
            descent_waypoints_with_connection_added =
                  AddConnectingLeg(cruise_waypoints_with_connection_added, descent_waypoints);
      } else if (has_descent) {
         descent_waypoints_with_connection_added = AddConnectingLeg(ascent_waypoints, descent_waypoints);
      }
   } else if (has_cruise) {
      cruise_waypoints_with_connection_added = cruise_waypoints;
      if (has_descent)
         descent_waypoints_with_connection_added =
               AddConnectingLeg(cruise_waypoints_with_connection_added, descent_waypoints);
   } else {
      descent_waypoints_with_connection_added = descent_waypoints;
   }

   std::list<Waypoint> ascent_waypoints_shortened_legs, cruise_waypoints_shortened_legs,
         descent_waypoints_shortened_legs;
   if (!has_ascent) {
      ascent_waypoints_shortened_legs = ascent_waypoints;
   } else {
      ascent_waypoints_shortened_legs = CoreUtils::ShortenLongLegs(ascent_waypoints);
   }
   if (cruise_waypoints_with_connection_added.empty()) {
      cruise_waypoints_shortened_legs = cruise_waypoints_with_connection_added;
   } else {
      cruise_waypoints_shortened_legs = CoreUtils::ShortenLongLegs(cruise_waypoints_with_connection_added);
   }
   if (descent_waypoints_with_connection_added.empty()) {
      descent_waypoints_shortened_legs = descent_waypoints_with_connection_added;
   } else {
      descent_waypoints_shortened_legs = CoreUtils::ShortenLongLegs(descent_waypoints_with_connection_added);
   }

   ClearAndResetRouteDataContent(DefaultAircraftIntent::ConvertListToVector(ascent_waypoints_shortened_legs),
                                 DefaultAircraftIntent::ConvertListToVector(cruise_waypoints_shortened_legs),
                                 DefaultAircraftIntent::ConvertListToVector(descent_waypoints_shortened_legs));

   const auto all_waypoints_as_list =
         DefaultAircraftIntent::RemoveZeroLengthLegs(DefaultAircraftIntent::ConvertVectorToList(m_ordered_waypoints));
   m_tangent_plane_sequence =
         std::shared_ptr<TangentPlaneSequence>(new SingleTangentPlaneSequence(all_waypoints_as_list));
   UpdateXYZFromLatLonWgs84();
   m_is_loaded = true;
   DoRouteDataLogging();
}

void DefaultAircraftIntent::UpdateXYZFromLatLonWgs84() {
   EarthModel::GeodeticPosition geoPosition;
   EarthModel::LocalPositionEnu xyPosition;

   route_data_.m_x.clear();
   route_data_.m_y.clear();
   route_data_.m_z.clear();
   route_data_.m_x_rf_center.clear();
   route_data_.m_y_rf_center.clear();
   for (int var = 0; var < route_data_.m_latitude.size(); ++var) {
      geoPosition.altitude = Units::ZERO_LENGTH;
      geoPosition.latitude = Units::RadiansAngle(route_data_.m_latitude[var]);
      geoPosition.longitude = Units::RadiansAngle(route_data_.m_longitude[var]);
      m_tangent_plane_sequence->ConvertGeodeticToLocal(geoPosition, xyPosition);
      route_data_.m_x.emplace_back(xyPosition.x);
      route_data_.m_y.emplace_back(xyPosition.y);
      route_data_.m_z.emplace_back(xyPosition.z);

      if (route_data_.m_rf_radius[var] == Units::zero()) {
         route_data_.m_x_rf_center.emplace_back(Units::zero());
         route_data_.m_y_rf_center.emplace_back(Units::zero());
      } else {
         geoPosition.altitude = Units::FeetLength(0);
         geoPosition.latitude = Units::RadiansAngle(route_data_.m_rf_latitude[var]);
         geoPosition.longitude = Units::RadiansAngle(route_data_.m_rf_longitude[var]);
         m_tangent_plane_sequence->ConvertGeodeticToLocal(geoPosition, xyPosition);
         route_data_.m_x_rf_center.emplace_back(xyPosition.x);
         route_data_.m_y_rf_center.emplace_back(xyPosition.y);
      }
   }
}

int DefaultAircraftIntent::GetWaypointIndexByName(const std::string &waypoint_name) const {
   int ix = -1;
   bool found_waypoint = false;
   for (const Waypoint &wp : m_ordered_waypoints) {
      ++ix;
      if (wp.GetName().compare(waypoint_name) == 0) {
         found_waypoint = true;
         break;
      }
   }

   if (!found_waypoint) ix = -1;

   return ix;
}

void DefaultAircraftIntent::Copy(const DefaultAircraftIntent &in) {
   DeleteRouteDataContent();
   m_planned_cruise_altitude = in.m_planned_cruise_altitude;
   planned_cruise_mach_ = in.planned_cruise_mach_;
   m_is_loaded = in.m_is_loaded;
   route_data_ = in.route_data_;
   m_tangent_plane_sequence = in.m_tangent_plane_sequence;
   m_ordered_waypoints = in.m_ordered_waypoints;
   m_descent_waypoints = in.m_descent_waypoints;
   m_cruise_waypoints = in.m_cruise_waypoints;
   m_ascent_waypoints = in.m_ascent_waypoints;
}

void DefaultAircraftIntent::GetLatLonFromXYZ(const Units::Length &xMeters, const Units::Length &yMeters,
                                            const Units::Length &zMeters, Units::Angle &lat, Units::Angle &lon) const {
   EarthModel::LocalPositionEnu localPos;
   localPos.x = xMeters;
   localPos.y = yMeters;
   localPos.z = zMeters;

   EarthModel::GeodeticPosition geo;
   m_tangent_plane_sequence->ConvertLocalToGeodetic(localPos, geo);
   lat = geo.latitude;
   lon = geo.longitude;
}

std::pair<int, int> DefaultAircraftIntent::FindCommonWaypoint(const AircraftIntent &intent) const {
   /*
    * Find the earliest common waypoint (closest to the start of own intent).
    *
    * Returns -1 if not found.
    */

   int ix = GetNumberOfWaypoints() - 1;
   int tx = intent.GetNumberOfWaypoints() - 1;
   int thisIndex = -1;
   int thatIndex = -1;

   while ((ix >= 0) && (tx >= 0)) {
      if (GetWaypointName(ix) == intent.GetWaypointName(tx)) {
         thisIndex = ix;
         thatIndex = tx;
         ix--;
         tx--;
      } else {
         break;
      }
   }

   return std::make_pair(thisIndex, thatIndex);
}

void DefaultAircraftIntent::InsertPairAtIndex(const std::string &wpname, const Units::Length &x, const Units::Length &y,
                                       const int index) {
   /*
    * The incoming point must have been validated by the caller.
    *
    * The point will be converted to a waypoint and inserted into the waypoint list that this object
    * was initialized with. Then the object will be re-initialized.
    */
   Units::Angle lat, lon;
   GetLatLonFromXYZ(x, y, Units::ZERO_LENGTH, lat, lon);

   Waypoint wp;
   wp.SetRfTurnArcRadius(Units::ZERO_LENGTH);
   wp.SetWaypointLatLon(lat, lon);
   wp.SetName(wpname);

   // copy constraints -- high from previous waypoint and low from next
   auto wp2 = std::next(m_ordered_waypoints.begin(), index - 1);
   wp.SetAltitudeConstraintHigh(wp2->GetAltitudeConstraintHigh());
   wp.SetSpeedConstraintHigh(wp2->GetSpeedConstraintHigh());
   ++wp2;
   wp.SetAltitudeConstraintLow(wp2->GetAltitudeConstraintLow());
   wp.SetSpeedConstraintLow(wp2->GetSpeedConstraintLow());

   InsertWaypointAtIndex(wp, index);
}

void DefaultAircraftIntent::InsertWaypointAtIndex(const Waypoint &wp, int index) {
   if (index < m_ascent_waypoints.size()) {
      std::vector<Waypoint> new_vector(m_ascent_waypoints);
      auto itr = std::next(new_vector.begin(), index);
      new_vector.insert(itr, wp);
      ClearAndResetRouteDataContent(new_vector, m_cruise_waypoints, m_descent_waypoints);
   } else if (index < m_ascent_waypoints.size() + m_cruise_waypoints.size()) {
      std::vector<Waypoint> new_vector(m_cruise_waypoints);
      auto itr = std::next(new_vector.begin(), index - m_ascent_waypoints.size());
      new_vector.insert(itr, wp);
      ClearAndResetRouteDataContent(m_ascent_waypoints, new_vector, m_descent_waypoints);
   } else {
      std::vector<Waypoint> new_vector(m_descent_waypoints);
      auto itr = std::next(new_vector.begin(), index - m_ascent_waypoints.size() - m_cruise_waypoints.size());
      new_vector.insert(itr, wp);
      ClearAndResetRouteDataContent(m_ascent_waypoints, m_cruise_waypoints, new_vector);
   }

   UpdateXYZFromLatLonWgs84();
}

void DefaultAircraftIntent::UpdateWaypoint(const Waypoint &waypoint) {
   int update_count(0);
   std::vector<Waypoint> new_vector(m_ordered_waypoints);
   for (auto it = new_vector.begin(); it != new_vector.end(); ++it) {
      if (it->GetName() == waypoint.GetName()) {
         (*it) = waypoint;
         update_count++;
      }
   }
   if (update_count != 1) {
      LOG4CPLUS_FATAL(m_logger, update_count << " waypoints updated.");
      throw std::runtime_error("Wrong number of waypoints matched.");
   }

   const std::vector<Waypoint> empty_vector;
   ClearAndResetRouteDataContent(empty_vector, empty_vector, new_vector);
   UpdateXYZFromLatLonWgs84();
}

void DefaultAircraftIntent::ClearWaypoints() {
   m_ordered_waypoints.clear();

   const std::vector<Waypoint> empty_vector;
   ClearAndResetRouteDataContent(empty_vector, empty_vector, m_ordered_waypoints);
   UpdateXYZFromLatLonWgs84();
}

const Waypoint &DefaultAircraftIntent::GetWaypoint(unsigned int i) const {
   if (i >= m_ordered_waypoints.size()) {
      LOG4CPLUS_FATAL(m_logger, "Index " << i << " is out of range for size " << m_ordered_waypoints.size());
      throw InvalidIndexException(i, 0, m_ordered_waypoints.size() - 1);
   }
   return m_ordered_waypoints[i];
}

void DefaultAircraftIntent::SetNumberOfWaypoints(unsigned int n) {
   if (n < m_ordered_waypoints.size()) {
      std::vector<Waypoint>::iterator it1 = std::next(m_ordered_waypoints.begin(), n);  // get an iterator pointing to index
      std::vector<Waypoint>::iterator it2 = m_ordered_waypoints.end();
      m_ordered_waypoints.erase(it1, it2);
   }
}

bool DefaultAircraftIntent::operator==(const DefaultAircraftIntent &obj) const {
   if (m_planned_cruise_altitude == obj.m_planned_cruise_altitude &&
       planned_cruise_mach_ == obj.planned_cruise_mach_ && m_is_loaded == obj.m_is_loaded &&
       route_data_.m_name == obj.route_data_.m_name && route_data_.m_x == obj.route_data_.m_x &&
       route_data_.m_y == obj.route_data_.m_y && route_data_.m_z == obj.route_data_.m_z &&
       route_data_.m_latitude == obj.route_data_.m_latitude && route_data_.m_longitude == obj.route_data_.m_longitude &&
       route_data_.m_nominal_altitude == obj.route_data_.m_nominal_altitude &&
       route_data_.m_nominal_ias == obj.route_data_.m_nominal_ias &&
       route_data_.m_high_altitude_constraint == obj.route_data_.m_high_altitude_constraint &&
       route_data_.m_low_altitude_constraint == obj.route_data_.m_low_altitude_constraint &&
       route_data_.m_high_speed_constraint == obj.route_data_.m_high_speed_constraint &&
       route_data_.m_low_speed_constraint == obj.route_data_.m_low_speed_constraint &&
       route_data_.m_rf_latitude == obj.route_data_.m_rf_latitude &&
       route_data_.m_rf_longitude == obj.route_data_.m_rf_longitude &&
       route_data_.m_x_rf_center == obj.route_data_.m_x_rf_center &&
       route_data_.m_y_rf_center == obj.route_data_.m_y_rf_center &&
       route_data_.m_rf_radius == obj.route_data_.m_rf_radius) {
      return true;
   }
   return false;
}

void DefaultAircraftIntent::SetPlannedCruiseAltitude(Units::Length altitude) { this->m_planned_cruise_altitude = altitude; }

void DefaultAircraftIntent::SetPlannedCruiseMach(BoundedValue<double, 0, 1> mach_number) {
   planned_cruise_mach_ = (double)mach_number;
}

const std::string &DefaultAircraftIntent::GetWaypointName(unsigned int i) const { return GetWaypoint(i).GetName(); }

Units::MetersLength DefaultAircraftIntent::GetWaypointX(unsigned int i) const {
   if (i >= route_data_.m_x.size()) {
      LOG4CPLUS_FATAL(m_logger, "Index " << i << " is out of range for size " << route_data_.m_x.size());
      throw InvalidIndexException(i, 0, route_data_.m_x.size() - 1);
   }
   return route_data_.m_x[i];
}

Units::MetersLength DefaultAircraftIntent::GetWaypointY(unsigned int i) const {
   if (i >= route_data_.m_y.size()) {
      LOG4CPLUS_FATAL(m_logger, "Index " << i << " is out of range for size " << route_data_.m_y.size());
      throw InvalidIndexException(i, 0, route_data_.m_y.size() - 1);
   }
   return route_data_.m_y[i];
}

std::list<Waypoint> DefaultAircraftIntent::RemoveZeroLengthLegs(const std::list<Waypoint> &waypoints) {
   std::list<Waypoint> resolved_waypoints;
   if (waypoints.empty()) {
      return resolved_waypoints;
   }
   auto wpt_itr = waypoints.begin();
   auto next_itr = std::next(wpt_itr);
   for (; next_itr != waypoints.end(); ++wpt_itr, ++next_itr) {
      const auto lat1 = wpt_itr->GetLatitude();
      const auto lon1 = wpt_itr->GetLongitude();
      const auto lat2 = next_itr->GetLatitude();
      const auto lon2 = next_itr->GetLongitude();
      const auto lat_diff = Units::abs(lat1 - lat2);
      const auto lon_diff = Units::abs(lon1 - lon2);
      const auto tolerance = Units::DegreesAngle(1e-5);
      const bool skip_waypoint = lat_diff < tolerance && lon_diff < tolerance;
      if (!skip_waypoint) {
         resolved_waypoints.push_back(*wpt_itr);
      }
   }
   // always include the last waypoint
   resolved_waypoints.push_back(waypoints.back());
   return resolved_waypoints;
}

DefaultAircraftIntent DefaultAircraftIntent::CopyAndTrimAfterNamedWaypoint(const AircraftIntent &aircraft_intent,
                                                             const std::string &waypoint_name) {
   DefaultAircraftIntent aircraft_intent_copy(*Builder(aircraft_intent).Build());
   const std::string final_waypoint_name =
         aircraft_intent.GetWaypoint(aircraft_intent.GetNumberOfWaypoints() - 1).GetName();
   if (aircraft_intent.ContainsWaypointName(waypoint_name) && final_waypoint_name != waypoint_name) {
      auto name_comparator = [&waypoint_name](const Waypoint &waypoint_to_test) {
         return waypoint_to_test.GetName() == waypoint_name;
      };
      auto trim_vector = [&name_comparator](const std::vector<Waypoint> &waypoints) {
         std::vector<Waypoint> trimmed_vector;
         for (const Waypoint &wp : waypoints) {
            trimmed_vector.push_back(wp);
            if (name_comparator(wp)) {
               break;
            }
         }
         return trimmed_vector;
      };
      std::vector<Waypoint> ascent_waypoints = aircraft_intent.GetAscentWaypoints();
      std::vector<Waypoint> cruise_waypoints = aircraft_intent.GetCruiseWaypoints();
      std::vector<Waypoint> descent_waypoints = aircraft_intent.GetDescentWaypoints();
      if (std::any_of(ascent_waypoints.rbegin(), ascent_waypoints.rend(), name_comparator)) {
         cruise_waypoints.clear();
         descent_waypoints.clear();
         ascent_waypoints = trim_vector(ascent_waypoints);
      } else if (std::any_of(cruise_waypoints.rbegin(), cruise_waypoints.rend(), name_comparator)) {
         cruise_waypoints = trim_vector(cruise_waypoints);
         descent_waypoints.clear();
      } else {
         descent_waypoints = trim_vector(descent_waypoints);
      }
      auto to_list = [](std::vector<Waypoint> waypoints) {
         std::list<Waypoint> dest;
         std::copy(waypoints.begin(), waypoints.end(), std::back_inserter(dest));
         return dest;
      };
      aircraft_intent_copy.ClearWaypoints();
      aircraft_intent_copy.LoadWaypointsFromList(to_list(ascent_waypoints), to_list(cruise_waypoints),
                                                 to_list(descent_waypoints));
   }
   return aircraft_intent_copy;
}
