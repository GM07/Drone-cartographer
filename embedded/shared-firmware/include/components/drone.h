#ifndef DRONE_H
#define DRONE_H

#include <array>
#include <random>

#include "controllers/abstract_controller.h"
#include "utils/commands.h"
#include "utils/drone_data.h"

constexpr size_t kMessageMaxSize = 32;
constexpr size_t kNbStartingDirection = 8;

// Meters and seconds
constexpr float kDroneSpeed = 0.25F;
constexpr float kTakeOffSpeed = 1.0F;
constexpr float kLandingSpeed = 0.25F;
constexpr float kHeight = 0.3F;

constexpr float kSimulationCollisionAvoidanceRange = 20.0F;
constexpr float kRealMinCollisionAvoidanceRange = 45.0F;
constexpr float kRealMaxCollisionAvoidanceRange = 45.0F;

extern uint8_t droneState;

class Drone {
 public:
  explicit Drone(std::shared_ptr<AbstractController>&& controller)
      : m_messageRX(), m_controller(controller) {
    constexpr float kFirstNumber = 0.5F;
    constexpr float kSecondNumber = 1.225F;

    std::default_random_engine generator;
    std::array<Vector3D, kNbStartingDirection> startingDirection{
        {Vector3D(kSecondNumber, kFirstNumber, 0.0F),
         Vector3D(kFirstNumber, kSecondNumber, 0.0F),
         Vector3D(-kFirstNumber, kSecondNumber, 0.0F),
         Vector3D(-kSecondNumber, kFirstNumber, 0.0F),
         Vector3D(-kSecondNumber, -kFirstNumber, 0.0F),
         Vector3D(-kFirstNumber, -kSecondNumber, 0.0F),
         Vector3D(kFirstNumber, -kSecondNumber, 0.0F),
         Vector3D(kSecondNumber, -kFirstNumber, 0.0F)}};

    std::uniform_int_distribution<int> distribution(
        0, startingDirection.size() - 1);

    m_data.m_direction = startingDirection.at(distribution(generator));
  }

  Drone(const Drone& other) = delete;
  Drone(Drone&& other) = delete;
  Drone& operator=(const Drone& other) = delete;
  Drone& operator=(Drone&& other) = delete;

  virtual ~Drone() = default;

  [[nodiscard]] inline std::shared_ptr<AbstractController> getController()
      const {
    return m_controller;
  };

  // Command Manager
  bool handleCommand(Command command);

  // Communication Manager
  void communicationManagerTask();

  // Navigation Manager
  void step();
  void wallAvoidance();
  void collisionAvoidance();
  void changeDirection();

  void updateCrashStatus();

  // Initialisation
  inline void initDrone() { m_data.m_id = m_controller->getId(); };

  static Drone& getEmbeddedDrone();

  DroneData m_data;

 protected:
  Vector3D m_normal;
  std::array<uint8_t, kMessageMaxSize> m_messageRX;
  std::shared_ptr<AbstractController> m_controller;
  std::unordered_map<size_t, DroneData> m_peerData;
  std::unordered_map<size_t, DroneData> m_usedPeerData;
};
#endif
