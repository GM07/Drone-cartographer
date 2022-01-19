#ifndef ABSTRACT_CONTROLLER_H
#define ABSTRACT_CONTROLLER_H

#include <string>

#include "directions.h"
#include "vector3d.h"

namespace bridge {

class AbstractController {
 public:
  virtual ~AbstractController() = default;

  virtual Vector3D& getCurrentLocation() = 0;

  virtual void setLEDState(bool isOn, uint32_t color) = 0;

  virtual void goTo(const Vector3D& location, float yaw, float pitch,
                    bool isRelative) = 0;
  virtual void goTo(const Vector3D& location, bool isRelative) = 0;
  virtual void takeoff(float height) = 0;
  virtual void land() = 0;

  virtual float getDistance(Direction direction) = 0;
  virtual float getBatteryLevel() = 0;

  virtual void sendRadioMessage(void* message) = 0;
  virtual void sendP2PMessage(void* message) = 0;
  virtual size_t receiveMessage(void* buffer) = 0;

  virtual void log(const std::string&& message) = 0;

  virtual void delay(const uint32_t ticks) = 0;
};

}  // namespace bridge

#endif
