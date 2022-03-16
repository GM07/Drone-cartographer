#include <chrono>
#include <thread>

#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/timer.h"

/////////////////////////////////////////////////////////
FirmwareController::FirmwareController() {}
void FirmwareController::blinkLED(LED led) {}
size_t FirmwareController::receiveMessage(void* message, size_t size) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
void FirmwareController::sendMessage(void* message, size_t size) {}
void Timer::delayMs(uint32_t delayMS) {}
Vector3D FirmwareController::getCurrentLocation() const {}
bool FirmwareController::isTrajectoryFinished() const {}
void FirmwareController::takeOff(float height) {}
void FirmwareController::land() {}
void FirmwareController::updateSensorsData() {}
bool FirmwareController::isDroneCrashed() const { return false; }
void FirmwareController::setVelocity(const Vector3D& direction, float speed) {}

void FirmwareController::sendP2PMessage(void* message, size_t size){/**/};
void FirmwareController::receiveP2PMessage(
    std::unordered_map<size_t, DroneData>* p2pData){/**/};
size_t FirmwareController::getId() { return 0; };
