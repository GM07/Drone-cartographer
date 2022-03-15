#include "components/drone.h"
#include "utils/math.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters

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
      wallAvoidance();
      collisionAvoidance();
      changeDirection();
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

  if (m_normal.isAlmostEqual(m_data.direction, kComparisonFactor) ||
      Vector3D::areSameDirection(m_data.direction, m_normal)) {
    m_normal = Vector3D();
  }
}

void Drone::collisionAvoidance() {
  for (auto data : m_peerData) {
    DroneData peerData = data.second;

    if (peerData.range <= kSimulationCollisionAvoidanceRange) {
      if (m_usedPeerData.find(peerData.id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.id, peerData);
        // m_normal += m_data.direction;
        m_normal += peerData.direction - m_data.direction;
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

<<<<<<< HEAD
void Drone::initDrone() { m_data.id = m_controller->getId(); }
=======
m_controller->setVelocity(m_direction, kDroneSpeed);
}
>>>>>>> d7b6495dee3d79871040f0b869febd316ac7bf20
