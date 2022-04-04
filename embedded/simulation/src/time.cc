#include <chrono>
#include <thread>

#include "utils/time.h"

///////////////////////
void Time::delayMs(uint32_t delayMS) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMS));
}
