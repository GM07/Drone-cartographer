#ifndef FIRMWARE_CONTROLLER_H
#define FIRMWARE_CONTROLLER_H

#include "controllers/abstract_controller.h"

class FirmwareController : public virtual AbstractController {
 public:
  FirmwareController() = default;
  virtual ~FirmwareController() = default;

  FirmwareController(FirmwareController&& other) = delete;
  FirmwareController operator=(FirmwareController&& other) = delete;
  FirmwareController(FirmwareController& other) = delete;
  FirmwareController operator=(FirmwareController& other) = delete;

  Vector3D& getCurrentLocation() override{/**/};

  void setLEDState(LED led, bool enable, bool blink) override;

  void goTo(const Vector3D& location, float yaw, float pitch,
            bool isRelative) override{/**/};
  void goTo(const Vector3D& location, bool isRelative) override{/**/};
  void takeOff(float height) override{/**/};
  void land() override{/**/};

  float getDistance(Direction direction) override{/**/};
  float getBatteryLevel() override{/**/};

  void sendP2PMessage(void* message) override{/**/};
  void initCommunicationManager() override{/**/};
  size_t receiveMessage(void* message, size_t size) override;
  void sendMessage(void* message, size_t size) override;

  void log(const std::string& message) override{/**/};

  void delay(const uint32_t ticks) override{/**/};
};
#endif
