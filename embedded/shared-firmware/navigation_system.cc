#include "components/navigation_system.h"

#include "controllers/abstract_controller.h"

#define HEIGHT 2.0f

void Navigation::step(std::string id) {
  switch (AbstractController::getController(id)->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      AbstractController::getController(id)->takeoff(HEIGHT);
      break;
    case State::kLanding:
      AbstractController::getController(id)->land();
      break;
    default:
      break;
  }
}