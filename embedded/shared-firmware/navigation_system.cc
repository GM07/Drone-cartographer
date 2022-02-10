#include "components/drone.h"

#define HEIGHT 0.5f

void Drone::step() {
  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      m_controller->takeOff(HEIGHT);
      break;
    case State::kLanding:
      m_controller->land();
      break;
    case State::kHover: {
      Vector3D pos = m_controller->getCurrentLocation();
      pos.m_z = HEIGHT;
      m_controller->goTo(pos, true);
    } break;
    default:
      break;
  }
}