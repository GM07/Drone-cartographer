#include <memory>

// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "firmware_controller.h"
#pragma GCC diagnostic pop

extern "C" {
#include "commander.h"
#include "FreeRTOS.h"
#include "task.h"
}

std::unique_ptr<bridge::AbstractController> getController() {
  return std::make_unique<bridge::FirmwareController>();
}

void setHoverSetpoint(setpoint_t *setpoint, float x, float y, float z,
                      float yawrate) {
  setpoint->mode.z = modeAbs;
  setpoint->position.z = z;

  setpoint->mode.yaw = modeVelocity;
  setpoint->attitudeRate.yaw = yawrate;

  setpoint->mode.x = modeVelocity;
  setpoint->mode.y = modeVelocity;
  setpoint->velocity.x = x;
  setpoint->velocity.y = y;

  setpoint->velocity_body = true;
}

extern "C" void appMain() {
  std::unique_ptr<bridge::AbstractController> controller = getController();

  vTaskDelay(M2T(3000));
  setpoint_t point;
  setHoverSetpoint(&point, 0.0f, 0.0f, 1.0f, 0.0f);

  while (1) {
    vTaskDelay(M2T(10));
    commanderSetSetpoint(&point, 3);
  }
}
