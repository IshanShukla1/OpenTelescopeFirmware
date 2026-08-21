#ifndef PLATFORM_ESP32_ESP32_STEP_EXECUTOR_H
#define PLATFORM_ESP32_ESP32_STEP_EXECUTOR_H

#include "ports/motion_executor_port.h"
#include "platform/drivers/lv8729/lv8729_driver.h"

namespace platform {
namespace esp32 {

class Esp32StepExecutor : public ports::IMotionExecutorPort {
public:
    Esp32StepExecutor(drivers::Lv8729Driver* ra_driver, drivers::Lv8729Driver* dec_driver);

    void set_axis_direction(ports::AxisId axis, bool forward) override;
    void set_axis_enabled(ports::AxisId axis, bool enabled) override;
    void pulse_step(ports::AxisId axis) override;
    void disable_all_outputs() override;

    void set_ra_direction_invert(bool invert) { ra_invert_ = invert; }
    void set_dec_direction_invert(bool invert) { dec_invert_ = invert; }

private:
    drivers::Lv8729Driver* ra_driver_;
    drivers::Lv8729Driver* dec_driver_;
    bool ra_invert_{false};
    bool dec_invert_{false};
};

} // namespace esp32
} // namespace platform

#endif // PLATFORM_ESP32_ESP32_STEP_EXECUTOR_H
