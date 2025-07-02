#include "CommandRegistry.hpp"
#include "PingCommand.hpp"
#include "SetCommand.hpp"
#include "GetCommand.hpp"
#include "EchoCommand.hpp"
#include "storage/Storage.hpp"

namespace redis::command {

void CommandRegistry::initialize(CommandRegistry &r, redis::storage::Storage& storage) {
    r.registerCommand("PING", [](auto const& f){ return std::make_unique<PingCommand>(f); });
    r.registerCommand("ECHO", [](auto const& f){ return std::make_unique<EchoCommand>(f); });
    r.registerCommand("SET", [&storage](auto const& f){ return std::make_unique<SetCommand>(f, storage); });
    r.registerCommand("GET", [&storage](auto const& f){ return std::make_unique<GetCommand>(f, storage); });
}

} // namespace
