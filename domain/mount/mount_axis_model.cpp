#include "domain/mount/mount_axis_model.h"
#include <cmath>

namespace domain {
namespace mount {

MountAxisModel::MountAxisModel(const adapters::configuration::AxisConfig& config)
    : config_(config) {}

void MountAxisModel::update_config(const adapters::configuration::AxisConfig& config) {
    config_ = config;
}

double MountAxisModel::steps_per_axis_revolution() const {
    double steps_per_rev = static_cast<double>(config_.steps_per_revolution);
    double ustep = static_cast<double>(config_.microstepping);
    double gear = static_cast<double>(config_.gear_ratio);
    return steps_per_rev * ustep * gear;
}

double MountAxisModel::arcsec_per_step() const {
    double steps_per_axis_rev = steps_per_axis_revolution();
    if (steps_per_axis_rev <= 0.0) return 0.0;
    return (360.0 * 3600.0) / steps_per_axis_rev;
}

astronomy::Angle MountAxisModel::steps_to_angle(int32_t steps) const {
    double total_steps_per_rev = steps_per_axis_revolution();
    if (total_steps_per_rev <= 0.0) {
        return astronomy::Angle::FromDegrees(0.0);
    }

    double sign = config_.invert_direction ? -1.0 : +1.0;
    double deg = (static_cast<double>(steps) / total_steps_per_rev) * 360.0 * sign;
    return astronomy::Angle::FromDegrees(deg);
}

int32_t MountAxisModel::angle_to_steps(const astronomy::Angle& angle) const {
    double total_steps_per_rev = steps_per_axis_revolution();
    if (total_steps_per_rev <= 0.0) {
        return 0;
    }

    double sign = config_.invert_direction ? -1.0 : +1.0;
    double deg = angle.as_degrees() * sign;
    double steps_double = (deg / 360.0) * total_steps_per_rev;
    return static_cast<int32_t>(std::round(steps_double));
}

bool MountAxisModel::is_step_in_limits(int32_t steps) const {
    if (!config_.enable_soft_limits) {
        return true;
    }
    return (steps >= config_.min_step_limit && steps <= config_.max_step_limit);
}

bool MountAxisModel::is_angle_in_limits(const astronomy::Angle& angle) const {
    if (!config_.enable_soft_limits) {
        return true;
    }
    int32_t steps = angle_to_steps(angle);
    return is_step_in_limits(steps);
}

} // namespace mount
} // namespace domain
