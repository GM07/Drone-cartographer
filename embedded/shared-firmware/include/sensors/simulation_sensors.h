#ifndef SIMULATION_SENSORS_H
#define SIMULATION_SENSORS_H

#include "crazyflie_sensing.h"
#include "sensors/abstract_sensors.h"

enum ReadingsMapSensorIndex {
  SimulationReadingsMapSensorRight = 0,
  SimulationReadingsMapSensorFront = 1,
  SimulationReadingsMapSensorLeft = 2,
  SimulationReadingsMapSensorBack = 3,
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