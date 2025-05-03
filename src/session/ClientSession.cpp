#include "session/ClientSession.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <string>

using namespace redis::session;

ClientSession::ClientSession(int fd)
  : _fd(fd)
{}

ClientSession::~ClientSession() {
  close(_fd);
}

int ClientSession::fd() const {
  return _fd;
}

bool ClientSession::onReadable() {
  static constexpr int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  ssize_t bytes_received = recv(_fd, buffer, sizeof(buffer), 0);
  if (bytes_received <= 0) {
    std::cerr << "Client Disconnected: fd=" << _fd << "\n";
    return false;
  }
  
  const std::string response = "+PONG\r\n";
  send(_fd, response.c_str(), response.size(), 0);
  return true;
}
