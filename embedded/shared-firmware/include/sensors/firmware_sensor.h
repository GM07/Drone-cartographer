#ifndef FIRMWARE_SENSOR_H
#define FIRMWARE_SENSOR_H

#include "sensors/abstract_sensor.h"

class FirmwareSensor : public AbstractSensor {
 public:
  FirmwareSensor() = default;
  virtual ~FirmwareSensor() = default;

  float getFrontDistance() const override;
  float getLeftDistance() const override;
  float getRightDistance() const override;
  float getBackDistance() const override;
  float getPosX() const override;
  float getPosY() const override;
  float getPosZ() const override;
  float getBatteryLevel() const override;
};

#endif
