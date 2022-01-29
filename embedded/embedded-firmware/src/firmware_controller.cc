// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "firmware_controller.h"
#pragma GCC diagnostic pop

extern "C" 
{
	#include "app_channel.h"
	#include "ledseq.h"
	#include "led.h"
}


///////////////////////////////////////
size_t bridge::FirmwareController::receiveMessage(void* message, size_t size)
{
	return appchannelReceiveDataPacket(message, sizeof(size), 0);
}

///////////////////////////////////////
void bridge::FirmwareController::setLEDState(uint8_t red, uint8_t green, uint8_t blue, bool blink)
{
	static ledseqStep_t seq_blink[] = {
  	{ true, LEDSEQ_WAITMS(500)},
  	{    0, LEDSEQ_LOOP},
	};

	static ledseqContext_t seqLED = {
  	sequence : seq_blink,
		nextContext : nullptr,
		state: 0,
  	led : LED_GREEN_R,
	};

	ledseqRegisterSequence(&seqLED);
	ledseqRunBlocking(&seqLED);
}
