// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/firmware_controller.h"

#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

extern "C" {
#include "app_channel.h"
#include "led.h"
#include "ledseq.h"
}

/////////////////////////////////////////////////////////////////////////
std::shared_ptr<AbstractController> AbstractController::getController() {
  static std::shared_ptr<AbstractController> controller =
      std::make_unique<FirmwareController>();
  return controller;
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
void FirmwareController::setLEDState(uint8_t red, uint8_t green, uint8_t blue,
                                     bool blink) {
  static ledseqStep_t seq_blink[] = {
      {true, LEDSEQ_WAITMS(500)},
      {0, LEDSEQ_LOOP},
  };

  static ledseqContext_t seqLED = {
    sequence : seq_blink,
    nextContext : nullptr,
    state : 0,
    led : LED_GREEN_R,
  };

  ledseqRegisterSequence(&seqLED);
  ledseqRunBlocking(&seqLED);
}