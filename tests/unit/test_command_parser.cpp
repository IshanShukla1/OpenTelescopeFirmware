#include "adapters/communications/command_parser.h"
#include <cassert>
#include <cstdio>

void test_command_parser() {
    using namespace adapters::communications;

    // 1. Test basic system commands
    auto res_help = CommandParser::parse("help");
    assert(res_help.is_ok());
    assert(res_help.value().type == CommandType::HELP);

    auto res_status = CommandParser::parse("status");
    assert(res_status.is_ok());
    assert(res_status.value().type == CommandType::STATUS);

    auto res_estop = CommandParser::parse("estop");
    assert(res_estop.is_ok());
    assert(res_estop.value().type == CommandType::ESTOP);

    // 2. Test move RA command parsing
    auto res_move_ra = CommandParser::parse("move ra 1000 500 1000");
    assert(res_move_ra.is_ok());
    assert(res_move_ra.value().type == CommandType::MOVE_RA);
    assert(res_move_ra.value().ra_steps == 1000);
    assert(res_move_ra.value().speed_sps == 500.0f);
    assert(res_move_ra.value().accel_spss == 1000.0f);

    // 3. Test move sync command parsing
    auto res_sync = CommandParser::parse("move sync -2000 4000 800 1500");
    assert(res_sync.is_ok());
    assert(res_sync.value().type == CommandType::MOVE_SYNC);
    assert(res_sync.value().ra_steps == -2000);
    assert(res_sync.value().dec_steps == 4000);

    // 4. Test invalid command handling
    auto res_invalid = CommandParser::parse("invalid_command_xyz");
    assert(res_invalid.is_error());
    assert(res_invalid.status() == core::StatusCode::UNKNOWN_COMMAND);

    std::printf("[PASS] test_command_parser\n");
}
