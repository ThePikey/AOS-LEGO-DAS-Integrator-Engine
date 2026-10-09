#include "../include/aos_bda_ahj_lego_cortex.hpp"
#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>

int main(int argc, char** argv) {
    std::cout << "==================================================================\n";
    std::cout << "⟁ AOS LEGO SOVEREIGN BDA & AHJ PERMITTING ENGINE (DAS INTEGRATOR)\n";
    std::cout << "==================================================================\n";
    std::cout << "Architecture: 100% C++20 SIMD | 0.000 MB VRAM | 0-Token Deterministic\n";
    std::cout << "Target: FCC Part 90.219 / IFC 510 / NFPA 1225 Radio Grid Intelligence\n\n";

    // Ingest sample DAS Integrator BDA database into cache-aligned vector
    std::vector<aos::BDARegistrationRecord> bda_registry(3);

    // Record 0: Fully Compliant Medical Center
    bda_registry[0].bda_id = 1001;
    std::strncpy(bda_registry[0].fcc_registration_number, "FCC-BSTR-700-0982", 31);
    std::strncpy(bda_registry[0].building_address, "500 Regional Health Blvd", 127);
    std::strncpy(bda_registry[0].ahj_jurisdiction, "County Fire Prevention Bureau", 63);
    bda_registry[0].latitude = 43.2165;
    bda_registry[0].longitude = -123.3417;
    bda_registry[0].booster_class = aos::BoosterClass::CLASS_A_CHANNELIZED;
    bda_registry[0].frequency_band = aos::FrequencyBand::BAND_700_FIRSTNET;
    bda_registry[0].donor_azimuth_deg = 210.0f;
    bda_registry[0].max_erp_watts = 3.0f;
    bda_registry[0].uplink_noise_floor_dbm = -55.0f; // Clean
    bda_registry[0].battery_backup_hours = 24.0f;
    bda_registry[0].grid_coverage_pct = 99.2f;
    bda_registry[0].permit_status = aos::PermittingStatus::PERMITTED_ACTIVE;

    // Record 1: Rogue Unpermitted Industrial Distribution Center (High Uplink Noise)
    bda_registry[1].bda_id = 1002;
    std::strncpy(bda_registry[1].fcc_registration_number, "FCC-BSTR-800-4410", 31);
    std::strncpy(bda_registry[1].building_address, "880 Heavy Freight Way", 127);
    std::strncpy(bda_registry[1].ahj_jurisdiction, "City Fire Marshal Bureau", 63);
    bda_registry[1].latitude = 43.1950;
    bda_registry[1].longitude = -123.3600;
    bda_registry[1].booster_class = aos::BoosterClass::CLASS_B_BROADBAND;
    bda_registry[1].frequency_band = aos::FrequencyBand::BAND_800_NPSPAC;
    bda_registry[1].donor_azimuth_deg = 45.0f;
    bda_registry[1].max_erp_watts = 5.0f;
    bda_registry[1].uplink_noise_floor_dbm = -32.0f; // ROGUE INTERFERENCE!
    bda_registry[1].battery_backup_hours = 4.0f;
    bda_registry[1].grid_coverage_pct = 88.0f;
    bda_registry[1].permit_status = aos::PermittingStatus::UNPERMITTED_ROGUE_RISK;

    // Record 2: Expired Permit High-Rise (Overdue Annual Recertification)
    bda_registry[2].bda_id = 1003;
    std::strncpy(bda_registry[2].fcc_registration_number, "FCC-BSTR-700-5591", 31);
    std::strncpy(bda_registry[2].building_address, "120 Corporate Center Dr", 127);
    std::strncpy(bda_registry[2].ahj_jurisdiction, "County Fire Prevention Bureau", 63);
    bda_registry[2].latitude = 43.2210;
    bda_registry[2].longitude = -123.3500;
    bda_registry[2].booster_class = aos::BoosterClass::CLASS_A_CHANNELIZED;
    bda_registry[2].frequency_band = aos::FrequencyBand::BAND_700_FIRSTNET;
    bda_registry[2].donor_azimuth_deg = 180.0f;
    bda_registry[2].max_erp_watts = 2.0f;
    bda_registry[2].uplink_noise_floor_dbm = -48.0f;
    bda_registry[2].battery_backup_hours = 12.0f;
    bda_registry[2].grid_coverage_pct = 95.5f;
    bda_registry[2].permit_status = aos::PermittingStatus::PERMIT_EXPIRED;

    auto t0 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < bda_registry.size(); ++i) {
        auto eval = aos::BDAPublicSafetyLegoCortex::evaluate_bda_installation(bda_registry[i]);
        std::cout << "--- [BDA Record #" << bda_registry[i].bda_id << ": " << bda_registry[i].building_address << "] ---\n";
        std::cout << "    • FCC Booster ID : " << bda_registry[i].fcc_registration_number << "\n";
        std::cout << "    • AHJ Authority  : " << bda_registry[i].ahj_jurisdiction << "\n";
        std::cout << "    • Uplink Noise   : " << bda_registry[i].uplink_noise_floor_dbm << " dBm/MHz (" 
                  << (eval.uplink_noise_clean ? "CLEAN" : "CRITICAL ROGUE INTERFERENCE") << ")\n";
        std::cout << "    • Grid Coverage  : " << bda_registry[i].grid_coverage_pct << "% (IFC 510 Min: 95%)\n";
        std::cout << "    • Battery Backup : " << bda_registry[i].battery_backup_hours << " hrs (NFPA 1225 Min: 12h)\n";
        std::cout << "    • Clifford SAT   : " << (eval.clifford_ahj_sat ? "APPROVED & FULLY PERMITTED" : "CODE VIOLATION VETOED")
                  << " [Score: " << eval.compliance_score_pct << "%]\n\n";
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed_ns = std::chrono::duration<double, std::nano>(t1 - t0).count();

    std::cout << "==================================================================\n";
    std::cout << ">>> EVALUATED ALL BDA SITES IN " << (elapsed_ns / bda_registry.size()) << " ns / SITE ON SILICON <<<\n";
    std::cout << "==================================================================\n";
    return 0;
}
