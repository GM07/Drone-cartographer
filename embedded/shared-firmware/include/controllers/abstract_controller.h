#ifndef ABSTRACT_CONTROLLER_H
#define ABSTRACT_CONTROLLER_H

#include <memory>
#include <string>

#include "sensors/abstract_sensors.h"
#include "utils/directions.h"
#include "utils/led.h"
#include "utils/state.h"
#include "utils/vector3d.h"

// packs the bytes
struct __attribute__((__packed__)) ControllerData {
 public:
  float front;
  float left;
  float back;
  float right;
  float posX;
  float posY;
  float batteryLevel;
  int state;
};

class AbstractController {
 public:
  virtual ~AbstractController() = default;
  AbstractController() = default;
  AbstractController(std::unique_ptr<AbstractSensors>&& abstractSensors);
  AbstractController(AbstractController&& other) = delete;
  AbstractController& operator=(AbstractController&& other) = delete;
  AbstractController(const AbstractController& other) = delete;
  AbstractController& operator=(const AbstractController& other) = delete;

  virtual void goTo(const Vector3D& location, bool isRelative) = 0;
  virtual void takeOff(float height) = 0;
  virtual void land() = 0;

  virtual Vector3D getCurrentLocation() const = 0;
  virtual bool isTrajectoryFinished() const = 0;

  virtual void initCommunicationManager() = 0;
  virtual size_t receiveMessage(void* message, size_t size) = 0;
  virtual void sendMessage(void* message, size_t size) = 0;
  virtual void sendP2PMessage(void* message) = 0;

  virtual void log(const std::string& message) = 0;
  virtual void blinkLED(LED led) = 0;

  virtual void delay(uint32_t ticks) = 0;
  virtual void updateSensorsData() = 0;
  virtual bool isDroneCrashed() const = 0;

  State state = State::kIdle;
  std::unique_ptr<AbstractSensors> m_abstractSensors;
  ControllerData data{};

 protected:
  Vector3D m_takeOffPosition;
  Vector3D m_targetPosition;
};

#endif
