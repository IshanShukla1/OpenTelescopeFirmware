#ifndef DOMAIN_MOUNT_MOUNT_TRANSFORM_H
#define DOMAIN_MOUNT_MOUNT_TRANSFORM_H

#include "domain/astronomy/angle.h"
#include "domain/astronomy/celestial_coordinates.h"
#include "domain/astronomy/sidereal_time.h"
#include "domain/astronomy/time_system.h"

namespace domain {
namespace mount {

struct MountAxisAngles {
    astronomy::Angle ra_axis_angle;   // Physical RA axis angle (Hour Angle)
    astronomy::Angle dec_axis_angle;  // Physical DEC axis angle (Declination)
};

class EquatorialMountTransform {
public:
    // Transform Celestial (RA, DEC) -> Mount Axis Angles (RA_axis, DEC_axis) at given time & location
    static MountAxisAngles celestial_to_mount(
        const astronomy::EquatorialCoordinates& coords,
        const astronomy::AstroTime& time,
        const astronomy::ObserverLocation& observer
    );

    // Transform Mount Axis Angles (RA_axis, DEC_axis) -> Celestial (RA, DEC) at given time & location
    static astronomy::EquatorialCoordinates mount_to_celestial(
        const MountAxisAngles& mount_angles,
        const astronomy::AstroTime& time,
        const astronomy::ObserverLocation& observer
    );
};

} // namespace mount
} // namespace domain

#endif // DOMAIN_MOUNT_MOUNT_TRANSFORM_H
