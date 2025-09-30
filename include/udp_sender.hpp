#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "command.hpp"

class UdpSender {
public:
  UdpSender(const std::string& destinationIp, uint16_t destinationPort);
  ~UdpSender();

  UdpSender(const UdpSender&) = delete;
  UdpSender& operator=(const UdpSender&) = delete;
  UdpSender(UdpSender&&) noexcept;
  UdpSender& operator=(UdpSender&&) noexcept;

  // Send raw bytes
  bool send(const std::vector<uint8_t>& payload) const;

  // Send only a CommandHead (header-only)
  bool send(const CommandHead& head) const;

  // Send entire Command struct (host-endian, raw struct bytes)
  bool send(const Command& cmd) const;

  // Returns whether the destination address and socket are valid
  bool is_valid() const { return valid_ && socketFd_ >= 0 && addr_ != nullptr; }

private:
  int socketFd_;
  bool valid_;
  struct sockaddr_in* addr_;
};


