#include "components/drone.h"

Drone::Drone(std::shared_ptr<AbstractController> controller)
    : m_controller(controller) {}

std::shared_ptr<AbstractController> Drone::getController() {
  return m_controller;
}
