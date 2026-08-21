#ifndef PLATFORM_ESP32_ESP32_GPIO_ADAPTER_H
#define PLATFORM_ESP32_ESP32_GPIO_ADAPTER_H

#include "ports/gpio_port.h"

namespace platform {
namespace esp32 {

class Esp32GpioAdapter : public ports::IGpioPort {
public:
    Esp32GpioAdapter() = default;

    void set_pin_mode(uint8_t pin, ports::PinMode mode) override;
    void write_pin(uint8_t pin, ports::PinState state) override;
    ports::PinState read_pin(uint8_t pin) override;
};

} // namespace esp32
} // namespace platform

#endif // PLATFORM_ESP32_ESP32_GPIO_ADAPTER_H
