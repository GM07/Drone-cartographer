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

  inline void rotate(float angle) {
    const float xValue = m_x;
    const float yValue = m_y;
    m_x = xValue * std::cos(angle) - yValue * std::sin(angle);
    m_y = yValue * std::cos(angle) + xValue * std::sin(angle);
  }

  static Vector3D x(float value);
  static Vector3D y(float value);
  static Vector3D z(float value);

  float m_x{0}, m_y{0}, m_z{0};
} __attribute__((packed));

#endif
