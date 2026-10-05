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

#include <scalar/Acceleration.h>
#include <scalar/Angle.h>
#include <scalar/Length.h>
#include <scalar/Mass.h>
#include <scalar/Speed.h>
#include <scalar/Time.h>
#include <scalar/Unit.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

#include "public/BadaUtils.h"
#include "public/CoreUtils.h"
#include "public/VerticalPath.h"

namespace mitre::oss::simcore
{
      struct VerticalPathUtils
      {
            struct VerticalPathDataSet final
            {
                  Units::Length along_path_distance{};
                  Units::Length altitude_msl{};
                  Units::Speed calibrated_airspeed{};
                  double mach{INT32_MIN};
                  Units::Speed altitude_rate{};
                  Units::Speed true_airspeed{};
                  Units::Acceleration tas_rate{};
                  Units::Angle theta{};
                  Units::Speed ground_speed{};
                  Units::Time time_to_go{};
                  Units::Mass mass{};
                  Units::MetersPerSecondSpeed wind_velocity_east{};
                  Units::MetersPerSecondSpeed wind_velocity_north{};
                  mitre::oss::simcore::bada_utils::FlapConfiguration flap_setting{
                      mitre::oss::simcore::bada_utils::FlapConfiguration::UNDEFINED};
                  int resolved_index{INT32_MIN};
                  VerticalPath::PredictionAlgorithmType algorithm_type{VerticalPath::PredictionAlgorithmType::UNDETERMINED};
            };

            /**
             * When allow_extrapolation is true, use the first or last interval outside the sampled range.
             * Extrapolation requires two samples with distinct distances in the selected interval.
             * Otherwise, retain the first value below the range and throw at or above its upper end.
             */
            static Units::Time CalculateTimeToFly(const VerticalPath &vertical_path,
                                                  Units::Length estimated_distance_to_path_end,
                                                  bool allow_extrapolation = false);

            static Units::Speed CalculateSpeedGuidance(const VerticalPath &vertical_path,
                                                       Units::Length estimated_distance_to_path_end);

            /** @see CalculateTimeToFly for allow_extrapolation behavior. */
            static Units::Speed CalculateSpeedGuidance(const VerticalPath &vertical_path,
                                                       Units::Length estimated_distance_to_path_end,
                                                       bool allow_extrapolation);

            static double CalculateMachGuidance(const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end);

            /** @see CalculateTimeToFly for allow_extrapolation behavior. */
            static double CalculateMachGuidance(const VerticalPath &vertical_path,
                                                 Units::Length estimated_distance_to_path_end,
                                                 bool allow_extrapolation);

            static Units::Mass GetExpectedMass(const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end);

            static VerticalPathDataSet GetVerticalPathData(const VerticalPath &vertical_path,
                                                           Units::Length estimated_distance_to_path_end);

            static VerticalPathDataSet GetPathDataAtTime(const VerticalPath &vertical_path, Units::Time time_to_go);

            static VerticalPathDataSet GetInterpolatedPathData(const VerticalPath &vertical_path,
                                                               Units::Length estimated_distance_to_path_end);

            static VerticalPathDataSet GetInterpolatedPathDataAtTime(const VerticalPath &vertical_path,
                                                                     Units::Time time_to_go);

            static VerticalPathDataSet GetPathDataAtIndex(const VerticalPath &vertical_path, int index);

            static std::size_t GetPathDataCount(const VerticalPath &vertical_path);
            static VerticalPathDataSet GetFirstPathData(const VerticalPath &vertical_path);
            static VerticalPathDataSet GetLastPathData(const VerticalPath &vertical_path);

            static std::vector<mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet> ConvertToPathDataSet(
                const VerticalPath &vertical_path);

      private:
            /**
             * Returns the first index whose sample is strictly greater than value_to_find.
             * Samples must be sorted in ascending order; returns size() when no sample qualifies.
             */
            static int FindUpperBoundIndex(double value_to_find, const std::vector<double> &samples)
            {
                  return static_cast<int>(std::ranges::distance(
                      samples.begin(), std::ranges::upper_bound(samples, value_to_find)));
            }
      };
} // namespace mitre::oss::simcore

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetVerticalPathData(const VerticalPath &vertical_path,
                                                            Units::Length estimated_distance_to_path_end)
{
      const Units::MetersLength distance_to_go{estimated_distance_to_path_end};
      const auto reference_lookup_index =
          CoreUtils::FindNearestIndex(distance_to_go.value(), vertical_path.along_path_distance_m);
      return GetPathDataAtIndex(vertical_path, reference_lookup_index);
}

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetPathDataAtTime(const VerticalPath &vertical_path,
                                                          Units::Time time_to_go)
{
      const Units::SecondsTime seconds_to_go{time_to_go};
      const auto reference_lookup_index =
          CoreUtils::FindNearestIndex(seconds_to_go.value(), vertical_path.time_to_go_sec);
      return GetPathDataAtIndex(vertical_path, reference_lookup_index);
}

