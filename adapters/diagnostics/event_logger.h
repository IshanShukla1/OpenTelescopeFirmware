#ifndef ADAPTERS_DIAGNOSTICS_EVENT_LOGGER_H
#define ADAPTERS_DIAGNOSTICS_EVENT_LOGGER_H

#include "ports/event_sink_port.h"
#include "ports/clock_port.h"
#include "core/types.h"
#include <cstddef>

namespace adapters {
namespace diagnostics {

constexpr size_t kMaxEventLogCapacity = 32;

class EventLogger : public ports::IEventSinkPort {
public:
    explicit EventLogger(ports::IClockPort* clock);

    void emit_event(
        const char* code,
        core::ErrorSeverity severity,
        core::ErrorSource source,
        uint32_t correlation_id,
        const char* message
    ) override;

    size_t event_count() const { return total_events_emitted_; }
    uint32_t warning_count() const { return warning_count_; }
    uint32_t error_count() const { return error_count_; }
    uint32_t fault_count() const { return fault_count_; }
    uint32_t dropped_count() const { return dropped_count_; }

    size_t get_records(core::EventRecord* out_buffer, size_t max_records) const;
    const core::EventRecord* get_last_event() const;
    const core::EventRecord* get_last_fault() const;

private:
    ports::IClockPort* clock_;
    core::EventRecord log_ring_[kMaxEventLogCapacity]{};
    size_t head_{0};
    size_t count_{0};
    uint32_t sequence_{0};
    uint32_t total_events_emitted_{0};
    uint32_t warning_count_{0};
    uint32_t error_count_{0};
    uint32_t fault_count_{0};
    uint32_t dropped_count_{0};
    int32_t last_fault_idx_{-1};
};

} // namespace diagnostics
} // namespace adapters

#endif // ADAPTERS_DIAGNOSTICS_EVENT_LOGGER_H
