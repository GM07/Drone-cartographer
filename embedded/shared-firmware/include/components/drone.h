#ifndef DRONE_H
#define DRONE_H

#include <array>
#include <mutex>
#include <queue>
#include <random>

#include "controllers/abstract_controller.h"
#include "utils/commands.h"
#include "utils/droneData.h"

// Meters and seconds
constexpr float kDroneSpeed = 1.0f;
constexpr float kTakeOffSpeed = 1.0f;
constexpr float kLandingSpeed = 0.25f;
constexpr float kHeight = 0.3f;
constexpr int kMaxNbPeerData = 20;
constexpr int kMessageMaxSize = 32;
constexpr float kSimulationCollisionAvoidanceRange = 20.0F;
constexpr size_t kNbStartingDirection = 8;

class Drone {
 public:
  explicit Drone(std::shared_ptr<AbstractController>&& controller)
      : m_messageRX(), m_controller(controller) {
    static constexpr float kFirstNumber = 0.5;
    static constexpr float kSecondNumber = 1.225;

    static std::array<Vector3D, kNbStartingDirection> startingDirection{
        {Vector3D(kSecondNumber, kFirstNumber, 0.0F),
         Vector3D(kFirstNumber, kSecondNumber, 0.0F),
         Vector3D(-kFirstNumber, kSecondNumber, 0.0F),
         Vector3D(-kSecondNumber, kFirstNumber, 0.0F),
         Vector3D(-kSecondNumber, -kFirstNumber, 0.0F),
         Vector3D(-kFirstNumber, -kSecondNumber, 0.0F),
         Vector3D(kFirstNumber, -kSecondNumber, 0.0F),
         Vector3D(kSecondNumber, -kFirstNumber, 0.0F)}};

    static std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(
        0, startingDirection.size() - 1);

    int direction = distribution(generator);
    m_data.direction = startingDirection.at(direction);
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
  /**
   * @brief Handles commands received from CommunicationManager
   *
   * @param command The command
   * @param extraArgs A pointer to an object containing the arguments
   */
  bool handleCommand(Command command, const void* extraArgs,
                     size_t extraArgsLength);

  // Communication Manager
  void communicationManagerTask();

  // Navigation Manager
  void step();
  void wallAvoidance();
  void collisionAvoidance();
  void changeDirection();

  // Sensor Manager
  void updateSensorsData();
  void updateCrashStatus();

  // Initialisation
  void initDrone();

  static Drone& getEmbeddedDrone();

 protected:
  std::array<uint8_t, kMessageMaxSize> m_messageRX;
  std::shared_ptr<AbstractController> m_controller;
  DroneData m_data;
  std::unordered_map<size_t, DroneData> m_usedPeerData;
  Vector3D m_normal;
  std::unordered_map<size_t, DroneData> m_peerData;
};
#endif
