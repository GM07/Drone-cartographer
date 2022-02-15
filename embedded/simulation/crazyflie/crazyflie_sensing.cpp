// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/simulation_controller.h"
#pragma GCC diagnostic pop

/* Include the controller definition */
#include "crazyflie_sensing.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
/* Logging */
#include <argos3/core/utility/logging/argos_log.h>

/* Communication */
#include <boost/asio.hpp>

#include "utils/timer.h"

/****************************************/
/****************************************/

CCrazyflieSensing::CCrazyflieSensing()
    : m_pcDistance(NULL),
      m_pcPropellers(NULL),
      m_pcRABA(NULL),
      m_pcRABS(NULL),
      m_pcPos(NULL),
      m_pcBattery(NULL),
      m_pcRNG(NULL),
      m_uiCurrentStep(0),
      m_communicationThread(nullptr),
      m_drone(std::make_shared<SimulationController>(this)) {}

/****************************************/
/****************************************/

void CCrazyflieSensing::Init(TConfigurationNode& t_node) {
  // Start socket connection
  m_communicationThread = std::make_unique<std::thread>(
      &CCrazyflieSensing::threadTasksWrapper, this);

  try {
    /*
     * Initialize sensors/actuators
     */
    m_pcDistance = GetSensor<CCI_CrazyflieDistanceScannerSensor>(
        "crazyflie_distance_scanner");
    m_pcPropellers =
        GetActuator<CCI_QuadRotorPositionActuator>("quadrotor_position");
    /* Get pointers to devices */
    m_pcRABA = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
    m_pcRABS = GetSensor<CCI_RangeAndBearingSensor>("range_and_bearing");
    try {
      m_pcPos = GetSensor<CCI_PositioningSensor>("positioning");
    } catch (CARGoSException& ex) {
    }
    try {
      m_pcBattery = GetSensor<CCI_BatterySensor>("battery");
    } catch (CARGoSException& ex) {
    }
  } catch (CARGoSException& ex) {
    THROW_ARGOSEXCEPTION_NESTED(
        "Error initializing the crazyflie sensing controller for robot \""
            << GetId() << "\"",
        ex);
  }
  /*
   * Initialize other stuff
   */
  /* Create a random number generator. We use the 'argos' category so
     that creation, reset, seeding and cleanup are managed by ARGoS. */
  m_pcRNG = CRandom::CreateRNG("argos");

  m_uiCurrentStep = 0;
  Reset();
}

/****************************************/
/****************************************/

void CCrazyflieSensing::ControlStep() {
  if (!m_communicationThread) attemptSocketConnection();
  if (m_communicationThread) m_drone.step();

  printLogs();

  m_uiCurrentStep++;
}

/****************************************/
/****************************************/

void CCrazyflieSensing::Reset() {}

/****************************************/
/****************************************/
void CCrazyflieSensing::printLogs() {
  std::lock_guard<std::mutex> logMutex(logBufferMutex);
  LOG << logBuffer.str();
  logBuffer.str(std::string());
}

void CCrazyflieSensing::attemptSocketConnection() {
  while (true) {
    try {
      if (m_drone.getController()->state != State::kDead) {
        m_drone.getController()->initCommunicationManager();
      }
      return;
    } catch (const boost::system::system_error& error) {
      // We try to connect to socket 4 times per second
      // Don't need to try more
      Timer::delayMs(250);
    }
  }
}

CCrazyflieSensing::~CCrazyflieSensing() {
  m_drone.getController()->state = State::kDead;
  if (m_communicationThread) m_communicationThread->join();
}

/****************************************/
void CCrazyflieSensing::threadTasksWrapper() {
  attemptSocketConnection();
  m_drone.communicationManagerTask();
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
