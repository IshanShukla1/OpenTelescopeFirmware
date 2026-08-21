#ifndef DOMAIN_MOUNT_MOUNT_AXIS_MODEL_H
#define DOMAIN_MOUNT_MOUNT_AXIS_MODEL_H

#include "adapters/configuration/config_schema.h"
#include "domain/astronomy/angle.h"
#include <cstdint>

namespace domain {
namespace mount {

class MountAxisModel {
public:
    explicit MountAxisModel(const adapters::configuration::AxisConfig& config);

    void update_config(const adapters::configuration::AxisConfig& config);
    const adapters::configuration::AxisConfig& config() const { return config_; }

    // Convert motor step position to physical axis angle
    astronomy::Angle steps_to_angle(int32_t steps) const;

    // Convert physical axis angle to motor step position
    int32_t angle_to_steps(const astronomy::Angle& angle) const;

    // Total microsteps per 360-degree revolution of physical axis
    double steps_per_axis_revolution() const;

    // Angular resolution in arcseconds per microstep
    double arcsec_per_step() const;

    // Check if step position is within soft limits
    bool is_step_in_limits(int32_t steps) const;

    // Check if axis angle is within soft limits
    bool is_angle_in_limits(const astronomy::Angle& angle) const;

private:
    adapters::configuration::AxisConfig config_;
};

} // namespace mount
} // namespace domain

#endif // DOMAIN_MOUNT_MOUNT_AXIS_MODEL_H
