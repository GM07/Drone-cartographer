#include "utils/time.h"

#include <chrono>
#include <thread>

///////////////////////
void Time::delayMs(uint32_t delayMS) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMS));
}
