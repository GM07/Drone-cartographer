#ifndef SIMULATION_SENSOR_H
#define SIMULATION_SENSOR_H

#include "crazyflie_sensing.h"
#include "sensors/abstract_sensor.h"

class SimulationSensor : public AbstractSensor {
 public:
  SimulationSensor(CCrazyflieSensing* ccrazyflieSensing);
  virtual ~SimulationSensor() = default;

  float getFrontDistance() const override;
  float getLeftDistance() const override;
  float getRightDistance() const override;
  float getBackDistance() const override;
  float getPosX() const override;
  float getPosY() const override;
  float getPosZ() const override;
  float getBatteryLevel() const override;

 private:
  CCrazyflieSensing* m_ccrazyflieSensing;
};

#endif
