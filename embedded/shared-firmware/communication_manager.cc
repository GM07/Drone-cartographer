#include "components/communication_manager.h"

#include <unordered_map>

#include "components/commands_handler.h"
#include "controllers/abstract_controller.h"
#include "utils/led.h"
#include "utils/timer.h"

#define MESSAGE_MAX_SIZE 60
#define ID_SIZE 2

static std::array<uint8_t, MESSAGE_MAX_SIZE> messageRX;

/////////////////////////////////////////////////////////////////////////
void CommunicationManager::communicationManagerTask(void* parameters) {
  Timer::delayMs(3000);

  std::string id((static_cast<char*>(parameters)), ID_SIZE);

  while (true) {
    if (AbstractController::getController(id)->receiveMessage(
            &messageRX, sizeof(messageRX))) {
      bool successfulCommand =
          CommandsHandler::getCommandsHandler()->handleCommand(
              static_cast<Command>(messageRX[0]), &messageRX[1],
              sizeof(messageRX) - sizeof(Command), id);

      AbstractController::getController(id)->sendMessage(
          &successfulCommand, sizeof(successfulCommand));
    }

    if (AbstractController::getController(id)->state == State::kDead) {
      return;
    }
    // Don't know where this should happen. It's here for now but maybe it would
    // be better in the step function ?
    CommandsHandler::getCommandsHandler()->tick(id);

    Timer::delayMs(50);
  }
}
