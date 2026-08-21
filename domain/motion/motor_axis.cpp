#include "domain/motion/motor_axis.h"

namespace domain {
namespace motion {

MotorAxis::MotorAxis(ports::AxisId axis_id, ports::IMotionExecutorPort* executor)
    : axis_id_(axis_id), executor_(executor) {}

void MotorAxis::configure(const adapters::configuration::AxisConfig& config) {
    config_ = config;
    if (executor_) {
        // Set hardware direction inversion if applicable
        if (axis_id_ == ports::AxisId::RA) {
            // Managed via step executor adapter settings if needed
        }
    }
}

void MotorAxis::reset_position(int32_t step_position) {
    current_position_steps_ = step_position;
}

void MotorAxis::update_step_count(int32_t step_delta) {
    current_position_steps_ += step_delta;
}

void MotorAxis::set_state(AxisState new_state) {
    state_ = new_state;
    if (state_ == AxisState::FAULTED) {
        faulted_ = true;
    }
}

void MotorAxis::set_velocity(float vel_sps) {
    current_velocity_sps_ = vel_sps;
}

void MotorAxis::set_direction(StepDirection dir) {
    current_direction_ = dir;
    if (executor_) {
        if (dir == StepDirection::FORWARD) {
            executor_->set_axis_direction(axis_id_, true);
        } else if (dir == StepDirection::BACKWARD) {
            executor_->set_axis_direction(axis_id_, false);
        }
    }
}

void MotorAxis::set_fault(bool faulted) {
    faulted_ = faulted;
    if (faulted) {
        state_ = AxisState::FAULTED;
    }
}

AxisTelemetry MotorAxis::get_telemetry() const {
    AxisTelemetry t{};
    t.state = state_;
    t.position_steps = current_position_steps_;
    t.target_steps = target_position_steps_;
    t.current_velocity_sps = current_velocity_sps_;
    t.direction = current_direction_;
    t.is_moving = (state_ == AxisState::ACCELERATING || state_ == AxisState::CRUISING ||
                   state_ == AxisState::DECELERATING || state_ == AxisState::STOPPING);
    t.fault_active = faulted_;
    return t;
}

const char* MotorAxis::state_to_string() const {
    switch (state_) {
        case AxisState::IDLE:         return "IDLE";
        case AxisState::ACCELERATING: return "ACCELERATING";
        case AxisState::CRUISING:     return "CRUISING";
        case AxisState::DECELERATING: return "DECELERATING";
        case AxisState::STOPPING:     return "STOPPING";
        case AxisState::FAULTED:      return "FAULTED";
        case AxisState::DISABLED:     return "DISABLED";
        default:                      return "UNKNOWN";
    }
}

} // namespace motion
} // namespace domain
