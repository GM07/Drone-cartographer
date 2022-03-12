#include "mock_functions.h"

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

#include "components/drone.h"

FunctionsMock* mock;

///////////////////////////////////////////////////////////////
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName,
                       const configSTACK_DEPTH_TYPE usStackDepth,
                       void* const pvParameters, UBaseType_t uxPriority,
                       TaskHandle_t* const pxCreatedTask) {
  return mock->xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters,
                           uxPriority, pxCreatedTask);
}

///////////////////////////////////////////////////////////////
void ledseqRegisterSequence(ledseqContext_t* context) {
  return mock->ledseqRegisterSequence(context);
}

///////////////////////////////////////////////////////////////
bool ledseqRun(ledseqContext_t* context) { return mock->ledseqRun(context); }

////////////////////////////////////////////////////////////////
void vTaskDelay(const TickType_t xTicksToDelay) {
  return mock->vTaskDelay(xTicksToDelay);
}

////////////////////////////////////////////////////////////////
bool supervisorIsTumbled() { return mock->supervisorIsTumbled(); }

////////////////////////////////////////////////////////////////
size_t appchannelReceiveDataPacket(void* buffer, size_t max_length,
                                   int timeout_ms) {
  return 0;
}
int appchannelSendDataPacket(void* data, size_t length) { return 0; }

paramVarId_t paramGetVarId(const char* group, const char* name) {
  return {0xffffu, 0xffffu};
}

logVarId_t logGetVarId(const char* group, const char* name) { return 0; }
unsigned int logGetUint(logVarId_t varid) { return 0U; }
float logGetFloat(logVarId_t varid) { return 0.0; }

void paramSetInt(paramVarId_t varid, int valuei) {}

int crtpCommanderHighLevelTakeoff(const float absoluteHeight_m,
                                  const float duration_s) {
  return 0;
}

int crtpCommanderHighLevelLand(const float absoluteHeight_m,
                               const float duration_s) {
  return 0;
}

void estimatorKalmanGetEstimatedPos(point_t* pos) {}

bool crtpCommanderHighLevelIsTrajectoryFinished() { return true; }

void ledClearAll() {}

void commanderNotifySetpointsStop(int remainValidMillisecs) {}

void commanderSetSetpoint(setpoint_t* setpoint, int priority) {}