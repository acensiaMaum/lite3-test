# UDP Command Project

Small C++ project to send and receive fixed-format `Command` messages over UDP.

## Build

```bash
mkdir -p build && cd build
cmake ..
cmake --build . -j
```

This produces two executables: `send_command` and `recv_command`.

## Message Format

```c++
struct CommandHead{
uint32_t code;
uint32_t params_size; // bytes; must be multiple of 4
uint32_t type;
};
static constexpr uint32_t kDataSize = 256;
struct Command{
CommandHead head;
uint32_t data[kDataSize];
};
```

On the wire, all fields are big-endian (network byte order). Only `params_size` bytes of data are transmitted.

## Usage

Run a receiver on port 5000:

```bash
./build/recv_command 5000
```

Send a command to 127.0.0.1:5000 with code=1, type=42, data values 10,20,30:

```bash
./build/send_command 127.0.0.1 5000 1 42 10,20,30
```
