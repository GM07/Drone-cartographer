#include "include/components/communication_manager.h"

#include "include/components/commands_handler.h"
#include "include/controllers/abstract_controller.h"
#include "include/utils/timer.h"

#define MESSAGE_MAX_SIZE 60

static uint8_t messageRX[MESSAGE_MAX_SIZE];

/////////////////////////////////////////////////////////////////////////
void CommunicationManager::communicationManagerTask(void* parameters) {
  while (true) {
    if (AbstractController::getController()->receiveMessage(
            &messageRX, sizeof(messageRX))) {
      bool successfulCommand =
          CommandsHandler::getCommandsHandler()->handleCommand(
              static_cast<Command>(messageRX[0]), &messageRX[1],
              sizeof(messageRX) - sizeof(Command));
      AbstractController::getController()->sendMessage(
          &successfulCommand, sizeof(successfulCommand));
    }

    Timer::waitMs(50);
  }
}
