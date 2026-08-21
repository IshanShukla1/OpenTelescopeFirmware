#ifndef DOMAIN_MOTION_MOTION_PLANNER_H
#define DOMAIN_MOTION_MOTION_PLANNER_H

#include "domain/motion/motor_axis.h"
#include "ports/motion_executor_port.h"
#include "ports/clock_port.h"
#include "ports/event_sink_port.h"
#include "core/result.h"

namespace domain {
namespace motion {

struct SingleAxisProfile {
    bool active{false};
    int32_t target_steps{0};
    int32_t steps_remaining{0};
    int32_t steps_executed{0};
    StepDirection dir{StepDirection::STOP};

    float max_vel_sps{1000.0f};
    float accel_spss{500.0f};

    uint32_t total_accel_steps{0};
    uint32_t total_decel_steps{0};
    uint32_t total_cruise_steps{0};

    uint64_t current_interval_us{1000};
    uint64_t min_interval_us{1000};
    uint64_t start_interval_us{5000};
    uint64_t last_step_timestamp_us{0};

    float current_velocity_sps{0.0f};
};

class MotionPlanner {
public:
    MotionPlanner(
        MotorAxis* ra_axis,
        MotorAxis* dec_axis,
        ports::IMotionExecutorPort* executor,
        ports::IClockPort* clock,
        ports::IEventSinkPort* event_sink = nullptr
    );

    core::Result<void> move_axis(ports::AxisId axis, int32_t steps, float max_vel_sps, float accel_spss, uint32_t correlation_id);
    core::Result<void> move_sync(int32_t ra_steps, int32_t dec_steps, float max_vel_sps, float accel_spss, uint32_t correlation_id);

    void stop_axis(ports::AxisId axis);
    void stop_all();
    void emergency_stop();

    // Main non-blocking update step (called repeatedly from loop/task)
    void update();

    bool is_moving() const { return ra_profile_.active || dec_profile_.active; }
    bool is_ra_moving() const { return ra_profile_.active; }
    bool is_dec_moving() const { return dec_profile_.active; }

private:
    void plan_profile(SingleAxisProfile& profile, int32_t steps, float max_vel_sps, float accel_spss);
    void process_axis_step(ports::AxisId axis, MotorAxis* axis_obj, SingleAxisProfile& profile, core::Microseconds now);

    MotorAxis* ra_axis_;
    MotorAxis* dec_axis_;
    ports::IMotionExecutorPort* executor_;
    ports::IClockPort* clock_;
    ports::IEventSinkPort* event_sink_;

    SingleAxisProfile ra_profile_{};
    SingleAxisProfile dec_profile_{};
    bool motion_inhibited_{false};
};

} // namespace motion
} // namespace domain

#endif // DOMAIN_MOTION_MOTION_PLANNER_H
