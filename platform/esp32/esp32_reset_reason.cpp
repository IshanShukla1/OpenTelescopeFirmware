#include "platform/esp32/esp32_reset_reason.h"

#ifdef ARDUINO
#include <esp_system.h>
#endif

namespace platform {
namespace esp32 {

const char* ResetReasonDetector::get_reset_reason_string() {
#ifdef ARDUINO
    esp_reset_reason_t reason = esp_reset_reason();
    switch (reason) {
        case ESP_RST_POWERON:   return "POWERON_RESET (Normal Cold Boot)";
        case ESP_RST_EXT:       return "EXT_RESET (External Reset Pin)";
        case ESP_RST_SW:        return "SW_RESET (Software Reset)";
        case ESP_RST_PANIC:     return "PANIC_RESET (Software Crash / Panic)";
        case ESP_RST_INT_WDT:   return "INT_WDT_RESET (Interrupt Watchdog)";
        case ESP_RST_TASK_WDT:  return "TASK_WDT_RESET (Task Watchdog)";
        case ESP_RST_WDT:       return "OTHER_WDT_RESET (Watchdog Reset)";
        case ESP_RST_DEEPSLEEP: return "DEEPSLEEP_RESET (Deep Sleep Wake)";
        case ESP_RST_BROWNOUT:  return "BROWNOUT_RESET (Brownout Voltage Drop)";
        case ESP_RST_SDIO:      return "SDIO_RESET (SDIO Reset)";
        default:                return "UNKNOWN_RESET";
    }
#else
    return "HOST_SIMULATED_POWERON_RESET";
#endif
}

} // namespace esp32
} // namespace platform
