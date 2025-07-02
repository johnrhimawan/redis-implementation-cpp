#pragma once
#include "Command.hpp"
#include "storage/Storage.hpp"

namespace redis::command {

class SetCommand : public Command {
public:
    SetCommand(const protocol::RESPObject& obj, redis::storage::Storage& storage)
        : _storage(storage) {
        const auto& arr = std::get<protocol::Array>(obj.value).values.value();
        if (arr.size() < 3) throw std::runtime_error("SET requires key and value");
        _key = std::get<protocol::BulkString>(arr[1].value).value.value();
        _value = std::get<protocol::BulkString>(arr[2].value).value.value();
    }

    protocol::RESPObject execute() override {
        _storage.set(_key, _value);
        return protocol::RESPObject{protocol::SimpleString{"OK"}};
    }

private:
    std::string _key, _value;
    redis::storage::Storage& _storage;
};

} // namespace redis::command
