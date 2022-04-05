#include "sensors/firmware_sensors.h"

extern "C" {
#include "log.h"
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getFrontDistance() const {
  logVarId_t idFront = logGetVarId("range", "front");
  return static_cast<float>(logGetUint(idFront));
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getLeftDistance() const {
  logVarId_t idLeft = logGetVarId("range", "left");
  return static_cast<float>(logGetUint(idLeft));
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getRightDistance() const {
  logVarId_t idRight = logGetVarId("range", "right");
  return static_cast<float>(logGetUint(idRight));
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getBackDistance() const {
  logVarId_t idBack = logGetVarId("range", "back");
  return static_cast<float>(logGetUint(idBack));
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getPosX() const {
  logVarId_t logIdStateEstimateX = logGetVarId("stateEstimate", "x");
  return logGetFloat(logIdStateEstimateX);
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getPosY() const {
  logVarId_t logIdStateEstimateY = logGetVarId("stateEstimate", "y");
  return logGetFloat(logIdStateEstimateY);
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getPosZ() const {
  logVarId_t logIdStateEstimateZ = logGetVarId("stateEstimate", "z");
  return logGetFloat(logIdStateEstimateZ);
}

///////////////////////////////////////
[[nodiscard]] float FirmwareSensors::getBatteryLevel() const {
  logVarId_t vbatid = logGetVarId("pm", "vbat");
  return logGetFloat(vbatid);
}












