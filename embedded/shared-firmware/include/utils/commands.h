#ifndef COMMANDS_H
#define COMMANDS_H

enum class Command : uint8_t {
  kIdentify = 0x01,
  kTakeoff = 0x02,
  kLand = 0x03,
};

#endif