#pragma once
#include "Command.hpp"
#include "protocol/RESPObject.hpp"
#include <stdexcept>

namespace redis::command {

class EchoCommand : public Command {
public:
  explicit EchoCommand(const protocol::RESPObject& obj) {
    const auto& arr = std::get<protocol::Array>(obj.value).values.value();
    if (arr.size() < 2) {
      throw std::runtime_error("ECHO command requires an argument");
    }
    arg_ = std::get<protocol::BulkString>(arr[1].value).value.value();
  }

  protocol::RESPObject execute() override {
    return protocol::RESPObject{protocol::BulkString{arg_}};
  }

private:
  std::string arg_;
};

} // namespace redis::command
