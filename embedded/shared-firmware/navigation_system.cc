#include "components/drone.h"

#define HEIGHT 2.0f

void Drone::step() {
  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      m_controller->takeOff(HEIGHT);
      break;
    case State::kLanding:
      m_controller->land();
      break;
    case State::kIdentify:
      m_controller->setLEDState(LED::kLedRedLeft, true, true);
      m_controller->state = State::kIdle;
      break;
    default:
      break;
  }
}