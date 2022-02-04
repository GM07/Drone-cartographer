extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/ccommunication_manager.h"
#include "config.h"
#include "debug.h"
#include "static_mem.h"
#include "task.h"
}

#include "components/communication_manager.h"
#include "components/navigation_system.h"

static bool isInit = false;
static std::string dummy("s1");

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit() {
  xTaskCreate(CommunicationManager::communicationManagerTask,
              "COMMUNICATION_MANAGER_NAME", configMINIMAL_STACK_SIZE,
              static_cast<void*>(&dummy), 0, nullptr);
  isInit = true;
}

/////////////////////////////////////////////////////////////////////////
bool communicationManagerTest() { return isInit; }

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  while (true) {
    Navigation::step();
  }
}
