#include "command.hpp"
#include "udp_sender.hpp"
#include "stable_sender.hpp"
#include "hex_parser.hpp"

#include <iostream>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>
#include <csignal>

static std::atomic<bool> g_running{true};

int main(int argc, char** argv) {
  // const std::string ip = (argc >= 2) ? std::string(argv[1]) : std::string("192.168.2.1");
  uint32_t cmd_value = (argc >= 3) ? static_cast<uint16_t>(std::stoi(argv[2])) : 0;
  const std::string ip = "192.168.2.1";
  const uint16_t port = 43893;
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <command> <value?> \n";
    return 1;
  }

  
  uint32_t code = hexStringToUint32(argv[1]);
  

  Command cmd{};
  cmd.head.code = code;
  cmd.head.type = 0;
  // for (size_t i = 0; i < kDataSize; ++i) cmd.data[i] = 0u;
  // cmd.data[0] = cmd_value;
  cmd.head.paramters_size = cmd_value; //sizeof(cmd.data);
  std::cout<<"Send value: "<<int(cmd_value)<<std::endl;
  UdpSender sender(ip, port);
  if (!sender.is_valid()) {
    std::cerr << "Invalid destination or socket init failed: " << ip << ":" << port << "\n";
    return 3;
  }

  std::signal(SIGINT, [](int){ g_running.store(false); });
  std::cout << "Starting 2 Hz sender to " << ip << ":" << port << " (Ctrl+C to stop)\n";

  StableSender stable(sender, cmd, 25.0);
  stable.start();

  while (g_running.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  stable.stop();
  std::cout << "Sender stopped.\n";
    return 0;
}


