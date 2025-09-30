#include "command.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <listen_port> [--once]\n";
    return 1;
  }

  const uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
  const bool once = (argc >= 3 && std::string(argv[2]) == "--once");

  int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    std::cerr << "socket() failed\n";
    return 2;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    std::cerr << "bind() failed (try another port or check permissions)\n";
    ::close(fd);
    return 3;
  }

  std::vector<uint8_t> buffer(command_io::kMaxWireBytes);
  while (true) {
    sockaddr_in peer{};
    socklen_t peer_len = sizeof(peer);
    ssize_t n = ::recvfrom(fd, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr*>(&peer), &peer_len);
    if (n < 0) {
      std::cerr << "recvfrom() failed\n";
      break;
    }
    Command cmd{};
    if (!command_io::deserialize(buffer.data(), static_cast<size_t>(n), cmd)) {
      std::cerr << "Invalid packet of size " << n << "\n";
      continue;
    }
    char peer_ip[INET_ADDRSTRLEN] = {0};
    ::inet_ntop(AF_INET, &peer.sin_addr, peer_ip, sizeof(peer_ip));
    std::cout << "From " << peer_ip << ":" << ntohs(peer.sin_port) << " -> " << command_io::to_string(cmd) << "\n";
    if (once) break;
  }

  ::close(fd);
  return 0;
}


