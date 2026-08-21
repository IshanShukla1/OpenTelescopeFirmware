#include "platform/drivers/lv8729/lv8729_driver.h"

namespace platform {
namespace drivers {

Lv8729Driver::Lv8729Driver(ports::IGpioPort* gpio, int8_t step_pin, int8_t dir_pin, int8_t enable_pin)
    : gpio_(gpio), step_pin_(step_pin), dir_pin_(dir_pin), enable_pin_(enable_pin) {}

void Lv8729Driver::init() {
    if (!gpio_) return;

    if (step_pin_ >= 0) {
        gpio_->set_pin_mode(static_cast<uint8_t>(step_pin_), ports::PinMode::OUTPUT_MODE);
        gpio_->write_pin(static_cast<uint8_t>(step_pin_), ports::PinState::LOW_STATE);
    }
    if (dir_pin_ >= 0) {
        gpio_->set_pin_mode(static_cast<uint8_t>(dir_pin_), ports::PinMode::OUTPUT_MODE);
        gpio_->write_pin(static_cast<uint8_t>(dir_pin_), ports::PinState::LOW_STATE);
    }
    if (enable_pin_ >= 0) {
        gpio_->set_pin_mode(static_cast<uint8_t>(enable_pin_), ports::PinMode::OUTPUT_MODE);
        // LV8729 active LOW enable: start disabled (HIGH state) for safe startup state
        gpio_->write_pin(static_cast<uint8_t>(enable_pin_), ports::PinState::HIGH_STATE);
    }
    enabled_ = false;
}

void Lv8729Driver::set_direction(bool forward, bool invert) {
    direction_forward_ = forward;
    if (!gpio_ || dir_pin_ < 0) return;

    bool pin_high = forward;
    if (invert) {
        pin_high = !pin_high;
    }

    gpio_->write_pin(
        static_cast<uint8_t>(dir_pin_),
        pin_high ? ports::PinState::HIGH_STATE : ports::PinState::LOW_STATE
    );
}

void Lv8729Driver::set_enabled(bool enabled) {
    enabled_ = enabled;
    if (!gpio_ || enable_pin_ < 0) return;

    // LV8729 enable pin is active LOW: LOW = active, HIGH = disabled
    gpio_->write_pin(
        static_cast<uint8_t>(enable_pin_),
        enabled ? ports::PinState::LOW_STATE : ports::PinState::HIGH_STATE
    );
}

void Lv8729Driver::pulse_step() {
    if (!gpio_ || step_pin_ < 0 || !enabled_) return;

    // Generate step pulse (HIGH -> LOW transition)
    gpio_->write_pin(static_cast<uint8_t>(step_pin_), ports::PinState::HIGH_STATE);
    // Short pulse duration
    gpio_->write_pin(static_cast<uint8_t>(step_pin_), ports::PinState::LOW_STATE);
}

} // namespace drivers
} // namespace platform
