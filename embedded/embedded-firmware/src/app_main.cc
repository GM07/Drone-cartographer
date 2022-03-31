extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/crazyflie_task.h"
#include "config.h"
#include "debug.h"
#include "log.h"
#include "param_logic.h"
#include "semphr.h"
#include "static_mem.h"
#include "supervisor.h"
#include "task.h"
}

#include <cmath>

#include "app_main.h"
#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/time.h"

namespace {

bool commIsInit = false;
bool p2pIsInit = false;
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
void p2pTaskWrapper(void* /*parameter*/) {
  constexpr int32_t kP2pTaskDelay = 200;
  constexpr uint32_t kInitDelay = 3000;

  Time::delayMs(kInitDelay);
  Drone& drone = Drone::getEmbeddedDrone();

  while (true) {
    drone.getController()->sendP2PMessage(static_cast<void*>(&drone.m_data),
                                          sizeof(drone.m_data));

    Time::delayMs(kP2pTaskDelay);
  }
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit() {
  xTaskCreate(communicationManagerTaskWrapper, "COMMUNICATION_MANAGER_NAME",
              configMINIMAL_STACK_SIZE, nullptr, 0, nullptr);
  commIsInit = true;
}

/////////////////////////////////////////////////////////////////////////
void p2pTaskInit() {
  xTaskCreate(p2pTaskWrapper, "P2P_TASK_NAME", configMINIMAL_STACK_SIZE,
              nullptr, 0, nullptr);
  p2pIsInit = true;
}

/////////////////////////////////////////////////////////////////////////
bool communicationManagerTest() { return commIsInit; }

/////////////////////////////////////////////////////////////////////////
bool p2pTaskTest() { return p2pIsInit; }

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
uint8_t logDroneState(uint32_t /*timestamp*/, void* /*data*/) {
  return static_cast<uint8_t>(
      Drone::getEmbeddedDrone().getController()->m_state);
}

/////////////////////////////////////////////////////////////////////////
void addCustomLoggingVariables() {
  static logByFunction_t droneStateLogger = {.acquireUInt8 = logDroneState,
                                             .data = nullptr};
  LOG_GROUP_START(custom)                                              // NOLINT
  LOG_ADD_BY_FUNCTION(LOG_UINT8, droneCustomState, &droneStateLogger)  // NOLINT
  LOG_GROUP_STOP(custom)
}

/////////////////////////////////////////////////////////////////////////
void p2pcallbackHandler(P2PPacket* p) {
  xSemaphoreTake(p2pPacketMutex, portMAX_DELAY);
  {
    P2PPacket packet;
    memcpy(&packet, p, sizeof(packet));

    DroneData data(*reinterpret_cast<DroneData*>(&packet.data));  // NOLINT
    data.m_range = p->rssi;

    Drone::getEmbeddedDrone().m_peerData.insert_or_assign(data.m_id, data);
  }
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
    Drone::getEmbeddedDrone().getController()->updateSensorsData();
    updateCrashStatus();

    Drone::getEmbeddedDrone().step();
  }
}
