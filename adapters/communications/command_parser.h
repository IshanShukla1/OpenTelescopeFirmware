#ifndef ADAPTERS_COMMUNICATIONS_COMMAND_PARSER_H
#define ADAPTERS_COMMUNICATIONS_COMMAND_PARSER_H

#include "core/types.h"
#include "core/result.h"
#include "ports/motion_executor_port.h"
#include <cstdint>

namespace adapters {
namespace communications {

enum class CommandType : uint8_t {
    UNKNOWN = 0,
    HELP,
    VERSION,
    STATUS,
    DIAGNOSTICS,
    CONFIG_INSPECT,
    MOVE_RA,
    MOVE_DEC,
    MOVE_SYNC,
    STOP,
    ESTOP,
    EVENTS_HISTORY,
    FAULTS_HISTORY,
    SYSTEM_RESET
};

struct ParsedCommand {
    CommandType type{CommandType::UNKNOWN};
    int32_t ra_steps{0};
    int32_t dec_steps{0};
    float speed_sps{0.0f};
    float accel_spss{0.0f};
    ports::AxisId target_axis{ports::AxisId::RA};
    uint32_t correlation_id{0};
    char raw_command[64]{0};
};

class CommandParser {
public:
    static core::Result<ParsedCommand> parse(const char* line, uint32_t correlation_id = 0);
};

} // namespace communications
} // namespace adapters

#endif // ADAPTERS_COMMUNICATIONS_COMMAND_PARSER_H
