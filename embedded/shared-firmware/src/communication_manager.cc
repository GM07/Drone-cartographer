#include "components/drone.h"
#include "utils/commands.h"
#include "utils/time.h"

/////////////////////////////////////////////////////////////////////////
void Drone::communicationManagerTask() {
  constexpr uint32_t kCommunicationDelay = 50;
  constexpr uint32_t kInitDelay = 3000;

  Time::delayMs(kInitDelay);

  while (m_controller->m_state != State::kDead) {
    if (m_controller->receiveMessage(&m_messageRX, sizeof(m_messageRX)) != 0U) {
      handleCommand(static_cast<Command>(m_messageRX[0]));
    }

    Time::delayMs(kCommunicationDelay);
  }
}
