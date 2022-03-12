#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <string>
#include <utility>

constexpr float kThreshold = 0.5;

class Vector3D {
 public:
  Vector3D() = default;
  Vector3D(const Vector3D& other) = default;
  Vector3D(Vector3D&& other) = default;
  Vector3D& operator=(const Vector3D& other) = default;
  Vector3D& operator=(Vector3D&& other) = default;

  Vector3D(float x, float y, float z);
  virtual ~Vector3D() = default;

  Vector3D& operator+=(const Vector3D& other);
  [[nodiscard]] Vector3D operator+(const Vector3D& other) const;
  [[nodiscard]] Vector3D operator-(const Vector3D& other) const;
  [[nodiscard]] Vector3D operator*(float factor) const;
  [[nodiscard]] Vector3D operator/(float factor) const;
  [[nodiscard]] bool operator==(const Vector3D& other) const;
  [[nodiscard]] bool operator!=(const Vector3D& other) const;

  [[nodiscard]] float distanceTo(const Vector3D& other) const;
  [[nodiscard]] bool isAlmostEqual(const Vector3D& other,
                                   float threshold = kThreshold) const;
  float m_x{0}, m_y{0}, m_z{0};
};

#endif
