// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/firmware_controller.h"

#include "controllers/abstract_controller.h"
#pragma GCC diagnostic pop

#include "utils/led.h"

extern "C" {
#include "app_channel.h"
#include "led.h"
#include "ledseq.h"
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
void FirmwareController::setLEDState(LED led, bool enable, bool blink) {
  static ledseqStep_t ledStep[] = {{enable, LEDSEQ_WAITMS(500)},
                                   {(enable && !blink), LEDSEQ_WAITMS(500)},
                                   {enable, LEDSEQ_WAITMS(500)},
                                   {(enable && !blink), LEDSEQ_WAITMS(500)},
                                   {enable, LEDSEQ_WAITMS(500)},
                                   {(enable && !blink), LEDSEQ_WAITMS(500)},
                                   {enable, LEDSEQ_WAITMS(500)},
                                   {(enable && !blink), LEDSEQ_WAITMS(500)},
                                   {0, LEDSEQ_STOP}};

  static ledseqContext_t seqLED = {
    sequence : ledStep,
    led : (led_t)led,
  };

  ledseqRegisterSequence(&seqLED);
  ledseqRun(&seqLED);
}
