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

namespace P2PGradient {

constexpr int kContextArraySize = 10;

std::array<ledseqContext_t, kContextArraySize> greenContext;
std::array<ledseqContext_t, kContextArraySize> redContext;

void registerColors() {
  constexpr int kStepCount = 3;

  for (int i = 0; i < kContextArraySize; ++i) {
    std::array<ledseqStep_t, kStepCount> *ledStep =
        new std::array<ledseqStep_t, kStepCount>{
            {{true, LEDSEQ_WAITMS(1 - i / (kContextArraySize - 1))},
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
    Time::delayMs(1000);

    if (!Drone::getEmbeddedDrone().m_p2pColorGradientIsActive) {
      if (isActiveContext) {
        ledseqStop(&greenContext[lastGreenContextId]);
        ledseqStop(&redContext[kContextArrayMaxIndex - lastGreenContextId]);

        isActiveContext = false;
      }

      continue;
    }

    int positionCounter = 0;
    for (auto itr = Drone::getEmbeddedDrone().m_peerData.begin();
         itr != Drone::getEmbeddedDrone().m_peerData.end(); ++itr) {
      if (Drone::getEmbeddedDrone().m_data.m_distanceFromTakeoff >
          itr->second.m_distanceFromTakeoff)
        positionCounter++;
    }

    ledseqStop(&greenContext[lastGreenContextId]);
    ledseqStop(&redContext[kContextArrayMaxIndex - lastGreenContextId]);
    if (Drone::getEmbeddedDrone().m_peerData.size() != 0) {
      float divisionSize = (kContextArrayMaxIndex + 1) /
                           Drone::getEmbeddedDrone().m_peerData.size();

      size_t index = std::min<size_t>(round(positionCounter * divisionSize - 1),
                                      kContextArrayMaxIndex);
      lastGreenContextId = index;

      ledseqRun(&greenContext[index]);
      ledseqRun(&redContext[kContextArrayMaxIndex - index]);

    } else {
      lastGreenContextId = 0;
      ledseqRun(&greenContext[0]);
      ledseqRun(&redContext[kContextArrayMaxIndex]);
    }
    isActiveContext = true;
  }
}

}  // namespace P2PGradient

void colorGradientTaskInit() {
  P2PGradient::registerColors();

  xTaskCreate(P2PGradient::flashP2PLed, "ColorGradientTask",
              configMINIMAL_STACK_SIZE * 2, nullptr, 0, nullptr);
  isInit = true;
}

bool colorGradientTest() { return isInit; };
