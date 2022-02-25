#ifndef DRONE_H
#define DRONE_H

#include <array>

#include "controllers/abstract_controller.h"
#include "utils/commands.h"

constexpr float kSpeed = 0.1f;
constexpr float kTakeOffSpeed = 1.0f;
constexpr float kLandingSpeed = 0.25f;
constexpr float kHeight = 0.5f;
constexpr int kMessageMaxSize = 32;

class Drone {
 public:
  explicit Drone(std::shared_ptr<AbstractController>&& controller)
      : m_messageRX(), m_controller(controller) {}
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
  void squareTrajectory(float sideLength, bool relative);

  // Sensor Manager

 protected:
  std::array<uint8_t, kMessageMaxSize> m_messageRX;
  std::shared_ptr<AbstractController> m_controller;

  // DEBUG VARIABLES
  Direction explorationDirection = Direction::kFront;

 public:
  static Drone& getEmbeddedDrone();
};

#endif
