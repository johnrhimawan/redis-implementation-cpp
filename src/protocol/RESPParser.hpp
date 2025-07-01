#include "RESPObject.hpp"
#include <vector>

namespace redis::protocol {

bool parseRESPObject(const std::vector<char>& buffer, RESPObject& out, size_t& consumed);

} // namespace redis::protocol
