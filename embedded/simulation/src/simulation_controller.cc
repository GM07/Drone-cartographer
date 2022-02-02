// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/simulation_controller.h"

#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include <argos3/core/utility/logging/argos_log.h>

#include "utils/led.h"

/////////////////////////////////////////////////////////////////////////
std::shared_ptr<AbstractController> AbstractController::getController() {
  static std::shared_ptr<AbstractController> controller =
      std::make_unique<SimulationController>();
  return controller;
}

///////////////////////////////////////
size_t SimulationController::receiveMessage(void* message, size_t size) {
  size_t n;

  if (n = m_socket->available()) {
    m_socket->receive(boost::asio::buffer(message, size));
  }

  return n;
}

///////////////////////////////////////
void SimulationController::sendMessage(void* message, size_t size) { return; }

///////////////////////////////////////
void SimulationController::setLEDState(LED led, bool enable, bool blink) {
  log("Identify :" + m_ccrazyflieSensing->GetId());
}

///////////////////////////////////////////////////
void SimulationController::initCommunicationManager() {
  boost::asio::io_service io_service;
  m_socket =
      std::make_unique<boost::asio::local::stream_protocol::socket>(io_service);
  m_socket->connect("/tmp/socket");
}

//////////////////////////////
void SimulationController::log(const std::string& message) {
  // We should be able to use argos::LOG here but it does not seem to work.
  // TODO - Look into THREADSAFE option when compiling .deb for argos3 simulator
  std::cout << message << std::endl;
  return;
}

void SimulationController::setSimulationDroneInstance(
    CCrazyflieSensing* ccrazyflieSensing) {
  m_ccrazyflieSensing = ccrazyflieSensing;
}

void SimulationController::takeoff(float height) {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;

  if (cPos.GetZ() == height) {
    state = State::kIdle;
    return;
  }

  cPos.SetZ(height);
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(cPos);
}

void SimulationController::land() {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;

  if (cPos.GetZ() == 0.1f) {
    state = State::kIdle;
    return;
  }

  cPos.SetZ(0.1f);
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(cPos);
}