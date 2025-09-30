#include "stable_sender.hpp"

#include <iostream>

StableSender::StableSender(UdpSender& sender, const Command& cmd, double hz)
  : sender_(sender), cmd_(cmd), type_(0) {
  if (hz <= 0.0) hz = 2.0;
  const auto period_ms = static_cast<int>(1000.0 / hz);
  period_ = std::chrono::milliseconds(period_ms > 1 ? period_ms : 1);
}
StableSender::StableSender(UdpSender& sender, const CommandHead& cmd, double hz)
  : sender_(sender), cmdhead_(cmd), type_(1) {
  if (hz <= 0.0) hz = 2.0;
  const auto period_ms = static_cast<int>(1000.0 / hz);
  period_ = std::chrono::milliseconds(period_ms > 1 ? period_ms : 1);
}

StableSender::~StableSender() {
  stop();
}

void StableSender::start() {
  if (running_.exchange(true)) return;
  worker_ = std::thread([this]{ this->run(); });
}

void StableSender::stop() {
  if (!running_.exchange(false)) return;
  if (worker_.joinable()) worker_.join();
}

void StableSender::run() {
  while (running_.load()) {
    if (type_ == 0){
      if (!sender_.send(cmd_)) {
      std::cerr << "Send failed\n";
      }
    }
    else if (type_ == 1){
      if (!sender_.send(cmdhead_)) {
        std::cerr << "Send failed\n";
      }
    }
    std::this_thread::sleep_for(period_);
  }
}


