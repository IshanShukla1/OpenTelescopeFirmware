#include "adapters/diagnostics/event_logger.h"
#include <cstring>

namespace adapters {
namespace diagnostics {

EventLogger::EventLogger(ports::IClockPort* clock)
    : clock_(clock) {}

void EventLogger::emit_event(
    const char* code,
    core::ErrorSeverity severity,
    core::ErrorSource source,
    uint32_t correlation_id,
    const char* message
) {
    uint32_t current_seq = ++sequence_;
    total_events_emitted_++;

    if (severity == core::ErrorSeverity::WARNING) {
        warning_count_++;
    } else if (severity == core::ErrorSeverity::RECOVERABLE_ERROR) {
        error_count_++;
    } else if (severity == core::ErrorSeverity::FAULT || severity == core::ErrorSeverity::CRITICAL_FAULT) {
        fault_count_++;
    }

    size_t write_idx = head_;
    if (count_ == kMaxEventLogCapacity) {
        dropped_count_++;
    } else {
        count_++;
    }
    head_ = (head_ + 1) % kMaxEventLogCapacity;

    core::EventRecord& rec = log_ring_[write_idx];
    std::strncpy(rec.code, code ? code : "EVT-UNKN-000", sizeof(rec.code) - 1);
    rec.code[sizeof(rec.code) - 1] = '\0';

    rec.severity = severity;
    rec.source = source;
    rec.sequence = current_seq;
    rec.timestamp_us = clock_ ? clock_->now_us() : 0;
    rec.correlation_id = correlation_id;
    rec.health_state = (fault_count_ > 0) ? core::SystemHealth::FAULTED : core::SystemHealth::READY;
    rec.op_state = (fault_count_ > 0) ? core::SystemOperation::FAULTED : core::SystemOperation::IDLE;

    std::strncpy(rec.message, message ? message : "", sizeof(rec.message) - 1);
    rec.message[sizeof(rec.message) - 1] = '\0';

    if (severity == core::ErrorSeverity::FAULT || severity == core::ErrorSeverity::CRITICAL_FAULT) {
        last_fault_idx_ = static_cast<int32_t>(write_idx);
    }
}

size_t EventLogger::get_records(core::EventRecord* out_buffer, size_t max_records) const {
    if (!out_buffer || max_records == 0 || count_ == 0) {
        return 0;
    }

    size_t to_copy = (count_ < max_records) ? count_ : max_records;
    size_t start_idx = (head_ + kMaxEventLogCapacity - count_) % kMaxEventLogCapacity;

    for (size_t i = 0; i < to_copy; ++i) {
        size_t idx = (start_idx + i) % kMaxEventLogCapacity;
        out_buffer[i] = log_ring_[idx];
    }
    return to_copy;
}

const core::EventRecord* EventLogger::get_last_event() const {
    if (count_ == 0) return nullptr;
    size_t last_idx = (head_ + kMaxEventLogCapacity - 1) % kMaxEventLogCapacity;
    return &log_ring_[last_idx];
}

const core::EventRecord* EventLogger::get_last_fault() const {
    if (last_fault_idx_ < 0) return nullptr;
    return &log_ring_[last_fault_idx_];
}

} // namespace diagnostics
} // namespace adapters
