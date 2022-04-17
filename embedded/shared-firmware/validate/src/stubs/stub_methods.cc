#include <chrono>
#include <thread>

#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/time.h"

/////////////////////////////////////////////////////////
FirmwareController::FirmwareController() {}
void FirmwareController::identify() {}
size_t FirmwareController::receiveMessage(void* message, size_t size) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
void FirmwareController::sendMessage(void* message, size_t size) const {}
void Time::delayMs(uint32_t delayMS) {}
Vector3D FirmwareController::getCurrentLocation() const {}
void FirmwareController::updateSensorsData() {}
void FirmwareController::stopMotors() const {}
bool FirmwareController::isDroneCrashed() const { return false; }
void FirmwareController::setVelocity(const Vector3D& direction, float speed,
                                     bool bodyReference) {}

void FirmwareController::sendP2PMessage(void* message, size_t size){/**/};
[[nodiscard]] size_t FirmwareController::getId() const { return 0; };

[[nodiscard]] float FirmwareController::getMinCollisionAvoidanceDistance()
    const {
  return 0.0F;
}

[[nodiscard]] float FirmwareController::getMaxCollisionAvoidanceDistance()
    const {
  return 0.0F;
};
