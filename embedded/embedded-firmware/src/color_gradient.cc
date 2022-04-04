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

constexpr int kContextArraySize = 10;

std::array<ledseqContext_t, kContextArraySize> greenContext;
std::array<ledseqContext_t, kContextArraySize> redContext;

void registerColors() {
  constexpr int kStepCount = 3;

  for (int i = 0; i < kContextArraySize; ++i) {
    std::array<ledseqStep_t, kStepCount> *ledStep =
        new std::array<ledseqStep_t, kStepCount>{
            {{true, LEDSEQ_WAITMS(1 - i / kContextArraySize - 1)},
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
    int positionCounter = 0;
    for (auto itr = Drone::getEmbeddedDrone().m_peerData.begin();
         itr != Drone::getEmbeddedDrone().m_peerData.end(); ++itr) {
      if (Drone::getEmbeddedDrone().m_data.m_distanceFromTakeoff >
          itr->second.m_distanceFromTakeoff)
        positionCounter++;
    }

    ledseqStop(&greenContext[lastGreenContextId]);
    ledseqStop(&redContext[kContextArrayMaxIndex - lastGreenContextId]);
    if (droneDistances.size() != 1) {
      float divisionSize =
          (kContextArrayMaxIndex + 1) / ((float)droneDistances.size() - 1);

      int index = (int)std::clamp<double>(
          round(positionCounter * divisionSize - 1), 0, kContextArrayMaxIndex);
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
