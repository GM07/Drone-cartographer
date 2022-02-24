extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
#include "param_logic.h"
#include "static_mem.h"
#include "task.h"
}

#include "components/drone.h"
#include "controllers/firmware_controller.h"

static bool isInit = false;

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
void enableCrtpHighLevelCommander() {
  paramVarId_t paramIdCommanderEnHighLevel =
      paramGetVarId("commander", "enHighLevel");
  paramSetInt(paramIdCommanderEnHighLevel, 1);
}

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  enableCrtpHighLevelCommander();

  while (true) {
    Drone::getEmbeddedDrone().step();
  }
}
