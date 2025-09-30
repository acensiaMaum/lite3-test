#include "command.hpp"
#include "udp_sender.hpp"
#include "stable_sender.hpp"

#include <iostream>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>
#include <csignal>

static std::atomic<bool> g_running{true};

int main(int argc, char** argv) {
  const std::string ip = (argc >= 2) ? std::string(argv[1]) : std::string("192.168.2.1");
  const uint16_t port = (argc >= 3) ? static_cast<uint16_t>(std::stoi(argv[2])) : static_cast<uint16_t>(43893);

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

  std::signal(SIGINT, [](int){ g_running.store(false); });
  std::cout << "Starting 2 Hz sender to " << ip << ":" << port << " (Ctrl+C to stop)\n";

  StableSender stable(sender, cmd, 2.0);
  stable.start();

  while (g_running.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  stable.stop();
  std::cout << "Sender stopped.\n";
  return 0;
}


