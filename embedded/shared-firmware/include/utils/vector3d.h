#ifndef VECTOR3D_H
#define VECTOR3D_H

#define ALMOST_EQUAL_THRESHOLD 0.05

class Vector3D {
 public:
  Vector3D();
  Vector3D(float x, float y, float z);
  virtual ~Vector3D() = default;

  Vector3D operator+(const Vector3D& other);
  Vector3D operator-(const Vector3D& other);
  Vector3D operator*(const float factor);
  Vector3D operator/(const float factor);
  bool operator==(const Vector3D& other) const;
  float distanceTo(const Vector3D& other) const;
  bool isAlmostEqual(const Vector3D& other) const;

  float m_x, m_y, m_z;
};

#endif
