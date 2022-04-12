#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <string>
#include <utility>

class Vector3D {
 public:
  Vector3D() = default;
  Vector3D(const Vector3D& other) = default;
  Vector3D(Vector3D&& other) = default;
  Vector3D& operator=(const Vector3D& other) = default;
  Vector3D& operator=(Vector3D&& other) = default;

  Vector3D(float x, float y, float z);
  ~Vector3D() = default;

  Vector3D& operator+=(const Vector3D& other);
  Vector3D& operator-=(const Vector3D& other);
  [[nodiscard]] Vector3D operator+(const Vector3D& other) const;
  [[nodiscard]] Vector3D operator-(const Vector3D& other) const;
  [[nodiscard]] Vector3D operator*(float factor) const;
  [[nodiscard]] float operator*(const Vector3D& other) const;
  [[nodiscard]] Vector3D operator/(float factor) const;
  [[nodiscard]] bool operator==(const Vector3D& other) const;
  [[nodiscard]] bool operator!=(const Vector3D& other) const;

  [[nodiscard]] float distanceTo(const Vector3D& other) const;
  [[nodiscard]] float distanceToXY(const Vector3D& other) const;
  [[nodiscard]] Vector3D reflect(const Vector3D& normal) const;
  [[nodiscard]] Vector3D toUnitVector() const;
  [[nodiscard]] float length() const;
  [[nodiscard]] std::string toString() const;

  [[nodiscard]] inline bool operator<(float value) const {
    return this->length() < value;
  }

  [[nodiscard]] inline bool operator>(float value) const {
    return this->length() > value;
  }

  static bool areSameDirection(const Vector3D& vec1, const Vector3D& vec2);

  inline Vector3D rotate(float angle) const {
    const float xRotated = m_x * std::cos(angle) - m_y * std::sin(angle);
    const float yRotated = m_y * std::cos(angle) + m_x * std::sin(angle);
    return Vector3D(xRotated, yRotated, m_z);
  }

  static Vector3D x(float value);
  static Vector3D y(float value);
  static Vector3D z(float value);

  float m_x{0}, m_y{0}, m_z{0};
} __attribute__((packed));

#endif
