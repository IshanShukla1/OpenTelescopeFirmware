#include "domain/mount/mount_transform.h"

namespace domain {
namespace mount {

MountAxisAngles EquatorialMountTransform::celestial_to_mount(
    const astronomy::EquatorialCoordinates& coords,
    const astronomy::AstroTime& time,
    const astronomy::ObserverLocation& observer
) {
    astronomy::Angle lst = astronomy::calculate_lst(time, observer);
    astronomy::Angle ha = astronomy::calculate_hour_angle(lst, coords.ra);

    MountAxisAngles angles{};
    angles.ra_axis_angle = ha;
    angles.dec_axis_angle = coords.dec.angle();
    return angles;
}

astronomy::EquatorialCoordinates EquatorialMountTransform::mount_to_celestial(
    const MountAxisAngles& mount_angles,
    const astronomy::AstroTime& time,
    const astronomy::ObserverLocation& observer
) {
    astronomy::Angle lst = astronomy::calculate_lst(time, observer);
    astronomy::Angle ra_angle = (lst - mount_angles.ra_axis_angle).normalize_24h();

    astronomy::EquatorialCoordinates coords{};
    coords.ra = astronomy::RightAscension(ra_angle);
    coords.dec = astronomy::Declination(mount_angles.dec_axis_angle);
    return coords;
}

} // namespace mount
} // namespace domain
