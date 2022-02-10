// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/firmware_controller.h"

#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include "utils/led.h"

extern "C" {
#include "app_channel.h"
#include "commander.h"
#include "led.h"
#include "ledseq.h"
}

FirmwareController::FirmwareController() : m_seqLED({}) {}

/////////////////////////////
void FirmwareController::goTo(const Vector3D& location, bool isRelative) {
  static setpoint_t setpoint;
  setpoint.mode.x = modeAbs;
  setpoint.mode.y = modeAbs;
  setpoint.mode.z = modeAbs;
  setpoint.position.x = location.m_x;
  setpoint.position.y = location.m_y;
  setpoint.position.z = location.m_z;

  commanderSetSetpoint(&setpoint, 3);
}

////////////////////////////////
void FirmwareController::takeOff(float height) {
  goTo(Vector3D(0, 0, height), true);
  blinkLED(LED::kLedGreenRight);
  state = State::kIdle;
}

///////////////////////////////
void FirmwareController::land() {
  goTo(Vector3D(0, 0, 0), true);
  blinkLED(LED::kLedBlueRight);
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
