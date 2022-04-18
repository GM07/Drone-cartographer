#include "sensors/firmware_sensors.h"

#include <algorithm>
#include <array>
#include <iterator>

extern "C" {
#include "log.h"
}

namespace {
constexpr std::size_t kNbLevel = 10;
std::array<float, kNbLevel> kBatLevels{{
    4.10,  // 90%
    4.04,  // 80%
    4.00,  // 70%
    3.96,  // 60%
    3.92,  // 50%
    3.89,  // 40%
    3.87,  // 30%
    3.83,  // 20%
    3.78,  // 10%
    3.00   // 00%
}};
}  // namespace

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
[[nodiscard]] float FirmwareSensors::getBatteryLevel(bool isInMission) const {
  const float modifier = isInMission ? 0.6F : 0.0F;
  logVarId_t id = logGetVarId("pm", "vbat");
  float voltage = logGetFloat(id);
  return (
      std::count_if(kBatLevels.begin(), kBatLevels.end(),
                    [=](float level) { return voltage > level - modifier; }) *
      10);
}
