extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
#include "log.h"
#include "param_logic.h"
#include "semphr.h"
#include "static_mem.h"
#include "supervisor.h"
#include "task.h"
}

#include "app_main.h"
#include "components/drone.h"
#include "controllers/firmware_controller.h"

static bool isInit = false;
StaticSemaphore_t mutexBuffer;
SemaphoreHandle_t p2pPacketMutex;
std::queue<P2PPacket> receivedP2PPacket;

/////////////////////////////////////////////////////////////////////////
Drone& Drone::getEmbeddedDrone() {
  static Drone drone(std::make_shared<FirmwareController>());
  return drone;
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerTaskWrapper(void* /*parameter*/) {
  Drone::getEmbeddedDrone().communicationManagerTask();
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit() {
  xTaskCreate(communicationManagerTaskWrapper, "COMMUNICATION_MANAGER_NAME",
              configMINIMAL_STACK_SIZE, nullptr, 0, nullptr);
  isInit = true;
}

/////////////////////////////////////////////////////////////////////////
bool communicationManagerTest() { return isInit; }

/////////////////////////////////////////////////////////////////////////
void updateCrashStatus() {
  if (supervisorIsTumbled()) {
    Drone::getEmbeddedDrone().getController()->state = State::kCrash;
  }
}

/////////////////////////////////////////////////////////////////////////
void enableCrtpHighLevelCommander() {
  paramVarId_t paramIdCommanderEnHighLevel =
      paramGetVarId("commander", "enHighLevel");
  paramSetInt(paramIdCommanderEnHighLevel, 1);
}

/////////////////////////////////////////////////////////////////////////
uint8_t logDroneState(uint32_t /*timestamp*/, void* /*data*/) {
  return static_cast<uint8_t>(Drone::getEmbeddedDrone().getController()->state);
}

/////////////////////////////////////////////////////////////////////////
void addCustomLoggingVariables() {
  static logByFunction_t droneStateLogger = {.acquireUInt8 = logDroneState,
                                             .data = nullptr};
  LOG_GROUP_START(custom)                                              // NOLINT
  LOG_ADD_BY_FUNCTION(LOG_UINT8, droneCustomState, &droneStateLogger)  // NOLINT
  LOG_GROUP_STOP(custom)
}

constexpr uint8_t MAX_QUEUE_SIZE = 50;
/////////////////////////////////////////////////////////////////////////
void p2pcallbackHandler(P2PPacket* p) {
  xSemaphoreTake(p2pPacketMutex, portMAX_DELAY);
  P2PPacket packet;
  memcpy(&packet, p, sizeof(P2PPacket));

  if (receivedP2PPacket.size() >= MAX_QUEUE_SIZE) {
    receivedP2PPacket.pop();
  }
  receivedP2PPacket.push(packet);
  xSemaphoreGive(p2pPacketMutex);
}

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  ledClearAll();
  addCustomLoggingVariables();
  enableCrtpHighLevelCommander();
  Drone::getEmbeddedDrone().initDrone();

  p2pPacketMutex = xSemaphoreCreateMutexStatic(&mutexBuffer);
  configASSERT(p2pPacketMutex);  // Verify that the mutex is created
  p2pRegisterCB(p2pcallbackHandler);

  while (true) {
    Drone::getEmbeddedDrone().updateSensorsData();
    updateCrashStatus();

    Drone::getEmbeddedDrone().step();
  }
}
