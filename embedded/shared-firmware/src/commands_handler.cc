#include "components/drone.h"
#include "utils/commands.h"

/////////////////////////////////////////////////////////////////////////
bool Drone::handleCommand(Command command, void* extraArgs) {
  if (m_controller->m_state == State::kCrash) {
    return false;
  }

  switch (command) {
    case Command::kIdentify:
      m_controller->identify();
      return true;
    case Command::kTakeOff:

      if (!m_controller->hasLowBattery()) {
        m_controller->m_orientation =
            Math::toRad<float>(*reinterpret_cast<int*>(extraArgs));
        m_controller->takeOff(kHeight);
      }
      return true;
    case Command::kLand:
      m_controller->land();
      return true;
    case Command::kStartP2PGradient:
      m_p2pColorGradientIsActive = true;
      return true;
    case Command::kEndP2PGradient:
      m_p2pColorGradientIsActive = false;
      return true;
    case Command::kReturnToBase:
      m_controller->m_state = State::kReturningToBase;
      return true;
    default:
      return false;
  }
}