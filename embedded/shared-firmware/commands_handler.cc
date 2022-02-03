#include "include/components/commands_handler.h"

#include "include/controllers/abstract_controller.h"
#include "include/utils/led.h"

std::shared_ptr<CommandsHandler> CommandsHandler::instance = nullptr;
void blinkLED();

/////////////////////////////////////////////////////////////////////////
std::shared_ptr<CommandsHandler> CommandsHandler::getCommandsHandler() {
  struct make_shared_enabler : public CommandsHandler {};
  return instance ? instance : std::make_shared<make_shared_enabler>();
}

/////////////////////////////////////////////////////////////////////////
bool CommandsHandler::handleCommand(Command command, const void* extraArgs,
                                    const size_t extraArgsLength) {
  switch (command) {
    case Command::kIdentify:
      AbstractController::getController()->setLEDState(LED::kLedGreenLeft, true,
                                                       true);
      identifyCommandBegin = std::chrono::steady_clock::now();
      break;
    default:
      return false;
      break;
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////
void CommandsHandler::tick() {
  if (std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::steady_clock::now() - identifyCommandBegin)
          .count() > 2) {
    AbstractController::getController()->setLEDState(LED::kLedGreenLeft, false,
                                                     false);
  }
}
