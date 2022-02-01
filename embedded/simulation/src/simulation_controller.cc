// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/simulation_controller.h"
#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include "utils/led.h"

/////////////////////////////////////////////////////////////////////////
std::shared_ptr<AbstractController> AbstractController::getController() {
  static std::shared_ptr<AbstractController> controller =
      std::make_unique<SimulationController>(nullptr);
  return controller;
}

///////////////////////////////////////
size_t SimulationController::receiveMessage(void* message, size_t size) {
  return 0;
}

///////////////////////////////////////
void SimulationController::sendMessage(void* message, size_t size) {
  return;
}

///////////////////////////////////////
void SimulationController::setLEDState(LED led, bool enable,
                                     bool blink) {
  return;
}