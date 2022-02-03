#include <chrono>
#include <thread>

#include "utils/timer.h"

///////////////////////
void Timer::delayMs(unsigned int delayMS) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMS));
}
