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
#include "utils/math.h"
#include "utils/state.h"
#include "utils/vector3d.h"

constexpr float kRealTrajectoryFinishedTreshold = 0.05;

// Meters and seconds
constexpr float kSpeed = 0.25F;
constexpr float kHeight = 0.3F;

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

  inline void takeOff(float height) {
    m_takeOffPosition += getCurrentLocation();
    m_targetPosition = Vector3D::z(height);
    m_state = State::kTakingOff;
    setVelocity(Vector3D::z(1.0F), kSpeed);
  };

  inline void land() {
    m_targetPosition = getCurrentLocation();
    m_targetPosition.m_z = 0;
    m_state = State::kLanding;
    setVelocity(Vector3D::z(-1.0F), kSpeed);
  };

  virtual void stopMotors() = 0;

  [[nodiscard]] virtual Vector3D getCurrentLocation() const = 0;
  [[nodiscard]] virtual bool isTrajectoryFinished() const = 0;

  [[nodiscard]] inline bool isAltitudeReached() const {
    return Math::areAlmostEqual(getCurrentLocation().m_z, m_targetPosition.m_z,
                                kRealTrajectoryFinishedTreshold);
  };

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
  Vector3D m_targetPosition;

 protected:
  Vector3D m_takeOffPosition;
};

#endif
