extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
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
static Drone drone(std::make_shared<FirmwareController>());

void communicationManagerTaskWrapper(void* parameter) {
  static_cast<Drone*>(parameter)->communicationManagerTask();
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit() {
  xTaskCreate(communicationManagerTaskWrapper, "COMMUNICATION_MANAGER_NAME",
              configMINIMAL_STACK_SIZE, &drone, 0, nullptr);
  isInit = true;
}

/////////////////////////////////////////////////////////////////////////
bool communicationManagerTest() { return isInit; }

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  while (true) {
    drone.step();
  }
}
