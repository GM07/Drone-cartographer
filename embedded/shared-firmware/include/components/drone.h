#ifndef DRONE_H
#define DRONE_H

#include <utils/math.h>

#include <array>
#include <chrono>
#include <cmath>
#include <deque>
#include <random>
#include <unordered_set>

#include "controllers/abstract_controller.h"
#include "utils/commands.h"
#include "utils/drone_data.h"
#include "utils/potential_shortcut.h"

constexpr size_t kMessageMaxSize = 30;
constexpr size_t kNbStartingDirection = 8;
constexpr size_t kNbLateralSensors = 4;

constexpr float kSimulationCollisionAvoidanceRange = 20.0F;
constexpr float kRealMinCollisionAvoidanceRange = 55.0F;
constexpr float kRealMaxCollisionAvoidanceRange = 55.0F;
constexpr float kDistanceForSafeShortcut = 0.1F;

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

    m_initialDirection = startingDirection.at(distribution(generator));
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
  bool handleCommand(Command command, void* extraArgs = nullptr);

  // Communication Manager
  void communicationManagerTask();

  // Navigation Manager
  void step();
  virtual void wallAvoidance();
  virtual void collisionAvoidance();
  virtual void analyzeShortcuts();
  void changeDirection();
  bool returnToBase();

  std::array<Segment, kNbLateralSensors> createSensorSegments(
      const Vector3D& currentLocation);
  void findPotentialShortCut(
      const Vector3D& currentLocation,
      const std::array<Segment, kNbLateralSensors>& sensorSegments);
  void validatePotentialShortCuts(
      const Vector3D& currentLocation,
      const std::array<Segment, kNbLateralSensors>& sensorSegments);

  // Sensor Manager
  void updateCrashStatus();
  void updateStatusFromBattery();

  // Initialisation
  inline void initDrone() { m_data.m_id = m_controller->getId(); };
  [[nodiscard]] static float getRealSensorDistance(float sensor);

  static Drone& getEmbeddedDrone();
  std::unordered_map<size_t, DroneData> m_peerData;
  DroneData m_data;
  bool m_p2pColorGradientIsActive{false};

 protected:
  std::vector<Vector3D> m_returnPath;
  std::unordered_set<PotentialShortcut> m_potentialShortCuts;

  Vector3D m_normal;
  Vector3D m_initialDirection;
  std::array<uint8_t, kMessageMaxSize> m_messageRX;
  std::shared_ptr<AbstractController> m_controller;
  std::unordered_map<size_t, DroneData> m_usedPeerData;
  bool m_peerCollision{false};
};
#endif
