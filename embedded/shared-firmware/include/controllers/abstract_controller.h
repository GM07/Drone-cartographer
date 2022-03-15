#ifndef ABSTRACT_CONTROLLER_H
#define ABSTRACT_CONTROLLER_H

#include <cstring>
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

  [[nodiscard]] bool operator==(const ControllerData& other) const {
    return (front == other.front && left == other.left && back == other.back &&
            right == other.right && posX == other.posX && posY == other.posY &&
            batteryLevel == other.batteryLevel && state == other.state);
  }
  [[nodiscard]] bool operator!=(const ControllerData& other) const {
    return !(*this == other);
  }
};

class AbstractController {
 public:
  virtual ~AbstractController() = default;
  AbstractController() = default;
  AbstractController(AbstractController&& other) = delete;
  AbstractController& operator=(AbstractController&& other) = delete;
  AbstractController(const AbstractController& other) = delete;
  AbstractController& operator=(const AbstractController& other) = delete;

  explicit AbstractController(
      std::unique_ptr<AbstractSensors>&& abstractSensors)
      : m_abstractSensors(std::move(abstractSensors)){};

  virtual void setVelocity(const Vector3D& direction, float speed) = 0;
  virtual void takeOff(float height) = 0;
  virtual void land() = 0;

  [[nodiscard]] virtual Vector3D getCurrentLocation() const = 0;
  [[nodiscard]] virtual bool isTrajectoryFinished() const = 0;

  virtual void initCommunicationManager() = 0;
  virtual size_t receiveMessage(void* message, size_t size) = 0;
  virtual void sendMessage(void* message, size_t size) = 0;

  virtual void log(const std::string& message) = 0;
  virtual void blinkLED(LED led) = 0;

  virtual void updateSensorsData() = 0;
  [[nodiscard]] virtual bool isDroneCrashed() const = 0;

  State state = State::kIdle;
  std::unique_ptr<AbstractSensors> m_abstractSensors;
  ControllerData data{};

 protected:
  Vector3D m_takeOffPosition;
  Vector3D m_targetPosition;
};

#endif
