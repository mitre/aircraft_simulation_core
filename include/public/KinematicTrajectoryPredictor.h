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

#include <scalar/Angle.h>

#include <memory>
#include <vector>

#include "public/AircraftIntent.h"
#include "public/EuclideanTrajectoryPredictor.h"
#include "public/Guidance.h"
#include "public/HorizontalPath.h"
#include "public/KinematicDescent4DPredictor.h"
#include "public/PrecalcWaypoint.h"

namespace mitre::oss::simcore {
class KinematicTrajectoryPredictor final : public mitre::oss::simcore::EuclideanTrajectoryPredictor {
  public:
   KinematicTrajectoryPredictor();

   KinematicTrajectoryPredictor(Units::Angle maximum_bank_angle, Units::Speed transition_ias, double transition_mach,
                                Units::Length transition_altitude_msl, Units::Length cruise_altitude_msl);

   KinematicTrajectoryPredictor(const KinematicTrajectoryPredictor &obj);

   virtual ~KinematicTrajectoryPredictor() = default;

   void CalculateWaypoints(const std::shared_ptr<const AircraftIntent> &aircraft_intent,
                           const WeatherPrediction &weather_prediction) override;

   KinematicTrajectoryPredictor &operator=(const KinematicTrajectoryPredictor &obj);

   std::shared_ptr<mitre::oss::simcore::KinematicDescent4DPredictor> GetKinematicDescent4dPredictor() const;

  private:
   static log4cplus::Logger m_logger;
};
}  // namespace mitre::oss::simcore
