#include "controllers/simulation_controller.h"

#include <argos3/core/utility/logging/argos_log.h>

#include <unordered_map>

#include "sensors/simulation_sensors.h"
#include "utils/led.h"
#include "utils/math.h"
#include "utils/timer.h"

using ::argos::CVector3;

//////////////////////////////////////////
SimulationController::SimulationController(CCrazyflieSensing* ccrazyflieSensing)
    : AbstractController(
          std::make_unique<SimulationSensors>(ccrazyflieSensing)),
      m_ccrazyflieSensing(ccrazyflieSensing),
      m_controllerDataSem(std::make_unique<Semaphore>(1)),
      m_sendDataThread(std::make_unique<std::thread>(
          &SimulationController::sendDroneDataToServerThread, this)) {}

SimulationController::~SimulationController() {
  m_threadContinueFlag = false;
  m_controllerDataSem->release();
  if (m_sendDataThread) {
    m_sendDataThread->join();
  }
}

// We multiply by 10 as we expect a reading in mm but receive a reading in cm
void SimulationController::updateSensorsData() {
  constexpr float kCmToMmFactor = 10.0F;
  data = {
      m_abstractSensors->getFrontDistance() * kCmToMmFactor,
      m_abstractSensors->getLeftDistance() * kCmToMmFactor,
      m_abstractSensors->getBackDistance() * kCmToMmFactor,
      m_abstractSensors->getRightDistance() * kCmToMmFactor,
      m_abstractSensors->getPosX(),
      m_abstractSensors->getPosY(),
      m_abstractSensors->getBatteryLevel(),
      static_cast<int>(state),
  };

  bool success = m_serverDataQueue.push(data);
  if (!success && m_controllerDataSem->tryAcquire()) {
    ControllerData removedData{};
    m_serverDataQueue.pop(removedData);
    m_serverDataQueue.push(data);
  }
  m_controllerDataSem->release();
}

bool SimulationController::isDroneCrashed() const { return false; }

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
  constexpr size_t kTryConnectionDelay = 50;
  constexpr size_t kPacketSize = 32;
  constexpr size_t kAckSize = 1;
  while (m_threadContinueFlag) {
    m_dataSocket =
        std::make_unique<boost::asio::local::stream_protocol::socket>(
            io_service);
    while (m_threadContinueFlag) {
      try {
        m_dataSocket->connect("/tmp/socket/data" +
                              m_ccrazyflieSensing->GetId());
        break;
      } catch (const boost::system::system_error& error) {
        Timer::delayMs(kTryConnectionDelay);
      }
    }
    while (m_threadContinueFlag) {
      m_controllerDataSem->acquire();

      ControllerData dataToSend{};
      bool dataPresent = m_serverDataQueue.pop(dataToSend);
      try {
        if (dataPresent) {
          uint8_t ack = 0;
          m_dataSocket->send(boost::asio::buffer(&dataToSend, kPacketSize));
          m_dataSocket->receive(boost::asio::buffer(&ack, kAckSize));
        }
      } catch (const boost::system::system_error& error) {
        break;
      }
    }
  }
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

  m_targetPosition = getCurrentLocation() + Vector3D(0, 0, height);

  setVelocity(Vector3D(0, 0, height), kTakeOffSpeed);
}

///////////////////////////////////////////////////
void SimulationController::land() {
  Vector3D pos = getCurrentLocation();
  m_targetPosition = Vector3D(pos.m_x, pos.m_z, 0.0);
  setVelocity(Vector3D(0, 0, -pos.m_z), kLandingSpeed);
}

// All positions are relative to takeOff position
Vector3D SimulationController::getCurrentLocation() const {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;
  Vector3D pos = Vector3D(cPos.GetX(), cPos.GetY(), cPos.GetZ());

  return pos - m_takeOffPosition;
}

bool SimulationController::isTrajectoryFinished() const {
  return areAlmostEqual(getCurrentLocation(), m_targetPosition);
}

void SimulationController::setVelocity(const Vector3D& direction, float speed) {
  Vector3D speedVector = direction.toUnitVector() * speed;
  m_ccrazyflieSensing->m_pcPropellers->SetLinearVelocity(
      CVector3(speedVector.m_x, speedVector.m_y, speedVector.m_z));
}

void SimulationController::sendP2PMessage(void* message, size_t size) {
  argos::CByteArray data(static_cast<argos::UInt8*>(message), size);
  m_ccrazyflieSensing->m_pcRABA->SetData(data);
}

void SimulationController::receiveP2PMessage(
    std::unordered_map<size_t, DroneData>& p2pData) {
  std::vector<argos::CCI_RangeAndBearingSensor::SPacket> readings =
      m_ccrazyflieSensing->m_pcRABS->GetReadings();
  for (auto reading : readings) {
    DroneData data((DroneData*)reading.Data.ToCArray());
    data.range = reading.Range;
    p2pData.insert_or_assign(data.id, data);
  }
}

size_t SimulationController::getId() {
  log(m_ccrazyflieSensing->GetId());
  return std::hash<std::string>{}(m_ccrazyflieSensing->GetId());
}