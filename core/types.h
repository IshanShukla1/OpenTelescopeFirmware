#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <cstdint>
#include <cstddef>

namespace core {

using Microseconds = uint64_t;
using Milliseconds = uint32_t;

// Firmware Version Metadata
constexpr uint8_t kVersionMajor = 0;
constexpr uint8_t kVersionMinor = 1;
constexpr uint8_t kVersionPatch = 0;
constexpr const char* kVersionString = "0.1.0";
constexpr const char* kHardwareTarget = "ESP32 (LV8729 Dual Axis)";

// System Subsystem Health States (per ERROR_MANAGEMENT.md Section 3)
enum class SystemHealth : uint8_t {
    UNINITIALIZED = 0,
    SELF_TEST,
    READY,
    ACTIVE,
    DEGRADED,
    FAULTED,
    DISABLED
};

// System Operating Modes (per ERROR_MANAGEMENT.md Section 3)
enum class SystemOperation : uint8_t {
    STARTUP = 0,
    IDLE,
    TRACKING,
    SLEWING,
    PARKING,
    STOPPING,
    INHIBITED,
    FAULTED
};

// Position Trust States (per PRD.md & ERROR_MANAGEMENT.md Section 6)
enum class PositionTrust : uint8_t {
    UNTRUSTED = 0,
    TRUSTED
};

// Severity Levels (per ERROR_MANAGEMENT.md Section 2 & DEBUGGING_AND_DIAGNOSTICS.md Section 4)
enum class ErrorSeverity : uint8_t {
    TRACE = 0,
    DEBUG,
    INFO,
    WARNING,
    RECOVERABLE_ERROR,
    FAULT,
    CRITICAL_FAULT
};

// Subsystem Error Sources (per ERROR_MANAGEMENT.md Section 4)
enum class ErrorSource : uint8_t {
    CONFIG = 0,
    STARTUP,
    POWER,
    DRIVER,
    MOTION,
    SENSOR,
    ASTRO,
    TIME,
    COMMAND,
    COMMS,
    RESOURCE,
    WATCHDOG,
    SAFETY
};

// Structured Diagnostic Event Record (per ERROR_MANAGEMENT.md Section 8)
struct EventRecord {
    char code[16];            // e.g. "FLT-CONFIG-001" or "EVT-DIAG-001"
    ErrorSeverity severity;
    ErrorSource source;
    uint32_t sequence;
    Microseconds timestamp_us;
    uint32_t correlation_id;
    SystemHealth health_state;
    SystemOperation op_state;
    char message[64];
};

} // namespace core

#endif // CORE_TYPES_H
