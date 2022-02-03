extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

#include "utils/timer.h"

///////////////////////
void Timer::delayMs(uint32_t delayMS) { vTaskDelay(M2T(delayMS)); }
