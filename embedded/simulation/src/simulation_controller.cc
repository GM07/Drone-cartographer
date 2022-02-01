// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/simulation_controller.h"
#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include "utils/led.h"
#include <argos3/core/utility/logging/argos_log.h>

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
    std::string strMessage((char*)message, n);
    log("Packet received : " + strMessage);
  }

  return n;
}

///////////////////////////////////////
void SimulationController::sendMessage(void* message, size_t size) {
  return;
}

///////////////////////////////////////
void SimulationController::setLEDState(LED led, bool enable,
                                     bool blink) {
  return;
}

void SimulationController::initCommunicationManager() {
  boost::asio::io_service io_service;
  m_socket = std::make_unique<boost::asio::local::stream_protocol::socket>(io_service);
  m_socket->connect("/tmp/socket");
}

void SimulationController::log(const std::string& message) {
  //We should be able to use argos::LOG here but it does not seem to work.
  //TODO - Look into THREADSAFE option when compiling .deb for argos3 simulator
  std::cout << message << std::endl;
  return;
}