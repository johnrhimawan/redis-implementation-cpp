#pragma once
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <optional>

namespace redis::protocol {

struct RESPObject;

struct SimpleString     { std::string value; };
struct BulkString       { std::optional<std::string> value; };
struct Array            { std::optional<std::vector<RESPObject>> values; };

using RESPObjectValue = std::variant<
    SimpleString,
    BulkString,
    Array
>;

struct RESPObject {
    RESPObjectValue value;
};

} // namespace
