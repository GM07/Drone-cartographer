#ifndef STUB_SIMULATION_SENSORS_H
#define STUB_SIMULATION_SENSORS_H

#include <gmock/gmock.h>

#include "sensors/simulation_sensors.h"

class StubSimulationSensors : public AbstractSensors {
 public:
  StubSimulationSensors() = default;
  StubSimulationSensors(const StubSimulationSensors&) = delete;
  StubSimulationSensors& operator=(const StubSimulationSensors&) = delete;
  virtual ~StubSimulationSensors() = default;

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