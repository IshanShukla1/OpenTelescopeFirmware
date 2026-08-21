#include "adapters/communications/command_parser.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>

namespace adapters {
namespace communications {

static void trim_string(char* str) {
    if (!str) return;
    // Trim leading whitespace
    char* start = str;
    while (*start && std::isspace(static_cast<unsigned char>(*start))) start++;
    if (start != str) std::memmove(str, start, std::strlen(start) + 1);

    // Trim trailing whitespace
    size_t len = std::strlen(str);
    while (len > 0 && std::isspace(static_cast<unsigned char>(str[len - 1]))) {
        str[--len] = '\0';
    }
}

core::Result<ParsedCommand> CommandParser::parse(const char* input_line, uint32_t correlation_id) {
    if (!input_line || input_line[0] == '\0') {
        return core::Result<ParsedCommand>::Failure(core::StatusCode::INVALID_ARGUMENT);
    }

    char buf[64];
    std::strncpy(buf, input_line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    trim_string(buf);

    if (buf[0] == '\0') {
        return core::Result<ParsedCommand>::Failure(core::StatusCode::INVALID_ARGUMENT);
    }

    ParsedCommand cmd{};
    cmd.correlation_id = correlation_id;
    std::strncpy(cmd.raw_command, buf, sizeof(cmd.raw_command) - 1);

    if (std::strcmp(buf, "help") == 0 || std::strcmp(buf, "?") == 0) {
        cmd.type = CommandType::HELP;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "version") == 0) {
        cmd.type = CommandType::VERSION;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "status") == 0) {
        cmd.type = CommandType::STATUS;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "diagnostics") == 0 || std::strcmp(buf, "diag") == 0) {
        cmd.type = CommandType::DIAGNOSTICS;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "config") == 0) {
        cmd.type = CommandType::CONFIG_INSPECT;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "estop") == 0 || std::strcmp(buf, "emergency_stop") == 0) {
        cmd.type = CommandType::ESTOP;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "stop") == 0) {
        cmd.type = CommandType::STOP;
        cmd.target_axis = ports::AxisId::RA; // Default stop all
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "events") == 0) {
        cmd.type = CommandType::EVENTS_HISTORY;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "faults") == 0) {
        cmd.type = CommandType::FAULTS_HISTORY;
        return core::Result<ParsedCommand>::Success(cmd);
    }
    if (std::strcmp(buf, "reset") == 0) {
        cmd.type = CommandType::SYSTEM_RESET;
        return core::Result<ParsedCommand>::Success(cmd);
    }

    // Motion command parsing
    if (std::strncmp(buf, "move ra ", 8) == 0) {
        cmd.type = CommandType::MOVE_RA;
        cmd.target_axis = ports::AxisId::RA;
        int count = std::sscanf(buf + 8, "%ld %f %f", &cmd.ra_steps, &cmd.speed_sps, &cmd.accel_spss);
        if (count < 1) return core::Result<ParsedCommand>::Failure(core::StatusCode::INVALID_ARGUMENT);
        if (cmd.speed_sps <= 0.0f) cmd.speed_sps = 500.0f;    // Default safe speed SPS
        if (cmd.accel_spss <= 0.0f) cmd.accel_spss = 1000.0f; // Default safe accel SPSS
        return core::Result<ParsedCommand>::Success(cmd);
    }

    if (std::strncmp(buf, "move dec ", 9) == 0) {
        cmd.type = CommandType::MOVE_DEC;
        cmd.target_axis = ports::AxisId::DEC;
        int count = std::sscanf(buf + 9, "%ld %f %f", &cmd.dec_steps, &cmd.speed_sps, &cmd.accel_spss);
        if (count < 1) return core::Result<ParsedCommand>::Failure(core::StatusCode::INVALID_ARGUMENT);
        if (cmd.speed_sps <= 0.0f) cmd.speed_sps = 500.0f;
        if (cmd.accel_spss <= 0.0f) cmd.accel_spss = 1000.0f;
        return core::Result<ParsedCommand>::Success(cmd);
    }

    if (std::strncmp(buf, "move sync ", 10) == 0) {
        cmd.type = CommandType::MOVE_SYNC;
        int count = std::sscanf(buf + 10, "%ld %ld %f %f", &cmd.ra_steps, &cmd.dec_steps, &cmd.speed_sps, &cmd.accel_spss);
        if (count < 2) return core::Result<ParsedCommand>::Failure(core::StatusCode::INVALID_ARGUMENT);
        if (cmd.speed_sps <= 0.0f) cmd.speed_sps = 500.0f;
        if (cmd.accel_spss <= 0.0f) cmd.accel_spss = 1000.0f;
        return core::Result<ParsedCommand>::Success(cmd);
    }

    return core::Result<ParsedCommand>::Failure(core::StatusCode::UNKNOWN_COMMAND);
}

} // namespace communications
} // namespace adapters
