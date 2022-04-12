#include "utils/math.h"

#include "utils/vector3d.h"

/* from
 * https://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/
 * ax + by = c
 */
namespace Math {
std::pair<bool, Vector3D> intersectsSegment(const Vector3D& start1,
                                            const Vector3D& end1,
                                            const Vector3D& start2,
                                            const Vector3D& end2) {
  // a1x + b1y = c1
  const float a1 = end1.m_y - start1.m_y;
  const float b1 = start1.m_x - end1.m_x;
  const float c1 = a1 * (start1.m_x) + b1 * (start1.m_y);

  // a2x + b2y = c2
  const float a2 = end2.m_y - start2.m_y;
  const float b2 = start2.m_x - end2.m_x;
  const float c2 = a2 * (start2.m_x) + b2 * (start2.m_y);

  const float determinant = a1 * b2 - a2 * b1;

  if (Math::areAlmostEqual(determinant, 0.0F)) {
    return std::make_pair(false, Vector3D());
  }
  const float kX = (b2 * c1 - b1 * c2) / determinant;
  const float kY = (a1 * c2 - a2 * c1) / determinant;
  const Vector3D returnedVector(kX, kY, 0);
  if (Math::isOnSegment(start1, end1, returnedVector) &&
      Math::isOnSegment(start2, end2, returnedVector)) {
    return std::make_pair(true, returnedVector);
  }
  return std::make_pair(false, Vector3D());
}

}  // namespace Math