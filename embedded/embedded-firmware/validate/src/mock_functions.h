extern "C" {
#include "FreeRTOS.h"
#include "ledseq.h"
#include "task.h"
}

#include <gmock/gmock.h>

struct Functions {
  virtual ~Functions() = default;
  virtual BaseType_t xTaskCrate(TaskFunction_t pxTaskCode,
                                const char* const pcName,
                                const configSTACK_DEPTH_TYPE usStackDepth,
                                void* const pvParameters,
                                UBaseType_t uxPriority,
                                TaskHandle_t* const pxCreatedTask) = 0;
  virtual void ledseqRegisterSequence(ledseqContext_t* context) = 0;
  virtual bool ledseqRun(ledseqContext_t* context) = 0;
  virtual void vTaskDelay(const TickType_t xTicksToDelay) = 0;
};

struct FunctionsMock {
  FunctionsMock() = default;
  virtual ~FunctionsMock() = default;
  MOCK_METHOD6(xTaskCreate,
               BaseType_t(TaskFunction_t, const char* const,
                          const configSTACK_DEPTH_TYPE, void* const,
                          UBaseType_t, TaskHandle_t* const));
  MOCK_METHOD1(ledseqRegisterSequence, void(ledseqContext_t*));
  MOCK_METHOD1(ledseqRun, bool(ledseqContext_t*));
  MOCK_METHOD1(vTaskDelay, void(const TickType_t));
};

extern FunctionsMock* mock;
