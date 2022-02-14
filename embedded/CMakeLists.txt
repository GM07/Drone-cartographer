cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -O0")

# These includes are needed by the shared-firmware and by the simulation-firmware
include_directories("${CMAKE_SOURCE_DIR}/shared-firmware/include/")
include_directories("${CMAKE_SOURCE_DIR}/simulation/crazyflie/")

add_subdirectory(shared-firmware)
add_subdirectory(simulation)