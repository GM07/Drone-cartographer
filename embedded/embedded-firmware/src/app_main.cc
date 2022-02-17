extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
#include "log.h"
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

Drone& Drone::getEmbeddedDrone() {
  static Drone drone(std::make_shared<FirmwareController>());
  return drone;
}

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
extern "C" void appMain() {
  LOG_GROUP_START(custom)
  LOG_ADD(LOG_UINT8, state, &Drone::getEmbeddedDrone().getController()->state)
  LOG_GROUP_STOP(custom)

  while (true) {
    Drone::getEmbeddedDrone().step();
  }
}
