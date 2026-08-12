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

#include "public/PassThroughAssap.h"

#include <memory>

#include "public/NullADSBReceiver.h"

mitre::oss::simcore::PassThroughAssap::PassThroughAssap()
   : m_adsb_receiver(std::make_shared<mitre::oss::simcore::NullADSBReceiver>()) {}

mitre::oss::simcore::AircraftState mitre::oss::simcore::PassThroughAssap::Update(
      const mitre::oss::simcore::AircraftState &state_to_sync_with,
      const mitre::oss::simcore::ADSBSVReport &most_recent_ads_b) const {
   return mitre::oss::simcore::AircraftState::FromAdsbReport(most_recent_ads_b);
}

void mitre::oss::simcore::PassThroughAssap::Initialize(
      std::shared_ptr<const mitre::oss::simcore::ADSBReceiver> adsb_receiver) {
   m_adsb_receiver = adsb_receiver;
}
