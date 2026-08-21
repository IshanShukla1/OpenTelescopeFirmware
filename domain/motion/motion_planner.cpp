#include "domain/motion/motion_planner.h"
#include <cmath>
#include <algorithm>

namespace domain {
namespace motion {

MotionPlanner::MotionPlanner(
    MotorAxis* ra_axis,
    MotorAxis* dec_axis,
    ports::IMotionExecutorPort* executor,
    ports::IClockPort* clock,
    ports::IEventSinkPort* event_sink
)   : ra_axis_(ra_axis),
      dec_axis_(dec_axis),
      executor_(executor),
      clock_(clock),
      event_sink_(event_sink) {}

void MotionPlanner::plan_profile(SingleAxisProfile& profile, int32_t steps, float max_vel_sps, float accel_spss) {
    profile.target_steps = steps;
    uint32_t abs_steps = std::abs(steps);
    profile.steps_remaining = abs_steps;
    profile.steps_executed = 0;
    profile.dir = (steps >= 0) ? StepDirection::FORWARD : StepDirection::BACKWARD;

    profile.max_vel_sps = max_vel_sps;
    profile.accel_spss = accel_spss;

    // Minimum interval at top speed (in microseconds)
    profile.min_interval_us = static_cast<uint64_t>(1000000.0f / max_vel_sps);
    // Initial interval for smooth starting step (start at 10% speed or minimum 2000us)
    float start_vel = std::min(max_vel_sps, std::max(50.0f, max_vel_sps * 0.1f));
    profile.start_interval_us = static_cast<uint64_t>(1000000.0f / start_vel);
    profile.current_interval_us = profile.start_interval_us;
    profile.current_velocity_sps = start_vel;

    // Calculate steps required to reach peak velocity: s = v^2 / (2 * a)
    uint32_t needed_accel_steps = static_cast<uint32_t>((max_vel_sps * max_vel_sps) / (2.0f * accel_spss));
    if (needed_accel_steps * 2 > abs_steps) {
        // Triangular profile (does not reach max speed)
        profile.total_accel_steps = abs_steps / 2;
        profile.total_decel_steps = abs_steps - profile.total_accel_steps;
        profile.total_cruise_steps = 0;
    } else {
        // Trapezoidal profile
        profile.total_accel_steps = needed_accel_steps;
        profile.total_decel_steps = needed_accel_steps;
        profile.total_cruise_steps = abs_steps - (2 * needed_accel_steps);
    }

    profile.last_step_timestamp_us = clock_ ? clock_->now_us() : 0;
    profile.active = (abs_steps > 0);
}

core::Result<void> MotionPlanner::move_axis(
    ports::AxisId axis, int32_t steps, float max_vel_sps, float accel_spss, uint32_t correlation_id
) {
    if (motion_inhibited_) {
        return core::Result<void>::Failure(core::StatusCode::SAFETY_INHIBITED);
    }
    if (steps == 0) {
        return core::Result<void>::Success();
    }
    if (max_vel_sps <= 0.0f || accel_spss <= 0.0f) {
        return core::Result<void>::Failure(core::StatusCode::INVALID_ARGUMENT);
    }

    MotorAxis* target_axis = (axis == ports::AxisId::RA) ? ra_axis_ : dec_axis_;
    SingleAxisProfile& profile = (axis == ports::AxisId::RA) ? ra_profile_ : dec_profile_;

    if (!target_axis || target_axis->is_faulted()) {
        return core::Result<void>::Failure(core::StatusCode::FAULT_ACTIVE);
    }

    executor_->set_axis_enabled(axis, true);
    target_axis->set_direction((steps >= 0) ? StepDirection::FORWARD : StepDirection::BACKWARD);
    target_axis->set_state(AxisState::ACCELERATING);

    plan_profile(profile, steps, max_vel_sps, accel_spss);

    if (event_sink_) {
        event_sink_->emit_event(
            "EVT-MOTION-001",
            core::ErrorSeverity::INFO,
            core::ErrorSource::MOTION,
            correlation_id,
            (axis == ports::AxisId::RA) ? "RA motion profile initialized" : "DEC motion profile initialized"
        );
    }

    return core::Result<void>::Success();
}

core::Result<void> MotionPlanner::move_sync(
    int32_t ra_steps, int32_t dec_steps, float max_vel_sps, float accel_spss, uint32_t correlation_id
) {
    if (motion_inhibited_) {
        return core::Result<void>::Failure(core::StatusCode::SAFETY_INHIBITED);
    }

    auto ra_res = move_axis(ports::AxisId::RA, ra_steps, max_vel_sps, accel_spss, correlation_id);
    if (ra_res.is_error() && ra_steps != 0) return ra_res;

    auto dec_res = move_axis(ports::AxisId::DEC, dec_steps, max_vel_sps, accel_spss, correlation_id);
    if (dec_res.is_error() && dec_steps != 0) return dec_res;

    return core::Result<void>::Success();
}

void MotionPlanner::stop_axis(ports::AxisId axis) {
    SingleAxisProfile& profile = (axis == ports::AxisId::RA) ? ra_profile_ : dec_profile_;
    MotorAxis* target_axis = (axis == ports::AxisId::RA) ? ra_axis_ : dec_axis_;

    if (profile.active) {
        profile.total_decel_steps = profile.steps_executed; // Initiate decel immediately
        if (target_axis) target_axis->set_state(AxisState::STOPPING);
    }
}

void MotionPlanner::stop_all() {
    stop_axis(ports::AxisId::RA);
    stop_axis(ports::AxisId::DEC);
}

void MotionPlanner::emergency_stop() {
    motion_inhibited_ = true;

    ra_profile_.active = false;
    dec_profile_.active = false;

    if (executor_) {
        executor_->disable_all_outputs();
    }
    if (ra_axis_) {
        ra_axis_->set_state(AxisState::DISABLED);
        ra_axis_->set_velocity(0.0f);
    }
    if (dec_axis_) {
        dec_axis_->set_state(AxisState::DISABLED);
        dec_axis_->set_velocity(0.0f);
    }

    if (event_sink_) {
        event_sink_->emit_event(
            "EVT-SAFETY-001",
            core::ErrorSeverity::CRITICAL_FAULT,
            core::ErrorSource::SAFETY,
            0,
            "EMERGENCY STOP TRIGGERED: Step outputs disabled immediately"
        );
    }
}

void MotionPlanner::process_axis_step(
    ports::AxisId axis, MotorAxis* axis_obj, SingleAxisProfile& profile, core::Microseconds now
) {
    if (!profile.active || !axis_obj) return;

    if (now - profile.last_step_timestamp_us >= profile.current_interval_us) {
        profile.last_step_timestamp_us = now;

        // Issue physical pulse to driver via executor port
        if (executor_) {
            executor_->pulse_step(axis);
        }

        // Update step counts
        int32_t step_delta = (profile.dir == StepDirection::FORWARD) ? 1 : -1;
        axis_obj->update_step_count(step_delta);
        profile.steps_executed++;
        profile.steps_remaining--;

        // Calculate state transition & step interval adjusting trapezoid
        uint32_t current_step = profile.steps_executed;
        uint32_t total = std::abs(profile.target_steps);

        if (profile.steps_remaining <= 0) {
            // Motion complete!
            profile.active = false;
            axis_obj->set_state(AxisState::IDLE);
            axis_obj->set_velocity(0.0f);
            axis_obj->set_direction(StepDirection::STOP);

            if (event_sink_) {
                event_sink_->emit_event(
                    "EVT-MOTION-002",
                    core::ErrorSeverity::INFO,
                    core::ErrorSource::MOTION,
                    0,
                    (axis == ports::AxisId::RA) ? "RA motion completed" : "DEC motion completed"
                );
            }
            return;
        }

        // Velocity ramping
        if (current_step <= profile.total_accel_steps) {
            // Acceleration phase
            axis_obj->set_state(AxisState::ACCELERATING);
            float progress = static_cast<float>(current_step) / std::max(1u, profile.total_accel_steps);
            profile.current_velocity_sps = std::max(50.0f, profile.max_vel_sps * progress);
        } else if (current_step > total - profile.total_decel_steps) {
            // Deceleration phase
            axis_obj->set_state(AxisState::DECELERATING);
            uint32_t decel_step_num = total - current_step;
            float progress = static_cast<float>(decel_step_num) / std::max(1u, profile.total_decel_steps);
            profile.current_velocity_sps = std::max(50.0f, profile.max_vel_sps * progress);
        } else {
            // Cruising phase
            axis_obj->set_state(AxisState::CRUISING);
            profile.current_velocity_sps = profile.max_vel_sps;
        }

        axis_obj->set_velocity(profile.current_velocity_sps);
        profile.current_interval_us = static_cast<uint64_t>(1000000.0f / profile.current_velocity_sps);
    }
}

void MotionPlanner::update() {
    if (!clock_) return;
    core::Microseconds now = clock_->now_us();

    if (ra_profile_.active) {
        process_axis_step(ports::AxisId::RA, ra_axis_, ra_profile_, now);
    }
    if (dec_profile_.active) {
        process_axis_step(ports::AxisId::DEC, dec_axis_, dec_profile_, now);
    }
}

} // namespace motion
} // namespace domain
