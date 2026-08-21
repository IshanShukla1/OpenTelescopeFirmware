#include "services/system_service.h"
#include <cstdio>

namespace services {

SystemService::SystemService(
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
)   : config_mgr_(config_mgr),
      logger_(logger),
      reporter_(reporter),
      console_(console),
      ra_axis_(ra_axis),
      dec_axis_(dec_axis),
      planner_(planner),
      safety_(safety),
      clock_(clock),
      reset_cause_str_(reset_cause_str) {}

void SystemService::print_help() {
    if (!console_) return;
    console_->print_line(
        "Available Commands:\r\n"
        "  help                     - Show this command list\r\n"
        "  version                  - Display firmware version metadata\r\n"
        "  status                   - Expose detailed system & axis status\r\n"
        "  diagnostics              - Display diagnostic summary & counters\r\n"
        "  config                   - Inspect active validated configuration\r\n"
        "  move ra <steps> [spd] [acc]   - Move RA axis relative steps\r\n"
        "  move dec <steps> [spd] [acc]  - Move DEC axis relative steps\r\n"
        "  move sync <ra> <dec> [s] [a]  - Move RA and DEC simultaneously\r\n"
        "  stop                     - Perform controlled motion stop\r\n"
        "  estop                    - Emergency stop (disable steps & latch)\r\n"
        "  events                   - Display recent event history\r\n"
        "  faults                   - Display last recorded fault\r\n"
        "  reset                    - Clear safety latch & reset faults"
    );
}

void SystemService::print_version() {
    if (!console_) return;
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Firmware Version: %s | Target: %s", core::kVersionString, core::kHardwareTarget);
    console_->print_line(buf);
}

void SystemService::print_status() {
    if (!console_ || !reporter_) return;
    char report_buf[1024];
    reporter_->format_status_report(
        report_buf, sizeof(report_buf),
        ra_axis_ ? ra_axis_->state_to_string() : "UNKNOWN",
        ra_axis_ ? ra_axis_->position_steps() : 0,
        ra_axis_ ? ra_axis_->current_velocity_sps() : 0.0f,
        dec_axis_ ? dec_axis_->state_to_string() : "UNKNOWN",
        dec_axis_ ? dec_axis_->position_steps() : 0,
        dec_axis_ ? dec_axis_->current_velocity_sps() : 0.0f,
        reset_cause_str_
    );
    console_->print(report_buf);
}

void SystemService::print_diagnostics() {
    if (!console_ || !reporter_) return;
    char report_buf[1024];
    reporter_->format_diagnostics_report(report_buf, sizeof(report_buf), reset_cause_str_);
    console_->print(report_buf);
}

void SystemService::print_config() {
    if (!console_ || !config_mgr_) return;
    if (!config_mgr_->is_valid()) {
        console_->print_line("Error: No valid configuration loaded!");
        return;
    }
    const auto& c = config_mgr_->get_config();
    char buf[512];
    std::snprintf(buf, sizeof(buf),
        "CONFIG SUMMARY:\r\n"
        "  Version: %u | Fingerprint: 0x%08X\r\n"
        "  RA Pins: STEP=%d, DIR=%d, EN=%d | Microstepping=%u | Steps/Rev=%u | MaxSpeed=%.1fsps\r\n"
        "  DEC Pins: STEP=%d, DIR=%d, EN=%d | Microstepping=%u | Steps/Rev=%u | MaxSpeed=%.1fsps\r\n"
        "  Safety: E-Stop Latch=%s | Max Allowed Speed=%.1fsps",
        c.config_version, c.config_fingerprint,
        c.pins.ra_step_pin, c.pins.ra_dir_pin, c.pins.ra_enable_pin,
        c.ra_axis.microstepping, c.ra_axis.steps_per_revolution, c.ra_axis.max_velocity_sps,
        c.pins.dec_step_pin, c.pins.dec_dir_pin, c.pins.dec_enable_pin,
        c.dec_axis.microstepping, c.dec_axis.steps_per_revolution, c.dec_axis.max_velocity_sps,
        c.safety.enable_estop_latch ? "YES" : "NO", c.safety.max_allowed_speed_sps
    );
    console_->print_line(buf);
}

void SystemService::process_command_line(const char* line) {
    command_count_++;
    uint32_t corr_id = ++correlation_counter_;

    auto parse_res = adapters::communications::CommandParser::parse(line, corr_id);
    if (parse_res.is_error()) {
        if (logger_) {
            logger_->emit_event(
                "ERR-COMMAND-001",
                core::ErrorSeverity::RECOVERABLE_ERROR,
                core::ErrorSource::COMMAND,
                corr_id,
                "Invalid command syntax or unrecognized command string"
            );
        }
        if (console_) {
            console_->print_line("ERROR [ERR-COMMAND-001]: Invalid or unrecognized command string. Type 'help' for syntax.");
        }
        return;
    }

    const auto& cmd = parse_res.value();
    switch (cmd.type) {
        case adapters::communications::CommandType::HELP:
            print_help();
            break;
        case adapters::communications::CommandType::VERSION:
            print_version();
            break;
        case adapters::communications::CommandType::STATUS:
            print_status();
            break;
        case adapters::communications::CommandType::DIAGNOSTICS:
            print_diagnostics();
            break;
        case adapters::communications::CommandType::CONFIG_INSPECT:
            print_config();
            break;
        case adapters::communications::CommandType::MOVE_RA: {
            if (safety_ && !safety_->is_motion_allowed()) {
                if (console_) console_->print_line("ERROR [FLT-SAFETY-001]: Motion inhibited by active safety fault latch!");
                return;
            }
            if (planner_) {
                auto res = planner_->move_axis(ports::AxisId::RA, cmd.ra_steps, cmd.speed_sps, cmd.accel_spss, corr_id);
                if (res.is_ok() && console_) {
                    console_->print_line("OK: RA motion commanded successfully.");
                } else if (console_) {
                    console_->print_line("ERROR: Failed to command RA motion.");
                }
            }
            break;
        }
        case adapters::communications::CommandType::MOVE_DEC: {
            if (safety_ && !safety_->is_motion_allowed()) {
                if (console_) console_->print_line("ERROR [FLT-SAFETY-001]: Motion inhibited by active safety fault latch!");
                return;
            }
            if (planner_) {
                auto res = planner_->move_axis(ports::AxisId::DEC, cmd.dec_steps, cmd.speed_sps, cmd.accel_spss, corr_id);
                if (res.is_ok() && console_) {
                    console_->print_line("OK: DEC motion commanded successfully.");
                } else if (console_) {
                    console_->print_line("ERROR: Failed to command DEC motion.");
                }
            }
            break;
        }
        case adapters::communications::CommandType::MOVE_SYNC: {
            if (safety_ && !safety_->is_motion_allowed()) {
                if (console_) console_->print_line("ERROR [FLT-SAFETY-001]: Motion inhibited by active safety fault latch!");
                return;
            }
            if (planner_) {
                auto res = planner_->move_sync(cmd.ra_steps, cmd.dec_steps, cmd.speed_sps, cmd.accel_spss, corr_id);
                if (res.is_ok() && console_) {
                    console_->print_line("OK: Simultaneous RA+DEC motion commanded successfully.");
                } else if (console_) {
                    console_->print_line("ERROR: Failed to command simultaneous RA+DEC motion.");
                }
            }
            break;
        }
        case adapters::communications::CommandType::STOP: {
            if (planner_) planner_->stop_all();
            if (console_) console_->print_line("OK: Motion stop initiated.");
            break;
        }
        case adapters::communications::CommandType::ESTOP: {
            if (planner_) planner_->emergency_stop();
            if (safety_) safety_->trigger_emergency_stop("Emergency stop commanded over serial interface");
            if (console_) console_->print_line("CRITICAL: EMERGENCY STOP EXECUTED. Steps disabled.");
            break;
        }
        case adapters::communications::CommandType::EVENTS_HISTORY: {
            if (reporter_ && console_) {
                char hist_buf[1536];
                reporter_->format_event_history(hist_buf, sizeof(hist_buf));
                console_->print(hist_buf);
            }
            break;
        }
        case adapters::communications::CommandType::FAULTS_HISTORY: {
            if (logger_ && console_) {
                const auto* fault = logger_->get_last_fault();
                if (fault) {
                    char buf[256];
                    std::snprintf(buf, sizeof(buf), "LAST FAULT: %s (Seq: %u) -> %s", fault->code, fault->sequence, fault->message);
                    console_->print_line(buf);
                } else {
                    console_->print_line("No faults recorded in history.");
                }
            }
            break;
        }
        case adapters::communications::CommandType::SYSTEM_RESET: {
            if (safety_) safety_->reset_safety_latch();
            if (console_) console_->print_line("OK: Fault latch cleared.");
            break;
        }
        default:
            break;
    }
}

void SystemService::update() {
    if (planner_) {
        planner_->update();
    }
}

} // namespace services
