// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/simulation_controller.h"
#pragma GCC diagnostic pop

#include <argos3/core/utility/logging/argos_log.h>

#include <mutex>
#include <unordered_map>

#include "utils/led.h"

#define ALMOST_THERE 0.01

//////////////////////////////////////////
SimulationController::SimulationController(CCrazyflieSensing* ccrazyflieSensing)
    : m_ccrazyflieSensing(ccrazyflieSensing) {}

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
  if (enable) log("Identify :" + m_ccrazyflieSensing->GetId());
}

///////////////////////////////////////////////////
void SimulationController::initCommunicationManager() {
  static boost::asio::io_service io_service;

  m_socket =
      std::make_unique<boost::asio::local::stream_protocol::socket>(io_service);

  m_socket->connect("/tmp/socket/" + m_ccrazyflieSensing->GetId());
}

//////////////////////////////
void SimulationController::log(const std::string& message) {
  std::lock_guard<std::mutex> logMutex(logBufferMutex);
  logBuffer << message << std::endl;
}

void SimulationController::setSimulationDroneInstance(
    CCrazyflieSensing* ccrazyflieSensing) {
  m_ccrazyflieSensing = ccrazyflieSensing;
}

void SimulationController::takeoff(float height) {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;

  if (cPos.GetZ() + ALMOST_THERE >= height) {
    state = State::kIdle;
    return;
  }

  cPos.SetZ(height);
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(cPos);
}

void SimulationController::land() {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;

  if (cPos.GetZ() - ALMOST_THERE <= 0.0) {
    state = State::kIdle;
    return;
  }

  cPos.SetZ(0.0f);
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(cPos);
}