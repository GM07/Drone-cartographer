#include "components/drone.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters
constexpr float kComparisonFactor = 10e-3;

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  m_controller->sendP2PMessage(static_cast<void*>(&m_data), sizeof(m_data));
  m_controller->receiveP2PMessage(m_peerData);

  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kExploring;
        m_controller->setVelocity(m_data.direction, kDroneSpeed);
      }
      break;
    case State::kLanding:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kIdle;
      }
      break;
    case State::kExploring:
      m_normal = Vector3D();
      explore();
      collisionAvoidance();
      changeDirection();
    default:
      break;
  }
}

void Drone::explore() {
  if (m_controller->data.front > 0 &&
      m_controller->data.front <= kMinDistanceObstacle) {
    m_normal += Vector3D(-1.0F, 0.0F, 0.0F);
  }

  if (m_controller->data.back > 0 &&
      m_controller->data.back <= kMinDistanceObstacle) {
    m_normal += Vector3D(1.0F, 0.0F, 0.0F);
  }

  if (m_controller->data.left > 0 &&
      m_controller->data.left <= kMinDistanceObstacle) {
    m_normal += Vector3D(0.0F, -1.0F, 0.0F);
  }

  if (m_controller->data.right > 0 &&
      m_controller->data.right <= kMinDistanceObstacle) {
    m_normal += Vector3D(0.0F, 1.0F, 0.0F);
  }

  if (m_normal.isAlmostEqual(m_data.direction, kComparisonFactor) ||
      Vector3D::areSameDirection(m_data.direction, m_normal)) {
    m_normal = Vector3D();
  }
}
// 3250
void Drone::collisionAvoidance() {
  for (auto data : m_peerData) {
    DroneData peerData = data.second;

    if (peerData.range <= kSimulationCollisionAvoidanceRange) {
      if (m_usedPeerData.find(peerData.id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.id, peerData);
        m_normal -= m_data.direction;
      }
    } else {
      m_usedPeerData.erase(peerData.id);
    }
  }
}

void Drone::changeDirection() {
  if (!m_normal.isAlmostEqual(Vector3D(), kComparisonFactor)) {
    Vector3D newDirection = m_data.direction.reflect(m_normal);

    if (!m_data.direction.isAlmostEqual(newDirection, kComparisonFactor)) {
      m_data.direction = newDirection;
      m_controller->setVelocity(m_data.direction, kDroneSpeed);
    }
  }
}

void Drone::initDrone() { m_data.id = m_controller->getId(); }