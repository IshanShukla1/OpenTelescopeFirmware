#include "adapters/diagnostics/diagnostic_reporter.h"
#include <cstdio>

namespace adapters {
namespace diagnostics {

DiagnosticReporter::DiagnosticReporter(
    EventLogger* logger,
    const configuration::ConfigManager* config_mgr,
    ports::IClockPort* clock
)   : logger_(logger), config_mgr_(config_mgr), clock_(clock) {}

void DiagnosticReporter::format_status_report(
    char* buffer, size_t max_len,
    const char* ra_state_str, int32_t ra_pos, float ra_vel,
    const char* dec_state_str, int32_t dec_pos, float dec_vel,
    const char* reset_cause_str
) const {
    uint32_t uptime_s = clock_ ? static_cast<uint32_t>(clock_->now_ms() / 1000) : 0;
    uint32_t faults = logger_ ? logger_->fault_count() : 0;
    const char* sys_state = (faults > 0) ? "FAULTED" : "READY";

    std::snprintf(
        buffer, max_len,
        "========================================\r\n"
        "SYSTEM STATUS REPORT [v%s]\r\n"
        "========================================\r\n"
        "Firmware: %s\r\n"
        "Target: %s\r\n"
        "Uptime: %u seconds\r\n"
        "Reset Cause: %s\r\n"
        "System State: %s\r\n"
        "\r\n"
        "RA AXIS\r\n"
        "  State: %s\r\n"
        "  Position (steps): %ld\r\n"
        "  Velocity (sps): %.1f\r\n"
        "\r\n"
        "DEC AXIS\r\n"
        "  State: %s\r\n"
        "  Position (steps): %ld\r\n"
        "  Velocity (sps): %.1f\r\n"
        "========================================\r\n",
        core::kVersionString,
        core::kVersionString,
        core::kHardwareTarget,
        uptime_s,
        reset_cause_str ? reset_cause_str : "UNKNOWN",
        sys_state,
        ra_state_str ? ra_state_str : "UNKNOWN",
        static_cast<long>(ra_pos),
        ra_vel,
        dec_state_str ? dec_state_str : "UNKNOWN",
        static_cast<long>(dec_pos),
        dec_vel
    );
}

void DiagnosticReporter::format_diagnostics_report(
    char* buffer, size_t max_len,
    const char* reset_cause_str
) const {
    uint32_t events = logger_ ? static_cast<uint32_t>(logger_->event_count()) : 0;
    uint32_t warnings = logger_ ? logger_->warning_count() : 0;
    uint32_t errors = logger_ ? logger_->error_count() : 0;
    uint32_t faults = logger_ ? logger_->fault_count() : 0;
    uint32_t dropped = logger_ ? logger_->dropped_count() : 0;

    const core::EventRecord* last_fault = logger_ ? logger_->get_last_fault() : nullptr;
    const char* last_fault_code = last_fault ? last_fault->code : "NONE";
    const char* last_fault_msg = last_fault ? last_fault->message : "No faults recorded";

    std::snprintf(
        buffer, max_len,
        "========================================\r\n"
        "DIAGNOSTIC SUMMARY REPORT\r\n"
        "========================================\r\n"
        "Reset Reason: %s\r\n"
        "Config Loaded: %s\r\n"
        "Total Events Emitted: %u\r\n"
        "Warnings: %u\r\n"
        "Recoverable Errors: %u\r\n"
        "Faults / Critical: %u\r\n"
        "Dropped Log Records: %u\r\n"
        "Last Fault Code: %s\r\n"
        "Last Fault Context: %s\r\n"
        "Note: Position steps are firmware counts, not calibrated celestial coordinates.\r\n"
        "========================================\r\n",
        reset_cause_str ? reset_cause_str : "UNKNOWN",
        (config_mgr_ && config_mgr_->is_valid()) ? "YES" : "NO",
        events,
        warnings,
        errors,
        faults,
        dropped,
        last_fault_code,
        last_fault_msg
    );
}

void DiagnosticReporter::format_event_history(char* buffer, size_t max_len) const {
    if (!logger_) {
        std::snprintf(buffer, max_len, "No logger attached.\r\n");
        return;
    }

    core::EventRecord records[kMaxEventLogCapacity];
    size_t count = logger_->get_records(records, kMaxEventLogCapacity);

    size_t offset = 0;
    int written = std::snprintf(buffer + offset, max_len - offset,
        "========================================\r\n"
        "EVENT HISTORY (%zu entries)\r\n"
        "========================================\r\n", count);
    if (written > 0 && static_cast<size_t>(written) < max_len - offset) {
        offset += written;
    }

    for (size_t i = 0; i < count; ++i) {
        const auto& r = records[i];
        written = std::snprintf(buffer + offset, max_len - offset,
            "[%u] %s (Seq: %u, Time: %lluus, Correlation: %u) -> %s\r\n",
            static_cast<unsigned int>(i + 1),
            r.code,
            r.sequence,
            static_cast<unsigned long long>(r.timestamp_us),
            r.correlation_id,
            r.message
        );
        if (written > 0 && static_cast<size_t>(written) < max_len - offset) {
            offset += written;
        } else {
            break;
        }
    }
}

} // namespace diagnostics
} // namespace adapters
