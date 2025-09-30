#include "command.hpp"
#include "udp_sender.hpp"

#include <iostream>
#include <string>
#include <vector>

// int main(int argc, char** argv) {
int main() {
//   if (argc < 3) {
//     std::cerr << "Usage: " << argv[0] << " <dest_ip> <port>\n";
//     return 1;
//   }

  const std::string ip = "192.168.2.1";
  const uint16_t port = 43893;

  // Example payload (host-endian raw struct send)
  Command cmd{};
  cmd.head.code = 0x21010202u;
  cmd.head.type = 0u;
  cmd.head.paramters_size = 2u * sizeof(uint32_t);
  for (size_t i = 0; i < kDataSize; ++i) cmd.data[i] = 0u;
  cmd.data[0] = 100u;
  cmd.data[1] = 200u;

  UdpSender sender(ip, port);
  if (!sender.is_valid()) {
    std::cerr << "Invalid destination or socket init failed: " << ip << ":" << port << "\n";
    return 3;
  }

  if (!sender.send(cmd)) {
    std::cerr << "Send failed\n";
    return 4;
  }

  std::cout << "Sent example command to " << ip << ":" << port << " -> " << command_io::to_string(cmd) << "\n";
  return 0;
}


