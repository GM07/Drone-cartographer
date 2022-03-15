extern "C" {
#include "FreeRTOS.h"
#include "app_channel.h"
#include "commander.h"
#include "crtp_commander_high_level.h"
#include "estimator_kalman.h"
#include "ledseq.h"
#include "log.h"
#include "param_logic.h"
#include "stabilizer_types.h"
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
  virtual bool supervisorIsTumbled() = 0;
  virtual size_t appchannelReceiveDataPacket(void* buffer, size_t max_length,
                                             int timeout_ms) = 0;
  virtual int appchannelSendDataPacket(void* data, size_t length) = 0;
  virtual bool crtpCommanderHighLevelIsTrajectoryFinished() = 0;
  virtual void estimatorKalmanGetEstimatedPos(point_t* pos) = 0;
  virtual void commanderSetSetpoint(setpoint_t* setpoint, int priority) = 0;
  virtual int crtpCommanderHighLevelTakeoff(const float, const float) = 0;
  virtual int crtpCommanderHighLevelLand(const float, const float) = 0;
  virtual paramVarId_t paramGetVarId(const char* group, const char* name) = 0;
  virtual void paramSetInt(paramVarId_t varid, int valuei) = 0;
  virtual logVarId_t logGetVarId(const char* group, const char* name) = 0;
  virtual unsigned int logGetUint(logVarId_t varid) = 0;
  virtual float logGetFloat(logVarId_t varid) = 0;
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
  MOCK_METHOD0(supervisorIsTumbled, bool());
  MOCK_METHOD3(appchannelReceiveDataPacket, size_t(void*, size_t, int));
  MOCK_METHOD2(appchannelSendDataPacket, int(void*, size_t));
  MOCK_METHOD0(crtpCommanderHighLevelIsTrajectoryFinished, bool());
  MOCK_METHOD1(estimatorKalmanGetEstimatedPos, void(point_t*));
  MOCK_METHOD2(commanderSetSetpoint, void(setpoint_t*, int));
  MOCK_METHOD2(crtpCommanderHighLevelTakeoff, int(const float, const float));
  MOCK_METHOD2(crtpCommanderHighLevelLand, int(const float, const float));
  MOCK_METHOD2(paramGetVarId, paramVarId_t(const char*, const char*));
  MOCK_METHOD2(paramSetInt, void(paramVarId_t, int));
  MOCK_METHOD2(logGetVarId, logVarId_t(const char*, const char*));
  MOCK_METHOD1(logGetUint, unsigned int(logVarId_t));
  MOCK_METHOD1(logGetFloat, float(logVarId_t));
};

extern FunctionsMock* mock;
