#include "components/navigation_system.h"

#include "controllers/abstract_controller.h"

#define HEIGHT 2.0f

void Navigation::step() {
  switch (AbstractController::getController()->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      AbstractController::getController()->takeoff(HEIGHT);
      break;
    case State::kLanding:
      AbstractController::getController()->land();
      break;
    default:
      break;
  }
}