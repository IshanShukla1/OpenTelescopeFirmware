#ifndef DOMAIN_MOTION_MOTOR_AXIS_H
#define DOMAIN_MOTION_MOTOR_AXIS_H

#include "domain/motion/motion_types.h"
#include "adapters/configuration/config_schema.h"
#include "core/types.h"
#include "ports/motion_executor_port.h"

namespace domain {
namespace motion {

/**
 * @brief Reusable Axis Abstraction for Telescope Mount Axes (RA and DEC).
 * 
 * IMPORTANT NOTE: For Firmware v0.1, axis position is simply a firmware motion count in steps.
 * This is NOT yet a calibrated celestial/mount coordinate (RA/DEC angle).
 */
class MotorAxis {
public:
    MotorAxis(ports::AxisId axis_id, ports::IMotionExecutorPort* executor);

    void configure(const adapters::configuration::AxisConfig& config);
    
    // Position manipulation (steps only)
    void reset_position(int32_t step_position = 0);
    void update_step_count(int32_t step_delta);

    // State mutators
    void set_state(AxisState new_state);
    void set_velocity(float vel_sps);
    void set_direction(StepDirection dir);
    void set_fault(bool faulted);

    // Telemetry getters
    ports::AxisId axis_id() const { return axis_id_; }
    AxisState state() const { return state_; }
    int32_t position_steps() const { return current_position_steps_; }
    float current_velocity_sps() const { return current_velocity_sps_; }
    StepDirection direction() const { return current_direction_; }
    core::PositionTrust position_trust() const { return position_trust_; }
    bool is_faulted() const { return faulted_; }

    void set_position_trust(core::PositionTrust trust) { position_trust_ = trust; }

    AxisTelemetry get_telemetry() const;
    const char* state_to_string() const;

private:
    ports::AxisId axis_id_;
    ports::IMotionExecutorPort* executor_;
    adapters::configuration::AxisConfig config_{};
    
    AxisState state_{AxisState::IDLE};
    int32_t current_position_steps_{0};
    int32_t target_position_steps_{0};
    float current_velocity_sps_{0.0f};
    StepDirection current_direction_{StepDirection::STOP};
    core::PositionTrust position_trust_{core::PositionTrust::UNTRUSTED};
    bool faulted_{false};
};

} // namespace motion
} // namespace domain

#endif // DOMAIN_MOTION_MOTOR_AXIS_H
