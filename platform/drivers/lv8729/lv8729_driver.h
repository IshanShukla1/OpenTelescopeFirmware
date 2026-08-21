#ifndef PLATFORM_DRIVERS_LV8729_LV8729_DRIVER_H
#define PLATFORM_DRIVERS_LV8729_LV8729_DRIVER_H

#include "ports/gpio_port.h"
#include <cstdint>

namespace platform {
namespace drivers {

class Lv8729Driver {
public:
    Lv8729Driver(ports::IGpioPort* gpio, int8_t step_pin, int8_t dir_pin, int8_t enable_pin);

    void init();
    void set_direction(bool forward, bool invert);
    void set_enabled(bool enabled);
    void pulse_step();

    bool is_enabled() const { return enabled_; }
    bool current_direction() const { return direction_forward_; }

private:
    ports::IGpioPort* gpio_;
    int8_t step_pin_;
    int8_t dir_pin_;
    int8_t enable_pin_;
    bool enabled_{false};
    bool direction_forward_{true};
};

} // namespace drivers
} // namespace platform

#endif // PLATFORM_DRIVERS_LV8729_LV8729_DRIVER_H
