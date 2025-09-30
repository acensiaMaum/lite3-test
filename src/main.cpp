#include "command.hpp"
#include "udp_sender.hpp"

#include <iostream>
#include <vector>
#include <string>

// int main(int argc, char** argv) {
int main() {
//   if (argc < 3) {
//     std::cerr << "Usage: " << argv[0] << " <dest_ip> <port>\n";
//     return 1;
//   }

  const std::string ip = "192.168.2.1";
  const uint16_t port = 43893;

  // Example payload
//   Command cmd{};
//   cmd.head.code = 0x21010202u;
//   cmd.head.type = 0u;
//   cmd.head.paramters_size = 2u * sizeof(uint32_t);
    int32_t data[64];
    CommandHead cmd{};
   cmd.code = 0x21010202u;
   cmd.type = 0;
   cmd.paramters_size = 2 * sizeof(data);
//   cmd.data[0] = 100u;
//   cmd.data[1] = 200u;
//   for (size_t i = 0; i < kDataSize; ++i) cmd.data[i] = 0u;

  std::vector<uint8_t> bytes;
  if (!command_io::serialize(cmd, bytes)) {
    std::cerr << "Serialization failed\n";
    return 2;
  }

  UdpSender sender(ip, port);
  if (!sender.is_valid()) {
    std::cerr << "Invalid destination or socket init failed: " << ip << ":" << port << "\n";
    return 3;
  }

  if (!sender.send(bytes)) {
    std::cerr << "Send failed\n";
    return 4;
  }

  std::cout << "Sent example command to " << ip << ":" << port << " -> " << command_io::to_string(cmd) << "\n";
  return 0;
}


