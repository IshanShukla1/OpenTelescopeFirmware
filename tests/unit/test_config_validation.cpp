#include "adapters/configuration/config_manager.h"
#include "adapters/diagnostics/event_logger.h"
#include "platform/esp32/esp32_clock_adapter.h"
#include <cassert>
#include <cstdio>

void test_config_validation() {
    platform::esp32::Esp32ClockAdapter clock;
    adapters::diagnostics::EventLogger logger(&clock);
    adapters::configuration::ConfigManager config_mgr(&logger);

    // 1. Test default valid configuration
    auto valid_cfg = adapters::configuration::CreateDefaultConfig();
    auto res1 = config_mgr.load_configuration(valid_cfg);
    assert(res1.is_ok());
    assert(config_mgr.is_valid());

    // 2. Test invalid microstepping rejection
    auto invalid_cfg = valid_cfg;
    invalid_cfg.ra_axis.microstepping = 7; // Invalid non-power of two microstepping
    auto res2 = config_mgr.load_configuration(invalid_cfg);
    assert(res2.is_error());
    assert(res2.status() == core::StatusCode::CONFIG_ERROR);

    // 3. Test negative speed rejection
    invalid_cfg = valid_cfg;
    invalid_cfg.dec_axis.max_velocity_sps = -500.0f;
    auto res3 = config_mgr.load_configuration(invalid_cfg);
    assert(res3.is_error());

    std::printf("[PASS] test_config_validation\n");
}
