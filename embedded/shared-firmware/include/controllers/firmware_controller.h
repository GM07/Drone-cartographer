#ifndef FIRMWARE_CONTROLLER_H
#define FIRMWARE_CONTROLLER_H

#include <queue>

#include "controllers/abstract_controller.h"

extern "C" {
#include "FreeRTOS.h"
#include "ledseq.h"
#include "radiolink.h"
#include "semphr.h"
}

// inline std::unordered<P2PPacket> receivedP2PPacket;
inline SemaphoreHandle_t p2pPacketMutex;

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

  size_t receiveMessage(void* message, size_t size) const override;
  void sendMessage(void* message, size_t size) const override;

  void sendP2PMessage(void* message, size_t size) override;
  void receiveP2PMessage(
      std::unordered_map<size_t, DroneData>* p2pData) override{};

  void identify() override;
  [[nodiscard]] size_t getId() const override;

  void updateSensorsData() override;
  [[nodiscard]] float getMinCollisionAvoidanceDistance() const override;
  [[nodiscard]] float getMaxCollisionAvoidanceDistance() const override;
  [[nodiscard]] bool isDroneCrashed() const override;

  void initCommunicationManager() override{/**/};
  void log(const std::string& message) override{/**/};

 private:
  ledseqContext_t m_seqLED{};
};
#endif
