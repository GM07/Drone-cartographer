#include "components/drone.h"

constexpr float MIN_DISTANCE_OBSTACLE = 20.0f;

void Drone::step() {
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
      squareTrajectory(radius, false);
    default:
      break;
  }
}

void Drone::explore() {
  static Vector3D direction = {0.0f, 1.0f, 0.0f};

  m_controller->goToRelative(direction);

  if (m_controller->data.front < MIN_DISTANCE_OBSTACLE) {
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

    // Absolute position are relative to m_takeOffPosition
    if (!relative) {
      destination += m_controller->getCurrentLocation();
    }
    m_controller->goTo(destination, relative);
  }
}