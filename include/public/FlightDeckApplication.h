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

#include <memory>

#include "public/ASSAP.h"
#include "public/AircraftIntent.h"
#include "public/AircraftState.h"
#include "public/BadaUtils.h"
#include "public/DynamicsState.h"
#include "public/Guidance.h"
#include "public/TangentPlaneSequence.h"
#include "public/WeatherPrediction.h"

namespace mitre::oss::simcore {
struct OwnshipPerformanceParameters {
   mitre::oss::simcore::bada_utils::FlapSpeeds flap_speeds{};
   mitre::oss::simcore::bada_utils::FlightEnvelope flight_envelope{};
   mitre::oss::simcore::bada_utils::Mass mass_data{};
   mitre::oss::simcore::bada_utils::Aerodynamics aerodynamics{};
};

struct OwnshipFmsPredictionParameters {
   Units::Angle maximum_allowable_bank_angle{};
   Units::Speed transition_ias{};
   double transition_mach{};
   Units::Length transition_altitude{};
   Units::Length expected_cruise_altitude{};
   WeatherPrediction weather_prediction{};
   std::shared_ptr<const AircraftIntent> fms_intent{};
};

struct FlightDeckApplicationInitializer {
   virtual ~FlightDeckApplicationInitializer() = default;
   OwnshipFmsPredictionParameters fms_prediction_parameters{};
   OwnshipPerformanceParameters performance_parameters{};
   std::shared_ptr<const mitre::oss::simcore::ASSAP> surveillance_processor{};
   std::shared_ptr<TangentPlaneSequence> position_converter{};
};

struct FlightDeckApplication {
   virtual ~FlightDeckApplication() = default;
   virtual void Initialize(FlightDeckApplicationInitializer &initializer_visitor) = 0;
   virtual mitre::oss::simcore::Guidance Update(const mitre::oss::simcore::SimulationTime &simtime,
                                                const mitre::oss::simcore::Guidance &current_guidance,
                                                const mitre::oss::simcore::DynamicsState &dynamics_state,
                                                const mitre::oss::simcore::AircraftState &own_state) = 0;
   virtual bool IsActive() const = 0;
};
}  // namespace mitre::oss::simcore
