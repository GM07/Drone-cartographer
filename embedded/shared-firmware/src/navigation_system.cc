#include "components/drone.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters
constexpr float kComparisonFactor = 10e-3;

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

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
      }
      break;
    case State::kExploring:
      explore();
    default:
      break;
  }
}

void Drone::explore() {
  Vector3D normal;

  static bool touchedFront = false;
  if (m_controller->data.front > 0 &&
      m_controller->data.front <= kMinDistanceObstacle) {
    if (!touchedFront) {
      touchedFront = true;
      normal += Vector3D(-1.0F, 0.0F, 0.0F);
    }
  } else {
    touchedFront = false;
  }

  static bool touchedBack = false;
  if (m_controller->data.back > 0 &&
      m_controller->data.back <= kMinDistanceObstacle) {
    if (!touchedBack) {
      touchedBack = true;
      normal += Vector3D(1.0F, 0.0F, 0.0F);
    }
  } else {
    touchedBack = false;
  }

  static bool touchedLeft = false;
  if (m_controller->data.left > 0 &&
      m_controller->data.left <= kMinDistanceObstacle) {
    if (!touchedLeft) {
      touchedLeft = true;
      normal += Vector3D(0.0F, -1.0F, 0.0F);
    }
  } else {
    touchedLeft = false;
  }

  static bool touchedRight = false;
  if (m_controller->data.right > 0 &&
      m_controller->data.right <= kMinDistanceObstacle) {
    if (!touchedRight) {
      touchedRight = true;
      normal += Vector3D(0.0F, 1.0F, 0.0F);
    }
  } else {
    touchedRight = false;
  }

  if (!normal.isAlmostEqual(Vector3D(), kComparisonFactor) &&
      !normal.isAlmostEqual(m_direction, kComparisonFactor)) {
    Vector3D newDirection = m_direction.reflect(normal);

    if (!m_direction.isAlmostEqual(newDirection, kComparisonFactor)) {
      m_direction = newDirection;
    }
  }

  m_controller->setVelocity(m_direction, kDroneSpeed);
}