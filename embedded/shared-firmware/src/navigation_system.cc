#include "components/drone.h"
#include "utils/math.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters

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
    normal += Vector3D(0.0F, -1.0F, 0.0F);
  }

  if (m_controller->data.right > 0 &&
      m_controller->data.right <= kMinDistanceObstacle) {
    normal += Vector3D(0.0F, 1.0F, 0.0F);
  }

  if (!areAlmostEqual(normal, Vector3D()) &&
      !areAlmostEqual(normal, m_direction) &&
      !Vector3D::areSameDirection(m_direction, normal)) {
    Vector3D newDirection = m_direction.reflect(normal);

    if (!areAlmostEqual(m_direction, newDirection)) {
      m_direction = newDirection;
    }
  }

  m_controller->setVelocity(m_direction, kDroneSpeed);
}
