#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D {
 public:
  Vector3D();
  Vector3D(float x, float y, float z);
  virtual ~Vector3D() = default;

  Vector3D operator+(const Vector3D& other);
  Vector3D operator-(const Vector3D& other);
  Vector3D operator*(const float factor);
  Vector3D operator/(const float factor);
  Vector3D operator=(const Vector3D& other);
  bool operator==(const Vector3D& other) const;

  float getX() const;
  float getY() const;
  float getZ() const;

 private:
  float m_x, m_y, m_z;
};

#endif
