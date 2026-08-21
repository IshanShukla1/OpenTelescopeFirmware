#include "adapters/configuration/config_manager.h"

namespace adapters {
namespace configuration {

ConfigManager::ConfigManager(ports::IEventSinkPort* event_sink)
    : event_sink_(event_sink) {}

core::Result<void> ConfigManager::validate_configuration(const SystemConfig& config) const {
    // Check config version
    if (config.config_version == 0) {
        return core::Result<void>::Failure(core::StatusCode::CONFIG_ERROR);
    }

    // Validate GPIO pins are positive and valid for ESP32
    if (config.pins.ra_step_pin < 0 || config.pins.ra_dir_pin < 0 ||
        config.pins.dec_step_pin < 0 || config.pins.dec_dir_pin < 0) {
        return core::Result<void>::Failure(core::StatusCode::CONFIG_ERROR);
    }

    // Validate microstepping is standard power of two up to 128
    auto check_microstepping = [](uint16_t ms) {
        return ms == 1 || ms == 2 || ms == 4 || ms == 8 || ms == 16 || ms == 32 || ms == 64 || ms == 128;
    };
    if (!check_microstepping(config.ra_axis.microstepping) ||
        !check_microstepping(config.dec_axis.microstepping)) {
        return core::Result<void>::Failure(core::StatusCode::CONFIG_ERROR);
    }

    // Validate steps per revolution and velocities
    if (config.ra_axis.steps_per_revolution == 0 || config.dec_axis.steps_per_revolution == 0 ||
        config.ra_axis.max_velocity_sps <= 0.0f || config.dec_axis.max_velocity_sps <= 0.0f ||
        config.ra_axis.max_acceleration_spss <= 0.0f || config.dec_axis.max_acceleration_spss <= 0.0f) {
        return core::Result<void>::Failure(core::StatusCode::CONFIG_ERROR);
    }

    // Check safety speed boundaries
    if (config.ra_axis.max_velocity_sps > config.safety.max_allowed_speed_sps ||
        config.dec_axis.max_velocity_sps > config.safety.max_allowed_speed_sps) {
        return core::Result<void>::Failure(core::StatusCode::CONFIG_ERROR);
    }

    return core::Result<void>::Success();
}

core::Result<void> ConfigManager::load_configuration(const SystemConfig& config) {
    auto validation_result = validate_configuration(config);
    if (validation_result.is_error()) {
        config_loaded_ = false;
        if (event_sink_) {
            event_sink_->emit_event(
                "FLT-CONFIG-001",
                core::ErrorSeverity::CRITICAL_FAULT,
                core::ErrorSource::CONFIG,
                0,
                "Invalid configuration loaded: verification failed"
            );
        }
        return validation_result;
    }

    current_config_ = config;
    config_loaded_ = true;
    if (event_sink_) {
        event_sink_->emit_event(
            "EVT-CONFIG-001",
            core::ErrorSeverity::INFO,
            core::ErrorSource::CONFIG,
            0,
            "System configuration validated and loaded successfully"
        );
    }

    return core::Result<void>::Success();
}

} // namespace configuration
} // namespace adapters
