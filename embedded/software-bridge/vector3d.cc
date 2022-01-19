#include "include/vector3d.h"

Vector3D::Vector3D() : m_x(0), m_y(0), m_z(0) {}

Vector3D::Vector3D(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

Vector3D Vector3D::operator+(const Vector3D& other) {
  return Vector3D(m_x + other.getX(), m_y + other.getY(), m_z + other.getZ());
}

Vector3D Vector3D::operator-(const Vector3D& other) {
  return Vector3D(m_x - other.getX(), m_y - other.getY(), m_z - other.getZ());
}

Vector3D Vector3D::operator*(const float other) {
  return Vector3D(m_x * other, m_y * other, m_z * other);
}

Vector3D Vector3D::operator/(const float other) {
  return Vector3D(m_x / other, m_y / other, m_z / other);
}

Vector3D Vector3D::operator=(const Vector3D& other) { return *this; }

float Vector3D::getX() const { return m_x; }
float Vector3D::getY() const { return m_y; }
float Vector3D::getZ() const { return m_z; }
