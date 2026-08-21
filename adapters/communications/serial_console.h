#ifndef ADAPTERS_COMMUNICATIONS_SERIAL_CONSOLE_H
#define ADAPTERS_COMMUNICATIONS_SERIAL_CONSOLE_H

#include "core/result.h"
#include <cstddef>
#include <cstdint>

namespace adapters {
namespace communications {

constexpr size_t kSerialRxBufferSize = 128;

class SerialConsole {
public:
    SerialConsole() = default;

    void init(uint32_t baud_rate = 115200);

    // Non-blocking poll for incoming complete line (returns true when line is available)
    bool poll_line(char* out_line_buffer, size_t max_len);

    void print(const char* str);
    void print_line(const char* str);

private:
    char rx_buffer_[kSerialRxBufferSize]{0};
    size_t rx_idx_{0};
};

} // namespace communications
} // namespace adapters

#endif // ADAPTERS_COMMUNICATIONS_SERIAL_CONSOLE_H
