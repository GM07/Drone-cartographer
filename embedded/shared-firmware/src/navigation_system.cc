#include "components/drone.h"

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  constexpr float radius = 0.5;
  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kIdle;
      }
      break;
    case State::kLanding:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kIdle;
      }
      break;
    case State::kExploring:
      //
    default:
      break;
  }
}
