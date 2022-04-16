#ifndef SEGMENT_H
#define SEGMENT_H

#include <optional>

#include "utils/math.h"
#include "utils/vector3d.h"

class __attribute__((__packed__)) Segment {
 public:
  Segment() = default;
  Segment(const Segment& other) = default;
  Segment(Segment&& other) = default;
  Segment& operator=(const Segment& other) = default;
  Segment& operator=(Segment&& other) = default;

  Segment(const Vector3D& start, const Vector3D& end);
  ~Segment() = default;

  [[nodiscard]] std::optional<Vector3D> intersects(const Segment& other) const;
  [[nodiscard]] bool isOnSegment(const Vector3D& point,
                                 float threshold = kThreshold) const;

  Vector3D m_start;
  Vector3D m_end;
};

namespace std {
template <>
struct hash<Segment> {
  size_t operator()(const Segment& segment) const {
    size_t seed = hash<Vector3D>()(segment.m_start);
    std::hash_combine<Vector3D>(seed, segment.m_end);
    return seed;
  }
};
}  // namespace std

#endif