#include "mock_functions.h"

#include "components/drone.h"

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
#include "supervisor.h"
#include "task.h"
}

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
  mock->ledseqRegisterSequence(context);
}

////////////////////////////////////////////////////////////////
size_t appchannelReceiveDataPacket(void* buffer, size_t max_length,
                                   int timeout_ms) {
  return mock->appchannelReceiveDataPacket(buffer, max_length, timeout_ms);
}

///////////////////////////////////////////////////////////////
bool ledseqRun(ledseqContext_t* context) { return mock->ledseqRun(context); }

bool ledseqStop(ledseqContext_t* context) { return mock->ledseqStop(context); }

////////////////////////////////////////////////////////////////
void vTaskDelay(const TickType_t xTicksToDelay) {
  mock->vTaskDelay(xTicksToDelay);
}

////////////////////////////////////////////////////////////////
bool supervisorIsTumbled() { return mock->supervisorIsTumbled(); }

////////////////////////////////////////////////////////////////
int appchannelSendDataPacket(void* data, size_t length) {
  return mock->appchannelSendDataPacket(data, length);
}

////////////////////////////////////////////////////////////////
bool crtpCommanderHighLevelIsTrajectoryFinished() {
  return mock->crtpCommanderHighLevelIsTrajectoryFinished();
}

////////////////////////////////////////////////////////////////
void estimatorKalmanGetEstimatedPos(point_t* pos) {
  return mock->estimatorKalmanGetEstimatedPos(pos);
}

////////////////////////////////////////////////////////////////
void commanderSetSetpoint(setpoint_t* setpoint, int priority) {
  return mock->commanderSetSetpoint(setpoint, priority);
}

////////////////////////////////////////////////////////////////
int crtpCommanderHighLevelTakeoff(const float absoluteHeight_m,
                                  const float duration_s) {
  return mock->crtpCommanderHighLevelTakeoff(absoluteHeight_m, duration_s);
}

////////////////////////////////////////////////////////////////
int crtpCommanderHighLevelLand(const float absoluteHeight_m,
                               const float duration_s) {
  return mock->crtpCommanderHighLevelLand(absoluteHeight_m, duration_s);
}

////////////////////////////////////////////////////////////////
paramVarId_t paramGetVarId(const char* group, const char* name) {
  return mock->paramGetVarId(group, name);
}

////////////////////////////////////////////////////////////////
void paramSetInt(paramVarId_t varid, int valuei) {
  mock->paramSetInt(varid, valuei);
}

////////////////////////////////////////////////////////////////
logVarId_t logGetVarId(const char* group, const char* name) {
  return mock->logGetVarId(group, name);
}

////////////////////////////////////////////////////////////////
unsigned int logGetUint(logVarId_t varid) { return mock->logGetUint(varid); }

////////////////////////////////////////////////////////////////
float logGetFloat(logVarId_t varid) { return mock->logGetFloat(varid); }

////////////////////////////////////////////////////////////////
void p2pRegisterCB(P2PCallback cb) { return mock->p2pRegisterCB(cb); }

////////////////////////////////////////////////////////////////
bool radiolinkSendP2PPacketBroadcast(P2PPacket* p2pp) {
  return mock->radiolinkSendP2PPacketBroadcast(p2pp);
}

////////////////////////////////////////////////////////////////
uint64_t configblockGetRadioAddress() {
  return mock->configblockGetRadioAddress();
}

////////////////////////////////////////////////////////////////
QueueHandle_t xQueueCreateMutexStatic(const uint8_t ucQueueType,
                                      StaticQueue_t* pxStaticQueue) {
  return mock->xQueueCreateMutexStatic(ucQueueType, pxStaticQueue);
}

////////////////////////////////////////////////////////////////
void assertFail(char* exp, char* file, int line) {
  return mock->assertFail(exp, file, line);
}

////////////////////////////////////////////////////////////////
BaseType_t xQueueGenericSend(QueueHandle_t xQueue,
                             const void* const pvItemToQueue,
                             TickType_t xTicksToWait,
                             const BaseType_t xCopyPosition) {
  return mock->xQueueGenericSend(xQueue, pvItemToQueue, xTicksToWait,
                                 xCopyPosition);
}

////////////////////////////////////////////////////////////////
BaseType_t xQueueSemaphoreTake(QueueHandle_t xQueue, TickType_t xTicksToWait) {
  mock->xQueueSemaphoreTake(xQueue, xTicksToWait);
}

void ledClearAll() {}
void commanderNotifySetpointsStop(int remainValidMillisecs) {}
