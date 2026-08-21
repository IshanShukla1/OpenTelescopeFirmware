#ifndef PORTS_CLOCK_PORT_H
#define PORTS_CLOCK_PORT_H

#include "core/types.h"

namespace ports {

class IClockPort {
public:
    virtual ~IClockPort() = default;

    virtual core::Microseconds now_us() = 0;
    virtual core::Milliseconds now_ms() = 0;
};

} // namespace ports

#endif // PORTS_CLOCK_PORT_H
