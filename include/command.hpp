#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

struct CommandHead{
uint32_t code;
uint32_t params_size;
uint32_t type;
};

static constexpr uint32_t kDataSize = 256u;

struct Command{
CommandHead head;
uint32_t data[kDataSize];
};

// Serialization helpers
namespace command_io {

// Total bytes for header only
static constexpr size_t kHeaderSizeBytes = sizeof(uint32_t) * 3u;

// Maximum bytes possible on the wire: header + full data
static constexpr size_t kMaxWireBytes = kHeaderSizeBytes + sizeof(uint32_t) * kDataSize;

// Serialize command to bytes using network byte order for all uint32_t.
// Writes header and only head.params_size bytes of data (not the entire array).
// Returns true on success; false if params_size is invalid.
bool serialize(const Command& cmd, std::vector<uint8_t>& out);

// Parse bytes into Command, validating sizes and converting from network byte order.
// Returns true on success.
bool deserialize(const uint8_t* buffer, size_t length, Command& out);

// Utility to stringify a Command for logging/debugging. Limits printed data values.
std::string to_string(const Command& cmd, uint32_t max_values_to_print = 16u);

}


