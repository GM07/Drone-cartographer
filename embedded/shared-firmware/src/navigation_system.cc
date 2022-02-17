#include "components/drone.h"

void Drone::step() {
  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->finishedTrajectory()) {
        // DEBUG ONLY REMOVE AFTER
        explorationDirection = Direction::kFront;
        m_controller->state = State::kExploring;
      }
      break;
    case State::kLanding:
      if (m_controller->finishedTrajectory())
        m_controller->state = State::kIdle;
      break;
    case State::kExploring:
      squareTrajectory(0.5, false);
    default:
      break;
  }
}

void Drone::squareTrajectory(float sideLength, bool relative) {
  if (m_controller->finishedTrajectory()) {
    Vector3D destination;
    switch (explorationDirection) {
      case Direction::kFront:
        destination = Vector3D(sideLength, 0.0, 0.0);
        explorationDirection = Direction::kRight;
        break;
      case Direction::kRight:
        destination = Vector3D(0.0, sideLength, 0.0);
        explorationDirection = Direction::kBack;
        break;
      case Direction::kBack:
        destination = Vector3D(-sideLength, 0.0, 0.0);
        explorationDirection = Direction::kLeft;
        break;
      case Direction::kLeft:
        destination = Vector3D(0.0, -sideLength, 0.0);
        explorationDirection = Direction::kFront;
        break;
      default:
        destination = Vector3D(0.0, 0.0, 0.0);
        break;
    }

    if (!relative)
      destination +=
          m_controller->getCurrentLocation() - m_controller->takeOffPosition;

    m_controller->goTo(destination, relative);
  }
}