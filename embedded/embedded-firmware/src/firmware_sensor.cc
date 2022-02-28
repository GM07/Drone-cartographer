#include "sensors/firmware_sensor.h"

extern "C" {
#include "log.h"
}

float FirmwareSensor::getFrontDistance() const {
  logVarId_t idLeft = logGetVarId("range", "front");
  return static_cast<float>(logGetUint(idLeft));
}

float FirmwareSensor::getLeftDistance() const {
  logVarId_t idLeft = logGetVarId("range", "left");
  return static_cast<float>(logGetUint(idLeft));
}

float FirmwareSensor::getRightDistance() const {
  logVarId_t idLeft = logGetVarId("range", "right");
  return static_cast<float>(logGetUint(idLeft));
}

float FirmwareSensor::getBackDistance() const {
  logVarId_t idLeft = logGetVarId("range", "back");
  return static_cast<float>(logGetUint(idLeft));
}

float FirmwareSensor::getPosX() const {
  logVarId_t logIdStateEstimateX = logGetVarId("stateEstimate", "x");
  return logGetFloat(logIdStateEstimateX);
}

float FirmwareSensor::getPosY() const {
  logVarId_t logIdStateEstimateY = logGetVarId("stateEstimate", "y");
  return logGetFloat(logIdStateEstimateY);
}

float FirmwareSensor::getPosZ() const {
  logVarId_t logIdStateEstimateZ = logGetVarId("stateEstimate", "z");
  return logGetFloat(logIdStateEstimateZ);
}

float FirmwareSensor::getBatteryLevel() const {
  logVarId_t vbatid = logGetVarId("pm", "vbat");
  return logGetFloat(vbatid);
}
