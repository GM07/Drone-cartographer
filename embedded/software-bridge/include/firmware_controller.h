#ifndef FIRMWARE_CONTROLLER_H
#define FIRMWARE_CONTROLLER_H

#include "abstractController.h"

namespace bridge {

class FirmwareController : public AbstractController {
 public:
  FirmwareController(){/**/};

  // No.
  FirmwareController(FirmwareController&& other) = delete;
  FirmwareController operator=(FirmwareController&& other) = delete;
  FirmwareController(FirmwareController& other) = delete;
  FirmwareController operator=(FirmwareController& other) = delete;

  Vector3D& getCurrentLocation() override{/**/};

  void setLEDState(bool isOn, uint32_t color) override{/**/};

  void goTo(const Vector3D& location, float yaw, float pitch,
            bool isRelative) override{/**/};
  void goTo(const Vector3D& location, bool isRelative) override{/**/};
  void takeoff(float height) override{/**/};
  void land() override{/**/};

  float getDistance(Direction direction) override{/**/};
  float getBatteryLevel() override{/**/};

  void sendRadioMessage(void* message) override{/**/};
  void sendP2PMessage(void* message) override{/**/};
  size_t receiveMessage(void* buffer) override{/**/};

  void log(const std::string&& message) override{/**/};

  void delay(const uint32_t ticks) override{/**/};
};

}  // namespace bridge

#endif
