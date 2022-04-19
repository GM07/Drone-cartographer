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

  [[nodiscard]] inline Vector3D rotate(float angle) const {
    const float xRotated = m_x * std::cos(angle) - m_y * std::sin(angle);
    const float yRotated = m_x * std::sin(angle) + m_y * std::cos(angle);
    return Vector3D(xRotated, yRotated, m_z);
  }

  static Vector3D x(float value);
  static Vector3D y(float value);
  static Vector3D z(float value);

  float m_x{0}, m_y{0}, m_z{0};
} __attribute__((packed));

namespace std {

///////////////////////////////////////////////
template <class T>
inline size_t hash_combine(const std::size_t& seed, const T& v) {
  std::hash<T> hasher;
  return seed ^ (hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));  // NOLINT
}

template <>
struct hash<Vector3D> {
  size_t operator()(const Vector3D& vec) const {
    size_t seed = hash<uint64_t>()(std::lround(vec.m_x * 100.0F));
    seed = std::hash_combine<uint64_t>(seed, std::lround(vec.m_y * 100.0F));
    seed = std::hash_combine<uint64_t>(seed, std::lround(vec.m_z * 100.0F));
    return seed;
  }
};
}  // namespace std

#endif
