#ifndef ADAPTERS_CONFIGURATION_CONFIG_MANAGER_H
#define ADAPTERS_CONFIGURATION_CONFIG_MANAGER_H

#include "adapters/configuration/config_schema.h"
#include "core/result.h"
#include "ports/event_sink_port.h"

namespace adapters {
namespace configuration {

class ConfigManager {
public:
    explicit ConfigManager(ports::IEventSinkPort* event_sink = nullptr);

    core::Result<void> load_configuration(const SystemConfig& config);
    core::Result<void> validate_configuration(const SystemConfig& config) const;

    const SystemConfig& get_config() const { return current_config_; }
    bool is_valid() const { return config_loaded_; }

private:
    SystemConfig current_config_{};
    bool config_loaded_{false};
    ports::IEventSinkPort* event_sink_;
};

} // namespace configuration
} // namespace adapters

#endif // ADAPTERS_CONFIGURATION_CONFIG_MANAGER_H
