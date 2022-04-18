#include "controllers/simulation_controller.h"

#include <argos3/core/utility/logging/argos_log.h>

#include <unordered_map>

#include "sensors/simulation_sensors.h"
#include "utils/led.h"
#include "utils/math.h"
#include "utils/time.h"

using ::argos::CVector3;

using ::boost::asio::buffer;
using ::boost::system::system_error;

//////////////////////////////////////////
SimulationController::SimulationController(CCrazyflieSensing* ccrazyflieSensing)
    : AbstractController(
          std::make_unique<SimulationSensors>(ccrazyflieSensing)),
      m_ccrazyflieSensing(ccrazyflieSensing),
      m_sendDataThread(std::make_unique<std::thread>(
          &SimulationController::sendDroneDataToServerThread, this)) {}

//////////////////////////////////////////
SimulationController::~SimulationController() {
  m_threadContinueFlag = false;
  if (m_sendDataThread) {
    m_sendDataThread->join();
  }
}

//////////////////////////////////////////
void SimulationController::updateSensorsData() {
  // We multiply by 10 as we expect a reading in mm but receive a reading in cm
  constexpr float kCmToMmFactor = 10.0F;
  m_data = {
      m_abstractSensors->getFrontDistance() * kCmToMmFactor,
      m_abstractSensors->getLeftDistance() * kCmToMmFactor,
      m_abstractSensors->getBackDistance() * kCmToMmFactor,
      m_abstractSensors->getRightDistance() * kCmToMmFactor,
      getCurrentLocation().rotate(-m_orientation).m_x,
      getCurrentLocation().rotate(-m_orientation).m_y,
      m_abstractSensors->getBatteryLevel(m_state != State::kIdle),
      m_state,
  };

  {
    std::lock_guard<decltype(m_controllerDataMutex)> guard(
        m_controllerDataMutex);
    ControllerData dataToEmplace =
        m_data;  // std::make_optional moves the value so we must copy it first
    m_controllerData = std::make_optional<ControllerData>(dataToEmplace);
  }
}

///////////////////////////////////////
[[nodiscard]] size_t SimulationController::receiveMessage(void* message,
                                                          size_t size) const {
  size_t messageSize = m_socket->available();

  if (messageSize != 0) {
    m_socket->receive(buffer(message, size));
  }

  return messageSize;
}

///////////////////////////////////////
void SimulationController::sendMessage(void* message, size_t size_bytes) const {
  m_socket->send(buffer(message, size_bytes));
}

///////////////////////////////////////
void SimulationController::sendDroneDataToServerThread() {
  constexpr uint32_t kTryConnectionDelay = 250;

  while (m_threadContinueFlag) {
    m_dataSocket =
        std::make_unique<boost::asio::local::stream_protocol::socket>(
            io_service);

    while (m_threadContinueFlag) {
      try {
        m_dataSocket->connect("/tmp/socket/data" +
                              m_ccrazyflieSensing->GetId());
        break;
      } catch (const system_error& error) {
        Time::delayMs(kTryConnectionDelay);
      }
    }

    while (m_threadContinueFlag) {
      try {
        if (m_controllerData.has_value()) {
          uint8_t ack = 0;

          ControllerData dataToSend{};
          {
            std::lock_guard<decltype(m_controllerDataMutex)> guard(
                m_controllerDataMutex);
            dataToSend = m_controllerData.value();
            m_controllerData.reset();
          }

          m_dataSocket->send(buffer(&dataToSend, sizeof(dataToSend)));
          m_dataSocket->receive(buffer(&ack, sizeof(ack)));
        }

      } catch (const system_error& error) {
        break;
      }
    }
  }
}

///////////////////////////////////////
void SimulationController::identify() {
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

///////////////////////////////////////////////////
void SimulationController::takeOff(float height) {
  // Since getCurrentLocation() is relative to the old m_takeOffPosition
  // We need to add the old m_takeOffPosition to get the new one.
  m_takeOffPosition += getCurrentLocation();

  m_targetPosition = getCurrentLocation() + Vector3D::z(height);

  setVelocity(Vector3D::z(height), kTakeOffSpeed);
}

///////////////////////////////////////////////////
void SimulationController::land() {
  Vector3D pos = getCurrentLocation();
  m_targetPosition = Vector3D(pos.m_x, pos.m_y, 0.0);
  setVelocity(Vector3D::z(-pos.m_z), kLandingSpeed);
}

// All positions are relative to takeOff position
///////////////////////////////////////////////////
[[nodiscard]] Vector3D SimulationController::getCurrentLocation() const {
  CVector3 cPos = m_ccrazyflieSensing->m_pcPos->GetReading().Position;

  return Vector3D(cPos.GetX(), cPos.GetY(), cPos.GetZ()) - m_takeOffPosition;
}

///////////////////////////////////////////////////
[[nodiscard]] bool SimulationController::isTrajectoryFinished() const {
  return areAlmostEqual(getCurrentLocation(), m_targetPosition);
}

///////////////////////////////////////////////////
void SimulationController::setVelocity(const Vector3D& direction, float speed,
                                       bool bodyReference) {
  Vector3D speedVector = (direction.toUnitVector() * speed);

  if (bodyReference) {
    speedVector = speedVector.rotate(m_orientation);
  }

  m_ccrazyflieSensing->m_pcPropellers->SetLinearVelocity(
      CVector3(speedVector.m_x, speedVector.m_y, speedVector.m_z));
}

///////////////////////////////////////////////////
void SimulationController::sendP2PMessage(void* message, size_t size) {
  argos::CByteArray data(static_cast<argos::UInt8*>(message), size);
  m_ccrazyflieSensing->m_pcRABA->SetData(data);
}

///////////////////////////////////////////////////
void SimulationController::receiveP2PMessage(
    std::unordered_map<size_t, DroneData>* p2pData) {
  std::vector<argos::CCI_RangeAndBearingSensor::SPacket> readings =
      m_ccrazyflieSensing->m_pcRABS->GetReadings();

  for (auto reading : readings) {
    DroneData data((*reinterpret_cast<DroneData*>(reading.Data.ToCArray()))
                       .transformReference(-m_orientation));

    data.m_range = reading.Range;
    p2pData->insert_or_assign(data.m_id, data);
  }
}

///////////////////////////////////////////////////
[[nodiscard]] float SimulationController::getMinCollisionAvoidanceDistance()
    const {
  return kSimulationCollisionAvoidanceRange;
}

///////////////////////////////////////////////////
[[nodiscard]] float SimulationController::getMaxCollisionAvoidanceDistance()
    const {
  return kSimulationCollisionAvoidanceRange;
}

///////////////////////////////////////////////////
[[nodiscard]] size_t SimulationController::getId() const {
  return std::hash<std::string>{}(m_ccrazyflieSensing->GetId());
}
