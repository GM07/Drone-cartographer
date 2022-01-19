#ifndef DIRECTIONS_H
#define DIRECTIONS_H

namespace bridge {

enum Direction : uint8_t {
  kLeft = 0x00,
  kRight = 0x01,
  kFront = 0x02,
  kTop = 0x03
};
}

#endif