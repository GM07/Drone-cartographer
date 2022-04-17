#include "components/drone.h"
#include "utils/state.h"

/////////////////////////////////////////////////////////////////////////
void Drone::updateCrashStatus() {
  if (m_controller->isDroneCrashed()) {
    m_controller->m_state = State::kCrash;
  } else if (m_controller->m_state == State::kCrash) {
    m_controller->m_state = State::kIdle;
  }
}

void Drone::updateStatusFromBattery() {
  if (m_controller->hasLowBattery() &&
      m_controller->m_state == State::kExploring) {
    m_controller->m_state = State::kReturningToBase;
  }
}
