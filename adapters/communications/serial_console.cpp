#include "adapters/communications/serial_console.h"
#include <cstdio>
#include <cstring>

#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace adapters {
namespace communications {

void SerialConsole::init(uint32_t baud_rate) {
#ifdef ARDUINO
    Serial.begin(baud_rate);
#else
    (void)baud_rate;
#endif
    rx_idx_ = 0;
    std::memset(rx_buffer_, 0, sizeof(rx_buffer_));
}

bool SerialConsole::poll_line(char* out_line_buffer, size_t max_len) {
    if (!out_line_buffer || max_len == 0) return false;

#ifdef ARDUINO
    while (Serial.available() > 0) {
        char c = static_cast<char>(Serial.read());
        if (c == '\r' || c == '\n') {
            if (rx_idx_ > 0) {
                rx_buffer_[rx_idx_] = '\0';
                std::strncpy(out_line_buffer, rx_buffer_, max_len - 1);
                out_line_buffer[max_len - 1] = '\0';
                rx_idx_ = 0;
                return true;
            }
        } else {
            if (rx_idx_ < kSerialRxBufferSize - 1) {
                rx_buffer_[rx_idx_++] = c;
            } else {
                // Buffer overflow protection: reset index
                rx_idx_ = 0;
            }
        }
    }
#else
    (void)out_line_buffer;
    (void)max_len;
#endif

    return false;
}

void SerialConsole::print(const char* str) {
    if (!str) return;
#ifdef ARDUINO
    Serial.print(str);
#else
    std::printf("%s", str);
#endif
}

void SerialConsole::print_line(const char* str) {
    if (!str) return;
#ifdef ARDUINO
    Serial.println(str);
#else
    std::printf("%s\n", str);
#endif
}

} // namespace communications
} // namespace adapters
