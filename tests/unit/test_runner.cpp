#include <unity.h>

extern void test_config_validation();
extern void test_command_parser();
extern void test_safety_controller();
extern void test_units_and_angles();
extern void test_astronomical_time();
extern void test_sidereal_time();
extern void test_mount_axis_model();
extern void test_celestial_coordinates();
extern void test_mount_transform();
extern void test_end_to_end_astronomy_pipeline();

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(test_config_validation);
    RUN_TEST(test_command_parser);
    RUN_TEST(test_safety_controller);
    RUN_TEST(test_units_and_angles);
    RUN_TEST(test_astronomical_time);
    RUN_TEST(test_sidereal_time);
    RUN_TEST(test_mount_axis_model);
    RUN_TEST(test_celestial_coordinates);
    RUN_TEST(test_mount_transform);
    RUN_TEST(test_end_to_end_astronomy_pipeline);
    return UNITY_END();
}
