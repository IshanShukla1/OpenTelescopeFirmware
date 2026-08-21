#include "domain/mount/mount_transform.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::mount;
using namespace domain::astronomy;

static bool approx_equal(double a, double b, double tol = 1e-5) {
    return std::abs(a - b) <= tol;
}

void test_mount_transform() {
    // Observer: Greenwich (Lat 51.4769 N, Lon 0.0)
    ObserverLocation greenwich{Angle::FromDegrees(51.4769), Angle::FromDegrees(0.0)};
    assert(greenwich.is_valid());

    // Time: J2000.0 Epoch
    UtcDateTime j2000_utc{2000, 1, 1, 12, 0, 0.0};
    AstroTime t1 = AstroTime::FromUtc(j2000_utc);

    // Target: Vega (RA = 18.6156472h, DEC = +38.7836889 deg)
    EquatorialCoordinates vega{
        RightAscension(Angle::FromHours(18.6156472)),
        Declination(Angle::FromDegrees(38.7836889))
    };
    assert(vega.is_valid());

    // 1. Forward Transform: Celestial -> Mount Axis Angles
    MountAxisAngles mount_angles = EquatorialMountTransform::celestial_to_mount(vega, t1, greenwich);
    Angle lst1 = calculate_lst(t1, greenwich);
    Angle expected_ha = (lst1 - vega.ra.angle()).normalize_180();

    assert(approx_equal(mount_angles.ra_axis_angle.as_hours(), expected_ha.as_hours()));
    assert(approx_equal(mount_angles.dec_axis_angle.as_degrees(), 38.7836889));

    // 2. Reverse Transform: Mount Axis Angles -> Celestial
    EquatorialCoordinates vega_rev = EquatorialMountTransform::mount_to_celestial(mount_angles, t1, greenwich);
    assert(approx_equal(vega_rev.ra.as_hours(), vega.ra.as_hours()));
    assert(approx_equal(vega_rev.dec.as_degrees(), vega.dec.as_degrees()));

    // 3. Round-trip Integrity Test
    MountAxisAngles angles_rt = EquatorialMountTransform::celestial_to_mount(vega_rev, t1, greenwich);
    assert(approx_equal(angles_rt.ra_axis_angle.as_hours(), mount_angles.ra_axis_angle.as_hours()));
    assert(approx_equal(angles_rt.dec_axis_angle.as_degrees(), mount_angles.dec_axis_angle.as_degrees()));

    // 4. Sidereal Time Progression (Time advances by 1 sidereal hour = 3590.17042 UTC seconds)
    // Over 1 sidereal hour, LST increases by +1h, so RA axis angle (HA) increases by +1h, while celestial RA remains unchanged!
    AstroTime t2 = AstroTime::FromJulianDate(t1.julian_date() + (1.0 / 24.0) * (1.0 / 1.00273790935));
    MountAxisAngles mount_angles_t2 = EquatorialMountTransform::celestial_to_mount(vega, t2, greenwich);

    Angle ha_diff = (mount_angles_t2.ra_axis_angle - mount_angles.ra_axis_angle).normalize_24h();
    assert(approx_equal(ha_diff.as_hours(), 1.0, 1e-4));

    // Mount to celestial at t2 must resolve to the SAME celestial coordinates for Vega
    EquatorialCoordinates vega_t2_rev = EquatorialMountTransform::mount_to_celestial(mount_angles_t2, t2, greenwich);
    assert(approx_equal(vega_t2_rev.ra.as_hours(), vega.ra.as_hours(), 1e-4));

    std::printf("[PASS] test_mount_transform\n");
}
