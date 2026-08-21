#include "platform/esp32/esp32_clock_adapter.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <chrono>
#endif

namespace platform {
namespace esp32 {

core::Microseconds Esp32ClockAdapter::now_us() {
#ifdef ARDUINO
    return static_cast<core::Microseconds>(micros());
#else
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
#endif
}

core::Milliseconds Esp32ClockAdapter::now_ms() {
#ifdef ARDUINO
    return static_cast<core::Milliseconds>(millis());
#else
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
#endif
}

} // namespace esp32
} // namespace platform
