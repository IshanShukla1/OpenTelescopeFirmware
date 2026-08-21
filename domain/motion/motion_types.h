#ifndef DOMAIN_MOTION_MOTION_TYPES_H
#define DOMAIN_MOTION_MOTION_TYPES_H

#include <cstdint>

namespace domain {
namespace motion {

enum class AxisState : uint8_t {
    IDLE = 0,
    ACCELERATING,
    CRUISING,
    DECELERATING,
    STOPPING,
    FAULTED,
    DISABLED
};

enum class StepDirection : int8_t {
    BACKWARD = -1,
    STOP = 0,
    FORWARD = 1
};

struct MotionCommand {
    int32_t target_steps;      // Relative step displacement
    float max_velocity_sps;    // Steps per second
    float acceleration_spss;   // Steps per second^2
    uint32_t correlation_id;
};

// State representation for motor axis telemetry snapshot
struct AxisTelemetry {
    AxisState state;
    int32_t position_steps;
    int32_t target_steps;
    float current_velocity_sps;
    StepDirection direction;
    bool is_moving;
    bool fault_active;
};

} // namespace motion
} // namespace domain

#endif // DOMAIN_MOTION_MOTION_TYPES_H
