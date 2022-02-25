#include "components/drone.h"
#include "utils/commands.h"

#define PACKET_SIZE 32

/////////////////////////////////////////////////////////////////////////
bool Drone::handleCommand(Command command, const void* /*extraArgs*/,
                          size_t /*extraArgsLength*/) {
  switch (command) {
    case Command::kIdentify:
      m_controller->blinkLED(LED::kLedRedLeft);
      break;
    case Command::kTakeOff:
      m_controller->takeOff(kHeight);
      m_controller->state = State::kTakingOff;
      break;
    case Command::kLand:
      m_controller->land();
      m_controller->state = State::kLanding;
      break;
    case Command::kLogs:
      m_controller->sendMessage(&m_controller->data, PACKET_SIZE);
      break;
    default:
      return false;
  }

  return true;
}