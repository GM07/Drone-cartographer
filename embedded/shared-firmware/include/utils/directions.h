#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include "vector3d.h"

enum class Direction : uint8_t {
  kLeft = 0x00,
  kRight = 0x01,
  kFront = 0x02,
  kBack = 0x03
};

constexpr size_t kNbStartingDirection = 8;
static std::array<Vector3D, kNbStartingDirection> startingDirection{
    {Vector3D(1.225F, 0.5F, 0.0F), Vector3D(0.5F, 1.225F, 0.0F),
     Vector3D(-0.5F, 1.225F, 0.0F), Vector3D(-1.225F, 0.5F, 0.0F),
     Vector3D(-1.225F, -0.5F, 0.0F), Vector3D(-0.5F, -1.225F, 0.0F),
     Vector3D(0.5F, -1.225F, 0.0F), Vector3D(1.225F, -0.5F, 0.0F)}};

#endif
