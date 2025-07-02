#pragma once
#include "Command.hpp"
#include "storage/Storage.hpp"

namespace redis::command {

class GetCommand : public Command {
public:
    GetCommand(const protocol::RESPObject& obj, redis::storage::Storage& storage)
        : _storage(storage) {
        const auto& arr = std::get<protocol::Array>(obj.value).values.value();
        if (arr.size() < 2) throw std::runtime_error("GET requires key");
        _key = std::get<protocol::BulkString>(arr[1].value).value.value();
    }

    protocol::RESPObject execute() override {
        auto val = _storage.get(_key);
        if (val) return protocol::RESPObject{protocol::BulkString{*val}};
        return protocol::RESPObject{protocol::BulkString{std::nullopt}};
    }

private:
    std::string _key;
    redis::storage::Storage& _storage;
};

} // namespace redis::command
