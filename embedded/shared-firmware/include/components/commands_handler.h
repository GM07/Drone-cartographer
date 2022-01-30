#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include <memory>

enum Command : uint8_t {
  kIdentify = 0x01,
  kCommand2 = 0x02,
  kCommand3 = 0x03
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

 private:
  static std::shared_ptr<CommandsHandler> instance;
  CommandsHandler(){};
};

#endif
