#pragma once
#include "protocol/RESPObject.hpp"

namespace redis::command {

class Command {
public:
  virtual ~Command() = default;
  virtual protocol::RESPObject execute() = 0;
};

} // namespace redis::command
