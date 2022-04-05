#ifndef ABSTRACT_CONTROLLER_H
#define ABSTRACT_CONTROLLER_H

#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>

#include "controller_data.h"
#include "sensors/abstract_sensors.h"
#include "utils/directions.h"
#include "utils/drone_data.h"
#include "utils/led.h"
#include "utils/state.h"
#include "utils/vector3d.h"

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
  [[nodiscard]] virtual bool isDroneCrashed() const = 0;

  virtual void initCommunicationManager() = 0;
  virtual size_t receiveMessage(void* message, size_t size) const = 0;
  virtual void sendMessage(void* message, size_t size) const = 0;

  virtual void sendP2PMessage(void* message, size_t size) = 0;
  virtual void receiveP2PMessage(
      std::unordered_map<size_t, DroneData>* p2pData) = 0;

  virtual void log(const std::string& message) = 0;
  [[nodiscard]] virtual size_t getId() const = 0;
  virtual void identify() = 0;

  virtual void updateSensorsData() = 0;
  [[nodiscard]] virtual float getMinCollisionAvoidanceDistance() const = 0;
  [[nodiscard]] virtual float getMaxCollisionAvoidanceDistance() const = 0;

  State m_state{State::kIdle};
  std::unique_ptr<AbstractSensors> m_abstractSensors;
  ControllerData m_data{};

 protected:
  Vector3D m_takeOffPosition;
  Vector3D m_targetPosition;
};

#endif












