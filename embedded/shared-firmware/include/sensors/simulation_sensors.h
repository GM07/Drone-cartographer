#ifndef SIMULATION_SENSORS_H
#define SIMULATION_SENSORS_H

#include "crazyflie_sensing.h"
#include "sensors/abstract_sensors.h"

enum ReadingsMapSensorIndex {
  SimulationReadingsMapSensorFront = 0,
  SimulationReadingsMapSensorLeft = 1,
  SimulationReadingsMapSensorBack = 2,
  SimulationReadingsMapSensorRight = 3
};

class SimulationSensors : public AbstractSensors {
 public:
  SimulationSensors(CCrazyflieSensing* ccrazyflieSensing);
  virtual ~SimulationSensors() = default;

  [[nodiscard]] float getFrontDistance() const override;
  [[nodiscard]] float getLeftDistance() const override;
  [[nodiscard]] float getRightDistance() const override;
  [[nodiscard]] float getBackDistance() const override;
  [[nodiscard]] float getPosX() const override;
  [[nodiscard]] float getPosY() const override;
  [[nodiscard]] float getPosZ() const override;
  [[nodiscard]] float getBatteryLevel() const override;

 private:
  CCrazyflieSensing* m_ccrazyflieSensing;
};

#endif
