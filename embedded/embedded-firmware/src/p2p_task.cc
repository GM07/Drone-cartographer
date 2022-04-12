extern "C" {
#include "FreeRTOS.h"
#include "components/crazyflie_task.h"
#include "task.h"
}

#include <algorithm>

#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/time.h"

namespace {
bool p2pIsInit = false;
}

/////////////////////////////////////////////////////////////////////////
[[nodiscard]] bool isValidP2PPacket(DroneData& data) {
  return std::equal(data.m_magicHeader.begin(), data.m_magicHeader.end(),
                    Drone::getEmbeddedDrone().m_data.m_magicHeader.begin());
}

/////////////////////////////////////////////////////////////////////////
void p2pcallbackHandler(P2PPacket* p) {
  {
    P2PPacket packet;
    memcpy(&packet, p, sizeof(packet));
    DroneData data(*reinterpret_cast<DroneData*>(&packet.data));  // NOLINT

    if (!isValidP2PPacket(data)) {
      return;
    }

    data.m_range = p->rssi;
    data.m_direction.rotate(
        Drone::getEmbeddedDrone().getController()->getOrientation());
    Drone::getEmbeddedDrone().m_peerData.insert_or_assign(data.m_id, data);
  }
}

/////////////////////////////////////////////////////////////////////////
void p2pTaskWrapper(void* /*parameter*/) {
  constexpr int32_t kP2pTaskDelay = 200;
  constexpr uint32_t kInitDelay = 3000;

  Time::delayMs(kInitDelay);
  Drone& drone = Drone::getEmbeddedDrone();

  while (true) {
    DroneData dataToSend = drone.m_data;
    dataToSend.m_direction.rotate(
        Drone::getEmbeddedDrone().getController()->getOrientation());
    drone.getController()->sendP2PMessage(static_cast<void*>(&dataToSend),
                                          sizeof(drone.m_data));

    Time::delayMs(kP2pTaskDelay);
  }
}

/////////////////////////////////////////////////////////////////////////
bool p2pTaskTest() { return p2pIsInit; }

/////////////////////////////////////////////////////////////////////////
void p2pTaskInit() {
  xTaskCreate(p2pTaskWrapper, "P2P_TASK_NAME", configMINIMAL_STACK_SIZE,
              nullptr, 0, nullptr);

  p2pRegisterCB(p2pcallbackHandler);

  p2pIsInit = true;
}
