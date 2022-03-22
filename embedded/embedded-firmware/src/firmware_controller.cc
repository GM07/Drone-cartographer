#include "controllers/firmware_controller.h"

#include "components/drone.h"
#include "controllers/abstract_controller.h"
#include "sensors/firmware_sensors.h"
#include "utils/led.h"
#include "utils/timer.h"

extern "C" {
#include "app_channel.h"
#include "commander.h"
#include "configblock.h"
#include "crtp_commander_high_level.h"
#include "estimator_kalman.h"
#include "led.h"
#include "ledseq.h"
#include "param_logic.h"
#include "radiolink.h"
#include "supervisor.h"
}

FirmwareController::FirmwareController()
    : AbstractController(std::make_unique<FirmwareSensors>()) {}

bool FirmwareController::isDroneCrashed() const {
  return supervisorIsTumbled();
}

void FirmwareController::updateSensorsData() {
  data = {
      m_abstractSensors->getFrontDistance(),
      m_abstractSensors->getLeftDistance(),
      m_abstractSensors->getBackDistance(),
      m_abstractSensors->getRightDistance(),
      m_abstractSensors->getPosX(),
      m_abstractSensors->getPosY(),
      m_abstractSensors->getBatteryLevel(),
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
  commanderNotifySetpointsStop(0);
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

void FirmwareController::setVelocity(const Vector3D& direction, float speed) {
  Vector3D speedVector = direction.toUnitVector() * speed;

  static setpoint_t setpoint;
  setpoint.mode.z = modeAbs;
  setpoint.position.z = kHeight;
  setpoint.mode.x = modeVelocity;
  setpoint.mode.y = modeVelocity;
  setpoint.velocity.x = speedVector.m_x;
  setpoint.velocity.y = speedVector.m_y;
  setpoint.velocity_body = false;

  commanderSetSetpoint(&setpoint, 3);
}

void FirmwareController::sendP2PMessage(void* message, size_t size) {
  static P2PPacket p_reply;
  static uint8_t p2pCounter = 0;
  constexpr uint8_t NB_PACKET_SEND_PER_TICK = 50;

  if (p2pCounter++ > NB_PACKET_SEND_PER_TICK) {
    p_reply.port = 0x00;                      // NOLINT
    memcpy(&p_reply.data[0], message, size);  // NOLINT
    p_reply.size = size;                      // NOLINT
    radiolinkSendP2PPacketBroadcast(&p_reply);
    p2pCounter = 0;
  }
}

void FirmwareController::receiveP2PMessage(
    std::unordered_map<size_t, DroneData>* p2pData) {
  xSemaphoreTake(p2pPacketMutex, portMAX_DELAY);
  while (!receivedP2PPacket.empty()) {
    DroneData data(reinterpret_cast<DroneData*>(  // NOLINT
        &receivedP2PPacket.front().data));        // NOLINT
    data.range = receivedP2PPacket.front().rssi;
    p2pData->insert_or_assign(data.id, data);
    receivedP2PPacket.pop();
  }
  xSemaphoreGive(p2pPacketMutex);
}

float FirmwareController::getMinCollisionAvoidanceDistance() {
  return kRealMinCollisionAvoidanceRange;
}

float FirmwareController::getMaxCollisionAvoidanceDistance() {
  return kRealMaxCollisionAvoidanceRange;
}

size_t FirmwareController::getId() {
  uint64_t address = configblockGetRadioAddress();
  auto my_id = static_cast<uint8_t>((address)&kRadioAddressMask);
  return my_id;
}
