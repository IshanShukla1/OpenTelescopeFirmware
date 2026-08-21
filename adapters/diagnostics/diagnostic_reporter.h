#ifndef ADAPTERS_DIAGNOSTICS_DIAGNOSTIC_REPORTER_H
#define ADAPTERS_DIAGNOSTICS_DIAGNOSTIC_REPORTER_H

#include "adapters/diagnostics/event_logger.h"
#include "adapters/configuration/config_manager.h"
#include "ports/clock_port.h"
#include "core/types.h"
#include <cstddef>

namespace adapters {
namespace diagnostics {

class DiagnosticReporter {
public:
    DiagnosticReporter(
        EventLogger* logger,
        const configuration::ConfigManager* config_mgr,
        ports::IClockPort* clock
    );

    void format_status_report(char* buffer, size_t max_len,
                             const char* ra_state_str, int32_t ra_pos, float ra_vel,
                             const char* dec_state_str, int32_t dec_pos, float dec_vel,
                             const char* reset_cause_str) const;

    void format_diagnostics_report(char* buffer, size_t max_len,
                                   const char* reset_cause_str) const;

    void format_event_history(char* buffer, size_t max_len) const;

private:
    EventLogger* logger_;
    const configuration::ConfigManager* config_mgr_;
    ports::IClockPort* clock_;
};

} // namespace diagnostics
} // namespace adapters

#endif // ADAPTERS_DIAGNOSTICS_DIAGNOSTIC_REPORTER_H
