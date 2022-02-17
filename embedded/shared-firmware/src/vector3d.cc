#include "utils/vector3d.h"

#include <cmath>

Vector3D::Vector3D() : m_x(0), m_y(0), m_z(0) {}

Vector3D::Vector3D(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

Vector3D Vector3D::operator+(const Vector3D& other) {
  return Vector3D(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
}

Vector3D Vector3D::operator-(const Vector3D& other) {
  return Vector3D(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
}

Vector3D Vector3D::operator*(const float other) {
  return Vector3D(m_x * other, m_y * other, m_z * other);
}

Vector3D Vector3D::operator/(const float other) {
  return Vector3D(m_x / other, m_y / other, m_z / other);
}

bool Vector3D::operator==(const Vector3D& other) const {
  return m_x == other.m_x && m_y == other.m_y && m_z == other.m_z;
}

bool Vector3D::operator!=(const Vector3D& other) const {
  return !(*this == other);
}

bool Vector3D::isAlmostEqual(const Vector3D& other) const {
  return distanceTo(other) <= ALMOST_EQUAL_THRESHOLD;
}

float Vector3D::distanceTo(const Vector3D& other) const {
  return std::sqrt(std::pow(m_x - other.m_x, 2) + std::pow(m_y - other.m_y, 2) +
                   std::pow(m_z - other.m_z, 2));
}

Vector3D& Vector3D::operator+=(const Vector3D& other) {
  m_x += other.m_x;
  m_y += other.m_y;
  m_z += other.m_z;
  return *this;
}
