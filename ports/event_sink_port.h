#ifndef PORTS_EVENT_SINK_PORT_H
#define PORTS_EVENT_SINK_PORT_H

#include "core/types.h"

namespace ports {

class IEventSinkPort {
public:
    virtual ~IEventSinkPort() = default;

    virtual void emit_event(
        const char* code,
        core::ErrorSeverity severity,
        core::ErrorSource source,
        uint32_t correlation_id,
        const char* message
    ) = 0;
};

} // namespace ports

#endif // PORTS_EVENT_SINK_PORT_H