inline std::size_t mitre::oss::simcore::VerticalPathUtils::GetPathDataCount(const VerticalPath &vertical_path)
{
      return vertical_path.along_path_distance_m.size();
}

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetFirstPathData(const VerticalPath &vertical_path)
{
      return GetPathDataAtIndex(vertical_path, 0);
}

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetLastPathData(const VerticalPath &vertical_path)
{
      return GetPathDataAtIndex(vertical_path, static_cast<int>(GetPathDataCount(vertical_path) - 1));
}

inline std::vector<mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet>
mitre::oss::simcore::VerticalPathUtils::ConvertToPathDataSet(const VerticalPath &vertical_path)
{
      std::vector<mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet> path_data_set{};
      for (auto idx = 0; idx < vertical_path.along_path_distance_m.size(); ++idx)
      {
            path_data_set.push_back(mitre::oss::simcore::VerticalPathUtils::GetPathDataAtIndex(vertical_path, idx));
      }
      return path_data_set;
}

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetPathDataAtIndex(const VerticalPath &vertical_path, int index)
{
      mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet single_data_row{};
      single_data_row.resolved_index = index;
      single_data_row.along_path_distance = Units::MetersLength(vertical_path.along_path_distance_m[index]);
      single_data_row.altitude_msl = Units::MetersLength(vertical_path.altitude_m[index]);
      single_data_row.calibrated_airspeed = Units::MetersPerSecondSpeed(vertical_path.cas_mps[index]);
      single_data_row.mach = vertical_path.mach[index];
      single_data_row.altitude_rate = Units::MetersPerSecondSpeed(vertical_path.altitude_rate_mps[index]);
      single_data_row.true_airspeed = vertical_path.true_airspeed[index];
      single_data_row.tas_rate = Units::MetersSecondAcceleration(vertical_path.tas_rate_mps[index]);
      single_data_row.theta = Units::RadiansAngle(vertical_path.theta_radians[index]);
      single_data_row.ground_speed = Units::MetersPerSecondSpeed(vertical_path.gs_mps[index]);
      single_data_row.time_to_go = Units::SecondsTime(vertical_path.time_to_go_sec[index]);
      single_data_row.mass = Units::KilogramsMass(vertical_path.mass_kg[index]);
      single_data_row.wind_velocity_east = vertical_path.wind_velocity_east[index];
      single_data_row.wind_velocity_north = vertical_path.wind_velocity_north[index];
      single_data_row.flap_setting = vertical_path.flap_setting[index];
      single_data_row.algorithm_type = vertical_path.algorithm_type[index];
      return single_data_row;
}

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetInterpolatedPathData(const VerticalPath &vertical_path,
                                                                Units::Length estimated_distance_to_path_end)
{
      const Units::MetersLength distance_to_go{estimated_distance_to_path_end};
      const auto reference_lookup_index =
          CoreUtils::FindNearestIndex(distance_to_go.value(), vertical_path.along_path_distance_m);

      if (reference_lookup_index < 1)
      {
            return GetPathDataAtIndex(vertical_path, reference_lookup_index);
      }

      mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet single_data_row{};
      single_data_row.resolved_index = reference_lookup_index;
      single_data_row.altitude_msl = Units::MetersLength(
          CoreUtils::LinearlyInterpolate(reference_lookup_index, distance_to_go.value(),
                                         vertical_path.along_path_distance_m, vertical_path.altitude_m));
      single_data_row.along_path_distance = estimated_distance_to_path_end;
      single_data_row.calibrated_airspeed = Units::MetersPerSecondSpeed(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, distance_to_go.value(), vertical_path.along_path_distance_m, vertical_path.cas_mps));
      single_data_row.mach = CoreUtils::LinearlyInterpolate(reference_lookup_index, distance_to_go.value(),
                                                            vertical_path.along_path_distance_m, vertical_path.mach);
      single_data_row.altitude_rate = Units::MetersPerSecondSpeed(
          CoreUtils::LinearlyInterpolate(reference_lookup_index, distance_to_go.value(),
                                         vertical_path.along_path_distance_m, vertical_path.altitude_rate_mps));
      single_data_row.true_airspeed = CoreUtils::LinearlyInterpolateByDistance(
          reference_lookup_index, distance_to_go, vertical_path.along_path_distance_m, vertical_path.true_airspeed);
      single_data_row.tas_rate = Units::MetersSecondAcceleration(
          CoreUtils::LinearlyInterpolate(reference_lookup_index, distance_to_go.value(),
                                         vertical_path.along_path_distance_m, vertical_path.tas_rate_mps));
      single_data_row.theta = Units::RadiansAngle(
          CoreUtils::LinearlyInterpolate(reference_lookup_index, distance_to_go.value(),
                                         vertical_path.along_path_distance_m, vertical_path.theta_radians));
      single_data_row.ground_speed = Units::MetersPerSecondSpeed(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, distance_to_go.value(), vertical_path.along_path_distance_m, vertical_path.gs_mps));
      single_data_row.time_to_go = Units::SecondsTime(
          CoreUtils::LinearlyInterpolate(reference_lookup_index, distance_to_go.value(),
                                         vertical_path.along_path_distance_m, vertical_path.time_to_go_sec));
      single_data_row.mass = Units::KilogramsMass(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, distance_to_go.value(), vertical_path.along_path_distance_m, vertical_path.mass_kg));
      single_data_row.wind_velocity_east = CoreUtils::LinearlyInterpolateByDistance(
          reference_lookup_index, distance_to_go, vertical_path.along_path_distance_m, vertical_path.wind_velocity_east);
      single_data_row.wind_velocity_north =
          CoreUtils::LinearlyInterpolateByDistance(reference_lookup_index, distance_to_go,
                                                   vertical_path.along_path_distance_m,
                                                   vertical_path.wind_velocity_north);
      single_data_row.flap_setting = vertical_path.flap_setting[reference_lookup_index];
      single_data_row.algorithm_type = vertical_path.algorithm_type[reference_lookup_index];
      return single_data_row;
}

