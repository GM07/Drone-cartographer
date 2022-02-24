#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/timer.h"

/////////////////////////////////////////////////////////
void FirmwareController::blinkLED(LED led) {}
size_t FirmwareController::receiveMessage(void* message, size_t size) {}
void FirmwareController::sendMessage(void* message, size_t size) {}
void Timer::delayMs(uint32_t delayMS) {}
void FirmwareController::goTo(const Vector3D& location, bool isRelative) {}
Vector3D FirmwareController::getCurrentLocation() const {}
bool FirmwareController::isTrajectoryFinished() const {}
void FirmwareController::takeOff(float height) {}
void FirmwareController::land() {}
