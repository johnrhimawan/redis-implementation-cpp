#include "RESPObject.hpp"
#include <string>

namespace redis::protocol {

std::string writeRESPObject(const RESPObject& resp_object);

} // namespace redis::protocol
