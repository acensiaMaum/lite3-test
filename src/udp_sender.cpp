#include "udp_sender.hpp"
#include "command.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

UdpSender::UdpSender(const std::string& destinationIp, uint16_t destinationPort)
  : socketFd_(-1), valid_(false), addr_(nullptr) {
  socketFd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (socketFd_ < 0) {
    return;
  }

  addr_ = new sockaddr_in();
  std::memset(addr_, 0, sizeof(sockaddr_in));
  addr_->sin_family = AF_INET;
  addr_->sin_port = htons(destinationPort);
  if (::inet_pton(AF_INET, destinationIp.c_str(), &addr_->sin_addr) != 1) {
    ::close(socketFd_);
    socketFd_ = -1;
    delete addr_;
    addr_ = nullptr;
    return;
  }

  valid_ = true;
}

UdpSender::~UdpSender() {
  if (socketFd_ >= 0) {
    ::close(socketFd_);
  }
  delete addr_;
}

UdpSender::UdpSender(UdpSender&& other) noexcept
  : socketFd_(other.socketFd_), valid_(other.valid_), addr_(other.addr_) {
  other.socketFd_ = -1;
  other.valid_ = false;
  other.addr_ = nullptr;
}

UdpSender& UdpSender::operator=(UdpSender&& other) noexcept {
  if (this != &other) {
    if (socketFd_ >= 0) ::close(socketFd_);
    delete addr_;
    socketFd_ = other.socketFd_;
    valid_ = other.valid_;
    addr_ = other.addr_;
    other.socketFd_ = -1;
    other.valid_ = false;
    other.addr_ = nullptr;
  }
  return *this;
}

bool UdpSender::send(const std::vector<uint8_t>& payload) const {
  if (!valid_ || socketFd_ < 0 || addr_ == nullptr) return false;
  const ssize_t sent = ::sendto(socketFd_, payload.data(), payload.size(), 0,
                                reinterpret_cast<sockaddr*>(addr_), sizeof(sockaddr_in));
  return sent == static_cast<ssize_t>(payload.size());
}

bool UdpSender::send(const CommandHead& head) const {
  if (!valid_ || socketFd_ < 0 || addr_ == nullptr) return false;
  std::vector<uint8_t> header_bytes;
  if (!command_io::serialize(head, header_bytes)) return false;
  const ssize_t sent = ::sendto(socketFd_, header_bytes.data(), header_bytes.size(), 0,
                                reinterpret_cast<sockaddr*>(addr_), sizeof(sockaddr_in));
  return sent == static_cast<ssize_t>(header_bytes.size());
}


