#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

enum Command {
  kCommand1 = 0x00,
  kCommand2 = 0x01,
  kCommand3 = 0x03
};  // Example commands

class CommandsHandler {
 public:
  /**
   * @brief Handles commands received from CommunicationManager
   *
   * @param command The command
   * @param extraArgs A pointer to an object containing the arguments
   */
  void handleCommand(Command command, const void* extraArgs);
};

#endif
