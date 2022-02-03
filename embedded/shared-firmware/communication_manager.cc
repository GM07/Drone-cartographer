#include "components/communication_manager.h"

#include "components/commands_handler.h"
#include "controllers/abstract_controller.h"
#include "utils/led.h"
#include "utils/timer.h"

#define MESSAGE_MAX_SIZE 60

static std::array<uint8_t, MESSAGE_MAX_SIZE> messageRX;

/////////////////////////////////////////////////////////////////////////
void CommunicationManager::communicationManagerTask(void* parameters) {
  Timer::delayMs(3000);

  std::string id(*(static_cast<std::string*>(parameters)));

  while (true) {
    if (AbstractController::getController(id)->receiveMessage(
            &messageRX, sizeof(messageRX))) {
      bool successfulCommand =
          CommandsHandler::getCommandsHandler()->handleCommand(
              static_cast<Command>(messageRX[0]), &messageRX[1],
              sizeof(messageRX) - sizeof(Command));

      AbstractController::getController()->sendMessage(
          &successfulCommand, sizeof(successfulCommand));
    }

    if (AbstractController::getController(id)->state == State::kDead) {
      break;
    }

    Timer::delayMs(50);
  }
}
