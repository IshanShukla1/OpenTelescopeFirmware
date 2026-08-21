#include "platform/esp32/esp32_step_executor.h"

namespace platform {
namespace esp32 {

Esp32StepExecutor::Esp32StepExecutor(drivers::Lv8729Driver* ra_driver, drivers::Lv8729Driver* dec_driver)
    : ra_driver_(ra_driver), dec_driver_(dec_driver) {}

void Esp32StepExecutor::set_axis_direction(ports::AxisId axis, bool forward) {
    if (axis == ports::AxisId::RA && ra_driver_) {
        ra_driver_->set_direction(forward, ra_invert_);
    } else if (axis == ports::AxisId::DEC && dec_driver_) {
        dec_driver_->set_direction(forward, dec_invert_);
    }
}

void Esp32StepExecutor::set_axis_enabled(ports::AxisId axis, bool enabled) {
    if (axis == ports::AxisId::RA && ra_driver_) {
        ra_driver_->set_enabled(enabled);
    } else if (axis == ports::AxisId::DEC && dec_driver_) {
        dec_driver_->set_enabled(enabled);
    }
}

void Esp32StepExecutor::pulse_step(ports::AxisId axis) {
    if (axis == ports::AxisId::RA && ra_driver_) {
        ra_driver_->pulse_step();
    } else if (axis == ports::AxisId::DEC && dec_driver_) {
        dec_driver_->pulse_step();
    }
}

void Esp32StepExecutor::disable_all_outputs() {
    if (ra_driver_) ra_driver_->set_enabled(false);
    if (dec_driver_) dec_driver_->set_enabled(false);
}

} // namespace esp32
} // namespace platform
