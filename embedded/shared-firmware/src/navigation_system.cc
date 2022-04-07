#include "components/drone.h"
#include "utils/math.h"

/////////////////////////////////////////////////////////////////////
void Drone::step() {
  updateCrashStatus();
  m_data.m_distanceFromTakeoff = m_controller->getCurrentLocation().length();
  m_controller->updateSensorsData();

  m_controller->receiveP2PMessage(&m_peerData);

  m_normal = Vector3D();

  switch (m_controller->m_state) {
    case State::kTakingOff:
      m_data.m_direction = Vector3D::z(1.0F);
      if (m_controller->isAltitudeReached()) {
        m_controller->m_state = State::kExploring;
        m_data.m_direction = m_initialDirection;
      }
      break;
    case State::kLanding:
      m_data.m_direction = Vector3D::z(-1.0F);
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
    case State::kIdle:  // Fallthrough
    default:
      return;
  }

  m_controller->setVelocity(m_data.m_direction, kSpeed);
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
  m_peerCollision = false;

  for (const auto &[id, peerData] : m_peerData) {
    if (peerData.m_range <= m_controller->getMinCollisionAvoidanceDistance()) {
      m_peerCollision = true;
      if (m_usedPeerData.find(peerData.m_id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.m_id, peerData);

        // Makes sure both drone use the same random angle
        float angle = peerData.m_id <= m_data.m_id ? m_data.m_randomAngleRad
                                                   : peerData.m_randomAngleRad;

        // Rotate normals by the random angle
        Vector3D peerNormal = peerData.m_direction - m_data.m_direction;
        peerNormal.m_x =
            cosf(angle) * peerNormal.m_x - sinf(angle) * peerNormal.m_y;
        peerNormal.m_y =
            sinf(angle) * peerNormal.m_x + cosf(angle) * peerNormal.m_y;

        // Add the normal so the drone can dodge
        m_normal += peerNormal;
      }
    } else {
      m_usedPeerData.erase(peerData.m_id);
    }
  }

  if (!m_peerCollision) {
    constexpr float maxRange = Math::kPi * 2;
    constexpr float minRange = 0;
    static const uint32_t kSeed =
        std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(kSeed);
    std::uniform_real_distribution<float> distribution(minRange, maxRange);

    m_data.m_randomAngleRad = distribution(generator);
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
