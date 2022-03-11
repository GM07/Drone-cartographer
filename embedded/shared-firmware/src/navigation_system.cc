#include "components/drone.h"

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  // DEBUG to remove
  m_controller->log(m_controller->getCurrentLocation().toString());

  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kExploring;
      }
      break;
    case State::kLanding:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kIdle;
        m_controller->setVelocity(Vector3D(1.0, 1.0, 1.0), 0.0f);
      }
      break;
    case State::kExploring:
      m_controller->setVelocity(Vector3D(0.0F, 1.0F, 0.0F), kSpeed);
    default:
      break;
  }
}