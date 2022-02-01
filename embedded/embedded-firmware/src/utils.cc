extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

#include "utils/timer.h"

///////////////////////
void Timer::delayMs(unsigned int delayMS) { vTaskDelay(M2T(delayMS)); }
