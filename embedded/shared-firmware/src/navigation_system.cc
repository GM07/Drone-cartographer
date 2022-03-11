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
        // DEBUG ONLY REMOVE AFTER
        explorationDirection = Direction::kFront;
        m_controller->state = State::kExploring;
      }
      break;
    case State::kLanding:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kIdle;
      }
      break;
    // DEBUG ONLY REMOVE AFTER
    case State::kExploring:
      squareTrajectory(radius, true);
    default:
      break;
  }
}

void Drone::squareTrajectory(float sideLength, bool relative) {
  if (m_controller->isTrajectoryFinished()) {
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

    m_controller->goTo(destination, relative);
  }
}