#ifndef SERVICES_SYSTEM_SERVICE_H
#define SERVICES_SYSTEM_SERVICE_H

#include "adapters/configuration/config_manager.h"
#include "adapters/diagnostics/event_logger.h"
#include "adapters/diagnostics/diagnostic_reporter.h"
#include "adapters/communications/command_parser.h"
#include "adapters/communications/serial_console.h"
#include "domain/motion/motor_axis.h"
#include "domain/motion/motion_planner.h"
#include "domain/safety/safety_controller.h"
#include "ports/clock_port.h"

namespace services {

class SystemService {
public:
    SystemService(
        adapters::configuration::ConfigManager* config_mgr,
        adapters::diagnostics::EventLogger* logger,
        adapters::diagnostics::DiagnosticReporter* reporter,
        adapters::communications::SerialConsole* console,
        domain::motion::MotorAxis* ra_axis,
        domain::motion::MotorAxis* dec_axis,
        domain::motion::MotionPlanner* planner,
        domain::safety::SafetyController* safety,
        ports::IClockPort* clock,
        const char* reset_cause_str
    );

    void process_command_line(const char* line);
    void update(); // Main periodic tick

    uint32_t command_count() const { return command_count_; }

private:
    void print_help();
    void print_version();
    void print_status();
    void print_diagnostics();
    void print_config();

    adapters::configuration::ConfigManager* config_mgr_;
    adapters::diagnostics::EventLogger* logger_;
    adapters::diagnostics::DiagnosticReporter* reporter_;
    adapters::communications::SerialConsole* console_;
    domain::motion::MotorAxis* ra_axis_;
    domain::motion::MotorAxis* dec_axis_;
    domain::motion::MotionPlanner* planner_;
    domain::safety::SafetyController* safety_;
    ports::IClockPort* clock_;
    const char* reset_cause_str_;

    uint32_t command_count_{0};
    uint32_t correlation_counter_{0};
};

} // namespace services

#endif // SERVICES_SYSTEM_SERVICE_H
