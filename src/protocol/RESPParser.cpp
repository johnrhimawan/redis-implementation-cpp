#include "RESPParser.hpp"
#include <cstring>
#include <stdexcept>

namespace redis::protocol {

auto findCRLF(const std::vector<char>& buffer, size_t start) {
  for (size_t i = start; i + 1 < buffer.size(); ++i) {
    if (buffer[i] == '\r' && buffer[i + 1] == '\n') return i;
  }
  return buffer.size();
}

bool parseRESPObject(const std::vector<char>& buffer, RESPObject& out, size_t& consumed) {
  static constexpr int CRLF_SIZE = 2;
  consumed = 0;
  if (buffer.empty()) return false;
  char prefix = buffer[0];

  switch (prefix) {
  case '+': { // Simple String
    auto pos = findCRLF(buffer, 1);
    if (pos == buffer.size()) return false;
    out.value = SimpleString{ std::string(buffer.begin() + 1, buffer.begin() + pos) };
    consumed = pos + CRLF_SIZE;
    return true;
  }
  case '$': { // Bulk String
    auto pos = findCRLF(buffer, 1);
    if (pos == buffer.size()) return false;
    int len = std::stoi(std::string(buffer.begin() + 1, buffer.begin() + pos));
    if (len < 0) {
      out.value = BulkString{ std::nullopt }; // null bulk
      consumed = pos + CRLF_SIZE;
      return true;
    }

    size_t total = pos + CRLF_SIZE + len + CRLF_SIZE;
    if (buffer.size() < total) return false;
    out.value = BulkString{ std::string(buffer.begin() + pos + CRLF_SIZE, buffer.begin() + pos + CRLF_SIZE + len) };
    consumed = total;
    return true;
  }
  case '*': { // Array
    auto pos = findCRLF(buffer, 1);
    if (pos == buffer.size()) return false;
    int count = std::stoi(std::string(buffer.begin() + 1, buffer.begin() + pos));
    size_t idx = pos + CRLF_SIZE;
    if (count < 0) {
      out.value = Array{ std::nullopt };
      consumed = idx;
      return true;
    }
    std::vector<RESPObject> values;
    values.reserve(count);

    for (int i = 0; i < count; ++i) {
      std::vector<char> subbuf(buffer.begin() + idx, buffer.end());

      RESPObject elem;
      size_t usedElem = 0;
      if (!parseRESPObject(subbuf, elem, usedElem)) {
        return false;
      }

      values.push_back(std::move(elem));
      idx += usedElem;
    }

    out.value = Array{ std::move(values) };
    consumed = idx;
    return true;
  }
  default:
    throw std::runtime_error("Unknown RESP prefix");
  }
}

} // namespace redis::protocol
