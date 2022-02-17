#include "components/drone.h"
#include "utils/commands.h"
#include "utils/timer.h"

/////////////////////////////////////////////////////////////////////////
void Drone::communicationManagerTask() {
  Timer::delayMs(3000);

  while (true) {
    if (m_controller->receiveMessage(&m_messageRX, sizeof(m_messageRX))) {
      handleCommand(static_cast<Command>(m_messageRX[0]), &m_messageRX[1],
                    sizeof(m_messageRX) - sizeof(Command));

      // m_controller->sendMessage(&successfulCommand,
      // sizeof(successfulCommand));
    }

    if (m_controller->state == State::kDead) {
      return;
    }

    Timer::delayMs(50);
  }
}
