project(Simulation)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_FLAGS "-g -O0")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -g -O0 -fsanitize-address-use-after-scope -fsanitize=address -fsanitize=bool -fsanitize=bounds -fsanitize=bounds-strict -fsanitize=builtin -fsanitize=enum -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=pointer-compare -fsanitize=pointer-overflow -fsanitize=pointer-subtract -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=signed-integer-overflow -fsanitize=shift -fsanitize=shift-base -fsanitize=shift-exponent -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr")

# These includes are needed by the shared-firmware and by the simulation-firmware
include_directories("${CMAKE_SOURCE_DIR}/shared-firmware/include/")
include_directories("${CMAKE_SOURCE_DIR}/simulation/crazyflie/")
add_compile_definitions("CMAKE_EXPORT_COMPILE_COMMANDS=ON")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /usr/local/lib/argos3/build/simulation/crazyflie/)

add_subdirectory(shared-firmware)
add_subdirectory(simulation)