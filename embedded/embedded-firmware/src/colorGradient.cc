#include <algorithm>
#include <memory>
#include <vector>

#include "components/drone.h"
#include "utils/timer.h"

extern "C" {
#include "FreeRTOS.h"
#include "components/ccommunication_manager.h"
#include "ledseq.h"
#include "task.h"
}

namespace {
bool isInit;

}  // namespace

namespace P2P {

// void blinkLED(LED led) {
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
//  // context[0].sequence = ledStep.data();
//  // context[0].led = static_cast<led_t>(led);
//
//  // ledseqRegisterSequence(&context[0]);
//  // ledseqRun(&context[0]);
//}

std::array<ledseqContext_t, 10> context;
std::vector<std::array<ledseqStep_t, 3>> ledSteps;

void registerColors() {
  for (int i = 0; i < 5; ++i) {
    std::array<ledseqStep_t, 3> *ledStep =
        new std::array<ledseqStep_t, 3>{{{true, LEDSEQ_WAITMS(1)},
                                         {false, LEDSEQ_WAITMS(abs(5 * i - 1))},
                                         {true, LEDSEQ_LOOP}}};

    context[i] = {.sequence = reinterpret_cast<ledseqStep_t *>(ledStep),
                  .led = static_cast<led_t>(LED(kLedGreenLeft))};

    ledseqRegisterSequence(&context[i]);
  }
  for (int i = 4; i >= 0; --i) {
    std::array<ledseqStep_t, 3> *ledStep =
        new std::array<ledseqStep_t, 3>{{{true, LEDSEQ_WAITMS(1)},
                                         {false, LEDSEQ_WAITMS(abs(5 * i - 1))},
                                         {true, LEDSEQ_LOOP}}};

    context[9 - i] = {.sequence = reinterpret_cast<ledseqStep_t *>(ledStep),
                      .led = static_cast<led_t>(LED(kLedRedLeft))};

    ledseqRegisterSequence(&context[9 - i]);
  }
}

void flashCorrectLed(void *) {
  Timer::delayMs(3000);
  registerColors();
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

    std::vector<float>::iterator itr =
        std::find(droneDistances.begin(), droneDistances.end(),
                  Drone::getEmbeddedDrone().m_data.distanceFromTakeoff);

    float distance = std::distance(droneDistances.begin(), itr);

    if (distance != 0 && droneDistances.size() != 0) {
      float divisionSize =
          (((float)context.size()) / ((float)droneDistances.size() - 1));

      ledseqRunBlocking(&context[round(distance * divisionSize)]);

    } else {
      ledseqRunBlocking(&context[0]);
    }

    Timer::delayMs(1000);
  }
}
}  // namespace P2P

void initColorGradient() {
  xTaskCreate(P2P::flashCorrectLed, "ColorGradientTask",
              configMINIMAL_STACK_SIZE, nullptr, 0, nullptr);
  isInit = true;
}
bool testColorGradient() { return isInit; };
