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

#include "public/ZeroWindTrueWeatherOperator.h"

#include <memory>

mitre::oss::simcore::ZeroWindTrueWeatherOperator::ZeroWindTrueWeatherOperator(
      std::shared_ptr<mitre::oss::simcore::WeatherTruth> true_weather)
   : AbstractTrueWeatherOperator(true_weather) {
   m_true_weather->east_west() = ZERO_STACK;
   m_true_weather->north_south() = ZERO_STACK;
}

void mitre::oss::simcore::ZeroWindTrueWeatherOperator::CalculateEnvironmentalWind(
      const EarthModel::GeodeticPosition &position, const Units::Length &altitude_msl) {
   m_true_weather->LoadConditionsAt(position.latitude, position.longitude, altitude_msl);
}