inline mitre::oss::simcore::VerticalPathUtils::VerticalPathDataSet
mitre::oss::simcore::VerticalPathUtils::GetInterpolatedPathDataAtTime(const VerticalPath &vertical_path,
                                                                      Units::Time time_to_go)
{
      const Units::SecondsTime seconds_to_go{time_to_go};
      const auto reference_lookup_index =
          CoreUtils::FindNearestIndex(seconds_to_go.value(), vertical_path.time_to_go_sec);
      if (reference_lookup_index < 1)
      {
            return GetPathDataAtIndex(vertical_path, reference_lookup_index);
      }

      VerticalPathDataSet single_data_row{};
      single_data_row.resolved_index = reference_lookup_index;
      single_data_row.along_path_distance = Units::MetersLength(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec,
          vertical_path.along_path_distance_m));
      single_data_row.altitude_msl = Units::MetersLength(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec, vertical_path.altitude_m));
      single_data_row.calibrated_airspeed = Units::MetersPerSecondSpeed(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec, vertical_path.cas_mps));
      single_data_row.mach = CoreUtils::LinearlyInterpolate(reference_lookup_index, seconds_to_go.value(),
                                                            vertical_path.time_to_go_sec, vertical_path.mach);
      single_data_row.altitude_rate = Units::MetersPerSecondSpeed(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec,
          vertical_path.altitude_rate_mps));
      single_data_row.true_airspeed = CoreUtils::LinearlyInterpolateByTime(
          reference_lookup_index, time_to_go, vertical_path.time_to_go_sec, vertical_path.true_airspeed);
      single_data_row.tas_rate = Units::MetersSecondAcceleration(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec, vertical_path.tas_rate_mps));
      single_data_row.theta = Units::RadiansAngle(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec, vertical_path.theta_radians));
      single_data_row.ground_speed = Units::MetersPerSecondSpeed(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec, vertical_path.gs_mps));
      single_data_row.time_to_go = time_to_go;
      single_data_row.mass = Units::KilogramsMass(CoreUtils::LinearlyInterpolate(
          reference_lookup_index, seconds_to_go.value(), vertical_path.time_to_go_sec, vertical_path.mass_kg));
      single_data_row.wind_velocity_east = CoreUtils::LinearlyInterpolateByTime(
          reference_lookup_index, time_to_go, vertical_path.time_to_go_sec, vertical_path.wind_velocity_east);
      single_data_row.wind_velocity_north = CoreUtils::LinearlyInterpolateByTime(
          reference_lookup_index, time_to_go, vertical_path.time_to_go_sec, vertical_path.wind_velocity_north);
      single_data_row.flap_setting = vertical_path.flap_setting[reference_lookup_index];
      single_data_row.algorithm_type = vertical_path.algorithm_type[reference_lookup_index];
      return single_data_row;
}

inline Units::Speed mitre::oss::simcore::VerticalPathUtils::CalculateSpeedGuidance(
    const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end)
{
      return CalculateSpeedGuidance(vertical_path, estimated_distance_to_path_end, false);
}

