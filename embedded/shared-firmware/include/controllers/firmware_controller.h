#ifndef FIRMWARE_CONTROLLER_H
#define FIRMWARE_CONTROLLER_H

#include <queue>

#include "controllers/abstract_controller.h"

extern "C" {
#include "ledseq.h"
#include "radiolink.h"
}

extern std::queue<P2PPacket> receivedP2PPacket;

class FirmwareController : public AbstractController {
 public:
  FirmwareController();
  ~FirmwareController() override = default;

  FirmwareController(FirmwareController&& other) = delete;
  FirmwareController operator=(FirmwareController&& other) = delete;
  FirmwareController(const FirmwareController& other) = delete;
  FirmwareController operator=(const FirmwareController& other) = delete;

  void setVelocity(const Vector3D& direction, float speed) override;
  void takeOff(float height) override;
  void land() override;

  [[nodiscard]] Vector3D getCurrentLocation() const override;
  [[nodiscard]] bool isTrajectoryFinished() const override;

  void initCommunicationManager() override{/**/};
  size_t receiveMessage(void* message, size_t size) override;
  void sendMessage(void* message, size_t size) override;

  void sendP2PMessage(void* message, size_t size) override;
  void receiveP2PMessage(
      std::unordered_map<size_t, DroneData>& p2pData) override;

  void log(const std::string& message) override{/**/};
  void blinkLED(LED led) override;
  size_t getId() override;

  void updateSensorsData() override;
  [[nodiscard]] bool isDroneCrashed() const override;

 private:
  ledseqContext_t m_seqLED{};
};
#endif
