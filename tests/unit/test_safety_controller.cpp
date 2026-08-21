#include "domain/safety/safety_controller.h"
#include "adapters/diagnostics/event_logger.h"
#include "platform/esp32/esp32_clock_adapter.h"
#include <cassert>
#include <cstdio>

class FakeStepExecutor : public ports::IMotionExecutorPort {
public:
    void set_axis_direction(ports::AxisId axis, bool forward) override { (void)axis; (void)forward; }
    void set_axis_enabled(ports::AxisId axis, bool enabled) override { (void)axis; (void)enabled; }
    void pulse_step(ports::AxisId axis) override { (void)axis; }
    void disable_all_outputs() override { disabled_count++; }

    uint32_t disabled_count{0};
};

void test_safety_controller() {
    platform::esp32::Esp32ClockAdapter clock;
    adapters::diagnostics::EventLogger logger(&clock);
    FakeStepExecutor executor;
    domain::safety::SafetyController safety(&executor, &logger);

    // Initial state is NORMAL
    assert(safety.is_motion_allowed());
    assert(safety.safety_state() == domain::safety::SafetyState::NORMAL);

    // Trigger emergency stop
    safety.trigger_emergency_stop("Test E-Stop");
    assert(!safety.is_motion_allowed());
    assert(safety.safety_state() == domain::safety::SafetyState::CRITICAL_LATCHED);
    assert(executor.disabled_count > 0);
    assert(logger.fault_count() == 1);

    // Request recovery & reset latch
    auto rec_res = safety.request_recovery();
    assert(rec_res.is_ok());
    assert(safety.is_motion_allowed());
    assert(safety.safety_state() == domain::safety::SafetyState::NORMAL);

    std::printf("[PASS] test_safety_controller\n");
}
