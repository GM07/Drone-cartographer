#ifndef STATE_H
#define STATE_H

enum State : uint8_t {
  kIdle = 0x00,
  kTakingOff = 0x01,
  kLanding = 0x02,
  kDead = 0x03,
  kExploring = 0x04,
  kReturningToBase = 0x05,
  kCrash = 0x06,
};

#endif
