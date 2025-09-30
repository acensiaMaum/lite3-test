cmake_minimum_required(VERSION 3.16)

project(udp_command LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(udp STATIC
  src/udp_sender.cpp
  src/command_serialization.cpp
  src/stable_sender.cpp
  src/hex_parser.cpp
)

target_include_directories(udp PUBLIC include)

add_executable(send_command apps/send_command.cpp)
target_link_libraries(send_command PRIVATE udp)

add_executable(recv_command apps/recv_command.cpp)
target_link_libraries(recv_command PRIVATE udp)

add_executable(stable_sender apps/stable_sender.cpp)
target_link_libraries(stable_sender PRIVATE udp)

add_executable(rapid_sender apps/rapid_sender.cpp)
target_link_libraries(rapid_sender PRIVATE udp)

add_executable(single_sender src/main.cpp)
target_link_libraries(single_sender PRIVATE udp)


