#include "session/ClientSession.hpp"
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include "network/Reactor.hpp"

namespace redis::session { class ClientSession; }
namespace redis::server {

class Server {
public:
  explicit Server(uint16_t port, size_t maxEvents = 1024);

  void run();

private:
  void setupListener();
  void acceptLoop();
  void onEvent(int fd, short revents);

  uint16_t _port;
  int      _listenerFd;
  redis::network::Reactor _reactor;
  std::unordered_map<int, std::unique_ptr<redis::session::ClientSession>> _sessions;
};

} // namespace
