#ifndef PLATFORM_ESP32_ESP32_RESET_REASON_H
#define PLATFORM_ESP32_ESP32_RESET_REASON_H

namespace platform {
namespace esp32 {

class ResetReasonDetector {
public:
    static const char* get_reset_reason_string();
};

} // namespace esp32
} // namespace platform

#endif // PLATFORM_ESP32_ESP32_RESET_REASON_H
