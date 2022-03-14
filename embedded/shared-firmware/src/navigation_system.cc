#include "components/drone.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters
constexpr float kComparisonFactor = 10e-3;

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  // TODO : sending and receiving P2P messages needs to be done in a thread/task
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
      collisionAvoidance();
      m_controller->setVelocity(m_data.direction, kDroneSpeed);
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

    m_usedPeerData.clear();

    if (!m_data.direction.isAlmostEqual(newDirection, kComparisonFactor)) {
      m_data.direction = newDirection;
    }
  }
}
// 3250
void Drone::collisionAvoidance() {
  Vector3D normal;
  for (auto data : m_peerData) {
    DroneData peerData = data.second;

    if (peerData.range <= kSimulationCollisionAvoidanceRange) {
      if (m_usedPeerData.find(peerData.id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.id, peerData);
        if (Vector3D::areSameDirection(m_data.direction, peerData.direction)) {
          normal += m_data.direction - peerData.direction;
        } else {
          normal += peerData.direction;
        }
      }
    } else {
      m_usedPeerData.erase(peerData.id);
    }
  }

  if (!normal.isAlmostEqual(Vector3D(), kComparisonFactor)) {
    Vector3D newDirection = m_data.direction.reflect(normal);

    if (!m_data.direction.isAlmostEqual(newDirection, kComparisonFactor)) {
      m_data.direction = newDirection;
    }
  }
}

void Drone::initDrone() { m_data.id = m_controller->getId(); }