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

#define COMMUNICATION_MANAGER_PRIO 4
#define COMMUNICATION_MANAGER_NAME "communication_manager"
#define COMMUNICATION_MANAGER_STACKSIZE configMINIMAL_STACK_SIZE

static bool isInit = false;

STATIC_MEM_TASK_ALLOC(communicationManagerTask,
                      COMMUNICATION_MANAGER_STACKSIZE);

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit() {
  STATIC_MEM_TASK_CREATE(
      communicationManagerTask, CommunicationManager::communicationManagerTask,
      COMMUNICATION_MANAGER_NAME, nullptr, COMMUNICATION_MANAGER_PRIO);
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
