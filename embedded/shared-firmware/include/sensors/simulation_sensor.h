#ifndef SIMULATION_SENSOR_H
#define SIMULATION_SENSOR_H

#include "crazyflie_sensing.h"
#include "sensors/abstract_sensor.h"

class SimulationSensor : public AbstractSensor {
 public:
  SimulationSensor(CCrazyflieSensing* ccrazyflieSensing);
  virtual ~SimulationSensor() = default;

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
