#include "controllers/firmware_controller.h"

#include "components/drone.h"
#include "controllers/abstract_controller.h"
#include "sensors/firmware_sensors.h"
#include "utils/led.h"
#include "utils/time.h"

extern "C" {
#include "app_channel.h"
#include "commander.h"
#include "configblock.h"
#include "crtp_commander_high_level.h"
#include "estimator_kalman.h"
#include "led.h"
#include "ledseq.h"
#include "log.h"
#include "param_logic.h"
#include "radiolink.h"
#include "supervisor.h"
}

namespace {
constexpr size_t kNbLEDSteps = 9;
std::array<ledseqStep_t, kNbLEDSteps> ledStep{{{true, LEDSEQ_WAITMS(500)},
                                               {false, LEDSEQ_WAITMS(500)},
                                               {true, LEDSEQ_WAITMS(500)},
                                               {false, LEDSEQ_WAITMS(500)},
                                               {true, LEDSEQ_WAITMS(500)},
                                               {false, LEDSEQ_WAITMS(500)},
                                               {true, LEDSEQ_WAITMS(500)},
                                               {false, LEDSEQ_WAITMS(500)},
                                               {false, LEDSEQ_STOP}}};
}  // namespace

////////////////////////////////////////////////
FirmwareController::FirmwareController()
    : AbstractController(std::make_unique<FirmwareSensors>()),
      m_height(kHeight) {
  m_seqLED.sequence = ledStep.data();
  m_seqLED.led = static_cast<led_t>(LED::kLedBlueLeft);
  ledseqRegisterSequence(&m_seqLED);
}

////////////////////////////////////////////////
[[nodiscard]] bool FirmwareController::isDroneCrashed() const {
  return supervisorIsTumbled();
}

////////////////////////////////////////////////
void FirmwareController::updateSensorsData() {
  m_data = {
      .front = m_abstractSensors->getFrontDistance(),
      .left = m_abstractSensors->getLeftDistance(),
      .back = m_abstractSensors->getBackDistance(),
      .right = m_abstractSensors->getRightDistance(),
      .posX = m_abstractSensors->getPosX(),
      .posY = m_abstractSensors->getPosY(),
      .batteryLevel = m_abstractSensors->getBatteryLevel(),
      .state = m_state,
  };
}

////////////////////////////////////////////////
[[nodiscard]] bool FirmwareController::isTrajectoryFinished() const {
  return Math::areAlmostEqual(getCurrentLocation(), m_targetPosition,
                              kRealTrajectoryFinishedTreshold);
}

////////////////////////////////////////////////
[[nodiscard]] Vector3D FirmwareController::getCurrentLocation() const {
  point_t point;
  estimatorKalmanGetEstimatedPos(&point);

  logVarId_t useFId = logGetVarId("kalman", "useF");
  if (logGetUint(useFId)) {
    logVarId_t floorHeightId = logGetVarId("kalman", "stateF");
    float floorHeight = logGetFloat(floorHeightId);
    point.z += floorHeight;
  }

  return Vector3D(point.x, point.y, point.z) - m_takeOffPosition;
}

////////////////////////////////
void FirmwareController::takeOff(float height) {
  // Disable useF for the start
  paramVarId_t paramIdUseF = paramGetVarId("kalman", "useF");
  paramSetInt(paramIdUseF, 0);

  m_takeOffPosition += getCurrentLocation();
  m_targetPosition = Vector3D::z(height);
}

///////////////////////////////
void FirmwareController::land() {
  // Disable useF for the start
  paramVarId_t paramIdUseF = paramGetVarId("kalman", "useF");
  paramSetInt(paramIdUseF, 0);

  m_targetPosition = getCurrentLocation();
  m_targetPosition.m_z = 0;
}

void FirmwareController::stopMotors() { commanderNotifySetpointsStop(0); }

///////////////////////////////////////
[[nodiscard]] size_t FirmwareController::receiveMessage(void* message,
                                                        size_t size) const {
  return appchannelReceiveDataPacket(message, size, 0);
}

///////////////////////////////////////
void FirmwareController::sendMessage(void* message, size_t size) const {
  appchannelSendDataPacket(message, size);
}

///////////////////////////////////////
void FirmwareController::identify() { ledseqRun(&m_seqLED); }

///////////////////////////////////////
void FirmwareController::setVelocity(const Vector3D& direction, float speed) {
  Vector3D speedVector = direction.toUnitVector() * speed;

  static setpoint_t setpoint;
  setpoint.mode.z = modeVelocity;
  setpoint.mode.x = modeVelocity;
  setpoint.mode.y = modeVelocity;
  setpoint.velocity.z = speedVector.m_z;
  setpoint.velocity.x = speedVector.m_x;
  setpoint.velocity.y = speedVector.m_y;
  setpoint.velocity_body = false;

  if (m_state == State::kExploring) {
    setpoint.mode.z = modeAbs;
    setpoint.position.z = kHeight;
  }

  commanderSetSetpoint(&setpoint, 3);
}

///////////////////////////////////////
void FirmwareController::sendP2PMessage(void* message, size_t size) {
  P2PPacket p_reply;

  p_reply.port = 0x00;
  p_reply.size = size;
  memcpy(&p_reply.data[0], message, size);
  radiolinkSendP2PPacketBroadcast(&p_reply);
}

///////////////////////////////////////
[[nodiscard]] float FirmwareController::getMinCollisionAvoidanceDistance()
    const {
  return kRealMinCollisionAvoidanceRange;
}

///////////////////////////////////////
[[nodiscard]] float FirmwareController::getMaxCollisionAvoidanceDistance()
    const {
  return kRealMaxCollisionAvoidanceRange;
}

///////////////////////////////////////
[[nodiscard]] size_t FirmwareController::getId() const {
  constexpr uint64_t kRadioAddressMask = 0xFF;

  return (configblockGetRadioAddress() & kRadioAddressMask);
}
