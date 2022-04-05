#ifndef FIRMWARE_SENSORS_H
#define FIRMWARE_SENSORS_H

#include "sensors/abstract_sensors.h"

class FirmwareSensors : public AbstractSensors {
 public:
  FirmwareSensors() = default;
  ~FirmwareSensors() override = default;

  FirmwareSensors(const FirmwareSensors&) = delete;
  FirmwareSensors(FirmwareSensors&&) = delete;
  FirmwareSensors& operator=(const FirmwareSensors&);
  FirmwareSensors& operator=(FirmwareSensors&&) noexcept;

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












