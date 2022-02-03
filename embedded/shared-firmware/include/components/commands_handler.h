#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <chrono>
#include <memory>

enum class Command : uint8_t {
  kIdentify = 0x01,
  kTakeoff = 0x02,
  kLand = 0x03
};  // Example commands

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
                     const size_t extraArgsLength);

  void tick();

 private:
  static std::shared_ptr<CommandsHandler> instance;
  CommandsHandler(){};

  std::chrono::time_point<std::chrono::steady_clock> identifyCommandBegin;
};

#endif
