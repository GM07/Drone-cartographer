#include "components/drone.h"

std::shared_ptr<AbstractController> Drone::getController() {
  return m_controller;
}
