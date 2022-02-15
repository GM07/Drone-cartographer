#ifndef STUB_CRAZYFLIE_SENSING_H
#define STUB_CRAZYFLIE_SENSING_H

#include <gmock/gmock.h>

#include "crazyflie_sensing.h"

class StubCCrazyflieSensing : public CCrazyflieSensing {
 public:
  virtual ~StubCCrazyflieSensing() = default;
};

#endif
