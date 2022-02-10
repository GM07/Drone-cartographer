#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/timer.h"

/////////////////////////////////////////////////////////
Drone& Drone::getEmbeddedDrone() {
  static Drone drone(std::make_shared<FirmwareController>());
  return drone;
}

/////////////////////////////////////////////////////////
void FirmwareController::setLEDState(LED led, bool enable, bool blink) {}
size_t FirmwareController::receiveMessage(void* message, size_t size) {}
void FirmwareController::sendMessage(void* message, size_t size) {}
void Timer::delayMs(uint32_t delayMS) {}
