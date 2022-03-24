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

namespace {

bool isInit = false;
StaticSemaphore_t mutexBuffer;

}  // namespace

uint8_t droneState = State::kIdle;

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
    Drone::getEmbeddedDrone().getController()->m_state = State::kCrash;
  }
}

/////////////////////////////////////////////////////////////////////////
void enableCrtpHighLevelCommander() {
  paramVarId_t paramIdCommanderEnHighLevel =
      paramGetVarId("commander", "enHighLevel");
  paramSetInt(paramIdCommanderEnHighLevel, 1);
}

/////////////////////////////////////////////////////////////////////////
void addLoggingVariables() {
  LOG_GROUP_START(custom)  // NOLINT
  LOG_ADD(LOG_UINT8, droneCustomState, &droneState)
  LOG_GROUP_STOP(custom)
}

/////////////////////////////////////////////////////////////////////////
void p2pcallbackHandler(P2PPacket* p) {
  constexpr size_t kMaxQueueSize = 50;

  xSemaphoreTake(p2pPacketMutex, portMAX_DELAY);
  {
    P2PPacket packet;
    memcpy(&packet, p, sizeof(packet));

    receivedP2PPacket.push(packet);
    if (receivedP2PPacket.size() >= kMaxQueueSize) {
      receivedP2PPacket.pop();
    }
  }
  xSemaphoreGive(p2pPacketMutex);
}

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  ledClearAll();
  addLoggingVariables();
  enableCrtpHighLevelCommander();
  Drone::getEmbeddedDrone().initDrone();

  p2pPacketMutex = xSemaphoreCreateMutexStatic(&mutexBuffer);
  configASSERT(p2pPacketMutex);  // Verify that the mutex is created
  p2pRegisterCB(p2pcallbackHandler);

  while (true) {
    Drone::getEmbeddedDrone().getController()->updateSensorsData();
    updateCrashStatus();

    Drone::getEmbeddedDrone().step();
  }
}
