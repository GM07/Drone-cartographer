#include "include/components/commands_handler.h"

#include "include/controllers/abstract_controller.h"

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
    case kIdentify:
      AbstractController::getController()->setLEDState(0xFF, 0x1A, 0xFF, true);
      break;
    default:
      // Unknown command. Should we log it ?
      break;
  }

  return true;
}
