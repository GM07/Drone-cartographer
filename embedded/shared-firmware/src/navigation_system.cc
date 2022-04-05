#include "components/drone.h"
#include "utils/math.h"

/////////////////////////////////////////////////////////////////////
void Drone::step() {
  updateCrashStatus();
  m_controller->updateSensorsData();

  m_controller->receiveP2PMessage(&m_peerData);

  m_normal = Vector3D();

  switch (m_controller->m_state) {
    case State::kTakingOff:
      if (m_controller->isAltitudeReached()) {
        m_controller->m_state = State::kExploring;
        m_data.m_direction = m_initialDirection;
      }
      break;
    case State::kLanding:
      if (m_controller->isAltitudeReached()) {
        m_controller->m_state = State::kIdle;
        m_controller->stopMotors();
      }
      break;
    case State::kExploring:
      wallAvoidance();
      collisionAvoidance();
      changeDirection();
      break;
    case State::kReturnToBase:
      static bool jumpingWall = false;
      wallAvoidance();
      // If there is a wall jump over it
      if (!Math::areAlmostEqual(m_normal, Vector3D())) {
        m_data.m_direction = Vector3D::z(1.0f);

        // Ensures we go twice as high as the obstacle
        m_controller->m_targetPosition = m_controller->getCurrentLocation();
        m_controller->m_targetPosition.m_z *= 2;
        jumpingWall = true;

      } else if (jumpingWall) {
        if (m_controller->isAltitudeReached()) jumpingWall = false;
      } else {
        m_data.m_direction = Vector3D() - m_controller->getCurrentLocation();
        m_data.m_direction.m_z = 0;

        // If there are no walls and we are above our origin land
        m_controller->m_targetPosition =
            Vector3D::z(m_controller->getCurrentLocation().m_z);
        if (m_controller->isTrajectoryFinished()) {
          m_controller->land();
          m_data.m_direction = Vector3D::z(-1.0f);
          m_controller->m_state = State::kLanding;
        }
      }
      break;
    case State::kIdle:  // Fallthrough
    default:
      return;
  }

  m_controller->setVelocity(m_data.m_direction, kDroneSpeed);
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

  if (Math::areAlmostEqual<Vector3D>(m_normal, m_data.m_direction) ||
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
  if (!Math::areAlmostEqual(m_normal, Vector3D())) {
    Vector3D newDirection = m_data.m_direction.reflect(m_normal);

    if (!Math::areAlmostEqual(m_data.m_direction, newDirection)) {
      m_data.m_direction = newDirection;
    }
  }
}
