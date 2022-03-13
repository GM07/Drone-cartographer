#include "components/drone.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters
constexpr float kComparisonFactor = 10e-3;

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  // Get distance to takeoff for LED identifying
  m_controller->sendP2PMessage(static_cast<void*>(&m_data), sizeof(m_data));
  m_controller->receiveP2PMessage(m_peerData);

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

  if (!normal.isAlmostEqual(Vector3D(), kComparisonFactor) &&
      !normal.isAlmostEqual(m_data.direction, kComparisonFactor) &&
      !Vector3D::areSameDirection(m_data.direction, normal)) {
    Vector3D newDirection = m_data.direction.reflect(normal);

    if (!m_data.direction.isAlmostEqual(newDirection, kComparisonFactor)) {
      m_data.direction = newDirection;
    }
  }

  m_controller->setVelocity(m_data.direction, kDroneSpeed);
}