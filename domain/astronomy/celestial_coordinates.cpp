#include "domain/astronomy/celestial_coordinates.h"

namespace domain {
namespace astronomy {

Angle calculate_hour_angle(const Angle& lst, const RightAscension& ra) {
    Angle diff = lst - ra.angle();
    return diff.normalize_180();
}

} // namespace astronomy
} // namespace domain
