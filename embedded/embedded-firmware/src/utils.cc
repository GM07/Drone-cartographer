extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

#include "utils/delay.h"

///////////////////////
void Delay::waitMS(uint32_t delayMS) { vTaskDelay(M2T(delayMS)); }