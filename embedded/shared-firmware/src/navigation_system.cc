#include "components/drone.h"

void Drone::step() {
  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->finishedTrajectory())
        m_controller->state = State::kIdle;
      break;
    case State::kLanding:
      if (m_controller->finishedTrajectory())
        m_controller->state = State::kIdle;
      break;
    default:
      break;
  }
}