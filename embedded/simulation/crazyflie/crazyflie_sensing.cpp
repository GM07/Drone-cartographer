#include "controllers/simulation_controller.h"

/* Include the controller definition */
#include "crazyflie_sensing.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
/* Logging */
#include <argos3/core/utility/logging/argos_log.h>

#include "utils/time.h"

using ::argos::CARGoSException;
using ::argos::CRandom;

/****************************************/
/****************************************/

CCrazyflieSensing::CCrazyflieSensing()
    : m_communicationThread(nullptr),
      m_drone(std::make_shared<SimulationController>(this)) {}

/****************************************/
/****************************************/

void CCrazyflieSensing::Init(argos::TConfigurationNode& /*t_node*/) {
  // Start socket connection
  m_communicationThread = std::make_unique<std::thread>(
      &CCrazyflieSensing::threadTasksWrapper, this);

  m_P2PThread =
      std::make_unique<std::thread>(&CCrazyflieSensing::p2pTaskWrapper, this);

  try {
    /*
     * Initialize sensors/actuators
     */
    m_pcDistance = GetSensor<argos::CCI_CrazyflieDistanceScannerSensor>(
        "crazyflie_distance_scanner");
    m_pcPropellers =
        GetActuator<argos::CCI_QuadRotorSpeedActuator>("quadrotor_speed");
    /* Get pointers to devices */
    m_pcRABA =
        GetActuator<argos::CCI_RangeAndBearingActuator>("range_and_bearing");
    m_pcRABS = GetSensor<argos::CCI_RangeAndBearingSensor>("range_and_bearing");
    try {
      m_pcPos = GetSensor<argos::CCI_PositioningSensor>("positioning");
    } catch (CARGoSException& ex) {
    }
    try {
      m_pcBattery = GetSensor<argos::CCI_BatterySensor>("battery");
    } catch (CARGoSException& ex) {
    }
  } catch (CARGoSException& ex) {
    THROW_ARGOSEXCEPTION_NESTED(
        "Error initializing the crazyflie sensing controller for robot \""
            << GetId() << "\"",
        ex);
  }
  /* Create a random number generator. We use the 'argos' category so
     that creation, reset, seeding and cleanup are managed by ARGoS. */
  m_pcRNG = CRandom::CreateRNG("argos");

  m_uiCurrentStep = 0;

  // Initialize drone
  m_drone.initDrone();

  Reset();
}

/****************************************/
/****************************************/
void CCrazyflieSensing::ControlStep() {
  m_drone.getController()->updateSensorsData();
  m_drone.step();
  printLogs();

  ++m_uiCurrentStep;
}

/****************************************/
/****************************************/
void CCrazyflieSensing::printLogs() {
  std::lock_guard<decltype(logBufferMutex)> logMutex(logBufferMutex);
  argos::LOG << logBuffer.str();
  logBuffer.str(std::string());
}

/****************************************/
void CCrazyflieSensing::attemptSocketConnection() {
  // We try to connect to socket 4 times per second
  // Don't need to try more
  constexpr uint32_t kDroneDelay = 250;

  while (m_drone.getController()->m_state != State::kDead) {
    try {
      m_drone.getController()->initCommunicationManager();
      break;
    } catch (const boost::system::system_error& error) {
      Time::delayMs(kDroneDelay);
    }
  }
}

/****************************************/
CCrazyflieSensing::~CCrazyflieSensing() {
  m_drone.getController()->m_state = State::kDead;
  if (m_communicationThread != nullptr) {
    m_communicationThread->join();
  }

  if (m_P2PThread != nullptr) {
    m_P2PThread->join();
  }
}

/****************************************/
void CCrazyflieSensing::threadTasksWrapper() {
  attemptSocketConnection();
  m_drone.communicationManagerTask();
}

void CCrazyflieSensing::p2pTaskWrapper() {
  int32_t kDelayBetweenP2PMessage = 50;
  while (m_drone.getController()->m_state != State::kDead) {
    Time::delayMs(kDelayBetweenP2PMessage);
    m_drone.getController()->sendP2PMessage(static_cast<void*>(&m_drone.m_data),
                                            sizeof(m_drone.m_data));
  }
}

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the XML configuration file to refer to
 * this controller.
 * When ARGoS reads that string in the XML file, it knows which controller
 * class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CCrazyflieSensing, "crazyflie_sensing_controller")
