#ifndef POTENTIAL_SHORTCUT_H
#define POTENTIAL_SHORTCUT_H

#include "utils/segment.h"
#include "utils/vector3d.h"

struct __attribute__((__packed__)) PotentialShortcut {
  Vector3D detectionPosition;
  Vector3D intersectionPosition;

  // sensorDirection must be a unit vector otherwise the hash and
  // hasTraveledEnoughDistance function wont work
  Vector3D sensorDirection;

  Segment intersectionSegment;
  size_t indexToAddAt;
  float distanceToTravel;

  [[nodiscard]] bool operator==(const PotentialShortcut& other) const;

  [[nodiscard]] inline bool hasTraveledEnoughDistance(
      const Vector3D& currentLocation) const {
    Vector3D difference = currentLocation - detectionPosition;
    difference.m_x -= difference.m_x * sensorDirection.m_x;
    difference.m_y -= difference.m_y * sensorDirection.m_y;
    difference.m_z = 0.0F;
    return difference.length() >= distanceToTravel;
  }
};

namespace std {
template <>
struct hash<PotentialShortcut> {
  size_t operator()(const PotentialShortcut& potentialShortcut) const {
    size_t seed = hash<Vector3D>()(potentialShortcut.sensorDirection);
    seed = std::hash_combine<Segment>(seed, potentialShortcut.intersectionSegment);
    return seed;
  }
};
}  // namespace std

#endif
