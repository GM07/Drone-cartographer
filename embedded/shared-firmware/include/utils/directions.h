#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include "vector3d.h"

enum class Direction : uint8_t {
  kLeft = 0x00,
  kRight = 0x01,
  kFront = 0x02,
  kBack = 0x03
};

#endif
