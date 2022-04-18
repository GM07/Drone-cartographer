#include "components/drone.h"
#include "utils/math.h"
#include "utils/segment.h"

/////////////////////////////////////////////////////////////////////
void Drone::step() {
  updateCrashStatus();
  m_data.m_distanceFromTakeoff = m_controller->getCurrentLocation().length();
  m_controller->updateSensorsData();
  updateStatusFromBattery();

  m_controller->receiveP2PMessage(&m_peerData);

  // Reset the avoidance normal and bodyReference boolean at each step
  m_normal = Vector3D();
  bool bodyReference = true;

  switch (m_controller->m_state) {
    case State::kTakingOff:
      m_data.m_direction = Vector3D::z(1.0F);
      if (m_controller->isAltitudeReached()) {
        // Reset the return to base algo
        m_potentialShortCuts.clear();
        m_returnPath.clear();
        m_returnPath.push_back(m_controller->getCurrentLocation());

        // Go into exploring
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
      analyzeShortcuts();
      break;
    case State::kReturningToBase:
      wallAvoidance();
      collisionAvoidance();
      changeDirection();
      bodyReference = !returnToBase();
      analyzeShortcuts();
      break;
    default:
      return;
  }

  m_controller->setVelocity(m_data.m_direction, kSpeed, bodyReference);
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
  if (Math::areAlmostEqual(m_normal, Vector3D())) {
    return;
  }

  Vector3D newDirection = m_data.m_direction.reflect(m_normal);
  if (Math::areAlmostEqual(m_data.m_direction, newDirection)) {
    return;
  }

  // Add point to the return path
  // Only add the point if we are not in returnToBase or if we had a
  // collision This is because we disable wallCollision when returning to
  // base Otherwise the drone gets stuck and we have -nan values as speed
  if (m_controller->m_state == State::kExploring || m_peerCollision) {
    m_returnPath.push_back(m_controller->getCurrentLocation());
  }

  // Invalidate pending shortcuts since we had a chage of direction
  m_potentialShortCuts.clear();

  m_data.m_direction = newDirection;
}

bool Drone::returnToBase() {
  if (m_controller->isNearBase()) {
    m_data.m_direction = Vector3D::z(-1.0F);
    m_controller->land();
    return false;
  }

  // If we are in a drone collision
  if (m_peerCollision) {
    return false;
  }

  if (!m_returnPath.empty()) {
    m_controller->m_targetPosition = m_returnPath[m_returnPath.size() - 1];
  }

  if (m_controller->hasReachedCheckpoint()) {
    if (!m_returnPath.empty()) {
      m_returnPath.pop_back();
    }

    // Invalidate pending shortcuts since we had a chage of direction
    m_potentialShortCuts.clear();
  }

  m_data.m_direction =
      m_controller->m_targetPosition - m_controller->getCurrentLocation();
  m_data.m_direction.m_z = 0;

  return true;
}

std::array<Segment, kNbLateralSensors> Drone::createSensorSegments(
    const Vector3D& currentLocation) {
  // Create the segments from the sensors
  constexpr float kMeterToMillimiter = 1000.0F;

  float frontData = getRealSensorDistance(m_controller->m_data.front);
  float backData = getRealSensorDistance(m_controller->m_data.back);
  float leftData = getRealSensorDistance(m_controller->m_data.left);
  float rightData = getRealSensorDistance(m_controller->m_data.right);

  // We must rotate segments
  const Vector3D& frontPoint =
      currentLocation + Vector3D::x(frontData / kMeterToMillimiter)
                            .rotate(m_controller->getSegmentOrientation());
  const Vector3D& backPoint =
      currentLocation - Vector3D::x(backData / kMeterToMillimiter)
                            .rotate(m_controller->getSegmentOrientation());
  const Vector3D& leftPoint =
      currentLocation + Vector3D::y(leftData / kMeterToMillimiter)
                            .rotate(m_controller->getSegmentOrientation());
  const Vector3D& rightPoint =
      currentLocation - Vector3D::y(rightData / kMeterToMillimiter)
                            .rotate(m_controller->getSegmentOrientation());

  return {{Segment(currentLocation, frontPoint),
           Segment(currentLocation, backPoint),
           Segment(currentLocation, leftPoint),
           Segment(currentLocation, rightPoint)}};
}

void Drone::findPotentialShortCut(
    const Vector3D& currentLocation,
    const std::array<Segment, kNbLateralSensors>& sensorSegments) {
  // Check for segment intersection with start from closest to base to last
  for (size_t i = 1; i < m_returnPath.size(); ++i) {
    Segment segment(m_returnPath[i - 1], m_returnPath[i]);

    for (const auto& sensorSegment : sensorSegments) {
      std::optional<Vector3D> intersection = segment.intersects(sensorSegment);
      if (intersection.has_value()) {
        // No need to add segment we will be going to
        if (Math::areAlmostEqual(intersection.value(),
                                 m_returnPath[m_returnPath.size() - 1],
                                 kMaxDistanceToCheckpoint)) {
          break;
        }

        // Return the potentialShortcut
        // kDistanceForSafeShortcut of 0.1m might be too small for the real
        // drones
        PotentialShortcut shortcut = {
            .detectionPosition = currentLocation,
            .intersectionPosition = intersection.value(),
            .sensorDirection =
                (sensorSegment.m_end - sensorSegment.m_start).toUnitVector(),
            .intersectionSegment = segment,
            .indexToAddAt = i,
            .distanceToTravel = kDistanceForSafeShortcut};

        if (m_potentialShortCuts.find(shortcut) == m_potentialShortCuts.end()) {
          m_potentialShortCuts.insert(shortcut);
        }
        return;
      }
    }
  }
}

void Drone::validatePotentialShortCuts(
    const Vector3D& currentLocation,
    const std::array<Segment, kNbLateralSensors>& sensorSegments) {
  // We use an iterator to be able to delete while iterating
  auto potentialShortcutIterator = m_potentialShortCuts.begin();
  while (potentialShortcutIterator != m_potentialShortCuts.end()) {
    if (potentialShortcutIterator->hasTraveledEnoughDistance(currentLocation)) {
      // Find the same sensorSegment that was used for the first detection
      Segment shortcutSensorSegment;
      for (const auto& sensorSegment : sensorSegments) {
        if (Math::areAlmostEqual(
                potentialShortcutIterator->sensorDirection,
                (sensorSegment.m_end - sensorSegment.m_start).toUnitVector())) {
          shortcutSensorSegment = sensorSegment;
          break;
        }
      }

      std::optional<Vector3D> intersection =
          potentialShortcutIterator->intersectionSegment.intersects(
              shortcutSensorSegment);
      if (intersection.has_value()) {
        // Erase all segments after the shortcut
        m_returnPath.erase(std::next(m_returnPath.begin(),
                                     potentialShortcutIterator->indexToAddAt),
                           m_returnPath.end());

        m_returnPath.push_back(
            potentialShortcutIterator->intersectionPosition +
            (intersection.value() -
             potentialShortcutIterator->intersectionPosition) /
                2);
        m_returnPath.push_back(
            potentialShortcutIterator->detectionPosition +
            (currentLocation - potentialShortcutIterator->detectionPosition) /
                2);

        // Once a shortcut is found remove all other potential shortcuts
        // This is becease a shortcut changes the segments we analyse
        m_potentialShortCuts.clear();

        // Once a shortcut is found leave the loop
        break;
      }
      // If we can't use the shortcut anymore delete it
      potentialShortcutIterator =
          m_potentialShortCuts.erase(potentialShortcutIterator);
    } else {
      ++potentialShortcutIterator;
    }
  }
}

void Drone::analyzeShortcuts() {
  Vector3D currentLocation = m_controller->getCurrentLocation();
  currentLocation.m_z = 0;

  std::array<Segment, kNbLateralSensors> sensorSegments =
      createSensorSegments(currentLocation);

  findPotentialShortCut(currentLocation, sensorSegments);

  // If segment is found make sure we can travel to it (check for
  // distance traveled and segment still intersecting)
  validatePotentialShortCuts(currentLocation, sensorSegments);
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
