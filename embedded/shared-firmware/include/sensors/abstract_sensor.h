#ifndef ABSTRACT_SENSOR_H
#define ABSTRACT_SENSOR_H

class AbstractSensor {
 public:
  AbstractSensor() = default;
  virtual ~AbstractSensor() = default;

  virtual float getFrontDistance() const = 0;
  virtual float getLeftDistance() const = 0;
  virtual float getRightDistance() const = 0;
  virtual float getBackDistance() const = 0;
  virtual float getPosX() const = 0;
  virtual float getPosY() const = 0;
  virtual float getPosZ() const = 0;
  virtual float getBatteryLevel() const = 0;
};

#endif
