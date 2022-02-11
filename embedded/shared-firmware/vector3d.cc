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

Vector3D Vector3D::operator=(const Vector3D& other) { return *this; }

float Vector3D::distanceTo(const Vector3D& other) const {
  return std::sqrt(std::pow(std::abs(m_x - other.m_x), 2.0f) +
                   std::pow(std::abs(m_y - other.m_y), 2.0f) +
                   std::pow(std::abs(m_z - other.m_z), 2.0f));
}
