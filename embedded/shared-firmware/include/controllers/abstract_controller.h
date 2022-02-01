#ifndef ABSTRACT_CONTROLLER_H
#define ABSTRACT_CONTROLLER_H

#include <memory>
#include <string>

#include "../utils/directions.h"
#include "../utils/led.h"
#include "../utils/vector3d.h"

class AbstractController {
 public:
  virtual ~AbstractController() = default;

  virtual Vector3D& getCurrentLocation() = 0;

  virtual void setLEDState(LED led, bool enable,
                           bool blink) = 0;

  virtual void goTo(const Vector3D& location, float yaw, float pitch,
                    bool isRelative) = 0;
  virtual void goTo(const Vector3D& location, bool isRelative) = 0;
  virtual void takeoff(float height) = 0;
  virtual void land() = 0;

  virtual float getDistance(Direction direction) = 0;
  virtual float getBatteryLevel() = 0;

  virtual void sendP2PMessage(void* message) = 0;
  virtual void initCommunicationManager() = 0;
  virtual size_t receiveMessage(void* message, size_t size) = 0;
  virtual void sendMessage(void* message, size_t size) = 0;

  virtual void log(const std::string&& message) = 0;

  virtual void delay(const uint32_t ticks) = 0;

  static std::shared_ptr<AbstractController> getController();
};

#endif
