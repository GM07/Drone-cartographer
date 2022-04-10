#ifndef COMMANDS_H
#define COMMANDS_H

enum class Command : uint8_t {
  kIdentify = 0x01,
  kTakeOff,
  kLand,
  kStartP2PGradient,
  kEndP2PGradient,
};

#endif