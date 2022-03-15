#include "utils/vector3d.h"

#include <cmath>

Vector3D::Vector3D(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

Vector3D Vector3D::operator+(const Vector3D& other) const {
  return Vector3D(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
}

Vector3D Vector3D::operator-(const Vector3D& other) const {
  return Vector3D(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
}

Vector3D Vector3D::operator*(const float factor) const {
  return Vector3D(m_x * factor, m_y * factor, m_z * factor);
}

float Vector3D::operator*(const Vector3D& other) const {
  return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;
}

Vector3D Vector3D::operator/(const float factor) const {
  return Vector3D(m_x / factor, m_y / factor, m_z / factor);
}

bool Vector3D::operator==(const Vector3D& other) const {
  return m_x == other.m_x && m_y == other.m_y && m_z == other.m_z;
}

bool Vector3D::operator!=(const Vector3D& other) const {
  return !(*this == other);
}

float Vector3D::distanceTo(const Vector3D& other) const {
  return std::sqrt(powf(m_x - other.m_x, 2) + powf(m_y - other.m_y, 2) +
                   powf(m_z - other.m_z, 2));
}

Vector3D Vector3D::reflect(const Vector3D& normal) const {
  return (*this) - normal.toUnitVector() * (*this * normal.toUnitVector()) * 2;
}

Vector3D Vector3D::toUnitVector() const {
  const float l = this->length();

  return Vector3D(m_x / l, m_y / l, m_z / l);
}

float Vector3D::length() const {
  return static_cast<float>(std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z));
}

Vector3D& Vector3D::operator+=(const Vector3D& other) {
  m_x += other.m_x;
  m_y += other.m_y;
  m_z += other.m_z;
  return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& other) {
  m_x -= other.m_x;
  m_y -= other.m_y;
  m_z -= other.m_z;
  return *this;
}

std::string Vector3D::toString() const {
  return "x: " + std::to_string(m_x) + " y: " + std::to_string(m_y) +
         " z: " + std::to_string(m_z);
}

bool Vector3D::areSameDirection(const Vector3D& vec1, const Vector3D& vec2) {
  constexpr float kLimit = 0.05;
  Vector3D projected = vec2 * ((vec1 * vec2) / (vec2 * vec2));
  return std::abs(projected * vec2 / (projected.length() * vec2.length()) - 1) <
         kLimit;
}

Vector3D Vector3D::x(float value) { return Vector3D(value, 0.0F, 0.0F); }
Vector3D Vector3D::y(float value) { return Vector3D(0.0F, value, 0.0F); }
Vector3D Vector3D::z(float value) { return Vector3D(0.0F, 0.0F, value); }