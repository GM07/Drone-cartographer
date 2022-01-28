#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

class CommunicationManager {
 public:
  /**
   * @brief Store the received message in the buffer
   *
   * @param buffer The buffer to store the message
   * @return whether or not the buffer is valid
   */
  bool getMessage(void* buffer);

  /**
   * @brief Sends a message to the crazyRadio PA
   *
   * @param message A pointer to the message to send. Should be null terminated
   * The max size of the message is 60 bytes
   */
  void sendMessage(void* message);

  /**
   * @brief Broadcasts a message to all the crazyflie drones
   *
   * @param message The message to broadcast. Should be null terminated
   * The max size of the message is 60 bytes
   */
  void sendP2PMessage(void* message);
};

#endif
