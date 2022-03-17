#include "components/drone.h"
#include "utils/math.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  static uint8_t p2pCounter = 0;
  constexpr uint8_t kP2PDelay = 100;
  if (p2pCounter++ > kP2PDelay) {
    m_controller->sendP2PMessage(static_cast<void *>(&m_data), sizeof(m_data));
    p2pCounter = 0;
  }

  m_controller->receiveP2PMessage(&m_peerData);

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
      wallAvoidance();
      collisionAvoidance();
      changeDirection();
      m_controller->setVelocity(m_data.direction, kDroneSpeed);
    default:
      break;
  }
}

void Drone::wallAvoidance() {
  if (m_controller->data.front > 0 &&
      m_controller->data.front <= kMinDistanceObstacle) {
    m_normal += Vector3D::x(-1.0F);
  }

  if (m_controller->data.back > 0 &&
      m_controller->data.back <= kMinDistanceObstacle) {
    m_normal += Vector3D::x(1.0F);
  }

  // This extra condition makes sure that if we are trapped between walls we
  // move away from the closest one
  if (m_controller->data.back > 0 && m_controller->data.front > 0 &&
      m_controller->data.back <= kMinDistanceObstacle &&
      m_controller->data.front <= kMinDistanceObstacle) {
    m_normal += m_controller->data.back < m_controller->data.front
                    ? Vector3D::x(1.0F)
                    : Vector3D::x(-1.0F);
  }

  if (m_controller->data.left > 0 &&
      m_controller->data.left <= kMinDistanceObstacle) {
    m_normal += Vector3D::y(-1.0F);
  }

  if (m_controller->data.right > 0 &&
      m_controller->data.right <= kMinDistanceObstacle) {
    m_normal += Vector3D::y(1.0F);
  }

  // This extra condition makes sure that if we are trapped between walls we
  // move away from the closest one
  if (m_controller->data.left > 0 && m_controller->data.right > 0 &&
      m_controller->data.left <= kMinDistanceObstacle &&
      m_controller->data.right <= kMinDistanceObstacle) {
    m_normal += m_controller->data.left < m_controller->data.right
                    ? Vector3D::y(-1.0F)
                    : Vector3D::y(1.0F);
  }

  if (areAlmostEqual<Vector3D>(m_normal, m_data.direction) ||
      Vector3D::areSameDirection(m_data.direction, m_normal)) {
    m_normal = Vector3D();
  }
}

void Drone::collisionAvoidance() {
  for (const std::pair<const uint64_t, DroneData> &data : m_peerData) {
    DroneData peerData = data.second;

    if (peerData.range <= m_controller->getMinCollisionAvoidanceDistance()) {
      if (m_usedPeerData.find(peerData.id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.id, peerData);
        m_normal += peerData.direction - m_data.direction;
      }
    } else if (peerData.range >
               m_controller->getMaxCollisionAvoidanceDistance()) {
      m_usedPeerData.erase(peerData.id);
    }
  }
}

void Drone::changeDirection() {
  if (!areAlmostEqual<Vector3D>(m_normal, Vector3D())) {
    Vector3D newDirection = m_data.direction.reflect(m_normal);

    if (!areAlmostEqual<Vector3D>(m_data.direction, newDirection)) {
      m_data.direction = newDirection;
    }
  }
}

void Drone::initDrone() { m_data.id = m_controller->getId(); }
