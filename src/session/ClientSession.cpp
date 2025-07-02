#include "session/ClientSession.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include "protocol/RESPParser.hpp"
#include "protocol/RESPWriter.hpp"
#include "command/CommandRegistry.hpp"
#include <vector>
#include "storage/Storage.hpp"

using namespace redis::session;
using namespace redis::protocol;
using namespace redis::command;

namespace {
static CommandRegistry registry;
static redis::storage::Storage kv_store;
static bool initialized = false;
}

ClientSession::ClientSession(int fd)
  : _fd(fd)
{
  if (!initialized) {
    CommandRegistry::initialize(registry, kv_store);
    initialized = true;
  }
}

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

  std::vector<char> input(buffer, buffer + bytes_received);
  RESPObject req;
  size_t consumed = 0;
  std::string response;
  try {
    if (parseRESPObject(input, req, consumed)) {
      auto cmd = registry.create(req);
      RESPObject resp = cmd->execute();
      response = writeRESPObject(resp);
    } else {
      response = "-ERR Invalid command format\r\n";
    }
  } catch (const std::exception& e) {
    response = std::string("-ERR ") + e.what() + "\r\n";
  }
  send(_fd, response.c_str(), response.size(), 0);
  return true;
}
