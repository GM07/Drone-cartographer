#ifndef ABSTRACT_SENSORS_H
#define ABSTRACT_SENSORS_H

class AbstractSensors {
 public:
  AbstractSensors() = default;
  AbstractSensors(AbstractSensors&&) = delete;
  AbstractSensors(const AbstractSensors&) = delete;
  AbstractSensors& operator=(const AbstractSensors&) = delete;
  AbstractSensors& operator=(AbstractSensors&&) = delete;

  virtual ~AbstractSensors() = default;

  [[nodiscard]] virtual float getFrontDistance() const = 0;
  [[nodiscard]] virtual float getLeftDistance() const = 0;
  [[nodiscard]] virtual float getRightDistance() const = 0;
  [[nodiscard]] virtual float getBackDistance() const = 0;
  [[nodiscard]] virtual float getPosX() const = 0;
  [[nodiscard]] virtual float getPosY() const = 0;
  [[nodiscard]] virtual float getPosZ() const = 0;
  [[nodiscard]] virtual float getBatteryLevel() const = 0;
};

#endif












