extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
<<<<<<< HEAD
#include "log.h"
=======
#include "param_logic.h"
>>>>>>> 41dceeb79f1823b5213a9da31229d4f6e7029823
#include "static_mem.h"
#include "task.h"
}

#include "components/drone.h"

// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/firmware_controller.h"
#pragma GCC diagnostic pop

static bool isInit = false;

/////////////////////////////////////////////////////////////////////////
Drone& Drone::getEmbeddedDrone() {
  static Drone drone(std::make_shared<FirmwareController>());
  return drone;
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerTaskWrapper(void* parameter) {
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
<<<<<<< HEAD
  LOG_GROUP_START(custom)
  LOG_ADD(LOG_UINT8, state, &Drone::getEmbeddedDrone().getController()->state)
  LOG_GROUP_STOP(custom)
=======
  enableCrtpHighLevelCommander();
>>>>>>> 41dceeb79f1823b5213a9da31229d4f6e7029823

  while (true) {
    Drone::getEmbeddedDrone().step();
  }
}
