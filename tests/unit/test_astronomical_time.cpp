#include "domain/astronomy/time_system.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::astronomy;

static bool approx_equal(double a, double b, double tol = 1e-6) {
    return std::abs(a - b) <= tol;
}

void test_astronomical_time() {
    // 1. J2000.0 Epoch Reference Case
    UtcDateTime j2000_utc{2000, 1, 1, 12, 0, 0.0};
    assert(j2000_utc.is_valid());

    AstroTime t_j2000 = AstroTime::FromUtc(j2000_utc);
    assert(approx_equal(t_j2000.julian_date(), 2451545.0));
    assert(approx_equal(t_j2000.modified_julian_date(), 51544.5));
    assert(approx_equal(t_j2000.julian_centuries_j2000(), 0.0));

    // Reverse check J2000
    UtcDateTime j2000_rev = t_j2000.to_utc();
    assert(j2000_rev.year == 2000);
    assert(j2000_rev.month == 1);
    assert(j2000_rev.day == 1);
    assert(j2000_rev.hour == 12);
    assert(j2000_rev.minute == 0);
    assert(approx_equal(j2000_rev.second, 0.0));

    // 2. Unix Epoch Reference Case (1970-01-01 00:00:00 UTC)
    UtcDateTime unix_epoch_utc{1970, 1, 1, 0, 0, 0.0};
    assert(unix_epoch_utc.is_valid());

    AstroTime t_unix = AstroTime::FromUtc(unix_epoch_utc);
    assert(approx_equal(t_unix.julian_date(), 2440587.5));
    assert(t_unix.unix_timestamp() == 0);

    AstroTime t_from_ts = AstroTime::FromUnixTimestamp(0);
    assert(approx_equal(t_from_ts.julian_date(), 2440587.5));

    // 3. Meeus Example 7.a (Sputnik launch: 1957-10-04 19:21:00 UTC)
    UtcDateTime sputnik_utc{1957, 10, 4, 19, 21, 0.0};
    assert(sputnik_utc.is_valid());

    AstroTime t_sputnik = AstroTime::FromUtc(sputnik_utc);
    assert(approx_equal(t_sputnik.julian_date(), 2436116.30625));

    UtcDateTime sputnik_rev = t_sputnik.to_utc();
    assert(sputnik_rev.year == 1957);
    assert(sputnik_rev.month == 10);
    assert(sputnik_rev.day == 4);
    assert(sputnik_rev.hour == 19);
    assert(sputnik_rev.minute == 21);
    assert(approx_equal(sputnik_rev.second, 0.0));

    // 4. Reference Case: 2009-06-19 18:00:00 UTC
    UtcDateTime ref_2009{2009, 6, 19, 18, 0, 0.0};
    assert(ref_2009.is_valid());
    AstroTime t_2009 = AstroTime::FromUtc(ref_2009);
    assert(approx_equal(t_2009.julian_date(), 2455002.25));

    // 5. Validation Check
    UtcDateTime invalid_month{2026, 13, 1, 10, 0, 0.0};
    assert(!invalid_month.is_valid());

    UtcDateTime invalid_feb{2025, 2, 29, 10, 0, 0.0}; // 2025 is not leap
    assert(!invalid_feb.is_valid());

    UtcDateTime valid_feb_leap{2024, 2, 29, 10, 0, 0.0}; // 2024 is leap
    assert(valid_feb_leap.is_valid());

    std::printf("[PASS] test_astronomical_time\n");
}
