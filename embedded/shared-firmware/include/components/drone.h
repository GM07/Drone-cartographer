#ifndef DRONE_H
#define DRONE_H

#include <array>

#include "controllers/abstract_controller.h"
#include "utils/commands.h"

#define MESSAGE_MAX_SIZE 60

class Drone {
 public:
  Drone(std::shared_ptr<AbstractController> controller);

  std::shared_ptr<AbstractController> getController();

  // Command Manager
  /**
   * @brief Handles commands received from CommunicationManager
   *
   * @param command The command
   * @param extraArgs A pointer to an object containing the arguments
   */
  bool handleCommand(Command command, const void* extraArgs,
                     const size_t extraArgsLength);

  // Communication Manager
  void communicationManagerTask();

  // Navigation Manager
  void step();

  // Sensor Manager

 private:
  std::shared_ptr<AbstractController> m_controller;
  std::array<uint8_t, MESSAGE_MAX_SIZE> m_messageRX;
};

#endif