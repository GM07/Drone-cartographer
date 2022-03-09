#include "controllers/abstract_controller.h"

AbstractController::AbstractController(
    std::unique_ptr<AbstractSensors>&& abstractSensors)
    : m_abstractSensors(std::move(abstractSensors)) {}
