extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

#include "utils/time.h"

///////////////////////
void Time::delayMs(uint32_t delayMS) { vTaskDelay(M2T(delayMS)); }












