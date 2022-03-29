#include "components/drone.h"
#include "utils/state.h"

/////////////////////////////////////////////////////////////////////////
void Drone::updateCrashStatus() {
  if (m_controller->isDroneCrashed()) {
    m_controller->m_state = State::kCrash;
  }
}
