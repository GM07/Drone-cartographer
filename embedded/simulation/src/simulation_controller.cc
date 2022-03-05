#include "controllers/simulation_controller.h"

#include <argos3/core/utility/logging/argos_log.h>

#include <mutex>
#include <unordered_map>

#include "utils/led.h"

using ::argos::CVector3;

//////////////////////////////////////////
SimulationController::SimulationController(CCrazyflieSensing* ccrazyflieSensing)
    : m_ccrazyflieSensing(ccrazyflieSensing) {}

///////////////////////////////////////
size_t SimulationController::receiveMessage(void* message, size_t size) {
  size_t messageSize = m_socket->available();

  if (messageSize != 0) {
    m_socket->receive(boost::asio::buffer(message, size));
  }

  return messageSize;
}

///////////////////////////////////////
void SimulationController::blinkLED(LED /*led*/) {
  log("Identify :" + m_ccrazyflieSensing->GetId());
}

///////////////////////////////////////////////////
void SimulationController::initCommunicationManager() {
  m_socket =
      std::make_unique<boost::asio::local::stream_protocol::socket>(io_service);

  m_socket->connect("/tmp/socket/" + m_ccrazyflieSensing->GetId());
}

///////////////////////////////////////////////////
void SimulationController::log(const std::string& message) {
  std::lock_guard<std::mutex> logMutex(logBufferMutex);
  logBuffer << message << std::endl;
}

void SimulationController::takeOff(float height) {
  // Since getCurrentLocation() is relative to the old m_takeOffPosition
  // We need to add the old m_takeOffPosition to get the new one.
  m_takeOffPosition = getCurrentLocation() + m_takeOffPosition;

  // We takeOff using absolute position to prevent multiple takeOff from
  // stacking one on top of the other
  goTo(Vector3D(0, 0, height), false);
}

///////////////////////////////////////////////////
void SimulationController::land() {
  Vector3D pos = getCurrentLocation();
  pos.m_z = 0.0;
  goTo(pos, false);
}

// All positions are relative to takeOff position
Vector3D SimulationController::getCurrentLocation() const {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;
  Vector3D pos = Vector3D(cPos.GetX(), cPos.GetY(), cPos.GetZ());

  return pos - m_takeOffPosition;
}

bool SimulationController::isTrajectoryFinished() const {
  return getCurrentLocation().isAlmostEqual(m_targetPosition);
}

void SimulationController::goTo(const Vector3D& location, bool isRelative) {
  if (isRelative) {
    m_targetPosition = getCurrentLocation() + location;
  } else {
    m_targetPosition = location;
  }

  Vector3D simulationPosition = m_takeOffPosition + m_targetPosition;
  m_ccrazyflieSensing->m_pcPropellers->SetAbsolutePosition(CVector3(
      simulationPosition.m_x, simulationPosition.m_y, simulationPosition.m_z));
}
