#include "../include/aos_bda_ahj_lego_cortex.hpp"
#include <iostream>
#include <cassert>
#include <cstring>

int main() {
    std::cout << "========================================================\n";
    std::cout << "⟁ AOS DAS INTEGRATOR BDA & AHJ PERMITTING LEGO VERIFIER\n";
    std::cout << "========================================================\n";

    // Test Case 1: Fully Compliant FirstNet 700 MHz BDA Installation
    aos::BDARegistrationRecord bda_good{};
    bda_good.bda_id = 90210;
    std::strncpy(bda_good.fcc_registration_number, "BSTR-700-FIRSTNET-8821", 31);
    std::strncpy(bda_good.building_address, "100 Sovereign Way, Suite 400", 127);
    std::strncpy(bda_good.ahj_jurisdiction, "Metro Fire Prevention Bureau / AHJ-01", 63);
    bda_good.latitude = 37.6828;
    bda_good.longitude = -113.0619;
    bda_good.booster_class = aos::BoosterClass::CLASS_A_CHANNELIZED;
    bda_good.frequency_band = aos::FrequencyBand::BAND_700_FIRSTNET;
    bda_good.donor_azimuth_deg = 145.0f;
    bda_good.max_erp_watts = 2.5f;             // <= 5.0W limit (PASS)
    bda_good.uplink_noise_floor_dbm = -52.0f;  // <= -43 dBm/MHz (PASS)
    bda_good.battery_backup_hours = 24.0f;     // >= 12.0h NFPA requirement (PASS)
    bda_good.grid_coverage_pct = 98.5f;        // >= 95.0% IFC 510 requirement (PASS)
    bda_good.permit_status = aos::PermittingStatus::PERMITTED_ACTIVE;

    auto eval_good = aos::BDAPublicSafetyLegoCortex::evaluate_bda_installation(bda_good);
    std::cout << "[Permitted BDA] FCC Part 90=" << (eval_good.fcc_part90_compliant ? "PASS" : "FAIL")
              << " | IFC 510=" << (eval_good.ifc510_coverage_compliant ? "PASS" : "FAIL")
              << " | Noise=" << (eval_good.uplink_noise_clean ? "CLEAN" : "NOISY")
              << " | Clifford Wedge SAT: " << (eval_good.clifford_ahj_sat ? "APPROVED" : "VETOED")
              << " (" << eval_good.compliance_score_pct << "%)\n";
    assert(eval_good.clifford_ahj_sat == true);
    assert(eval_good.compliance_score_pct == 100.0f);

    // Test Case 2: Rogue Unpermitted BDA (Excessive uplink noise, no AHJ permit)
    aos::BDARegistrationRecord bda_rogue = bda_good;
    bda_rogue.uplink_noise_floor_dbm = -35.0f; // Violation! (Exceeds -43 dBm limit)
    bda_rogue.permit_status = aos::PermittingStatus::UNPERMITTED_ROGUE_RISK;

    auto eval_rogue = aos::BDAPublicSafetyLegoCortex::evaluate_bda_installation(bda_rogue);
    std::cout << "\n[Rogue BDA] FCC Part 90=" << (eval_rogue.fcc_part90_compliant ? "PASS" : "FAIL")
              << " | Noise=" << (eval_rogue.uplink_noise_clean ? "CLEAN" : "NOISY (-35dBm)")
              << " | Clifford Wedge SAT: " << (eval_rogue.clifford_ahj_sat ? "APPROVED" : "VETOED (ROGUE)")
              << " (" << eval_rogue.compliance_score_pct << "%)\n";
    assert(eval_rogue.clifford_ahj_sat == false);
    assert(eval_rogue.uplink_noise_clean == false);

    std::cout << "\n>>> ALL BDA / AHJ PERMITTING LEGO CHECKS 100% CERTIFIED <<<\n";
    return 0;
}
