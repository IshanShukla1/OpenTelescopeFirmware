#include "domain/astronomy/angle.h"
#include "domain/astronomy/celestial_coordinates.h"
#include "domain/astronomy/sidereal_time.h"
#include "domain/astronomy/time_system.h"
#include "domain/mount/mount_axis_model.h"
#include "domain/mount/mount_transform.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::astronomy;
using namespace domain::mount;
using namespace adapters::configuration;

static bool approx_equal(double a, double b, double tol = 1e-4) {
    return std::abs(a - b) <= tol;
}

void test_end_to_end_astronomy_pipeline() {
    // 1. Observer Location: Greenwich Observatory (51.4769 deg N, 0.0 deg E)
    ObserverLocation greenwich{Angle::FromDegrees(51.4769), Angle::FromDegrees(0.0)};
    assert(greenwich.is_valid());

    // 2. Observation UTC Time: 2024-03-20 21:00:00.0 UTC
    UtcDateTime utc_time{2024, 3, 20, 21, 0, 0.0};
    AstroTime astro_time = AstroTime::FromUtc(utc_time);
    assert(astro_time.julian_date() > 2460000.0);

    // 3. Compute Local Sidereal Time
    Angle lst = calculate_lst(astro_time, greenwich);
    assert(lst.as_hours() >= 0.0 && lst.as_hours() < 24.0);

    // 4. Celestial Target: Betelgeuse (RA = 5h 55m 10.3s = 5.919528h, DEC = +7 deg 24' 25" = +7.406944 deg)
    EquatorialCoordinates betelgeuse{
        RightAscension(Angle::FromHMS(5, 55, 10.3)),
        Declination(Angle::FromDMS(7, 24, 25.0))
    };
    assert(betelgeuse.is_valid());

    // 5. Mount Mechanical Configuration (Equatorial 180:1 Worm Gear Mount, NEMA17 200 steps/rev, 16 usteps)
    AxisConfig ra_cfg{};
    ra_cfg.steps_per_revolution = 200;
    ra_cfg.microstepping = 16;
    ra_cfg.gear_ratio = 180.0f;
    ra_cfg.invert_direction = false;
    ra_cfg.min_step_limit = -576000;
    ra_cfg.max_step_limit = 576000;
    ra_cfg.enable_soft_limits = true;

    AxisConfig dec_cfg = ra_cfg;

    MountAxisModel ra_axis_model(ra_cfg);
    MountAxisModel dec_axis_model(dec_cfg);

    assert(approx_equal(ra_axis_model.steps_per_axis_revolution(), 576000.0));
    assert(approx_equal(ra_axis_model.arcsec_per_step(), 2.25));

    // 6. FORWARD PIPELINE: Celestial Target -> Mount Axis Angles -> Motor Steps
    MountAxisAngles target_mount_angles = EquatorialMountTransform::celestial_to_mount(betelgeuse, astro_time, greenwich);
    int32_t target_ra_steps = ra_axis_model.angle_to_steps(target_mount_angles.ra_axis_angle);
    int32_t target_dec_steps = dec_axis_model.angle_to_steps(target_mount_angles.dec_axis_angle);

    assert(ra_axis_model.is_step_in_limits(target_ra_steps));
    assert(dec_axis_model.is_step_in_limits(target_dec_steps));

    // 7. REVERSE PIPELINE: Motor Steps -> Mount Axis Angles -> Celestial Target
    Angle rev_ra_axis_angle = ra_axis_model.steps_to_angle(target_ra_steps);
    Angle rev_dec_axis_angle = dec_axis_model.steps_to_angle(target_dec_steps);

    MountAxisAngles rev_mount_angles{rev_ra_axis_angle, rev_dec_axis_angle};
    EquatorialCoordinates resolved_celestial = EquatorialMountTransform::mount_to_celestial(rev_mount_angles, astro_time, greenwich);

    // 8. ACCURACY VERIFICATION: Reconstructed celestial target must match original within 1 step (2.25 arcsec)
    // 2.25 arcsec = 0.000625 deg = 0.00004167 hours
    assert(approx_equal(resolved_celestial.ra.as_hours(), betelgeuse.ra.as_hours(), 0.0002));
    assert(approx_equal(resolved_celestial.dec.as_degrees(), betelgeuse.dec.as_degrees(), 0.001));

    std::printf("[PASS] test_end_to_end_astronomy_pipeline\n");
}
