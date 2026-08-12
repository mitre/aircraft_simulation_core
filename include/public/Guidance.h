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

#include <cmath>
#include <string>

#include "public/AircraftSpeed.h"
#include "public/PrecalcWaypoint.h"

namespace mitre::oss::simcore
{
   enum GuidanceFlightPhase
   {
      TAKEOFF_ROLL,
      CLIMB,
      CRUISE_DESCENT
   };

   static std::string GuidanceFlightPhaseAsString(GuidanceFlightPhase guidance_flight_phase)
   {
      switch (guidance_flight_phase)
      {
      case mitre::oss::simcore::GuidanceFlightPhase::TAKEOFF_ROLL:
         return "TAKEOFF_ROLL";
      case mitre::oss::simcore::GuidanceFlightPhase::CLIMB:
         return "CLIMB";
      case mitre::oss::simcore::GuidanceFlightPhase::CRUISE_DESCENT:
         return "CRUISE_DESCENT";
      default:
         throw std::logic_error("Invalid guidance flight phase encountered: " + std::to_string(guidance_flight_phase));
      }
   }

   class Guidance
   {
   public:
      Guidance() = default;

      virtual ~Guidance() = default;

      void SetValid(bool value);

      const bool IsValid() const;

      SpeedValueType GetSelectedSpeedType() const;

      void SetSelectedSpeedType(SpeedValueType selected_speed_type);

      double GetMachCommand() const;

      void SetMachCommand(double mach_value);

      PrecalcConstraint m_active_precalc_constraints;

      Units::Speed m_ias_command{Units::ZERO_SPEED};
      double m_mach_command{0};
      Units::Speed m_ground_speed{Units::ZERO_SPEED};
      Units::Speed m_vertical_speed{Units::ZERO_SPEED};
      Units::Length m_reference_altitude{Units::ZERO_LENGTH};
      Units::Length m_cross_track_error{Units::ZERO_LENGTH};
      Units::Angle m_reference_bank_angle{Units::ZERO_ANGLE};
      Units::SignedAngle m_enu_track_angle{Units::ZERO_ANGLE};
      GuidanceFlightPhase m_active_guidance_phase{GuidanceFlightPhase::TAKEOFF_ROLL};

      bool m_use_cross_track{false};

   private:
      bool m_valid{false};
      SpeedValueType m_selected_speed_type{UNSPECIFIED_SPEED};
   };

   inline void Guidance::SetValid(bool value) { m_valid = value; }

   inline const bool Guidance::IsValid() const { return m_valid; }

   inline SpeedValueType Guidance::GetSelectedSpeedType() const { return m_selected_speed_type; }

   inline void Guidance::SetSelectedSpeedType(SpeedValueType selected_speed_type)
   {
      m_selected_speed_type = selected_speed_type;
   }

   inline double Guidance::GetMachCommand() const { return m_mach_command; }

   inline void Guidance::SetMachCommand(double mach_value) { m_mach_command = mach_value; }

} // namespace mitre::oss::simcore
