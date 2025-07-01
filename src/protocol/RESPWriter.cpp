#include "RESPWriter.hpp"
#include <variant>

namespace redis::protocol {

std::string writeRESPObject(const RESPObject& resp_object) {
  static constexpr std::string CRLF_SYMBOL = "\r\n";

  struct Writer {
    std::string operator()(const SimpleString& simple_string) const {
      return "+" + simple_string.value + CRLF_SYMBOL;
    }
    std::string operator()(const BulkString& bulk_string) const {
      if (!bulk_string.value) return "$-1\r\n";
      auto &s = *bulk_string.value;
      return "$" + std::to_string(s.size()) + CRLF_SYMBOL + s + CRLF_SYMBOL;
    }
    std::string operator()(const Array& array) const {
      if (!array.values) return "*-1\r\n";
      const auto& vals = *array.values;
      std::string out = "*" + std::to_string(vals.size()) + CRLF_SYMBOL;
      for (const auto& elem : vals) {
        out += writeRESPObject(elem);
      }
      return out;
    }
  } writer;
  return std::visit(writer, resp_object.value);
}

}