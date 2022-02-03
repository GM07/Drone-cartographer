#include "include/components/communication_manager.h"

#include "include/components/commands_handler.h"
#include "include/controllers/abstract_controller.h"
#include "include/utils/led.h"
#include "include/utils/timer.h"
#include "system.h"

#define MESSAGE_MAX_SIZE 60

static std::array<uint8_t, MESSAGE_MAX_SIZE> messageRX;

/////////////////////////////////////////////////////////////////////////
void CommunicationManager::communicationManagerTask(void* parameters) {
  Timer::delayMs(3000);

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

    // Don't know where this should happen. It's here for now but maybe it would
    // be better in the step function ?
    CommandsHandler::getCommandsHandler()->tick();

    Timer::delayMs(50);
  }
}
