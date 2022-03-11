#ifndef FIRMWARE_CONTROLLER_H
#define FIRMWARE_CONTROLLER_H

#include "controllers/abstract_controller.h"

extern "C" {
#include "ledseq.h"
}

class FirmwareController : public AbstractController {
 public:
  FirmwareController();
  virtual ~FirmwareController() = default;

  FirmwareController(FirmwareController&& other) = delete;
  FirmwareController operator=(FirmwareController&& other) = delete;
  FirmwareController(FirmwareController& other) = delete;
  FirmwareController operator=(FirmwareController& other) = delete;

  void goTo(const Vector3D& location, bool isRelative) override;
  void setVelocity(const Vector3D& direction, float speed) override;
  void takeOff(float height) override;
  void land() override;

  Vector3D getCurrentLocation() const override;
  bool isTrajectoryFinished() const override;

  void initCommunicationManager() override{/**/};
  size_t receiveMessage(void* message, size_t size) override;
  void sendMessage(void* message, size_t size) override;
  void sendP2PMessage(void* message) override{/**/};

  void log(const std::string& message) override{/**/};
  void blinkLED(LED led) override;

  void delay(const uint32_t ticks) override{/**/};
  void updateSensorsData() override;
  bool isDroneCrashed() const override;

 private:
  ledseqContext_t m_seqLED{};
};
#endif
