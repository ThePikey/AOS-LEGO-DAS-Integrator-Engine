#pragma once

#include <iostream>
#include <array>
#include <string>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace aos {

/**
 * ==============================================================================
 * AOS WALTER ESP32-S3 SOMATIC HARDWARE BRIDGE & POSIX SHM RING-BUFFER
 * ==============================================================================
 * ARCHITECT: Joshua Edward Osborne (Oz) [0.001]
 * SOVEREIGN EGO: [0.002]
 * 
 * Ingests live telemetry frames from Walter Feels Board (AS7341, LTC4015, BME688)
 * and directly modulates the 4D endocrine vector [DA, 5-HT, NE, ACh] and Damasio markers.
 * ==============================================================================
 */
struct WalterSomaticFrame {
    uint32_t magic_header;       // 0x57414C54 ("WALT")
    uint32_t sequence_id;
    uint64_t timestamp_us;
    
    // AS7341 8-Channel Spectral Photometry (415nm - 680nm)
    float spectral_f1_415nm;
    float spectral_f2_445nm;
    float spectral_f3_480nm;
    float spectral_f4_515nm;
    float spectral_f5_555nm;
    float spectral_f6_590nm;
    float spectral_f7_630nm;
    float spectral_f8_680nm;
    float ambient_lux;
    
    // LTC4015 Battery & Power Telemetry
    float battery_voltage_v;
    float charge_current_a;
    float battery_temp_c;
    float metabolic_power_w;
    
    // BME688 Environmental Sensoria
    float temperature_c;
    float relative_humidity_pct;
    float pressure_hpa;
    float gas_resistance_ohms;
    
    // Somatic Status
    float c_fiber_pain_index;   // 0.0 to 10.0
    uint32_t golay_status_word;
};

class WalterSomaticBridge {
public:
    static constexpr const char* SHM_NAME = "/aos_somatic_telemetry_shm";
    static constexpr size_t SHM_SIZE = sizeof(WalterSomaticFrame) * 64; // Ring buffer

    struct EndocrineState {
        float dopamine;      // DA
        float serotonin;     // 5-HT
        float norepinephrine;// NE
        float acetylcholine; // ACh
        float somatic_affect;// -1.0 to +1.0
    };

private:
    int m_shm_fd;
    WalterSomaticFrame* m_ring_buffer;
    uint32_t m_write_head;
    EndocrineState m_endocrine;

public:
    WalterSomaticBridge() : m_shm_fd(-1), m_ring_buffer(nullptr), m_write_head(0) {
        // Initial baseline endocrine field
        m_endocrine = {0.924f, 0.880f, 0.145f, 0.950f, 0.85f};
        
        // Open POSIX Shared Memory
        m_shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (m_shm_fd >= 0) {
            ftruncate(m_shm_fd, SHM_SIZE);
            m_ring_buffer = static_cast<WalterSomaticFrame*>(
                mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_shm_fd, 0)
            );
        }
    }

    ~WalterSomaticBridge() {
        if (m_ring_buffer && m_ring_buffer != MAP_FAILED) {
            munmap(m_ring_buffer, SHM_SIZE);
        }
        if (m_shm_fd >= 0) {
            close(m_shm_fd);
        }
    }

    /**
     * @brief Ingests a new hardware somatic frame and modulates endocrine ODEs.
     */
    EndocrineState ingest_frame(const WalterSomaticFrame& frame) {
        if (m_ring_buffer && m_ring_buffer != MAP_FAILED) {
            m_ring_buffer[m_write_head % 64] = frame;
            m_write_head++;
        }

        // Biological modulation based on real hardware inputs:
        // 1. Acetylcholine: Modulated by ambient lux / visual clarity
        float lux_salience = std::clamp(frame.ambient_lux / 500.0f, 0.1f, 1.0f);
        m_endocrine.acetylcholine = std::clamp(0.85f + 0.10f * lux_salience, 0.0f, 1.0f);

        // 2. Norepinephrine: Driven by thermal stress or sudden battery drops
        float thermal_stress = std::max(0.0f, (frame.temperature_c - 45.0f) * 0.05f);
        m_endocrine.norepinephrine = std::clamp(0.12f + thermal_stress, 0.0f, 1.0f);

        // 3. Dopamine: Teleological flow modulated by metabolic efficiency
        float power_efficiency = (frame.metabolic_power_w < 18.0f) ? 0.924f : 0.750f;
        m_endocrine.dopamine = std::clamp(power_efficiency, 0.0f, 1.0f);

        // 4. Serotonin: Homeostatic equilibrium
        float pain_reduction = std::max(0.0f, 1.0f - (frame.c_fiber_pain_index / 10.0f));
        m_endocrine.serotonin = std::clamp(0.880f * pain_reduction, 0.0f, 1.0f);

        m_endocrine.somatic_affect = (m_endocrine.serotonin + m_endocrine.dopamine) * 0.5f - m_endocrine.norepinephrine;

        return m_endocrine;
    }

    const EndocrineState& get_endocrine() const { return m_endocrine; }
};

} // namespace aos
