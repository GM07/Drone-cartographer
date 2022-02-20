// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/firmware_controller.h"

#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include "components/drone.h"
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

FirmwareController::FirmwareController() : m_seqLED({}) {}

/////////////////////////
bool FirmwareController::finishedTrajectory() {
  return crtpCommanderHighLevelIsTrajectoryFinished();
}

////////////////////////////////////////////////
Vector3D FirmwareController::getCurrentLocation() {
  point_t point;
  estimatorKalmanGetEstimatedPos(&point);
  return Vector3D(point.x, point.y, point.z);
}

////////////////////////////////
void FirmwareController::takeOff(float height) {
  takeOffPosition = getCurrentLocation();
  crtpCommanderHighLevelTakeoff(height, TAKEOFF_TIME);
}

///////////////////////////////
void FirmwareController::land() { crtpCommanderHighLevelLand(0, LANDING_TIME); }

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

void FirmwareController::goTo(const Vector3D& location, bool isRelative) {
  float time;
  Vector3D objective = location;
  if (isRelative) {
    time = objective.distanceTo(Vector3D(0, 0, 0)) / SPEED;
  } else {
    objective += takeOffPosition;
    time = objective.distanceTo(getCurrentLocation()) / SPEED;
  }

  crtpCommanderHighLevelGoTo(objective.m_x, objective.m_y, objective.m_z, 0.0,
                             time, isRelative);
}
