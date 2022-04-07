extern "C" {
#include "FreeRTOS.h"
#include "commander.h"
#include "components/crazyflie_task.h"
#include "config.h"
#include "debug.h"
#include "log.h"
#include "param_logic.h"
#include "static_mem.h"
#include "supervisor.h"
#include "task.h"
}

#include <cmath>

#include "app_main.h"
#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "utils/time.h"

namespace {

bool commIsInit = false;

}  // namespace

static_assert(kDroneSpeed == 0.25,
              "You changed the drone speed! Was that really intended ? ");

/////////////////////////////////////////////////////////////////////////
Drone& Drone::getEmbeddedDrone() {
  static Drone drone(std::make_shared<FirmwareController>());
  return drone;
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerTaskWrapper(void* /*parameter*/) {
  Drone::getEmbeddedDrone().communicationManagerTask();
}

/////////////////////////////////////////////////////////////////////////
void communicationManagerInit() {
  xTaskCreate(communicationManagerTaskWrapper, "COMMUNICATION_MANAGER_NAME",
              configMINIMAL_STACK_SIZE, nullptr, 0, nullptr);
  commIsInit = true;
}

/////////////////////////////////////////////////////////////////////////
bool communicationManagerTest() { return commIsInit; }

/////////////////////////////////////////////////////////////////////////
void updateCrashStatus() {
  if (supervisorIsTumbled()) {
    Drone::getEmbeddedDrone().getController()->m_state = State::kCrash;
  }
}

/////////////////////////////////////////////////////////////////////////
void enableCrtpHighLevelCommander() {
  paramVarId_t paramIdCommanderEnHighLevel =
      paramGetVarId("commander", "enHighLevel");
  paramSetInt(paramIdCommanderEnHighLevel, 1);
}

/////////////////////////////////////////////////////////////////////////
uint8_t logDroneState(uint32_t /*timestamp*/, void* /*data*/) {
  return static_cast<uint8_t>(
      Drone::getEmbeddedDrone().getController()->m_state);
}

/////////////////////////////////////////////////////////////////////////
void addCustomLoggingVariables() {
  static logByFunction_t droneStateLogger = {.acquireUInt8 = logDroneState,
                                             .data = nullptr};
  LOG_GROUP_START(custom)                                              // NOLINT
  LOG_ADD_BY_FUNCTION(LOG_UINT8, droneCustomState, &droneStateLogger)  // NOLINT
  LOG_GROUP_STOP(custom)
}

/////////////////////////////////////////////////////////////////////////
extern "C" void appMain() {
  ledClearAll();
  addCustomLoggingVariables();
  enableCrtpHighLevelCommander();
  Drone::getEmbeddedDrone().initDrone();

  while (true) {
    Drone::getEmbeddedDrone().getController()->updateSensorsData();
    updateCrashStatus();

    Drone::getEmbeddedDrone().step();
  }
}
