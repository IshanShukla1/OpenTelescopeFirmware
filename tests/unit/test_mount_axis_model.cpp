#include "domain/mount/mount_axis_model.h"
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace domain::mount;
using namespace domain::astronomy;
using namespace adapters::configuration;

static bool approx_equal(double a, double b, double tol = 1e-6) {
    return std::abs(a - b) <= tol;
}

void test_mount_axis_model() {
    // 1. Direct Drive / 1:1 Gear Ratio Axis
    AxisConfig direct_cfg{};
    direct_cfg.steps_per_revolution = 200;
    direct_cfg.microstepping = 16;
    direct_cfg.gear_ratio = 1.0f;
    direct_cfg.invert_direction = false;
    direct_cfg.min_step_limit = -3200;
    direct_cfg.max_step_limit = 3200;
    direct_cfg.enable_soft_limits = true;

    MountAxisModel direct_model(direct_cfg);
    assert(approx_equal(direct_model.steps_per_axis_revolution(), 3200.0));
    assert(approx_equal(direct_model.arcsec_per_step(), 405.0)); // 1296000 / 3200 = 405 arcsec

    Angle a_90 = direct_model.steps_to_angle(800);
    assert(approx_equal(a_90.as_degrees(), 90.0));

    int32_t s_45 = direct_model.angle_to_steps(Angle::FromDegrees(45.0));
    assert(s_45 == 400);

    // 2. Equatorial Worm Gear Axis (180:1 gear ratio)
    AxisConfig eq_cfg = direct_cfg;
    eq_cfg.gear_ratio = 180.0f;
    eq_cfg.min_step_limit = -576000;
    eq_cfg.max_step_limit = 576000;

    MountAxisModel eq_model(eq_cfg);
    assert(approx_equal(eq_model.steps_per_axis_revolution(), 576000.0));
    assert(approx_equal(eq_model.arcsec_per_step(), 2.25)); // 1296000 / 576000 = 2.25 arcsec

    Angle a_eq_90 = eq_model.steps_to_angle(144000);
    assert(approx_equal(a_eq_90.as_degrees(), 90.0));

    int32_t s_eq_1deg = eq_model.angle_to_steps(Angle::FromDegrees(1.0));
    assert(s_eq_1deg == 1600);

    // 3. Direction Inversion
    AxisConfig inv_cfg = eq_cfg;
    inv_cfg.invert_direction = true;
    MountAxisModel inv_model(inv_cfg);

    Angle a_inv = inv_model.steps_to_angle(1600);
    assert(approx_equal(a_inv.as_degrees(), -1.0));

    int32_t s_inv = inv_model.angle_to_steps(Angle::FromDegrees(1.0));
    assert(s_inv == -1600);

    // 4. Soft Limits Validation
    assert(eq_model.is_step_in_limits(100000));
    assert(!eq_model.is_step_in_limits(600000));
    assert(!eq_model.is_step_in_limits(-600000));
    assert(eq_model.is_angle_in_limits(Angle::FromDegrees(90.0)));
    assert(!eq_model.is_angle_in_limits(Angle::FromDegrees(400.0)));

    // 5. Round-trip Integrity Check
    int32_t test_steps = 123456;
    Angle mapped_angle = eq_model.steps_to_angle(test_steps);
    int32_t roundtrip_steps = eq_model.angle_to_steps(mapped_angle);
    assert(roundtrip_steps == test_steps);

    std::printf("[PASS] test_mount_axis_model\n");
}
