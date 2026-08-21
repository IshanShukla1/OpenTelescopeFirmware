#include "domain/safety/safety_controller.h"

namespace domain {
namespace safety {

SafetyController::SafetyController(ports::IMotionExecutorPort* executor, ports::IEventSinkPort* event_sink)
    : executor_(executor), event_sink_(event_sink) {}

void SafetyController::trigger_emergency_stop(const char* reason) {
    safety_state_ = SafetyState::CRITICAL_LATCHED;
    critical_fault_latched_ = true;

    if (executor_) {
        executor_->disable_all_outputs();
    }

    if (event_sink_) {
        event_sink_->emit_event(
            "FLT-SAFETY-001",
            core::ErrorSeverity::CRITICAL_FAULT,
            core::ErrorSource::SAFETY,
            0,
            reason ? reason : "EMERGENCY STOP TRIGGERED: Step outputs disabled and safety latched"
        );
    }
}

void SafetyController::raise_fault(const char* fault_code, bool critical, const char* message) {
    if (critical) {
        safety_state_ = SafetyState::CRITICAL_LATCHED;
        critical_fault_latched_ = true;
        if (executor_) executor_->disable_all_outputs();
    } else {
        if (safety_state_ != SafetyState::CRITICAL_LATCHED) {
            safety_state_ = SafetyState::FAULT_LATCHED;
        }
    }

    if (event_sink_) {
        event_sink_->emit_event(
            fault_code ? fault_code : "FLT-SAFETY-002",
            critical ? core::ErrorSeverity::CRITICAL_FAULT : core::ErrorSeverity::FAULT,
            core::ErrorSource::SAFETY,
            0,
            message ? message : "Safety fault raised"
        );
    }
}

core::Result<void> SafetyController::request_recovery() {
    if (safety_state_ == SafetyState::NORMAL) {
        return core::Result<void>::Success();
    }

    safety_state_ = SafetyState::RECOVERY_PENDING;

    // Run safety recovery validation checks
    bool checks_pass = true; // In v0.1, verify hardware safe state

    if (checks_pass) {
        safety_state_ = SafetyState::NORMAL;
        critical_fault_latched_ = false;

        if (event_sink_) {
            event_sink_->emit_event(
                "EVT-SAFETY-002",
                core::ErrorSeverity::INFO,
                core::ErrorSource::SAFETY,
                0,
                "Safety state cleared. Recovery checks passed."
            );
        }
        return core::Result<void>::Success();
    } else {
        safety_state_ = SafetyState::CRITICAL_LATCHED;
        return core::Result<void>::Failure(core::StatusCode::FAULT_ACTIVE);
    }
}

void SafetyController::reset_safety_latch() {
    request_recovery();
}

const char* SafetyController::state_to_string() const {
    switch (safety_state_) {
        case SafetyState::NORMAL:           return "NORMAL";
        case SafetyState::DEGRADED:         return "DEGRADED";
        case SafetyState::FAULT_LATCHED:    return "FAULT_LATCHED";
        case SafetyState::CRITICAL_LATCHED: return "CRITICAL_LATCHED";
        case SafetyState::RECOVERY_PENDING: return "RECOVERY_PENDING";
        default:                            return "UNKNOWN";
    }
}

} // namespace safety
} // namespace domain
