// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/firmware_controller.h"

#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include "components/drone.h"
#include "utils/led.h"

extern "C" {
#include "app_channel.h"
#include "commander.h"
#include "crtp_commander_high_level.h"
#include "estimator_kalman.h"
#include "led.h"
#include "ledseq.h"
}

FirmwareController::FirmwareController() : m_seqLED({}) {}

////////////////////////////////////////////////
Vector3D FirmwareController::getCurrentLocation() {
  point_t point;
  estimatorKalmanGetEstimatedPos(&point);
  return Vector3D(point.x, point.y, point.z);
}

/////////////////////////////
void FirmwareController::goTo(const Vector3D& location, bool isRelative) {
  point_t pos;
  estimatorKalmanGetEstimatedPos(&pos);
  float distance = location.distanceTo(Vector3D(pos.x, pos.y, pos.z));
  float time = distance / SPEED;
  crtpCommanderHighLevelGoTo(location.m_x, location.m_y, location.m_z, 0.0,
                             time, false);
}

////////////////////////////////
void FirmwareController::takeOff(float height) {
  crtpCommanderHighLevelTakeoffWithVelocity(height, SPEED, false);
  state = State::kIdle;
}

///////////////////////////////
void FirmwareController::land() {
  crtpCommanderHighLevelLandWithVelocity(0, SPEED, false);
  state = State::kIdle;
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
  static ledseqStep_t ledStep[] = {{true, LEDSEQ_WAITMS(500)},
                                   {false, LEDSEQ_WAITMS(500)},
                                   {true, LEDSEQ_WAITMS(500)},
                                   {false, LEDSEQ_WAITMS(500)},
                                   {true, LEDSEQ_WAITMS(500)},
                                   {false, LEDSEQ_WAITMS(500)},
                                   {true, LEDSEQ_WAITMS(500)},
                                   {false, LEDSEQ_WAITMS(500)},
                                   {0, LEDSEQ_STOP}};

  m_seqLED.sequence = ledStep;
  m_seqLED.led = (led_t)led;

  ledseqRegisterSequence(&m_seqLED);
  ledseqRun(&m_seqLED);
}
