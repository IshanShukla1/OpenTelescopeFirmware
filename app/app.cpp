#include "app/app.h"
#include "platform/esp32/esp32_clock_adapter.h"
#include "platform/esp32/esp32_gpio_adapter.h"
#include "platform/esp32/esp32_reset_reason.h"
#include "platform/esp32/esp32_step_executor.h"
#include "platform/drivers/lv8729/lv8729_driver.h"
#include "adapters/configuration/config_manager.h"
#include "adapters/diagnostics/event_logger.h"
#include "adapters/diagnostics/diagnostic_reporter.h"
#include "adapters/communications/serial_console.h"
#include "domain/motion/motor_axis.h"
#include "domain/motion/motion_planner.h"
#include "domain/safety/safety_controller.h"
#include "services/system_service.h"

namespace app {

namespace {
// Global composition instance pointers
platform::esp32::Esp32ClockAdapter* g_clock = nullptr;
platform::esp32::Esp32GpioAdapter* g_gpio = nullptr;
platform::drivers::Lv8729Driver* g_ra_driver = nullptr;
platform::drivers::Lv8729Driver* g_dec_driver = nullptr;
platform::esp32::Esp32StepExecutor* g_executor = nullptr;

adapters::diagnostics::EventLogger* g_logger = nullptr;
adapters::configuration::ConfigManager* g_config_mgr = nullptr;
adapters::diagnostics::DiagnosticReporter* g_reporter = nullptr;
adapters::communications::SerialConsole* g_console = nullptr;

domain::motion::MotorAxis* g_ra_axis = nullptr;
domain::motion::MotorAxis* g_dec_axis = nullptr;
domain::motion::MotionPlanner* g_planner = nullptr;
domain::safety::SafetyController* g_safety = nullptr;
services::SystemService* g_service = nullptr;

const char* g_reset_reason = nullptr;
} // namespace

void init() {
    // 1. Initialize HAL adapters
    static platform::esp32::Esp32ClockAdapter clock_inst;
    static platform::esp32::Esp32GpioAdapter gpio_inst;
    g_clock = &clock_inst;
    g_gpio = &gpio_inst;

    // Detect reset reason
    g_reset_reason = platform::esp32::ResetReasonDetector::get_reset_reason_string();

    // 2. Initialize serial console
    static adapters::communications::SerialConsole console_inst;
    g_console = &console_inst;
    g_console->init(115200);

    // 3. Initialize diagnostics logger
    static adapters::diagnostics::EventLogger logger_inst(g_clock);
    g_logger = &logger_inst;

    // 4. Initialize configuration manager and load default configuration
    static adapters::configuration::ConfigManager config_mgr_inst(g_logger);
    g_config_mgr = &config_mgr_inst;

    adapters::configuration::SystemConfig defaultConfig = adapters::configuration::CreateDefaultConfig();
    auto cfg_res = g_config_mgr->load_configuration(defaultConfig);

    // 5. Initialize LV8729 Drivers
    static platform::drivers::Lv8729Driver ra_driver_inst(
        g_gpio, defaultConfig.pins.ra_step_pin, defaultConfig.pins.ra_dir_pin, defaultConfig.pins.ra_enable_pin
    );
    static platform::drivers::Lv8729Driver dec_driver_inst(
        g_gpio, defaultConfig.pins.dec_step_pin, defaultConfig.pins.dec_dir_pin, defaultConfig.pins.dec_enable_pin
    );
    g_ra_driver = &ra_driver_inst;
    g_dec_driver = &dec_driver_inst;

    g_ra_driver->init();
    g_dec_driver->init();

    // 6. Initialize Step Executor
    static platform::esp32::Esp32StepExecutor executor_inst(g_ra_driver, g_dec_driver);
    g_executor = &executor_inst;

    // 7. Initialize Motor Axes
    static domain::motion::MotorAxis ra_axis_inst(ports::AxisId::RA, g_executor);
    static domain::motion::MotorAxis dec_axis_inst(ports::AxisId::DEC, g_executor);
    g_ra_axis = &ra_axis_inst;
    g_dec_axis = &dec_axis_inst;

    g_ra_axis->configure(defaultConfig.ra_axis);
    g_dec_axis->configure(defaultConfig.dec_axis);

    // 8. Initialize Safety Controller & Motion Planner
    static domain::safety::SafetyController safety_inst(g_executor, g_logger);
    g_safety = &safety_inst;

    static domain::motion::MotionPlanner planner_inst(g_ra_axis, g_dec_axis, g_executor, g_clock, g_logger);
    g_planner = &planner_inst;

    // 9. Initialize Diagnostic Reporter & System Service
    static adapters::diagnostics::DiagnosticReporter reporter_inst(g_logger, g_config_mgr, g_clock);
    g_reporter = &reporter_inst;

    static services::SystemService service_inst(
        g_config_mgr, g_logger, g_reporter, g_console,
        g_ra_axis, g_dec_axis, g_planner, g_safety, g_clock, g_reset_reason
    );
    g_service = &service_inst;

    // 10. Output Firmware Boot Summary over Serial
    g_console->print_line("\r\n==================================================");
    g_console->print_line("OPEN-SOURCE RETROFIT GOTO FIRMWARE v0.1");
    g_console->print_line("==================================================");
    g_console->print("Firmware Version : "); g_console->print_line(core::kVersionString);
    g_console->print("Hardware Target  : "); g_console->print_line(core::kHardwareTarget);
    g_console->print("Reset Reason     : "); g_console->print_line(g_reset_reason);
    g_console->print("Boot Result      : ");
    if (cfg_res.is_ok()) {
        g_console->print_line("SUCCESS (Configuration Validated & Hardware Initialized)");
    } else {
        g_console->print_line("FAILED (Invalid Configuration)");
    }
    g_console->print_line("Ready for serial diagnostic commands. Type 'help' for commands.\r\n");
}

void update() {
    // Poll non-blocking serial input
    if (g_console && g_service) {
        char line_buf[128];
        if (g_console->poll_line(line_buf, sizeof(line_buf))) {
            g_service->process_command_line(line_buf);
        }
    }

    // Process system service and motion step updates
    if (g_service) {
        g_service->update();
    }
}

} // namespace app
