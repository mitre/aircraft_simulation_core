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

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <scalar/Speed.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"
#include "public/AircraftIntent.h"
#include "public/TangentPlaneSequence.h"
#include "public/Waypoint.h"
#include "utility/BoundedValue.h"
#include "utility/UtilityConstants.h"

namespace aaesim::open_source
{
   class DefaultAircraftIntent final : public AircraftIntent
   {
   public:
      class Builder final
      {
      public:
         Builder() = default;
         Builder(const AircraftIntent &aircraft_intent);
         Builder &SetAscentWaypoints(const std::vector<Waypoint> &ascent_waypoints);
         Builder &SetCruiseWaypoints(const std::vector<Waypoint> &cruise_waypoints);
         Builder &SetDescentWaypoints(const std::vector<Waypoint> &descent_waypoints);
         Builder &SetPlannedCruiseMach(BoundedValue<double, 0, 1> planned_cruise_mach);
         Builder &SetPlannedCruiseAltitude(Units::Length planned_cruise_altitude);
         std::shared_ptr<DefaultAircraftIntent> Build() const;

         private:
         std::vector<Waypoint> ascent_waypoints_{};
         std::vector<Waypoint> cruise_waypoints_{};
         std::vector<Waypoint> descent_waypoints_{};
         BoundedValue<double, 0, 1> planned_cruise_mach_{0};
         Units::Length planned_cruise_altitude_{Units::ZERO_LENGTH};
      };

      DefaultAircraftIntent() = default;
      ~DefaultAircraftIntent() override = default;
      DefaultAircraftIntent(const DefaultAircraftIntent &) = default;
      DefaultAircraftIntent &operator=(const DefaultAircraftIntent &) = default;
      bool operator==(const DefaultAircraftIntent &in) const;

      void UpdateWaypoint(const Waypoint &waypoint); // TODO why does this exist?
      void GetLatLonFromXYZ(const Units::Length &xMeters, const Units::Length &yMeters,
                            const Units::Length &zMeters, Units::Angle &lat, Units::Angle &lon) const; // TODO why does this exist?
      void SetNumberOfWaypoints(unsigned int n); // TODO should not exist, client code should use builder
      std::optional<Waypoint> GetWaypoint(unsigned int i) const override;
      const std::vector<Waypoint> &GetWaypoints() const override;
      std::optional<std::string> GetWaypointName(unsigned int i) const override;
      Units::MetersLength GetWaypointX(unsigned int i) const;
      Units::MetersLength GetWaypointY(unsigned int i) const;
      Units::MetersLength GetPlannedCruiseAltitude() const override;
      void SetPlannedCruiseAltitude(Units::Length altitude); // TODO should not exist, client code should use builder
      const RouteData &GetRouteData() const override;
      std::optional<unsigned int> GetWaypointIndexByName(const std::string &waypoint_name) const override;
      std::pair<int, int> FindCommonWaypoint(const AircraftIntent &intent) const override;
      void InsertPairAtIndex(const std::string &wpname, const Units::Length &x, const Units::Length &y,
                             int index);
      void InsertWaypointAtIndex(const Waypoint &waypoint, int index);
      void ClearWaypoints(); // TODO should not exist
      unsigned int GetNumberOfWaypoints() const override;
      bool IsLoaded() const; // TODO should not exist
      bool ContainsAscentWaypoints() const;
      const std::vector<Waypoint> &GetAscentWaypoints() const override;
      bool ContainsCruiseWaypoints() const;
      const std::vector<Waypoint> &GetCruiseWaypoints() const override;
      bool ContainsDescentWaypoints() const;
      const std::vector<Waypoint> &GetDescentWaypoints() const override;
      double GetPlannedCruiseMach() const override;
      void SetPlannedCruiseMach(BoundedValue<double, 0, 1> mach_number); // TODO should not be public, client code should use builder
      bool ContainsWaypointName(const std::string &waypoint_name) const override;