inline Units::Speed mitre::oss::simcore::VerticalPathUtils::CalculateSpeedGuidance(
    const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end,
    bool allow_extrapolation)
{
      auto reference_lookup_index = FindUpperBoundIndex(
          Units::MetersLength(estimated_distance_to_path_end).value(), vertical_path.along_path_distance_m);

      if (allow_extrapolation &&
          (reference_lookup_index == 0 ||
           static_cast<std::size_t>(reference_lookup_index) == vertical_path.along_path_distance_m.size()))
      {
            const int extrapolation_index = reference_lookup_index == 0 ? 1 : reference_lookup_index - 1;
            return Units::MetersPerSecondSpeed(CoreUtils::LinearlyExtrapolate(
                extrapolation_index, Units::MetersLength(estimated_distance_to_path_end).value(),
                vertical_path.along_path_distance_m, vertical_path.cas_mps));
      }

      if (reference_lookup_index == 0)
      {
            return Units::MetersPerSecondSpeed(vertical_path.cas_mps.at(0));
      }
      else
      {
            return Units::MetersPerSecondSpeed(CoreUtils::LinearlyInterpolate(
                reference_lookup_index, Units::MetersLength(estimated_distance_to_path_end).value(),
                vertical_path.along_path_distance_m, vertical_path.cas_mps));
      }

      throw std::out_of_range("Unable to calculate CAS guidance; estimated distance is out of range and extrapolation is not allowed.");
}

inline double mitre::oss::simcore::VerticalPathUtils::CalculateMachGuidance(
    const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end)
{
      return CalculateMachGuidance(vertical_path, estimated_distance_to_path_end, false);
}

inline double mitre::oss::simcore::VerticalPathUtils::CalculateMachGuidance(
    const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end,
    bool allow_extrapolation)
{
      auto reference_lookup_index = FindUpperBoundIndex(
          Units::MetersLength(estimated_distance_to_path_end).value(), vertical_path.along_path_distance_m);

      if (allow_extrapolation &&
          (reference_lookup_index == 0 ||
           static_cast<std::size_t>(reference_lookup_index) == vertical_path.along_path_distance_m.size()))
      {
            const int extrapolation_index = reference_lookup_index == 0 ? 1 : reference_lookup_index - 1;
            return CoreUtils::LinearlyExtrapolate(
                extrapolation_index, Units::MetersLength(estimated_distance_to_path_end).value(),
                vertical_path.along_path_distance_m, vertical_path.mach);
      }

      if (reference_lookup_index == 0)
      {
            return vertical_path.mach.at(0);
      }
      else
      {
            return CoreUtils::LinearlyInterpolate(reference_lookup_index,
                                                   Units::MetersLength(estimated_distance_to_path_end).value(),
                                                   vertical_path.along_path_distance_m, vertical_path.mach);
      }

      throw std::out_of_range("Unable to calculate mach guidance; estimated distance is out of range and extrapolation is not allowed.");
}

inline Units::Time mitre::oss::simcore::VerticalPathUtils::CalculateTimeToFly(
    const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end,
    bool allow_extrapolation)
{
      auto reference_lookup_index = FindUpperBoundIndex(
          Units::MetersLength(estimated_distance_to_path_end).value(), vertical_path.along_path_distance_m);

      if (allow_extrapolation &&
          (reference_lookup_index == 0 ||
           static_cast<std::size_t>(reference_lookup_index) == vertical_path.along_path_distance_m.size()))
      {
            const int extrapolation_index = reference_lookup_index == 0 ? 1 : reference_lookup_index - 1;
            return Units::SecondsTime(CoreUtils::LinearlyExtrapolate(
                extrapolation_index, Units::MetersLength(estimated_distance_to_path_end).value(),
                vertical_path.along_path_distance_m, vertical_path.time_to_go_sec));
      }

      if (reference_lookup_index == 0)
      {
            return Units::SecondsTime(vertical_path.time_to_go_sec.at(0));
      }
      else
      {
            return Units::SecondsTime(CoreUtils::LinearlyInterpolate(
                reference_lookup_index, Units::MetersLength(estimated_distance_to_path_end).value(),
                vertical_path.along_path_distance_m, vertical_path.time_to_go_sec));
      }

      throw std::out_of_range("Unable to calculate time to fly; estimated distance is out of range and extrapolation is not allowed.");
}

inline Units::Mass mitre::oss::simcore::VerticalPathUtils::GetExpectedMass(
    const VerticalPath &vertical_path, Units::Length estimated_distance_to_path_end)
{
      auto reference_lookup_index = CoreUtils::FindNearestIndex(
          Units::MetersLength(estimated_distance_to_path_end).value(), vertical_path.along_path_distance_m);

      Units::Mass mass = Units::zero();
      if (reference_lookup_index == 0)
      {
            mass = Units::KilogramsMass(vertical_path.mass_kg[0]);
      }
      else
      {
            mass = Units::KilogramsMass(CoreUtils::LinearlyInterpolate(
                reference_lookup_index, Units::MetersLength(estimated_distance_to_path_end).value(),
                vertical_path.along_path_distance_m, vertical_path.mass_kg));
      }

      return mass;
}
