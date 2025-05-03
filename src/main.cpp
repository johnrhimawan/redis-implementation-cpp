#include "server/Server.hpp"
#include <iostream>
#include <exception>

int main() {
  try {
    redis::server::Server srv{6379};
    srv.run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
