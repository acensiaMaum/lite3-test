#include "command.hpp"

#include <arpa/inet.h>
#include <sstream>
#include <cstring>

namespace command_io {

static inline void write_u32_be(uint32_t value, uint8_t* out) {
  uint32_t be = htonl(value);
  std::memcpy(out, &be, sizeof(uint32_t));
}

static inline uint32_t read_u32_be(const uint8_t* in) {
  uint32_t be;
  std::memcpy(&be, in, sizeof(uint32_t));
  return ntohl(be);
}

bool serialize(const CommandHead& head, std::vector<uint8_t>& out) {
  // Validate that paramters_size aligns to 4 bytes (u32)
  if (head.paramters_size % sizeof(uint32_t) != 0) return false;
  out.resize(kHeaderSizeBytes);
  write_u32_be(head.code, out.data() + 0);
  write_u32_be(head.paramters_size, out.data() + 4);
  write_u32_be(head.type, out.data() + 8);
  return true;
}

bool serialize(const Command& cmd, std::vector<uint8_t>& out) {
  if (cmd.head.paramters_size % sizeof(uint32_t) != 0) return false;
  const uint32_t num_params = cmd.head.paramters_size / sizeof(uint32_t);
  if (num_params > kDataSize) return false;

  out.resize(kHeaderSizeBytes + cmd.head.paramters_size);

  // Header
  write_u32_be(cmd.head.code, out.data() + 0);
  write_u32_be(cmd.head.paramters_size, out.data() + 4);
  write_u32_be(cmd.head.type, out.data() + 8);

  // Data
  uint8_t* data_ptr = out.data() + kHeaderSizeBytes;
  for (uint32_t i = 0; i < num_params; ++i) {
    write_u32_be(cmd.data[i], data_ptr + i * sizeof(uint32_t));
  }
  return true;
}

bool deserialize(const uint8_t* buffer, size_t length, Command& out) {
  if (length < kHeaderSizeBytes) return false;

  out.head.code = read_u32_be(buffer + 0);
  out.head.paramters_size = read_u32_be(buffer + 4);
  out.head.type = read_u32_be(buffer + 8);

  if (out.head.paramters_size % sizeof(uint32_t) != 0) return false;
  const uint32_t num_params = out.head.paramters_size / sizeof(uint32_t);
  if (num_params > kDataSize) return false;

  const size_t expected = kHeaderSizeBytes + static_cast<size_t>(out.head.paramters_size);
  if (length < expected) return false;

  const uint8_t* data_ptr = buffer + kHeaderSizeBytes;
  for (uint32_t i = 0; i < num_params; ++i) {
    out.data[i] = read_u32_be(data_ptr + i * sizeof(uint32_t));
  }
  // Zero the rest for determinism
  for (uint32_t i = num_params; i < kDataSize; ++i) {
    out.data[i] = 0u;
  }
  return true;
}

std::string to_string(const Command& cmd, uint32_t max_values_to_print) {
  std::ostringstream oss;
  oss << "Command{";
  oss << "code=" << cmd.head.code << ", type=" << cmd.head.type
      << ", paramters_size=" << cmd.head.paramters_size << " (" << (cmd.head.paramters_size/4) << " u32)";
  const uint32_t count = cmd.head.paramters_size / 4u;
  oss << ", data=[";
  const uint32_t limit = (count < max_values_to_print) ? count : max_values_to_print;
  for (uint32_t i = 0; i < limit; ++i) {
    if (i) oss << ", ";
    oss << cmd.data[i];
  }
  if (count > limit) {
    oss << ", ...(" << (count - limit) << " more)";
  }
  oss << "]}";
  return oss.str();
}

std::string to_string(const CommandHead& head) {
  std::ostringstream oss;
  oss << "CommandHead{";
  oss << "code=" << head.code << ", type=" << head.type
      << ", paramters_size=" << head.paramters_size << " (" << (head.paramters_size/4) << " u32)";
  oss << "}";
  return oss.str();
}

}