   private:
      static inline std::map<std::string, Arinc424LegType> m_arinc424_dictionary{
          {"IF", AircraftIntent::Arinc424LegType::IF},
          {"UNSET", AircraftIntent::Arinc424LegType::UNSET},
          {"RF", AircraftIntent::Arinc424LegType::RF},
          {"TF", AircraftIntent::Arinc424LegType::TF},
          {"VI", AircraftIntent::Arinc424LegType::VI},
          {"CF", AircraftIntent::Arinc424LegType::CF},
          {"VA", AircraftIntent::Arinc424LegType::VA},
          {"CA", AircraftIntent::Arinc424LegType::CA},
      };

      static std::vector<Waypoint> RemoveZeroLengthLegs(const std::vector<Waypoint> &waypoints);

      std::vector<Waypoint> AddConnectingLeg(const std::vector<Waypoint> &first_waypoint_vector,
                                             const std::vector<Waypoint> &second_waypoint_vector) const;

      void ClearAndResetRouteDataContent(const std::vector<Waypoint> &ascent_waypoints,
                                         const std::vector<Waypoint> &cruise_waypoints,
                                         const std::vector<Waypoint> &descent_waypoints);

      void LoadWaypoints(const std::vector<Waypoint> &ascent_waypoints,
                         const std::vector<Waypoint> &cruise_waypoints,
                         const std::vector<Waypoint> &descent_waypoints);
      void UpdateXYZFromLatLonWgs84();

      void DoRouteDataLogging() const;

      struct RouteData route_data_{};
      std::shared_ptr<TangentPlaneSequence> m_tangent_plane_sequence{};
      double planned_cruise_mach_{0};
      std::vector<Waypoint> m_ordered_waypoints{};
      bool m_is_loaded{false};
      std::vector<Waypoint> m_ascent_waypoints{}, m_cruise_waypoints{}, m_descent_waypoints{};
      Units::MetersLength m_planned_cruise_altitude{Units::ZERO_LENGTH};

      static inline log4cplus::Logger m_logger{log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("DefaultAircraftIntent"))};

