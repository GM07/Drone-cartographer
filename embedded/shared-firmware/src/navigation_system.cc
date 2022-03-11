#include "components/drone.h"

constexpr float kMinDistanceObstacle = 120.0F;  // Millimeters
constexpr float kComparisonFactor = 10e-3;
constexpr float kMaxSpeed = 0.1F;  // Meters per seconds
constexpr uint32_t kTicksPerSecond = 10;

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
      if (m_controller->isTrajectoryFinished()) {
        explore();
      }
    default:
      break;
  }
}

void Drone::explore() {
  static Vector3D direction = {1.0F, 1.0F, 0.0F};

  Vector3D normal;

  if (m_controller->data.front > 0 &&
      m_controller->data.front <= kMinDistanceObstacle) {
    normal += Vector3D(-1.0F, 0.0F, 0.0F);
  }

  if (m_controller->data.back > 0 &&
      m_controller->data.back <= kMinDistanceObstacle) {
    normal += Vector3D(1.0F, 0.0F, 0.0F);
  }

  if (m_controller->data.left > 0 &&
      m_controller->data.left <= kMinDistanceObstacle) {
    normal += Vector3D(0.0F, 1.0F, 0.0F);
  }

  if (m_controller->data.right > 0 &&
      m_controller->data.right <= kMinDistanceObstacle) {
    normal += Vector3D(0.0F, -1.0F, 0.0F);
  }

  if (!normal.isAlmostEqual(Vector3D(), kComparisonFactor) &&
      !normal.isAlmostEqual(direction, kComparisonFactor)) {
    Vector3D newDirection = direction.reflect(normal);

    if (!direction.isAlmostEqual(newDirection, kComparisonFactor)) {
      direction = newDirection;
    }
  }

  m_controller->setVelocity(direction, kMaxSpeed);
}