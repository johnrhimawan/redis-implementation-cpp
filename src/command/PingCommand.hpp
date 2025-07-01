#pragma once
#include "Command.hpp"

namespace redis::command {

class PingCommand : public Command {
public:
  explicit PingCommand(const protocol::RESPObject&) {}
  protocol::RESPObject execute() override {
    return protocol::RESPObject{protocol::SimpleString{"PONG"}};
  }
};

} // namespace redis::command
