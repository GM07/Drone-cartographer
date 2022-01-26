#include <memory>

// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include <firmware_controller.h>
#pragma GCC diagnostic pop

#include "FreeRTOS.h"
#include "crtp_commander_high_level.h"
#include "task.h"

std::unique_ptr<bridge::AbstractController> getController() {
  return std::make_unique<bridge::FirmwareController>();
}

extern "C" void appMain() {
  std::unique_ptr<bridge::AbstractController> controller = getController();
  // crtpCommanderHighLevelTakeoff(1.0f, 5.0f);
  while (true) {
    vTaskDelay(M2T(5000));
    // crtpCommanderHighLevelLand(0.0f, 5.0f);
  }
}
