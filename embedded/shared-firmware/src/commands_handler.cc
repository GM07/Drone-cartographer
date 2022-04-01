#include "components/drone.h"
#include "utils/commands.h"

/////////////////////////////////////////////////////////////////////////
bool Drone::handleCommand(Command command) {
  if (m_controller->m_state == State::kCrash) {
    return false;
  }

  switch (command) {
    case Command::kIdentify:
      m_controller->identify();
      return true;
    case Command::kTakeOff:
      m_controller->takeOff(kHeight);
      m_controller->m_state = State::kTakingOff;
      return true;
    case Command::kLand:
      m_controller->land();
      m_controller->m_state = State::kLanding;
      return true;
    case Command::kStartP2P:
      m_p2pIsActive = true;
      return true;
    case Command::kEndP2P:
      m_p2pIsActive = false;
      return true;

    default:
      return false;
  }
}