#ifndef FIRMWARE_SENSOR_H
#define FIRMWARE_SENSOR_H

#include "sensors/abstract_sensor.h"

class FirmwareSensor : public AbstractSensor {
 public:
  FirmwareSensor() = default;
  virtual ~FirmwareSensor() = default;

  [[nodiscard]] float getFrontDistance() const override;
  [[nodiscard]] float getLeftDistance() const override;
  [[nodiscard]] float getRightDistance() const override;
  [[nodiscard]] float getBackDistance() const override;
  [[nodiscard]] float getPosX() const override;
  [[nodiscard]] float getPosY() const override;
  [[nodiscard]] float getPosZ() const override;
  [[nodiscard]] float getBatteryLevel() const override;
};

#endif
