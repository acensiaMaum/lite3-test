#include "command.hpp"
#include "udp_sender.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] << " <dest_ip> <port> <code> <type> [u32,u32,...]\n";
    return 1;
  }

  const std::string ip = argv[1];
  const uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));
  const uint32_t code = static_cast<uint32_t>(std::stoul(argv[3]));
  const uint32_t type = static_cast<uint32_t>(std::stoul(argv[4]));

  std::vector<uint32_t> values;
  if (argc >= 6) {
    std::string list = argv[5];
    size_t start = 0;
    while (start <= list.size()) {
      size_t pos = list.find(',', start);
      std::string token = (pos == std::string::npos) ? list.substr(start) : list.substr(start, pos - start);
      if (!token.empty()) {
        values.push_back(static_cast<uint32_t>(std::stoul(token)));
      }
      if (pos == std::string::npos) break;
      start = pos + 1;
    }
  }

  if (values.size() > kDataSize) {
    std::cerr << "Too many values: max " << kDataSize << "\n";
    return 1;
  }

  Command cmd{};
  cmd.head.code = code;
  cmd.head.type = type;
  cmd.head.paramters_size = static_cast<uint32_t>(values.size() * sizeof(uint32_t));
  for (size_t i = 0; i < values.size(); ++i) cmd.data[i] = values[i];
  for (size_t i = values.size(); i < kDataSize; ++i) cmd.data[i] = 0u;

  std::vector<uint8_t> bytes;
  if (!command_io::serialize(cmd, bytes)) {
    std::cerr << "Serialization failed\n";
    return 1;
  }

  UdpSender sender(ip, port);
  if (!sender.is_valid()) {
    std::cerr << "Invalid destination IP or socket init failed: " << ip << ":" << port << "\n";
    return 2;
  }
  if (!sender.send(bytes)) {
    std::cerr << "Send failed (sendto returned short or error)\n";
    return 2;
  }

  std::cout << "Sent: " << command_io::to_string(cmd) << "\n";
  return 0;
}


