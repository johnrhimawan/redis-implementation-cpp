#include "CommandRegistry.hpp"
#include "PingCommand.hpp"
#include "SetCommand.hpp"
#include "GetCommand.hpp"
#include "EchoCommand.hpp"

namespace redis::command {

void CommandRegistry::initialize(CommandRegistry &r) {
    r.registerCommand("PING", [](auto const& f){ return std::make_unique<PingCommand>(f); });
    r.registerCommand("ECHO", [](auto const& f){ return std::make_unique<EchoCommand>(f); });
}

} // namespace
