#ifndef DRONE_H
#define DRONE_H

#include <utils/math.h>

#include <array>
#include <chrono>
#include <cmath>
#include <random>

#include "controllers/abstract_controller.h"
#include "utils/commands.h"
#include "utils/drone_data.h"

constexpr size_t kMessageMaxSize = 30;
constexpr size_t kNbStartingDirection = 8;

// Meters and seconds
constexpr float kDroneSpeed = 0.25F;
constexpr float kTakeOffSpeed = 1.0F;
constexpr float kLandingSpeed = 0.25F;
constexpr float kHeight = 0.3F;

constexpr float kSimulationCollisionAvoidanceRange = 20.0F;
constexpr float kRealMinCollisionAvoidanceRange = 42.0F;
constexpr float kRealMaxCollisionAvoidanceRange = 45.0F;

class Drone {
 public:
  explicit Drone(std::shared_ptr<AbstractController>&& controller)
      : m_messageRX(), m_controller(controller) {
    constexpr float kTrigoHalf = Math::sin(Math::pi<float> / 6.0F);
    constexpr float kTrigoSqrt3On2 = Math::cos(Math::pi<float> / 6.0F);
    const uint32_t kSeed =
        std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(kSeed);
    std::array<Vector3D, kNbStartingDirection> startingDirection{
        {Vector3D(kTrigoSqrt3On2, kTrigoHalf, 0.0F),     // PI / 3
         Vector3D(kTrigoHalf, kTrigoSqrt3On2, 0.0F),     // PI / 6
         Vector3D(-kTrigoHalf, kTrigoSqrt3On2, 0.0F),    // 2 * PI / 3
         Vector3D(-kTrigoSqrt3On2, kTrigoHalf, 0.0F),    // 5 * PI / 6
         Vector3D(-kTrigoSqrt3On2, -kTrigoHalf, 0.0F),   // 7 * PI / 6
         Vector3D(-kTrigoHalf, -kTrigoSqrt3On2, 0.0F),   // 4 * PI / 3
         Vector3D(kTrigoHalf, -kTrigoSqrt3On2, 0.0F),    // 5 * PI / 3
         Vector3D(kTrigoSqrt3On2, -kTrigoHalf, 0.0F)}};  // 11 * PI / 6

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
  std::unordered_map<size_t, DroneData> m_peerData;
  DroneData m_data;
  bool m_p2pColorGradientIsActive = false;

 protected:
  Vector3D m_normal;
  std::array<uint8_t, kMessageMaxSize> m_messageRX;
  std::shared_ptr<AbstractController> m_controller;
  std::unordered_map<size_t, DroneData> m_usedPeerData;
};
#endif
