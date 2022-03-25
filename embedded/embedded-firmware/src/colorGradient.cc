#include <algorithm>
#include <memory>
#include <vector>

#include "components/drone.h"
#include "utils/timer.h"

extern "C" {
#include "FreeRTOS.h"
#include "components/ccommunication_manager.h"
#include "task.h"
}

namespace {
bool isInit;

}  // namespace
namespace P2P {

void flashCorrectLed(void *) {
  Timer::delayMs(3000);
  while (true) {
    std::vector<float> droneDistances;

    std::transform(Drone::getEmbeddedDrone().m_peerData.begin(),
                   Drone::getEmbeddedDrone().m_peerData.end(),
                   std::back_inserter(droneDistances),
                   [](std::pair<size_t, DroneData> const &pair) {
                     return pair.second.distanceFromTakeoff;
                   });

    droneDistances.push_back(
        Drone::getEmbeddedDrone().m_data.distanceFromTakeoff);
    std::sort(droneDistances.begin(), droneDistances.end());
    Drone::getEmbeddedDrone().getController()->blinkLED(LED(kLedRedLeft));
    Timer::delayMs(1000);
  }
}
// void blinkLED() {
//  constexpr size_t kNbLEDSteps = 9;
//  static std::array<ledseqStep_t, kNbLEDSteps> ledStep{
//      {{true, LEDSEQ_WAITMS(500)},
//       {false, LEDSEQ_WAITMS(500)},
//       {true, LEDSEQ_WAITMS(500)},
//       {false, LEDSEQ_WAITMS(500)},
//       {true, LEDSEQ_WAITMS(500)},
//       {false, LEDSEQ_WAITMS(500)},
//       {true, LEDSEQ_WAITMS(500)},
//       {false, LEDSEQ_WAITMS(500)},
//       {false, LEDSEQ_STOP}}};
//
//  m_seqLED.sequence = ledStep.data();
//  m_seqLED.led = static_cast<led_t>(led);
//
//  ledseqRegisterSequence(&m_seqLED);
//  ledseqRun(&m_seqLED);
//}
}  // namespace P2P

void initColorGradient() {
  xTaskCreate(P2P::flashCorrectLed, "ColorGradientTask",
              configMINIMAL_STACK_SIZE, nullptr, 0, nullptr);
  isInit = true;
}
bool testColorGradient() { return isInit; };
