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

static std::vector<ledseqContext_t *> redSequences;
static std::vector<ledseqContext_t *> greenSequences;
static ledseqContext_t test[10] = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}};
static ledseqContext_t greenContext[10] = {{}, {}, {}, {}, {},
                                           {}, {}, {}, {}, {}};

void blinkLED(LED led) {
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

  test[0].sequence = ledStep.data();
  test[0].led = static_cast<led_t>(led);

  ledseqRegisterSequence(&test[0]);
  ledseqRun(&test[0]);
}

void registerColors() {
  for (int i = 0; i < 10; i++) {
    static std::array<ledseqStep_t, 3> ledStep{
        {{true, LEDSEQ_WAITMS(1 + i * 100)},
         {false, LEDSEQ_WAITMS(0)},
         {true, LEDSEQ_LOOP}}};

    test[i].sequence = ledStep.data();
    test[i].led = static_cast<led_t>(LED(kLedRedLeft));

    greenContext[i].sequence = ledStep.data();
    greenContext[i].led = static_cast<led_t>(LED(kLedGreenLeft));

    redSequences.push_back(&test[i]);
    greenSequences.push_back(&greenContext[i]);
  }

  for (auto it = redSequences.begin(); it < redSequences.end(); it++) {
    ledseqRegisterSequence(*it);
    ledseqRun(redSequences[0]);
  }

  for (auto it = greenSequences.begin(); it < greenSequences.end(); it++) {
    ledseqRegisterSequence(*it);
    ledseqRun(greenSequences[0]);
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

    //    blinkLED(LED(kLedBlueLeft));

    std::vector<float>::iterator itr =
        std::find(droneDistances.begin(), droneDistances.end(),
                  Drone::getEmbeddedDrone().m_data.distanceFromTakeoff);

    float distance = std::distance(droneDistances.begin(), itr);

    if (distance != 0) {
      // float divisionSize =
      //(((float)greenSequences.size()) / ((float)droneDistances.size() - 1));

      // distance * divisionSize ceil

      // ledseqRun(redSequences[round(distance * divisionSize)]);
    } else {
      // ledseqRun(redSequences[0]);
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
