#include "components/drone.h"
#include "utils/commands.h"

/////////////////////////////////////////////////////////////////////////
bool Drone::handleCommand(Command command, const void* extraArgs,
                          const size_t extraArgsLength) {
  switch (command) {
    case Command::kIdentify:
      m_controller->state = State::kIdentify;
      break;
    case Command::kTakeOff:
      m_controller->state = State::kTakingOff;
      m_controller->log("TAKEOFF");
      m_controller->sendMessage(&m_controller->data, 32);
      break;
    case Command::kLand:
      m_controller->state = State::kLanding;
      m_controller->log("LANDING");
      break;
    default:
      /*m_controller->log(
          std::to_string(static_cast<uint8_t>(command)));*/
      return false;
  }

  return true;
}