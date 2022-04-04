#ifndef SIMULATION_SENSORS_H
#define SIMULATION_SENSORS_H

#include "crazyflie_sensing.h"
#include "sensors/abstract_sensors.h"

enum ReadingsMapSensorIndex {
  kRight = 0,
  kFront,
  kLeft,
  kBack,
};

class SimulationSensors : public AbstractSensors {
 public:
  explicit SimulationSensors(CCrazyflieSensing* ccrazyflieSensing);
  ~SimulationSensors() override = default;
  SimulationSensors() = delete;
  SimulationSensors& operator=(const SimulationSensors&) = delete;
  SimulationSensors& operator=(SimulationSensors&&) = delete;
  SimulationSensors(const SimulationSensors&) = delete;
  SimulationSensors(const SimulationSensors&&) = delete;

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
