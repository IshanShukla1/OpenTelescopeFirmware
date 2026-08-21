#include "domain/astronomy/sidereal_time.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::astronomy;

static bool approx_equal(double a, double b, double tol = 1e-5) {
    return std::abs(a - b) <= tol;
}

void test_sidereal_time() {
    // 1. Meeus Example 12.a (1987 April 10 at 19:21:00 UT)
    UtcDateTime dt_12a{1987, 4, 10, 19, 21, 0.0};
    AstroTime t_12a = AstroTime::FromUtc(dt_12a);
    assert(approx_equal(t_12a.julian_date(), 2446896.30625));

    Angle gmst_12a = calculate_gmst(t_12a);
    assert(approx_equal(gmst_12a.as_hours(), 8.582524883, 1e-5));

    HMS gmst_hms = gmst_12a.to_hms();
    assert(gmst_hms.hours == 8);
    assert(gmst_hms.minutes == 34);
    assert(approx_equal(gmst_hms.seconds, 57.0896, 0.1));

    // 2. Meeus Example 12.b (1987 April 10 at 0h UT)
    UtcDateTime dt_12b{1987, 4, 10, 0, 0, 0.0};
    AstroTime t_12b = AstroTime::FromUtc(dt_12b);
    assert(approx_equal(t_12b.julian_date(), 2446895.5));

    Angle gmst_12b = calculate_gmst(t_12b);
    // Expected GMST_0h: 13h 10m 46.3668s = 13.179546333 hours
    assert(approx_equal(gmst_12b.as_hours(), 13.179546333, 1e-5));

    // 3. Local Sidereal Time for Washington D.C. Observer (77.0365 deg West = -77.0365 deg)
    ObserverLocation dc_obs{Angle::FromDegrees(38.9072), Angle::FromDegrees(-77.0365)};
    assert(dc_obs.is_valid());

    Angle lst_12a = calculate_lst(t_12a, dc_obs);
    double expected_lst_hours = (gmst_12a + dc_obs.longitude).normalize_24h().as_hours();
    assert(approx_equal(lst_12a.as_hours(), expected_lst_hours, 1e-5));

    // 4. Location Validation Checks
    ObserverLocation invalid_lat{Angle::FromDegrees(95.0), Angle::FromDegrees(0.0)};
    assert(!invalid_lat.is_valid());

    ObserverLocation invalid_lon{Angle::FromDegrees(0.0), Angle::FromDegrees(-185.0)};
    assert(!invalid_lon.is_valid());

    std::printf("[PASS] test_sidereal_time\n");
}
