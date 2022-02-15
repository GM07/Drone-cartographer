#include "components/drone.h"

void Drone::step() {
  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->finishedTrajectory())
        m_controller->state = State::kExploring;
      break;
    case State::kLanding:
      if (m_controller->finishedTrajectory())
        m_controller->state = State::kIdle;
      break;
    case State::kExploring:
      if (m_controller->finishedTrajectory()) {
        switch (explorationDirection) {
          case Direction::kFront:
            m_controller->goTo(Vector3D(0.5, 0.0, 0.0), true);
            explorationDirection = Direction::kRight;
            break;
          case Direction::kRight:
            m_controller->goTo(Vector3D(0.0, 0.5, 0.0), true);
            explorationDirection = Direction::kBack;
            break;
          case Direction::kBack:
            m_controller->goTo(Vector3D(-0.5, 0.0, 0.0), true);
            explorationDirection = Direction::kLeft;
            break;
          case Direction::kLeft:
            m_controller->goTo(Vector3D(0.0, -0.5, 0.0), true);
            explorationDirection = Direction::kFront;
            break;
          default:
            break;
        }
      }

    default:
      break;
  }
}