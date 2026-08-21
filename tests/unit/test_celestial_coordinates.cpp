#include "domain/astronomy/celestial_coordinates.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::astronomy;

static bool approx_equal(double a, double b, double tol = 1e-6) {
    return std::abs(a - b) <= tol;
}

void test_celestial_coordinates() {
    // 1. Vega Coordinate Representation
    RightAscension vega_ra(Angle::FromHMS(18, 36, 56.33));
    Declination vega_dec(Angle::FromDMS(38, 47, 1.28));

    EquatorialCoordinates vega{vega_ra, vega_dec};
    assert(vega.is_valid());
    assert(approx_equal(vega.ra.as_hours(), 18.6156472, 1e-5));
    assert(approx_equal(vega.dec.as_degrees(), 38.7836889, 1e-5));

    // 2. Declination Validity Boundary Checks
    Declination valid_north(Angle::FromDegrees(90.0));
    assert(valid_north.is_valid());

    Declination valid_south(Angle::FromDegrees(-90.0));
    assert(valid_south.is_valid());

    Declination invalid_north(Angle::FromDegrees(90.001));
    assert(!invalid_north.is_valid());

    Declination invalid_south(Angle::FromDegrees(-90.001));
    assert(!invalid_south.is_valid());

    EquatorialCoordinates invalid_eq{vega_ra, invalid_north};
    assert(!invalid_eq.is_valid());

    // 3. Hour Angle Calculations
    // Case A: Meridian transit (LST == RA) -> HA = 0h
    Angle lst_transit = Angle::FromHours(18.6156472);
    Angle ha_transit = calculate_hour_angle(lst_transit, vega_ra);
    assert(approx_equal(ha_transit.as_hours(), 0.0, 1e-5));

    // Case B: LST = 10h, RA = 8h -> HA = +2h
    RightAscension ra_8h(Angle::FromHours(8.0));
    Angle lst_10h = Angle::FromHours(10.0);
    Angle ha_pos = calculate_hour_angle(lst_10h, ra_8h);
    assert(approx_equal(ha_pos.as_hours(), 2.0));

    // Case C: LST = 2h, RA = 22h -> HA = +4h (2 - 22 = -20h -> +4h in [-12, +12))
    RightAscension ra_22h(Angle::FromHours(22.0));
    Angle lst_2h = Angle::FromHours(2.0);
    Angle ha_wrap_pos = calculate_hour_angle(lst_2h, ra_22h);
    assert(approx_equal(ha_wrap_pos.as_hours(), 4.0));

    // Case D: LST = 20h, RA = 2h -> HA = -6h (20 - 2 = 18h -> -6h in [-12, +12))
    RightAscension ra_2h(Angle::FromHours(2.0));
    Angle lst_20h = Angle::FromHours(20.0);
    Angle ha_wrap_neg = calculate_hour_angle(lst_20h, ra_2h);
    assert(approx_equal(ha_wrap_neg.as_hours(), -6.0));

    std::printf("[PASS] test_celestial_coordinates\n");
}
