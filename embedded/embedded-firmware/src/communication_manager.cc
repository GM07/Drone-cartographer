extern "C" {
	#include "config.h"
	#include "FreeRTOS.h"
	#include "task.h"
	#include "static_mem.h"
	#include "debug.h"
	#include "include/ccommunication_manager.h"
}

#include "include/commands_handler.h"
#include "abstract_controller.h"

#define COMMUNICATION_MANAGER_PRIO 4
#define COMMUNICATION_MANAGER_NAME "commands_handler"
#define COMMUNICATION_MANAGER_STACKSIZE configMINIMAL_STACK_SIZE
#define MESSAGE_MAX_SIZE 60

static uint8_t messageRX[MESSAGE_MAX_SIZE];
static bool isInit = false;

STATIC_MEM_TASK_ALLOC(communicationManagerTask, COMMUNICATION_MANAGER_STACKSIZE);

static void communicationManagerTask(void* parameters);

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit()
{
	STATIC_MEM_TASK_CREATE(communicationManagerTask, communicationManagerTask, COMMUNICATION_MANAGER_NAME,
	 nullptr, COMMUNICATION_MANAGER_PRIO);
	isInit = true;
}

/////////////////////////////////////////////////////////////////////////
bool communicationManagerTest()
{
	return isInit;
}

/////////////////////////////////////////////////////////////////////////
static void communicationManagerTask(void* parameters)
{
	while(true)
	{
		if (bridge::AbstractController::getController()->receiveMessage(&messageRX, sizeof(messageRX))) {
				CommandsHandler::getCommandsHandler()->handleCommand(static_cast<Command>(messageRX[0]), &messageRX[1], sizeof(messageRX) - 1);		
		}
		
		vTaskDelay(M2T(50));
	}
}
