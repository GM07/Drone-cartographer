
#include "controllers/firmware_controller.h"

#include "components/drone.h"
#include "controllers/abstract_controller.h"
#include "sensors/firmware_sensor.h"
#include "utils/led.h"
#include "utils/timer.h"

extern "C" {
#include "app_channel.h"
#include "commander.h"
#include "crtp_commander_high_level.h"
#include "estimator_kalman.h"
#include "led.h"
#include "ledseq.h"
#include "param_logic.h"
}

FirmwareController::FirmwareController()
    : AbstractController(std::make_unique<FirmwareSensor>()) {}

void FirmwareController::updateSensorData() {
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
}

/////////////////////////
bool FirmwareController::isTrajectoryFinished() const {
  return crtpCommanderHighLevelIsTrajectoryFinished();
}

////////////////////////////////////////////////
Vector3D FirmwareController::getCurrentLocation() const {
  point_t point;
  estimatorKalmanGetEstimatedPos(&point);
  return Vector3D(point.x, point.y, point.z) - m_takeOffPosition;
}

////////////////////////////////
void FirmwareController::takeOff(float height) {
  m_takeOffPosition = getCurrentLocation() + m_takeOffPosition;
  m_targetPosition = Vector3D(0, 0, height);
  float time =
      m_targetPosition.distanceTo(getCurrentLocation()) / kTakeOffSpeed;
  crtpCommanderHighLevelTakeoff(height, time);
}

///////////////////////////////
void FirmwareController::land() {
  m_targetPosition = getCurrentLocation();
  m_targetPosition.m_z = 0;
  float time =
      m_targetPosition.distanceTo(getCurrentLocation()) / kLandingSpeed;
  crtpCommanderHighLevelLand(0, time);
}

///////////////////////////////////////
size_t FirmwareController::receiveMessage(void* message, size_t size) {
  return appchannelReceiveDataPacket(message, size, 0);
}

///////////////////////////////////////
void FirmwareController::sendMessage(void* message, size_t size) {
  appchannelSendDataPacket(message, size);
}

///////////////////////////////////////
void FirmwareController::blinkLED(LED led) {
  constexpr size_t kNbLEDSteps = 9;
  static std::array<ledseqStep_t, kNbLEDSteps> ledStep{
      {{true, LEDSEQ_WAITMS(500)},
       {false, LEDSEQ_WAITMS(500)},
       {true, LEDSEQ_WAITMS(500)},
       {false, LEDSEQ_WAITMS(500)},
       {true, LEDSEQ_WAITMS(500)},
       {false, LEDSEQ_WAITMS(500)},
       {true, LEDSEQ_WAITMS(500)},
       {false, LEDSEQ_WAITMS(500)},
       {false, LEDSEQ_STOP}}};

  m_seqLED.sequence = ledStep.data();
  m_seqLED.led = static_cast<led_t>(led);

  ledseqRegisterSequence(&m_seqLED);
  ledseqRun(&m_seqLED);
}

void FirmwareController::goTo(const Vector3D& location, bool isRelative) {
  float time = 0;
  if (isRelative) {
    m_targetPosition = location;
    time = m_targetPosition.distanceTo(Vector3D(0, 0, 0)) / kDroneSpeed;
  } else {
    m_targetPosition = m_takeOffPosition + location;
    time = location.distanceTo(getCurrentLocation()) / kDroneSpeed;
  }

  crtpCommanderHighLevelGoTo(m_targetPosition.m_x, m_targetPosition.m_y,
                             m_targetPosition.m_z, 0.0, time, isRelative);
}
