#ifndef ADAPTERS_CONFIGURATION_CONFIG_SCHEMA_H
#define ADAPTERS_CONFIGURATION_CONFIG_SCHEMA_H

#include <cstdint>

namespace adapters {
namespace configuration {

// Hardware Pin Assignments for LV8729 STEP/DIR drivers on ESP32
struct PinConfig {
    int8_t ra_step_pin;
    int8_t ra_dir_pin;
    int8_t ra_enable_pin;  // -1 if not wired
    int8_t dec_step_pin;
    int8_t dec_dir_pin;
    int8_t dec_enable_pin; // -1 if not wired
};

// Motor and Mount Axis Parameters
struct AxisConfig {
    uint16_t microstepping;       // e.g. 1, 2, 4, 8, 16, 32, 64, 128
    uint16_t steps_per_revolution;// e.g. 200 for 1.8 deg NEMA17
    float gear_ratio;             // Total reduction ratio
    bool invert_direction;        // Direction inversion
    float max_velocity_sps;       // Max velocity in steps/second
    float max_acceleration_spss;  // Max acceleration in steps/second^2
    int32_t min_step_limit;       // Soft minimum step position limit
    int32_t max_step_limit;       // Soft maximum step position limit
    bool enable_soft_limits;      // Soft limit enforcement active
};

// Safety Limits and Rules
struct SafetyConfig {
    bool enable_estop_latch;      // Critical fault latches system
    float default_accel_spss;     // Conservative startup acceleration limit
    float max_allowed_speed_sps;  // Global upper speed boundary
};

// Combined Versioned System Configuration
struct SystemConfig {
    uint32_t config_version;
    uint32_t config_fingerprint;  // Fingerprint for config validation
    PinConfig pins;
    AxisConfig ra_axis;
    AxisConfig dec_axis;
    SafetyConfig safety;
};

// Helper function to create valid default configuration for v0.1 hardware bring-up
inline SystemConfig CreateDefaultConfig() {
    SystemConfig cfg{};
    cfg.config_version = 1;
    cfg.config_fingerprint = 0x01000001;

    // ESP32 default GPIO assignments (configurable TBD)
    cfg.pins.ra_step_pin = 12;
    cfg.pins.ra_dir_pin = 14;
    cfg.pins.ra_enable_pin = 27;

    cfg.pins.dec_step_pin = 26;
    cfg.pins.dec_dir_pin = 25;
    cfg.pins.dec_enable_pin = 33;

    // RA Axis mechanics defaults (LV8729 + NEMA17)
    cfg.ra_axis.microstepping = 16;
    cfg.ra_axis.steps_per_revolution = 200;
    cfg.ra_axis.gear_ratio = 1.0f;
    cfg.ra_axis.invert_direction = false;
    cfg.ra_axis.max_velocity_sps = 2000.0f;       // Steps/sec
    cfg.ra_axis.max_acceleration_spss = 1000.0f;  // Steps/sec^2
    cfg.ra_axis.min_step_limit = -100000;
    cfg.ra_axis.max_step_limit = 100000;
    cfg.ra_axis.enable_soft_limits = false;       // Disabled for v0.1 untrusted steps

    // DEC Axis mechanics defaults (LV8729 + NEMA17)
    cfg.dec_axis.microstepping = 16;
    cfg.dec_axis.steps_per_revolution = 200;
    cfg.dec_axis.gear_ratio = 1.0f;
    cfg.dec_axis.invert_direction = false;
    cfg.dec_axis.max_velocity_sps = 2000.0f;
    cfg.dec_axis.max_acceleration_spss = 1000.0f;
    cfg.dec_axis.min_step_limit = -100000;
    cfg.dec_axis.max_step_limit = 100000;
    cfg.dec_axis.enable_soft_limits = false;

    // Safety defaults
    cfg.safety.enable_estop_latch = true;
    cfg.safety.default_accel_spss = 1000.0f;
    cfg.safety.max_allowed_speed_sps = 5000.0f;

    return cfg;
}

} // namespace configuration
} // namespace adapters

#endif // ADAPTERS_CONFIGURATION_CONFIG_SCHEMA_H
