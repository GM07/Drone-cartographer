#include "components/commands_handler.h"

#include "controllers/abstract_controller.h"
#include "utils/led.h"

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
      break;
    case Command::kTakeoff:
      AbstractController::getController()->state = State::kTakingOff;
      break;
    case Command::kLand:
      AbstractController::getController()->state = State::kLanding;
      break;
    default:
      AbstractController::getController()->log("Unknown Command");
      return false;
      break;
  }

  return true;
}
