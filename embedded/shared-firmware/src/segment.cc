#include "utils/segment.h"

Segment::Segment(const Vector3D& start, const Vector3D& end)
    : m_start(start), m_end(end) {}

[[nodiscard]] std::optional<Vector3D> Segment::intersects(
    const Segment& other) const {
  // a1x + b1y = c1
  const float a1 = this->m_end.m_y - this->m_start.m_y;
  const float b1 = this->m_start.m_x - this->m_end.m_x;
  const float c1 = a1 * (this->m_start.m_x) + b1 * (this->m_start.m_y);

  // a2x + b2y = c2
  const float a2 = other.m_end.m_y - other.m_start.m_y;
  const float b2 = other.m_start.m_x - other.m_end.m_x;
  const float c2 = a2 * (other.m_start.m_x) + b2 * (other.m_start.m_y);

  const float determinant = a1 * b2 - a2 * b1;

  if (Math::areAlmostEqual(determinant, 0.0F)) {
    return std::nullopt;
  }

  const float kX = (b2 * c1 - b1 * c2) / determinant;
  const float kY = (a1 * c2 - a2 * c1) / determinant;
  const Vector3D returnedVector(kX, kY, 0);

  if (this->isOnSegment(returnedVector) && other.isOnSegment(returnedVector)) {
    return std::make_optional<Vector3D>(returnedVector);
  }

  return std::nullopt;
}

[[nodiscard]] bool Segment::isOnSegment(const Vector3D& point,
                                        float threshold) const {
  return point.m_x >= std::min(m_start.m_x, m_end.m_x) - threshold &&
         point.m_x <= std::max(m_start.m_x, m_end.m_x) + threshold &&
         point.m_y >= std::min(m_start.m_y, m_end.m_y) - threshold &&
         point.m_y <= std::max(m_start.m_y, m_end.m_y) + threshold;
}