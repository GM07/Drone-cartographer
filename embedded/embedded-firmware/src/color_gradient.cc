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

constexpr size_t kContextArraySize = 10;

std::array<ledseqContext_t, kContextArraySize> greenContext;
std::array<ledseqContext_t, kContextArraySize> redContext;

void registerColors() {
  constexpr size_t kStepCount = 3;

  for (int i = 0; i < kContextArraySize; ++i) {
    std::array<ledseqStep_t, kStepCount> *ledStep{
        new std::array<ledseqStep_t, kStepCount>{
            {{true, LEDSEQ_WAITMS(1 - i / (kContextArraySize - 1))},
             {false, LEDSEQ_WAITMS(i * 2)},
             {true, LEDSEQ_LOOP}}}};

    greenContext.at(i) = {.sequence = ledStep->data(),
                          .led = static_cast<led_t>(LED(kLedGreenLeft))};

    redContext.at(i) = {.sequence = ledStep->data(),
                        .led = static_cast<led_t>(LED(kLedRedLeft))};

    ledseqRegisterSequence(&redContext.at(i));
    ledseqRegisterSequence(&greenContext.at(i));
  }
}

void flashP2PLed(void * /*parameter*/) {
  constexpr size_t kContextArrayMaxIndex = kContextArraySize - 1;
  size_t lastGreenContextId = 0;
  bool isActiveContext = false;
  constexpr uint32_t kTaskDelay = 1000;

  while (true) {
    Time::delayMs(kTaskDelay);

    if (!Drone::getEmbeddedDrone().m_p2pColorGradientIsActive) {
      if (isActiveContext) {
        ledseqStop(&greenContext.at(lastGreenContextId));
        ledseqStop(&redContext.at(kContextArrayMaxIndex - lastGreenContextId));

        isActiveContext = false;
      }

      continue;
    }

    int positionCounter = std::count_if(
        Drone::getEmbeddedDrone().m_peerData.begin(),
        Drone::getEmbeddedDrone().m_peerData.end(),
        [&](std::pair<size_t, DroneData> const &pair) {
          return Drone::getEmbeddedDrone().m_data.m_distanceFromTakeoff >
                 pair.second.m_distanceFromTakeoff;
        });

    ledseqStop(&greenContext.at(lastGreenContextId));
    ledseqStop(&redContext.at(kContextArrayMaxIndex - lastGreenContextId));
    if (!Drone::getEmbeddedDrone().m_peerData.empty()) {
      const float divisionSize = static_cast<float>(kContextArraySize) /
                                 Drone::getEmbeddedDrone().m_peerData.size();

      const size_t index = std::min<size_t>(
          std::round(static_cast<float>(positionCounter) * divisionSize - 1),
          kContextArrayMaxIndex);
      lastGreenContextId = index;

      ledseqRun(&greenContext.at(index));
      ledseqRun(&redContext.at(kContextArrayMaxIndex - index));

    } else {
      lastGreenContextId = 0;
      ledseqRun(&greenContext.at(0));
      ledseqRun(&redContext.at(kContextArrayMaxIndex));
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
