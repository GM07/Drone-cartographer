cmake_minimum_required(VERSION 3.14)
project(shared_firmware)

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
	src/*.cc
	src/stubs/*.cc
	../src/*.cc)

add_executable(
  shared_firmware
	${PROJECT_SRC}
)

target_link_libraries(
  shared_firmware
  gtest_main
	gmock_main
)

target_include_directories(
	shared_firmware
	PRIVATE
	"../include/"
	"src/stubs/"
  "../../shared-firmware/include"
	"../../crazyflie-firmware/vendor/FreeRTOS/include/"
	"../../crazyflie-firmware/src/config/"
	"../../crazyflie-firmware/src/drivers/interface/"
	"../../crazyflie-firmware/src/hal/interface/"
	"../../crazyflie-firmware/src/utils/interface/"
	"../../crazyflie-firmware/src/modules/interface/"
	"../../crazyflie-firmware/vendor/FreeRTOS/portable/oWatcom/16BitDOS/PC/"
	"../../crazyflie-firmware/src/utils/interface/lighthouse/"
	"../../crazyflie-firmware/src/lib/CMSIS/STM32F4xx/Include/"
	"../../crazyflie-firmware/vendor/CMSIS/CMSIS/Core/Include/"
	)

set(CMAKE_CXX_FLAGS "-Wno-return-type")


include(GoogleTest)
gtest_discover_tests(shared_firmware)
