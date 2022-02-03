#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <chrono>
#include <memory>

#include "utils/commands.h"

class CommandsHandler {
 public:
  static std::shared_ptr<CommandsHandler> getCommandsHandler();

  /**
   * @brief Handles commands received from CommunicationManager
   *
   * @param command The command
   * @param extraArgs A pointer to an object containing the arguments
   */
  bool handleCommand(Command command, const void* extraArgs,
                     const size_t extraArgsLength, std::string id = "");

  void tick(std::string id = "");

 private:
  static std::shared_ptr<CommandsHandler> instance;
  CommandsHandler(){};

  std::chrono::time_point<std::chrono::steady_clock> identifyCommandBegin;
};

#endif
