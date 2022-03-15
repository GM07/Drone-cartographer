extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
#include "log.h"
#include "param_logic.h"
#include "static_mem.h"
#include "supervisor.h"
#include "task.h"
}

#include "app_main.h"
#include "components/drone.h"
#include "controllers/firmware_controller.h"

static bool isInit = false;
std::queue<P2PPacket> receivedP2PPacket;

// TODO - Remove this after testing // NOLINT
static int rssi = 255;  // NOLINT

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
void addLoggingVariables() {
  LOG_GROUP_START(custom)  // NOLINT
  LOG_ADD(LOG_UINT8, state, &Drone::getEmbeddedDrone().getController()->state)

  // TODO - remove after rssi test has been established // NOLINT
  LOG_ADD(LOG_UINT8, rssi, &rssi)
  LOG_GROUP_STOP(custom)
}

/////////////////////////////////////////////////////////////////////////
void p2pcallbackHandler(P2PPacket* p) {
  receivedP2PPacket.push(*p);

  // TODO - remove after rssi test has been established // NOLINT
  rssi = p->rssi;
}

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  ledClearAll();
  addLoggingVariables();
  enableCrtpHighLevelCommander();
  Drone::getEmbeddedDrone().initDrone();

  p2pRegisterCB(p2pcallbackHandler);

  while (true) {
    Drone::getEmbeddedDrone().updateSensorsData();
    updateCrashStatus();

    Drone::getEmbeddedDrone().step();
  }
}
