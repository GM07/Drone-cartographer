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
void FirmwareController::setLEDState(LEDColor color, Side side, bool enable,
                                     bool blink) {
  static ledseqStep_t ledStep[] = {{enable, LEDSEQ_WAITMS(500)},
                                   {!blink, LEDSEQ_WAITMS(500)},
                                   {0, LEDSEQ_LOOP}};

  led_t led;

  if (side == Side::kLeft) {
    switch (color) {
      case LEDColor::kRed:
        led = led_t::LED_RED_L;
        break;
      case LEDColor::kBlue:
        led = led_t::LED_BLUE_L;
        break;
      case LEDColor::kGreen:
        led = led_t::LED_GREEN_L;
        break;
    }
  } else {
    switch (color) {
      case LEDColor::kRed:
        led = led_t::LED_RED_R;
        break;
      case LEDColor::kBlue:
        led = led_t::LED_BLUE_NRF;
        break;
      case LEDColor::kGreen:
        led = led_t::LED_GREEN_R;
        break;
    }
  }

  static ledseqContext_t seqLED = {
    sequence : ledStep,
    led : led,
  };

  ledseqRegisterSequence(&seqLED);
  ledseqRun(&seqLED);
}