#include "controllers/abstract_controller.h"

AbstractController::AbstractController(
    std::unique_ptr<AbstractSensor>&& abstractSensor)
    : m_abstractSensor(std::move(abstractSensor)) {}
