#include <algorithm>
#include <memory>
#include <vector>

#include "components/drone.h"
#include "utils/time.h"

extern "C" {
#include "FreeRTOS.h"
#include "components/crazyflie_task.h"
#include "ledseq.h"
#include "task.h"
}

namespace {
bool isInit;

}  // namespace

namespace P2P {

std::array<ledseqContext_t, 10> greenContext;
std::array<ledseqContext_t, 10> redContext;

void registerColors() {
  for (int i = 0; i < 10; ++i) {
    std::array<ledseqStep_t, 3> *ledStep =
        new std::array<ledseqStep_t, 3>{{{true, LEDSEQ_WAITMS(1 - i / 9)},
                                         {false, LEDSEQ_WAITMS(abs(i * 2))},
                                         {true, LEDSEQ_LOOP}}};

    greenContext[i] = {.sequence = reinterpret_cast<ledseqStep_t *>(ledStep),
                       .led = static_cast<led_t>(LED(kLedGreenLeft))};

    redContext[i] = {.sequence = reinterpret_cast<ledseqStep_t *>(ledStep),
                     .led = static_cast<led_t>(LED(kLedRedLeft))};

    ledseqRegisterSequence(&redContext[i]);
    ledseqRegisterSequence(&greenContext[i]);
  }
}

void flashCorrectLed(void *) {
  Time::delayMs(3000);
  const int CONTEXT_ARRAY_MAX_INDEX = 9;
  int lastGreenContextId = 0;
  while (true) {
    std::vector<float> droneDistances;

    std::transform(Drone::getEmbeddedDrone().m_peerData.begin(),
                   Drone::getEmbeddedDrone().m_peerData.end(),
                   std::back_inserter(droneDistances),
                   [](std::pair<size_t, DroneData> const &pair) {
                     return pair.second.m_distanceFromTakeoff;
                   });

    droneDistances.push_back(
        Drone::getEmbeddedDrone().m_data.m_distanceFromTakeoff);
    std::sort(droneDistances.begin(), droneDistances.end());

    std::vector<float>::iterator itr =
        std::find(droneDistances.begin(), droneDistances.end(),
                  Drone::getEmbeddedDrone().m_data.m_distanceFromTakeoff);

    int distance = std::distance(droneDistances.begin(), itr);
    ledseqStop(&greenContext[lastGreenContextId]);
    ledseqStop(&redContext[CONTEXT_ARRAY_MAX_INDEX - lastGreenContextId]);
    if (droneDistances.size() != 1) {
      float divisionSize =
          (CONTEXT_ARRAY_MAX_INDEX + 1) / ((float)droneDistances.size() - 1);

      int index =
          (int)std::clamp<double>(round(distance * divisionSize - 1), 0, 9);
      lastGreenContextId = index;

      ledseqRunBlocking(&greenContext[index]);
      ledseqRunBlocking(&redContext[CONTEXT_ARRAY_MAX_INDEX - index]);

    } else {
      lastGreenContextId = 0;
      ledseqRunBlocking(&greenContext[0]);
      ledseqRunBlocking(&redContext[CONTEXT_ARRAY_MAX_INDEX - 0]);
    }

    Time::delayMs(1000);
  }
}
}  // namespace P2P

void initColorGradient() {
  P2P::registerColors();

  xTaskCreate(P2P::flashCorrectLed, "ColorGradientTask",
              configMINIMAL_STACK_SIZE * 2, nullptr, 0, nullptr);
  isInit = true;
}
bool testColorGradient() { return isInit; };
