#ifndef PORTS_GPIO_PORT_H
#define PORTS_GPIO_PORT_H

#include <cstdint>

namespace ports {

enum class PinMode : uint8_t {
    INPUT_MODE = 0,
    OUTPUT_MODE,
    INPUT_PULLUP_MODE
};

enum class PinState : uint8_t {
    LOW_STATE = 0,
    HIGH_STATE
};

class IGpioPort {
public:
    virtual ~IGpioPort() = default;

    virtual void set_pin_mode(uint8_t pin, PinMode mode) = 0;
    virtual void write_pin(uint8_t pin, PinState state) = 0;
    virtual PinState read_pin(uint8_t pin) = 0;
};

} // namespace ports

#endif // PORTS_GPIO_PORT_H
