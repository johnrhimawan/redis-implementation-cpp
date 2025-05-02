#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

static constexpr int PORT = 6379;
static constexpr int BUFFER_SIZE = 1024;
static constexpr int MAX_EVENTS = 1024;

int setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }

  if (setNonBlocking(server_fd) < 0) {
    std::cerr << "Failed to make server non-blocking";
    return 1;
  }
  
  std::vector<pollfd> watch_fds;
  watch_fds.reserve(MAX_EVENTS);
  watch_fds.push_back({ .fd = server_fd, .events = POLLIN, .revents = 0});

  std::cout << "Server listening on port " << PORT << "\n";

  while (true) {
    int num_events = poll(watch_fds.data(), watch_fds.size(), -1);
    if (num_events < 0) {
      std::cerr << "There are no events to be polled";
      break;
    }

    for (size_t i = 0; i < watch_fds.size() && num_events > 0; ++i) {
      if (watch_fds[i].revents == 0) continue;
      --num_events;

      if (watch_fds[i].fd == server_fd) {
        while (true) {
          struct sockaddr_in client_addr;
          int client_addr_len = sizeof(client_addr);
          std::cout << "Waiting for a client to connect...\n";

          int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
          if (client_fd < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
              break;
            } else {
              std::cerr << "Failed to accept client connection\n";
              break;
            }
          }
          setNonBlocking(client_fd);
          watch_fds.push_back({ .fd = client_fd, .events = POLLIN, .revents = 0 });
          std::cout << "Accepted new client: fd=" << client_fd << "\n";
        }
      } else if (watch_fds[i].revents & POLLIN) {
        int client_fd = watch_fds[i].fd;
        char buffer[BUFFER_SIZE];
        ssize_t bytes_received = recv(client_fd, &buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
          std::cerr << "Failed to receive message\n";
          close(client_fd);
          watch_fds.erase(watch_fds.begin() + i);
          --i;
        } else {
          const std::string response = "+PONG\r\n";
          send(client_fd, response.c_str(), response.size(), 0);
        }
      }

      if (i < watch_fds.size()) {
        watch_fds[i].revents = 0;
      }
    }
  }

  for (auto &p : watch_fds) {
    close(p.fd);
  }

  close(server_fd);

  return 0;
}
