#pragma once
#include "Command.hpp"
#include <unordered_map>
#include <functional>
#include <memory>

namespace redis::command {

class CommandRegistry {
public:
  using Factory = std::function<std::unique_ptr<Command>(const protocol::RESPObject&)>;

  void registerCommand(std::string name, Factory f) {
    _factories[std::move(name)] = std::move(f);
  }

  std::unique_ptr<Command> create(const protocol::RESPObject& resp_object) const {
    auto &arr = std::get<protocol::Array>(resp_object.value).values.value();
    std::string cmd = std::get<protocol::BulkString>(arr[0].value).value.value();
    auto it = _factories.find(cmd);
    if (it == _factories.end()) {
      throw std::runtime_error("Unknown command: " + cmd);
    }
    return it->second(resp_object);
  }

  static void initialize(CommandRegistry&);

private:
  std::unordered_map<std::string, Factory> _factories;
};

} // namespace redis::command
