#include "platform/esp32/esp32_gpio_adapter.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace platform {
namespace esp32 {

void Esp32GpioAdapter::set_pin_mode(uint8_t pin, ports::PinMode mode) {
#ifdef ARDUINO
    uint8_t arduino_mode = OUTPUT;
    if (mode == ports::PinMode::INPUT_MODE) {
        arduino_mode = INPUT;
    } else if (mode == ports::PinMode::INPUT_PULLUP_MODE) {
        arduino_mode = INPUT_PULLUP;
    }
    pinMode(pin, arduino_mode);
#else
    (void)pin;
    (void)mode;
#endif
}

void Esp32GpioAdapter::write_pin(uint8_t pin, ports::PinState state) {
#ifdef ARDUINO
    digitalWrite(pin, (state == ports::PinState::HIGH_STATE) ? HIGH : LOW);
#else
    (void)pin;
    (void)state;
#endif
}

ports::PinState Esp32GpioAdapter::read_pin(uint8_t pin) {
#ifdef ARDUINO
    int val = digitalRead(pin);
    return (val == HIGH) ? ports::PinState::HIGH_STATE : ports::PinState::LOW_STATE;
#else
    (void)pin;
    return ports::PinState::LOW_STATE;
#endif
}

} // namespace esp32
} // namespace platform
