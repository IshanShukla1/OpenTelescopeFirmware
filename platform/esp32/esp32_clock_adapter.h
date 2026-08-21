#ifndef PLATFORM_ESP32_ESP32_CLOCK_ADAPTER_H
#define PLATFORM_ESP32_ESP32_CLOCK_ADAPTER_H

#include "ports/clock_port.h"

namespace platform {
namespace esp32 {

class Esp32ClockAdapter : public ports::IClockPort {
public:
    Esp32ClockAdapter() = default;

    core::Microseconds now_us() override;
    core::Milliseconds now_ms() override;
};

} // namespace esp32
} // namespace platform

#endif // PLATFORM_ESP32_ESP32_CLOCK_ADAPTER_H
