#include "server/Server.hpp"
#include "util/SocketUtils.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <stdexcept>

using namespace redis;
using namespace redis::network;
using namespace redis::session;
using namespace redis::server;

Server::Server(uint16_t port, size_t maxEvents)
  : _port(port)
  , _reactor(maxEvents) 
{
  setupListener();
  _reactor.add(_listenerFd, POLLIN);
  _reactor.setEventHandler([&](int fd, short re) { onEvent(fd, re); });
}

void Server::setupListener() {
  _listenerFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_listenerFd < 0) throw std::runtime_error("socket() failed");

  int reuse = 1;
  if (setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
    throw std::runtime_error("setsockopt() failed");

  sockaddr_in addr{};
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port        = htons(_port);

  if (bind(_listenerFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
    throw std::runtime_error("bind() failed");
  if (listen(_listenerFd, SOMAXCONN) != 0)
    throw std::runtime_error("listen() failed");
  if (redis::util::setNonBlocking(_listenerFd) < 0)
    throw std::runtime_error("setNonBlocking() failed");
}

void Server::acceptLoop() {
  while (true) {
    sockaddr_in cli{};
    socklen_t len = sizeof(cli);
    int clientFd = accept(_listenerFd, reinterpret_cast<sockaddr*>(&cli), &len);
    if (clientFd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      perror("accept");
      break;
    }
    redis::util::setNonBlocking(clientFd);
    _sessions[clientFd] = std::make_unique<ClientSession>(clientFd);
    _reactor.add(clientFd, POLLIN);
    std::cout << "New client fd=" << clientFd << "\n";
  }
}

void Server::onEvent(int fd, short revents) {
  if (fd == _listenerFd) {
    acceptLoop();
  } else if (revents & POLLIN) {
    auto it = _sessions.find(fd);
    if (it != _sessions.end() && !it->second->onReadable()) {
      _reactor.remove(fd);
      _sessions.erase(it);
    }
  }
}

void Server::run() {
  std::cout << "Server listening on port " << _port << "\n";
  _reactor.run();
}
