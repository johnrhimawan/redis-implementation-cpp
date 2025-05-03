#include <vector>
#include <poll.h>
#include <functional>

namespace redis::network {
class Reactor {
  public:
    using EventCb = std::function<void(int /*fd*/, short /*revents*/)>;
    explicit Reactor(size_t max_events = 1024);
    void add(int fd, short events = POLLIN);
    void remove(int fd);
    void setEventHandler(EventCb cb);
    void run();
    
  private:
    std::vector<pollfd> _fds;
    EventCb             _cb;
};

} // namespace
