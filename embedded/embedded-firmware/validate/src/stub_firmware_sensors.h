#ifndef STUB_FIRMWARE_SENSORS_H
#define STUB_FIRMWARE_SENSORS_H

#include <gmock/gmock.h>

#include "sensors/firmware_sensors.h"

class StubFirmwareSensors : public AbstractSensors {
 public:
  StubFirmwareSensors(){};
  StubFirmwareSensors(const StubFirmwareSensors&) = delete;
  StubFirmwareSensors& operator=(const StubFirmwareSensors&) = delete;
  virtual ~StubFirmwareSensors() = default;

  MOCK_CONST_METHOD0(getFrontDistance, float());
  MOCK_CONST_METHOD0(getLeftDistance, float());
  MOCK_CONST_METHOD0(getRightDistance, float());
  MOCK_CONST_METHOD0(getBackDistance, float());
  MOCK_CONST_METHOD0(getPosX, float());
  MOCK_CONST_METHOD0(getPosY, float());
  MOCK_CONST_METHOD0(getPosZ, float());
  MOCK_CONST_METHOD0(getBatteryLevel, float());
};

#endif