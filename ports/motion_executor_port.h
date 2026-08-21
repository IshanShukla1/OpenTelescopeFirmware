#ifndef PORTS_MOTION_EXECUTOR_PORT_H
#define PORTS_MOTION_EXECUTOR_PORT_H

#include "core/types.h"
#include <cstdint>

namespace ports {

enum class AxisId : uint8_t {
    RA = 0,
    DEC = 1
};

class IMotionExecutorPort {
public:
    virtual ~IMotionExecutorPort() = default;

    virtual void set_axis_direction(AxisId axis, bool forward) = 0;
    virtual void set_axis_enabled(AxisId axis, bool enabled) = 0;
    virtual void pulse_step(AxisId axis) = 0;
    virtual void disable_all_outputs() = 0;
};

} // namespace ports

#endif // PORTS_MOTION_EXECUTOR_PORT_H
