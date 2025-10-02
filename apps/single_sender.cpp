#include "command.hpp"
#include "udp_sender.hpp"
#include "hex_parser.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
  std::string help("--help"); //cstring
  if (argc >= 2){
    std::string arg1(argv[1]);
    if (arg1 == help){
    std::cout<<"Single sender manual"<<std::endl;
    return 0;}
  }
  uint32_t code = (argc >= 2) ? hexStringToUint32(argv[1]) : 0x21010202u;
  // uint32_t cmd_type = (argc >= 3) ? std::stoi(argv[2]) : 0;
  uint32_t cmd_value = (argc >= 3) ? static_cast<uint16_t>(std::stoi(argv[2])) : 0;

  const std::string ip = "192.168.2.1";
  const uint16_t port = 43893;

  
  // Example payload (host-endian raw struct send)
  Command cmd{};
  cmd.head.code = code;
  cmd.head.type = 0u;
  cmd.head.paramters_size = cmd_value; //2u * sizeof(uint32_t);
  // for (size_t i = 0; i < kDataSize; ++i) cmd.data[i] = 0u;
  // cmd.data[0] = cmd_value;
  // cmd.data[1] = 200u;

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


