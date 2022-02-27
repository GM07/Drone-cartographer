#include "controllers/simulation_controller.h"

#include <argos3/core/utility/logging/argos_log.h>

#include <unordered_map>

#include "sensors/simulation_sensor.h"
#include "utils/led.h"
#include "utils/timer.h"

using ::argos::CVector3;

//////////////////////////////////////////
SimulationController::SimulationController(CCrazyflieSensing* ccrazyflieSensing)
    : m_ccrazyflieSensing(ccrazyflieSensing),
      m_controllerDataSem(std::make_unique<Semaphore>(1)),
      m_sendDataThread(std::make_unique<std::thread>(
          &SimulationController::sendDroneDataToServerThread, this)) {
  m_abstractSensor = std::make_unique<SimulationSensor>(ccrazyflieSensing);
  static int count = 1;
  data.front = count++;
}

SimulationController::~SimulationController() { m_sendDataThread->join(); }

void SimulationController::updateSensorData() {
  data = {
      m_abstractSensor->getFrontDistance(),
      m_abstractSensor->getLeftDistance(),
      m_abstractSensor->getBackDistance(),
      m_abstractSensor->getRightDistance(),
      m_abstractSensor->getPosX(),
      m_abstractSensor->getPosY(),
      m_abstractSensor->getBatteryLevel(),
      static_cast<int>(state),
  };

  bool success = m_serverDataQueue.push(data);
  if (!success && m_controllerDataSem->tryAcquire()) {
    ControllerData removedData;
    m_serverDataQueue.pop(removedData);
    m_serverDataQueue.push(data);
  }
  m_controllerDataSem->release();
}

///////////////////////////////////////
size_t SimulationController::receiveMessage(void* message, size_t size) {
  size_t messageSize = m_socket->available();

  if (messageSize != 0) {
    m_socket->receive(boost::asio::buffer(message, size));
  }

  return messageSize;
}

///////////////////////////////////////
void SimulationController::sendMessage(void* message, size_t size_bytes) {
  m_socket->send(boost::asio::buffer(message, size_bytes));
}

void SimulationController::sendDroneDataToServerThread() {
  while (true) {
    m_dataSocket =
        std::make_unique<boost::asio::local::stream_protocol::socket>(
            io_service);
    while (true) {
      try {
        m_dataSocket->connect("/tmp/socket/data" +
                              m_ccrazyflieSensing->GetId());
        break;
      } catch (const boost::system::system_error& error) {
        Timer::delayMs(50);
      }
    }
    while (true) {
      m_controllerDataSem->acquire();
      ControllerData dataToSend;
      m_serverDataQueue.pop(dataToSend);
      try {
        m_dataSocket->send(boost::asio::buffer(&dataToSend, 32));
      } catch (const boost::system::system_error& error) {
        break;
      }
    }
  }
}

///////////////////////////////////////
void SimulationController::blinkLED(LED led) {
  if (led == kLedBlueLeft) {
  }

  log("Identify :" + m_ccrazyflieSensing->GetId());
}

///////////////////////////////////////
void SimulationController::sendP2PMessage(void* message) {
  if (message) {
  }
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
