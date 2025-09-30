#pragma once

#include <atomic>
#include <thread>
#include <chrono>

#include "udp_sender.hpp"
#include "command.hpp"

class StableSender {
public:
  StableSender(UdpSender& sender, const Command& cmd, double hz = 2.0);
  StableSender(UdpSender& sender, const CommandHead& cmd, double hz = 2.0);
  ~StableSender();

  StableSender(const StableSender&) = delete;
  StableSender& operator=(const StableSender&) = delete;
  StableSender(StableSender&&) = delete;
  StableSender& operator=(StableSender&&) = delete;

  void start();
  void stop();
  bool is_running() const { return running_.load(); }

private:
  void run();

  UdpSender& sender_;
  Command cmd_;
  CommandHead cmdhead_;
  int type_;
  std::chrono::milliseconds period_;
  std::atomic<bool> running_{false};
  std::thread worker_;
};


