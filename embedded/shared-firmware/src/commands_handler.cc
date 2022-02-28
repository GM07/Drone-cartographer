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
      if (m_controller->state == State::kCrash) {
        return false;
      }
      m_controller->takeOff(kHeight);
      m_controller->state = State::kTakingOff;
      break;
    case Command::kLand:
      m_controller->land();
      if (m_controller->state != State::kCrash) {
        m_controller->state = State::kLanding;
      }
      break;
    default:
      return false;
  }

  return true;
}