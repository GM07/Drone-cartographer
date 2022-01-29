#include <memory>

extern "C" {
#include "commander.h"
#include "FreeRTOS.h"
#include "task.h"
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "firmware_controller.h"
#include "abstract_controller.h"
#pragma GCC diagnostic pop

/////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////
std::shared_ptr<bridge::AbstractController> bridge::AbstractController::getController(){
	static std::shared_ptr<bridge::AbstractController> controller = std::make_unique<bridge::FirmwareController>();
  return controller;
}

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  vTaskDelay(M2T(3000));
  setpoint_t point;
  setHoverSetpoint(&point, 0.0f, 0.0f, 1.0f, 0.0f);

  while (1) {
    vTaskDelay(M2T(10));
    commanderSetSetpoint(&point, 3);
  }
}
