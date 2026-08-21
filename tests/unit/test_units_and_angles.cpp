#include "domain/astronomy/angle.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::astronomy;

static bool approx_equal(double a, double b, double tol = 1e-9) {
    return std::abs(a - b) <= tol;
}

void test_units_and_angles() {
    // 1. Basic Unit Conversions & Identities
    Angle a1 = Angle::FromDegrees(180.0);
    assert(approx_equal(a1.as_degrees(), 180.0));
    assert(approx_equal(a1.as_radians(), kPi));
    assert(approx_equal(a1.as_hours(), 12.0));

    Angle a2 = Angle::FromHours(1.0);
    assert(approx_equal(a2.as_degrees(), 15.0));
    assert(approx_equal(a2.as_radians(), kPi / 12.0));

    Angle a3 = Angle::FromRadians(kTwoPi);
    assert(approx_equal(a3.as_degrees(), 360.0));
    assert(approx_equal(a3.as_hours(), 24.0));

    // 2. Normalizations
    // 360-degree wrap
    Angle deg_over = Angle::FromDegrees(370.0).normalize_360();
    assert(approx_equal(deg_over.as_degrees(), 10.0));

    Angle deg_neg = Angle::FromDegrees(-30.0).normalize_360();
    assert(approx_equal(deg_neg.as_degrees(), 330.0));

    Angle deg_multi = Angle::FromDegrees(-750.0).normalize_360(); // -750 + 360*3 = 330
    assert(approx_equal(deg_multi.as_degrees(), 330.0));

    // 180-degree wrap [-180, +180)
    Angle deg_180_pos = Angle::FromDegrees(190.0).normalize_180();
    assert(approx_equal(deg_180_pos.as_degrees(), -170.0));

    Angle deg_180_neg = Angle::FromDegrees(-200.0).normalize_180();
    assert(approx_equal(deg_180_neg.as_degrees(), 160.0));

    // 24h wrap
    Angle h_over = Angle::FromHours(25.5).normalize_24h();
    assert(approx_equal(h_over.as_hours(), 1.5));

    Angle h_neg = Angle::FromHours(-2.0).normalize_24h();
    assert(approx_equal(h_neg.as_hours(), 22.0));

    // 2pi wrap
    Angle rad_over = Angle::FromRadians(3.0 * kPi).normalize_2pi();
    assert(approx_equal(rad_over.as_radians(), kPi));

    // 3. HMS Decompositions and Round-trip
    Angle hms_in = Angle::FromHMS(14, 25, 36.5, false);
    HMS hms_out = hms_in.to_hms();
    assert(!hms_out.is_negative);
    assert(hms_out.hours == 14);
    assert(hms_out.minutes == 25);
    assert(approx_equal(hms_out.seconds, 36.5));

    Angle hms_neg_in = Angle::FromHMS(3, 10, 15.0, true);
    HMS hms_neg_out = hms_neg_in.to_hms();
    assert(hms_neg_out.is_negative);
    assert(hms_neg_out.hours == 3);
    assert(hms_neg_out.minutes == 10);
    assert(approx_equal(hms_neg_out.seconds, 15.0));

    // 4. DMS Decompositions and Round-trip
    Angle dms_in = Angle::FromDMS(45, 30, 15.0, false);
    DMS dms_out = dms_in.to_dms();
    assert(!dms_out.is_negative);
    assert(dms_out.degrees == 45);
    assert(dms_out.arcminutes == 30);
    assert(approx_equal(dms_out.arcseconds, 15.0));

    Angle dms_neg_in = Angle::FromDMS(89, 59, 59.9, true);
    DMS dms_neg_out = dms_neg_in.to_dms();
    assert(dms_neg_out.is_negative);
    assert(dms_neg_out.degrees == 89);
    assert(dms_neg_out.arcminutes == 59);
    assert(approx_equal(dms_neg_out.arcseconds, 59.9));

    // 5. Operators
    Angle sum = Angle::FromDegrees(30.0) + Angle::FromDegrees(60.0);
    assert(approx_equal(sum.as_degrees(), 90.0));

    Angle diff = Angle::FromDegrees(90.0) - Angle::FromDegrees(45.0);
    assert(approx_equal(diff.as_degrees(), 45.0));

    Angle scaled = Angle::FromDegrees(15.0) * 3.0;
    assert(approx_equal(scaled.as_degrees(), 45.0));

    Angle scaled_lhs = 2.0 * Angle::FromDegrees(45.0);
    assert(approx_equal(scaled_lhs.as_degrees(), 90.0));

    Angle div = Angle::FromDegrees(90.0) / 2.0;
    assert(approx_equal(div.as_degrees(), 45.0));

    assert(Angle::FromDegrees(30.0) < Angle::FromDegrees(45.0));
    assert(Angle::FromDegrees(60.0) > Angle::FromDegrees(45.0));
    assert(Angle::FromDegrees(45.0) == Angle::FromDegrees(45.0));

    std::printf("[PASS] test_units_and_angles\n");
}
