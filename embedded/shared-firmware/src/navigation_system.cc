#include <cmath>

#include "components/drone.h"
#include "utils/math.h"

/////////////////////////////////////////////////////////////////////
void Drone::step() {
  updateCrashStatus();
  m_controller->updateSensorsData();

  m_controller->receiveP2PMessage(&m_peerData);

  switch (m_controller->m_state) {
    case State::kTakingOff:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->m_state = State::kExploring;
        m_controller->setVelocity(m_data.m_direction, kDroneSpeed);
      }
      break;
    case State::kLanding:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->m_state = State::kIdle;
      }
      break;
    case State::kExploring:
      m_normal = Vector3D();
      wallAvoidance();
      collisionAvoidance();
      changeDirection();
      m_controller->setVelocity(m_data.m_direction, kDroneSpeed);
    case State::kIdle:  // Fallthrough
    default:
      break;
  }
}

/////////////////////////////////////////////////////////////////////
void Drone::wallAvoidance() {
  constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters

  if (m_controller->m_data.front > 0 &&
      m_controller->m_data.front <= kMinDistanceObstacle) {
    m_normal.m_x += -1.0F;
  }

  if (m_controller->m_data.back > 0 &&
      m_controller->m_data.back <= kMinDistanceObstacle) {
    m_normal.m_x += 1.0F;
  }

  if (m_controller->m_data.left > 0 &&
      m_controller->m_data.left <= kMinDistanceObstacle) {
    m_normal.m_y += -1.0F;
  }

  if (m_controller->m_data.right > 0 &&
      m_controller->m_data.right <= kMinDistanceObstacle) {
    m_normal.m_y += 1.0F;
  }

  // This extra condition makes sure that if we are trapped between walls we
  // move away from the closest one
  if (m_controller->m_data.back > 0 && m_controller->m_data.front > 0 &&
      m_controller->m_data.back <= kMinDistanceObstacle &&
      m_controller->m_data.front <= kMinDistanceObstacle) {
    m_normal.m_x +=
        m_controller->m_data.back < m_controller->m_data.front ? 1.0F : -1.0F;
  }

  // This extra condition makes sure that if we are trapped between walls we
  // move away from the closest one
  if (m_controller->m_data.left > 0 && m_controller->m_data.right > 0 &&
      m_controller->m_data.left <= kMinDistanceObstacle &&
      m_controller->m_data.right <= kMinDistanceObstacle) {
    m_normal.m_y +=
        m_controller->m_data.left < m_controller->m_data.right ? -1.0F : 1.0F;
  }

  if (areAlmostEqual<Vector3D>(m_normal, m_data.m_direction) ||
      Vector3D::areSameDirection(m_data.m_direction, m_normal)) {
    m_normal = Vector3D();
  }
}

/////////////////////////////////////////////////////////////////////
void Drone::collisionAvoidance() {
  for (const auto &[id, peerData] : m_peerData) {
    if (peerData.m_range <= m_controller->getMinCollisionAvoidanceDistance()) {
      if (m_usedPeerData.find(peerData.m_id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.m_id, peerData);
        m_normal += peerData.m_direction - m_data.m_direction;
      }
    } else {
      m_usedPeerData.erase(peerData.m_id);
    }
  }
}

/////////////////////////////////////////////////////////////////////
void Drone::changeDirection() {
  if (!areAlmostEqual(m_normal, Vector3D())) {
    Vector3D newDirection = m_data.m_direction.reflect(m_normal);

    if (!areAlmostEqual(m_data.m_direction, newDirection)) {
      m_data.m_direction = newDirection;
    }
  }
}
