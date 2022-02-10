#include "mock_functions.h"

extern "C" {
#include "FreeRTOS.h"
#include "app_channel.h"
#include "ledseq.h"
#include "task.h"
}

FunctionsMock* mock;

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName,
                       const configSTACK_DEPTH_TYPE usStackDepth,
                       void* const pvParameters, UBaseType_t uxPriority,
                       TaskHandle_t* const pxCreatedTask) {
  return mock->xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters,
                           uxPriority, pxCreatedTask);
}

void vTaskDelay(const TickType_t xTicksToDelay) {}
void ledseqRegisterSequence(ledseqContext_t* context) {}
bool ledseqRun(ledseqContext_t* context) { return true; }
size_t appchannelReceiveDataPacket(void* buffer, size_t max_length,
                                   int timeout_ms) {
  return 0;
}
int appchannelSendDataPacket(void* data, size_t length) { return 0; }