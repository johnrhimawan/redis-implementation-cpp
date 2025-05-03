#include "network/Reactor.hpp"
#include <unistd.h>
using namespace redis::network;

Reactor::Reactor(size_t max_events) { _fds.reserve(max_events); }

void Reactor::add(int fd, short events) {
  _fds.push_back({ .fd = fd, .events = events, .revents = 0});
}

void Reactor::remove(int fd) {
  for (size_t i = 0; i < _fds.size(); ++i) {
    if (_fds[i].fd == fd) {
      _fds[i] = _fds.back();
      _fds.pop_back();
      return;
    }
  }
}

void Reactor::setEventHandler(EventCb cb) {
    _cb = std::move(cb);
}

void Reactor::run() {
  while (true) {
    int num_events = poll(_fds.data(), _fds.size(), -1);
    if (num_events < 0) break;
    for (auto &p : _fds) {
      if (p.revents) {
        _cb(p.fd, p.revents);
        p.revents = 0;
        if (--num_events == 0) break;
      }
    }
  }
}
