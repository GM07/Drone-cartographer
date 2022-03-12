#include "sensors/firmware_sensors.h"

extern "C" {
#include "log.h"
}

float FirmwareSensors::getFrontDistance() const {
  logVarId_t idFront = logGetVarId("range", "front");
  return static_cast<float>(logGetUint(idFront));
}

float FirmwareSensors::getLeftDistance() const {
  logVarId_t idLeft = logGetVarId("range", "left");
  return static_cast<float>(logGetUint(idLeft));
}

float FirmwareSensors::getRightDistance() const {
  logVarId_t idRight = logGetVarId("range", "right");
  return static_cast<float>(logGetUint(idRight));
}

float FirmwareSensors::getBackDistance() const {
  logVarId_t idBack = logGetVarId("range", "back");
  return static_cast<float>(logGetUint(idBack));
}

float FirmwareSensors::getPosX() const {
  logVarId_t logIdStateEstimateX = logGetVarId("stateEstimate", "x");
  return logGetFloat(logIdStateEstimateX);
}

float FirmwareSensors::getPosY() const {
  logVarId_t logIdStateEstimateY = logGetVarId("stateEstimate", "y");
  return logGetFloat(logIdStateEstimateY);
}

float FirmwareSensors::getPosZ() const {
  logVarId_t logIdStateEstimateZ = logGetVarId("stateEstimate", "z");
  return logGetFloat(logIdStateEstimateZ);
}

float FirmwareSensors::getBatteryLevel() const {
  logVarId_t vbatid = logGetVarId("pm", "vbat");
  return logGetFloat(vbatid);
}
