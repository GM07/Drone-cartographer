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
  n = m_socket->available();
  if (n) {
    m_socket->receive(boost::asio::buffer(message, size));
  }

  return n;
}

///////////////////////////////////////
void SimulationController::blinkLED(LED led) {
  log("Identify :" + m_ccrazyflieSensing->GetId());
}

///////////////////////////////////////////////////
void SimulationController::initCommunicationManager() {
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

void SimulationController::takeOff(float height) {
  Vector3D pos = getCurrentLocation();
  pos.m_z = height;
  objective = pos;
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(
      CVector3(pos.m_x, pos.m_y, pos.m_z));
}

void SimulationController::land() {
  Vector3D pos = getCurrentLocation();
  pos.m_z = 0.0;
  objective = pos;
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(
      CVector3(pos.m_x, pos.m_y, pos.m_z));
}

Vector3D SimulationController::getCurrentLocation() {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;
  return Vector3D(cPos.GetX(), cPos.GetY(), cPos.GetZ());
}

bool SimulationController::finishedTrajectory() {
  return getCurrentLocation().isAlmostEqual(objective);
}