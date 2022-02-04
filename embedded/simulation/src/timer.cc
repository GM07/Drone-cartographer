#include "utils/timer.h"

#include <chrono>
#include <thread>

///////////////////////
void Timer::delayMs(uint32_t delayMS) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMS));
}
