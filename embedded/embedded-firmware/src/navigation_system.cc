#include <memory>

// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include <firmwareController.h>
#pragma GCC diagnostic pop

std::unique_ptr<bridge::AbstractController> getController() {
  return std::make_unique<bridge::FirmwareController>();
}

extern "C" void appMain() {
  std::unique_ptr<bridge::AbstractController> controller = getController();
  while (true) {
    //
  }
}
