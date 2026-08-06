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

#include "public/EuclideanWaypointMonitor.h"

#include <memory>

#include "public/AircraftCalculations.h"

mitre::oss::simcore::EuclideanWaypointMonitor::EuclideanWaypointMonitor(
      const mitre::oss::simcore::LatitudeLongitudePoint &lat_lon_point)
   : m_point_to_monitor(
           mitre::oss::simcore::LatitudeLongitudePoint::CreateFromGeolibPrimitive(lat_lon_point.GetGeolibPrimitiveLLPoint())) {};

void mitre::oss::simcore::EuclideanWaypointMonitor::Update(const mitre::oss::simcore::LatitudeLongitudePoint &position,
                                                           const Units::SignedAngle &ground_course_enu) {
   // Treat the lat/lon values as y/x values
   Units::Length current_position_x(Units::infinity()), current_position_y(Units::infinity());
   PerformFakeTranslationToEuclidean(position, current_position_x, current_position_y);

   Units::Length predicted_next_x(Units::infinity()), predicted_next_y(Units::infinity());
   const LatitudeLongitudePoint projected_lat_lon =
         position.ProjectDistanceAlongCourse(Units::MetersLength(500), ground_course_enu);
   PerformFakeTranslationToEuclidean(projected_lat_lon, predicted_next_x, predicted_next_y);

   Units::Length monitored_point_x(Units::infinity()), monitored_point_y(Units::infinity());
   PerformFakeTranslationToEuclidean(m_point_to_monitor, monitored_point_x, monitored_point_y);

   const Units::SignedAngle angle =
         AircraftCalculations::ComputeAngleBetweenVectors(current_position_x, current_position_y, predicted_next_x,
                                                          predicted_next_y, monitored_point_x, monitored_point_y);
   m_is_passed_waypoint = Units::abs(angle) > Units::PI_RADIANS_ANGLE / 2;
}

void mitre::oss::simcore::EuclideanWaypointMonitor::PerformFakeTranslationToEuclidean(
      const mitre::oss::simcore::LatitudeLongitudePoint &lat_lon_point, Units::Length &x, Units::Length &y) {
   x = Units::MetersLength(Units::SignedRadiansAngle(lat_lon_point.GetLongitude()).value());
   y = Units::MetersLength(Units::SignedRadiansAngle(lat_lon_point.GetLatitude()).value());
}

std::shared_ptr<mitre::oss::simcore::EuclideanWaypointMonitor>
      mitre::oss::simcore::EuclideanWaypointMonitor::OfWgs84PrecalcWaypoint(
            const mitre::oss::simcore::Wgs84PrecalcWaypoint &waypoint) {
   return std::make_shared<mitre::oss::simcore::EuclideanWaypointMonitor>(
         mitre::oss::simcore::EuclideanWaypointMonitor(waypoint.m_position));
}

std::shared_ptr<mitre::oss::simcore::EuclideanWaypointMonitor>
      mitre::oss::simcore::EuclideanWaypointMonitor::OfEllipsoidalPoint(
            const mitre::oss::simcore::LatitudeLongitudePoint &ellipsoidal_point) {
   return std::make_shared<mitre::oss::simcore::EuclideanWaypointMonitor>(
         mitre::oss::simcore::EuclideanWaypointMonitor(ellipsoidal_point));
}

std::shared_ptr<mitre::oss::simcore::EuclideanWaypointMonitor>
      mitre::oss::simcore::EuclideanWaypointMonitor::OfGeodeticPoint(
            const EarthModel::GeodeticPosition &geodetic_point) {
   const LatitudeLongitudePoint monitor_this(geodetic_point.latitude, geodetic_point.longitude);
   return std::make_shared<mitre::oss::simcore::EuclideanWaypointMonitor>(
         mitre::oss::simcore::EuclideanWaypointMonitor(monitor_this));
}
