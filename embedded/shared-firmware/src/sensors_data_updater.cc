#include "components/drone.h"
#include "utils/state.h"

/////////////////////////////////////////////////////////////////////////
void Drone::updateSensorsData() { m_controller->updateSensorsData(); }
void Drone::updateCrashStatus() {
  if (m_controller->isDroneCrashed()) {
    m_controller->state = State::kCrash;
  }
}