      void DeleteRouteDataContent();
      void AddWaypointsToRouteDataVectors(const std::vector<Waypoint> &waypoints, enum WaypointPhaseOfFlight add_as_phase);
   };

   inline const DefaultAircraftIntent::RouteData &DefaultAircraftIntent::GetRouteData() const { return route_data_; }

   inline unsigned int DefaultAircraftIntent::GetNumberOfWaypoints() const { return route_data_.m_name.size(); }

   inline Units::MetersLength DefaultAircraftIntent::GetPlannedCruiseAltitude() const { return m_planned_cruise_altitude; }

   inline bool DefaultAircraftIntent::IsLoaded() const { return m_is_loaded; }

   inline void DefaultAircraftIntent::AddWaypointsToRouteDataVectors(const std::vector<Waypoint> &waypoints,
                                                                     enum WaypointPhaseOfFlight add_as_phase)
   {
      auto waypoint_itr = waypoints.begin();
      while (waypoint_itr != waypoints.end())
      {
         m_ordered_waypoints.push_back(*waypoint_itr);
         route_data_.m_name.push_back(waypoint_itr->GetName());
         route_data_.m_waypoint_phase_of_flight.push_back(add_as_phase);
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

         ++waypoint_itr;
      }
   }

   inline std::vector<Waypoint> DefaultAircraftIntent::AddConnectingLeg(
         const std::vector<Waypoint> &first_waypoint_vector,
         const std::vector<Waypoint> &second_waypoint_vector) const
   {
      if (first_waypoint_vector.empty())
         // nothing to do
         return second_waypoint_vector;
      if (!second_waypoint_vector.empty() &&
          (first_waypoint_vector.back().GetName() == second_waypoint_vector.front().GetName()))
      {
         // nothing to do
         return second_waypoint_vector;
      }

      std::vector<Waypoint> updated_waypoints;
      Waypoint to_copy(first_waypoint_vector.back());
      Waypoint new_tf_leg(to_copy.GetName() + "_copy_as_IF", to_copy.GetLatitude(), to_copy.GetLongitude(),
                          to_copy.GetAltitudeConstraintHigh(), to_copy.GetAltitudeConstraintLow(),
                          to_copy.GetSpeedConstraintHigh(), to_copy.GetAltitude(), to_copy.GetNominalIas(), "IF");
      updated_waypoints.push_back(new_tf_leg);
      updated_waypoints.insert(updated_waypoints.end(), second_waypoint_vector.begin(), second_waypoint_vector.end());
      return updated_waypoints;
   }

   inline bool DefaultAircraftIntent::ContainsAscentWaypoints() const { return !m_ascent_waypoints.empty(); }

   inline bool DefaultAircraftIntent::ContainsCruiseWaypoints() const { return !m_cruise_waypoints.empty(); }

   inline bool DefaultAircraftIntent::ContainsDescentWaypoints() const { return !m_descent_waypoints.empty(); }

   inline double DefaultAircraftIntent::GetPlannedCruiseMach() const { return planned_cruise_mach_; }

   inline void DefaultAircraftIntent::DoRouteDataLogging() const
   {
      using json = nlohmann::json;
      if (m_logger.getLogLevel() == log4cplus::TRACE_LOG_LEVEL)
      {
         for (auto idx = 0; idx < route_data_.m_name.size(); ++idx)
         {
            json j;
            j["segment_index"] = idx;
            j["name"] = route_data_.m_name[idx];
            j["phase_of_flight_int"] = route_data_.m_waypoint_phase_of_flight[idx];
            j["x_position_m"] = Units::MetersLength(route_data_.m_x[idx]).value();
            j["y_position_m"] = Units::MetersLength(route_data_.m_y[idx]).value();
            j["z_position_m"] = Units::MetersLength(route_data_.m_z[idx]).value();
            j["nominal_alitude_ft"] = Units::FeetLength(route_data_.m_nominal_altitude[idx]).value();
            j["latitude_deg"] = Units::DegreesAngle(route_data_.m_latitude[idx]).value();
            j["longitude_deg"] = Units::DegreesAngle(route_data_.m_longitude[idx]).value();
            j["nominal_ias_kts"] = Units::KnotsSpeed(route_data_.m_nominal_ias[idx]).value();
            j["alt_high_ft"] = Units::FeetLength(route_data_.m_high_altitude_constraint[idx]).value();
            j["alt_low_ft"] = Units::FeetLength(route_data_.m_low_altitude_constraint[idx]).value();
            j["speed_high_knots"] = Units::KnotsSpeed(route_data_.m_high_speed_constraint[idx]).value();
            j["speed_low_knots"] = Units::KnotsSpeed(route_data_.m_low_speed_constraint[idx]).value();
            j["leg_type_int"] = route_data_.m_leg_type[idx];
            j["rf_turn_x_position_m"] = Units::MetersLength(route_data_.m_x_rf_center[idx]).value();
            j["rf_turn_y_position_m"] = Units::MetersLength(route_data_.m_y_rf_center[idx]).value();
            j["rf_turn_radius_nm"] = Units::NauticalMilesLength(route_data_.m_rf_radius[idx]).value();
            j["rf_turn_lat_deg"] = Units::SignedDegreesAngle(route_data_.m_rf_latitude[idx]).value();
            j["rf_turn_lon_deg"] = Units::SignedDegreesAngle(route_data_.m_rf_longitude[idx]).value();
            LOG4CPLUS_TRACE(m_logger, j.dump());
         }
      }
   }

   inline bool DefaultAircraftIntent::ContainsWaypointName(const std::string &waypoint_name) const
   {
      auto name_comparator = [&waypoint_name](const Waypoint &waypoint_to_test)
      {
         return waypoint_to_test.GetName().compare(waypoint_name) == 0;
      };
      return std::any_of(m_ordered_waypoints.rbegin(), m_ordered_waypoints.rend(), name_comparator);
   }

   inline const std::vector<Waypoint> &DefaultAircraftIntent::GetAscentWaypoints() const { return m_ascent_waypoints; }

   inline const std::vector<Waypoint> &DefaultAircraftIntent::GetDescentWaypoints() const { return m_descent_waypoints; }

   inline const std::vector<Waypoint> &DefaultAircraftIntent::GetCruiseWaypoints() const { return m_cruise_waypoints; }

} // namespace aaesim::open_source

using DefaultAircraftIntent = aaesim::open_source::DefaultAircraftIntent;
