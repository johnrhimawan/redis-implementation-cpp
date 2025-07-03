#pragma once
#include "Command.hpp"
#include "storage/Storage.hpp"
#include <optional>
#include <cctype>

namespace redis::command {

class SetCommand : public Command {
public:
    SetCommand(const protocol::RESPObject& obj, redis::storage::Storage& storage)
        : _storage(storage) {
        const auto& arr = std::get<protocol::Array>(obj.value).values.value();
        if (arr.size() < 3) throw std::runtime_error("SET requires key and value");
        _key = std::get<protocol::BulkString>(arr[1].value).value.value();
        _value = std::get<protocol::BulkString>(arr[2].value).value.value();
        // Parse PX argument if present
        for (size_t i = 3; i + 1 < arr.size(); ++i) {
            auto arg = std::get<protocol::BulkString>(arr[i].value).value.value();
            if (arg.size() == 2 && (arg[0] == 'P' || arg[0] == 'p') && (arg[1] == 'X' || arg[1] == 'x')) {
                auto px_val = std::get<protocol::BulkString>(arr[i+1].value).value.value();
                _px_expiry = std::stoll(px_val);
                break;
            }
        }
    }

    protocol::RESPObject execute() override {
        _storage.set(_key, _value, _px_expiry);
        return protocol::RESPObject{protocol::SimpleString{"OK"}};
    }

private:
    std::string _key, _value;
    std::optional<int64_t> _px_expiry;
    redis::storage::Storage& _storage;
};

} // namespace redis::command
