cmake_minimum_required(VERSION 3.14)
project(simulation_firmware)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

FILE(
	GLOB
	PROJECT_SRC
  stubs/*.cc
	src/*.cc
	../src/*.cc
  ../crazyflie/*.cpp
	../../shared-firmware/*.cc)

add_executable(
  simulation_firmware
  ${PROJECT_SRC}
)

target_link_libraries(
  simulation_firmware
  gtest_main
	gmock_main
)

add_compile_definitions(GTEST)

set(CMAKE_CXX_FLAGS "-Wno-return-type")


target_include_directories(
	simulation_firmware
	PRIVATE
  "stubs"
  "../crazyflie"
  "../../shared-firmware/include"
  "/usr/include/lua5.3"
)

include(GoogleTest)
gtest_discover_tests(simulation_firmware)