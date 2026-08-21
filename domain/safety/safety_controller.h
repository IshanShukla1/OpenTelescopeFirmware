#ifndef DOMAIN_SAFETY_SAFETY_CONTROLLER_H
#define DOMAIN_SAFETY_SAFETY_CONTROLLER_H

#include "core/types.h"
#include "core/result.h"
#include "ports/motion_executor_port.h"
#include "ports/event_sink_port.h"

namespace domain {
namespace safety {

enum class SafetyState : uint8_t {
    NORMAL = 0,
    DEGRADED,
    FAULT_LATCHED,
    CRITICAL_LATCHED,
    RECOVERY_PENDING
};

class SafetyController {
public:
    SafetyController(ports::IMotionExecutorPort* executor, ports::IEventSinkPort* event_sink);

    void trigger_emergency_stop(const char* reason = nullptr);
    void raise_fault(const char* fault_code, bool critical, const char* message);

    core::Result<void> request_recovery();
    void reset_safety_latch();

    bool is_motion_allowed() const {
        return safety_state_ == SafetyState::NORMAL || safety_state_ == SafetyState::DEGRADED;
    }

    SafetyState safety_state() const { return safety_state_; }
    const char* state_to_string() const;

private:
    ports::IMotionExecutorPort* executor_;
    ports::IEventSinkPort* event_sink_;
    SafetyState safety_state_{SafetyState::NORMAL};
    bool critical_fault_latched_{false};
};

} // namespace safety
} // namespace domain

#endif // DOMAIN_SAFETY_SAFETY_CONTROLLER_H
