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

namespace P2PColorGradient {
std::array<ledseqContext_t, 10> greenContext;
std::array<ledseqContext_t, 10> redContext;

void registerColors() {
  for (int i = 0; i < 10; ++i) {
    std::array<ledseqStep_t, 3> *ledStep =
        new std::array<ledseqStep_t, 3>{{{true, LEDSEQ_WAITMS(1 - i / 9)},
                                         {false, LEDSEQ_WAITMS(i * 2)},
                                         {true, LEDSEQ_LOOP}}};

    greenContext[i] = {.sequence = ledStep->data(),
                       .led = static_cast<led_t>(LED(kLedGreenLeft))};

    redContext[i] = {.sequence = ledStep->data(),
                     .led = static_cast<led_t>(LED(kLedRedLeft))};

    ledseqRegisterSequence(&redContext[i]);
    ledseqRegisterSequence(&greenContext[i]);
  }
}

void flashP2PLed(void *) {
  constexpr int kContextArrayMaxIndex = 9;
  int lastGreenContextId = 0;
  bool isActiveContext = false;
  while (true) {
    if (!Drone::getEmbeddedDrone().m_p2pColorGradientIsActive) {
      if (isActiveContext) {
        ledseqStop(&greenContext[lastGreenContextId]);
        ledseqStop(&redContext[kContextArrayMaxIndex - lastGreenContextId]);
        isActiveContext = false;
      }
      Time::delayMs(1000);

      continue;
    }
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
    ledseqStop(&redContext[kContextArrayMaxIndex - lastGreenContextId]);
    if (droneDistances.size() != 1) {
      float divisionSize =
          (kContextArrayMaxIndex + 1) / ((float)droneDistances.size() - 1);

      int index =
          (int)std::clamp<double>(round(distance * divisionSize - 1), 0, 9);
      lastGreenContextId = index;

      ledseqRunBlocking(&greenContext[index]);
      ledseqRunBlocking(&redContext[kContextArrayMaxIndex - index]);

    } else {
      lastGreenContextId = 0;
      ledseqRunBlocking(&greenContext[0]);
      ledseqRunBlocking(&redContext[kContextArrayMaxIndex - 0]);
    }
    isActiveContext = true;

    Time::delayMs(1000);
  }
}
}  // namespace P2PColorGradient

void initColorGradient() {
  P2PColorGradient::registerColors();

  xTaskCreate(P2PColorGradient::flashP2PLed, "ColorGradientTask",
              configMINIMAL_STACK_SIZE * 2, nullptr, 0, nullptr);
  isInit = true;
}
bool ColorGradientTest() { return isInit; };
