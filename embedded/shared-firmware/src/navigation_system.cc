#include "components/drone.h"
#include "utils/math.h"

/////////////////////////////////////////////////////////////////////
void Drone::step() {
  updateCrashStatus();
  m_data.m_distanceFromTakeoff = m_controller->getCurrentLocation().length();
  m_controller->updateSensorsData();
  updateStatusFromBattery();

  m_controller->receiveP2PMessage(&m_peerData);

  m_normal = Vector3D();

  switch (m_controller->m_state) {
    case State::kTakingOff:
      m_data.m_direction = Vector3D::z(1.0F);
      if (m_controller->isAltitudeReached()) {
        resetCollisionHistory();
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
      analyzeShortcuts();
      changeDirection();
      break;
    case State::kReturningToBase:
      returnToBaseStateSteps();
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

  for (const auto& [id, peerData] : m_peerData) {
    if (peerData.m_range <= m_controller->getMinCollisionAvoidanceDistance()) {
      m_peerCollision = true;
      if (m_usedPeerData.find(peerData.m_id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.m_id, peerData);

        // Makes sure both drone use the same random angle
        const float angle = peerData.m_id <= m_data.m_id
                                ? m_data.m_randomAngleRad
                                : peerData.m_randomAngleRad;

        // Rotate normals by the random angle
        Vector3D peerNormal = peerData.m_direction - m_data.m_direction;
        peerNormal = peerNormal.rotate(angle);

        // Add the normal so the drone can dodge
        m_normal += peerNormal;
      }
    } else {
      m_usedPeerData.erase(peerData.m_id);
    }
  }

  if (!m_peerCollision) {
    constexpr float maxRange = Math::pi<double> * 2;
    constexpr float minRange = 0.0F;
    static const uint32_t kSeed =
        std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(kSeed);
    std::uniform_real_distribution<float> distribution(minRange, maxRange);

    m_data.m_randomAngleRad = distribution(generator);
  }
}

void Drone::changeDirection() {
  if (!Math::areAlmostEqual(m_normal, Vector3D())) {
    Vector3D newDirection = m_data.m_direction.reflect(m_normal);

    if (!Math::areAlmostEqual(m_data.m_direction, newDirection)) {
      addToCollisionHistory();
      m_data.m_direction = newDirection;
    }
  }
}

void Drone::resetCollisionHistory() {
  m_collisionHistory.clear();
  m_collisionHistory.emplace_back();

  for (size_t i = 0; i < m_lastPathShortcutPosition.size(); ++i) {
    m_lastBaseShortcutPosition[i].reset();
    m_lastPathShortcutPosition[i].reset();
  }
  m_additionnalCollisionRange = 0.0F;
}

void Drone::addToCollisionHistory() {
  if (m_controller->m_state == State::kExploring) {
    for (size_t i = 0; i < m_lastBaseShortcutPosition.size(); ++i) {
      pushValidPath(i);
      m_lastBaseShortcutPosition[i].reset();
      m_lastPathShortcutPosition[i].reset();
    }
  }

  const bool kCanPushCheckpoint =
      (m_controller->m_state == State::kExploring || m_peerCollision) &&
      m_collisionHistory.size() < kMaxCheckpoints;

  if (kCanPushCheckpoint) {
    m_collisionHistory.push_back(m_controller->getCurrentLocation());
  }
}

void Drone::pushValidPath(size_t index) {
  if (index < m_lastPathShortcutPosition.size() &&
      m_lastPathShortcutPosition[index].isPathIntersectionValid) {
    m_collisionHistory.pop_back();
    m_collisionHistory.emplace_back(
        m_lastPathShortcutPosition[index].intersectionPosition);
    m_collisionHistory.emplace_back(m_lastPathShortcutPosition[index].source);
  }
}

void Drone::returnToBaseDirection() {
  changeDirection();

  if (m_collisionHistory.empty()) {
    m_collisionHistory.emplace_back();
  }

  m_controller->m_targetPosition = m_collisionHistory.back();

  if (!m_peerCollision && Math::areAlmostEqual(m_normal, Vector3D())) {
    m_data.m_direction =
        m_controller->m_targetPosition - m_controller->getCurrentLocation();
    m_data.m_direction.m_z = 0;
  }
}

void Drone::returnToBaseStateSteps() {
  wallAvoidance();
  collisionAvoidance();
  returnToBaseDirection();
  if (m_controller->isNearBase()) {
    m_collisionHistory.clear();
    m_collisionHistory.emplace_back();
    m_controller->land();
    m_controller->m_state = State::kLanding;
    return;
  }
  m_controller->setVelocity(m_data.m_direction, kSpeed);

  if (!m_peerCollision && !m_collisionHistory.empty() &&
      m_controller->hasReachedCheckpoint()) {
    m_collisionHistory.pop_back();
  }
}

void Drone::analyzeShortcuts() {
  constexpr float kMeterToMillimiter = 1000.0F;

  const Vector3D& kLocation = m_controller->getCurrentLocation();

  float frontData = getRealSensorDistance(m_controller->m_data.front);
  float backData = getRealSensorDistance(m_controller->m_data.back);
  float leftData = getRealSensorDistance(m_controller->m_data.left);
  float rightData = getRealSensorDistance(m_controller->m_data.right);

  const Vector3D& frontPoint =
      kLocation + Vector3D::x(frontData / kMeterToMillimiter);
  const Vector3D& backPoint =
      kLocation - Vector3D::x(backData / kMeterToMillimiter);
  const Vector3D& leftPoint =
      kLocation + Vector3D::y(leftData / kMeterToMillimiter);
  const Vector3D& rightPoint =
      kLocation - Vector3D::y(rightData / kMeterToMillimiter);

  analyzeBaseShortcuts(frontPoint, backPoint, leftPoint, rightPoint);
  analyzePathShortcuts(frontPoint, backPoint, leftPoint, rightPoint);
}

float Drone::getRealSensorDistance(float sensor) {
  constexpr float kMaxDist = 1500.0F;
  constexpr float kTooFar = -19.0F;

  if (sensor > 0.0F) {
    return sensor;
  }
  if (sensor <= kTooFar) {
    return kMaxDist;
  }
  return 0.0F;
}

[[nodiscard]] std::array<float, kNbLateralSensors> Drone::getPathDifferenceList(
    const std::vector<ShortcutVerifier>& shortcuts, const Vector3D& location) {
  float pathDifferenceFront = std::fabs(std::fabs(location.m_x) -
                                        std::fabs(shortcuts.at(0).source.m_x));
  float pathDifferenceBack = std::fabs(std::fabs(location.m_x) -
                                       std::fabs(shortcuts.at(1).source.m_x));
  float pathDifferenceLeft = std::fabs(std::fabs(location.m_y) -
                                       std::fabs(shortcuts.at(2).source.m_y));
  float pathDifferenceRight = std::fabs(std::fabs(location.m_y) -
                                        std::fabs(shortcuts.at(3).source.m_y));
  return {{pathDifferenceFront, pathDifferenceBack, pathDifferenceLeft,
           pathDifferenceRight}};
}

void Drone::analyzeBaseShortcuts(const Vector3D& frontPoint,
                                 const Vector3D& backPoint,
                                 const Vector3D& leftPoint,
                                 const Vector3D& rightPoint) {
  constexpr float kHalfBaseCross = 0.3F;
  const Vector3D& kLocation = m_controller->getCurrentLocation();

  std::pair<bool, Vector3D> frontIntersect = Math::intersectsSegment(
      kLocation, frontPoint, Vector3D::y(-kHalfBaseCross),
      Vector3D::y(kHalfBaseCross));
  std::pair<bool, Vector3D> backIntersect = Math::intersectsSegment(
      kLocation, backPoint, Vector3D::y(-kHalfBaseCross),
      Vector3D::y(kHalfBaseCross));
  std::pair<bool, Vector3D> leftIntersect = Math::intersectsSegment(
      kLocation, leftPoint, Vector3D::x(-kHalfBaseCross),
      Vector3D::x(kHalfBaseCross));
  std::pair<bool, Vector3D> rightIntersect = Math::intersectsSegment(
      kLocation, rightPoint, Vector3D::x(-kHalfBaseCross),
      Vector3D::x(kHalfBaseCross));

  std::array<std::pair<bool, Vector3D>, kNbLateralSensors> intersectionList{
      {frontIntersect, backIntersect, leftIntersect, rightIntersect}};

  std::array<float, kNbLateralSensors> differenceList =
      getPathDifferenceList(m_lastBaseShortcutPosition, kLocation);

  for (size_t i = 0; i < kNbLateralSensors; ++i) {
    if (intersectionList.at(i).first &&
        !m_lastBaseShortcutPosition[i].hasFirstIntersection) {
      m_lastBaseShortcutPosition[i].hasFirstIntersection = true;
      m_lastBaseShortcutPosition[i].source = kLocation;
    } else if (intersectionList.at(i).first &&
               m_lastBaseShortcutPosition.at(i).hasFirstIntersection &&
               differenceList.at(i) > kMinPathDistanceInterval &&
               differenceList.at(i) < kMaxPathDistanceInterval) {
      if (m_lastBaseShortcutPosition.at(i).hasSecondIntersection) {
        // remove old path
        m_collisionHistory.clear();
        m_collisionHistory.emplace_back();
        m_collisionHistory.emplace_back(
            m_lastBaseShortcutPosition[i].intersectionPosition);
        m_collisionHistory.emplace_back(m_lastBaseShortcutPosition[i].source);
      }
      m_lastBaseShortcutPosition[i].hasSecondIntersection = true;
      m_lastBaseShortcutPosition[i].source = kLocation;
      m_lastBaseShortcutPosition[i].intersectionPosition =
          intersectionList.at(i).second;
    } else if (!intersectionList.at(i).first ||
               (m_lastBaseShortcutPosition.at(i).hasFirstIntersection &&
                differenceList.at(i) >= kMaxPathDistanceInterval)) {
      m_lastBaseShortcutPosition[i].reset();
    }
  }
}

void Drone::analyzePathShortcuts(const Vector3D& frontPoint,
                                 const Vector3D& backPoint,
                                 const Vector3D& leftPoint,
                                 const Vector3D& rightPoint) {
  if (m_collisionHistory.size() < 2) {
    return;
  }

  const Vector3D& kLocation = m_controller->getCurrentLocation();
  const Vector3D& endPoint1 = m_collisionHistory[m_collisionHistory.size() - 2];
  const Vector3D& endPoint2 = m_collisionHistory.back();

  std::pair<bool, Vector3D> frontIntersect =
      Math::intersectsSegment(kLocation, frontPoint, endPoint1, endPoint2);
  std::pair<bool, Vector3D> backIntersect =
      Math::intersectsSegment(kLocation, backPoint, endPoint1, endPoint2);
  std::pair<bool, Vector3D> leftIntersect =
      Math::intersectsSegment(kLocation, leftPoint, endPoint1, endPoint2);
  std::pair<bool, Vector3D> rightIntersect =
      Math::intersectsSegment(kLocation, rightPoint, endPoint1, endPoint2);

  std::array<std::pair<bool, Vector3D>, kNbLateralSensors> pathIntersectionList{
      {frontIntersect, backIntersect, leftIntersect, rightIntersect}};

  std::array<float, kNbLateralSensors> pathDifferenceList =
      getPathDifferenceList(m_lastPathShortcutPosition, kLocation);

  for (size_t i = 0; i < kNbLateralSensors; ++i) {
    if (pathIntersectionList.at(i).first &&
        !m_lastPathShortcutPosition.at(i).hasFirstIntersection) {
      m_lastPathShortcutPosition[i].hasFirstIntersection = true;
      m_lastPathShortcutPosition[i].source = kLocation;

    } else if (pathIntersectionList.at(i).first &&
               m_lastPathShortcutPosition.at(i).hasFirstIntersection &&
               pathDifferenceList.at(i) > kMinPathDistanceInterval &&
               pathDifferenceList.at(i) < kMaxPathDistanceInterval) {
      if (m_lastPathShortcutPosition.at(i).hasSecondIntersection) {
        m_lastPathShortcutPosition[i].isPathIntersectionValid = true;
      }

      m_lastPathShortcutPosition[i].hasSecondIntersection = true;
      m_lastPathShortcutPosition[i].source = kLocation;
      m_lastPathShortcutPosition[i].intersectionPosition =
          pathIntersectionList.at(i).second;

    } else if (!pathIntersectionList.at(i).first ||
               (m_lastPathShortcutPosition.at(i).hasFirstIntersection &&
                pathDifferenceList.at(i) >= kMaxPathDistanceInterval)) {
      pushValidPath(i);
      m_lastPathShortcutPosition[i].reset();
    }
  }
}

/////////////////////////////////////////////////////////////////////
[[nodiscard]] float Drone::getAddedCollisionRange() const {
  if (m_controller->m_state != State::kReturningToBase) {
    return 0.0F;
  }

  return m_additionnalCollisionRange;
}
