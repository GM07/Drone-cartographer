// TODO doit être enlevé lorsque le controlleur sera implémenté
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "controllers/abstract_controller.h"
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

#include "components/communication_manager.h"

/* Navigation */
#include "components/navigation_system.h"

/****************************************/
/****************************************/

CCrazyflieSensing::CCrazyflieSensing()
    : m_pcDistance(NULL),
      m_pcPropellers(NULL),
      m_pcRNG(NULL),
      m_pcRABA(NULL),
      m_pcRABS(NULL),
      m_pcPos(NULL),
      m_pcBattery(NULL),
      m_uiCurrentStep(0) {}

/****************************************/
/****************************************/

void CCrazyflieSensing::Init(TConfigurationNode& t_node) {
  // Provide drone instance to the controller
  std::dynamic_pointer_cast<SimulationController>(
      AbstractController::getController(m_strId))
      ->setSimulationDroneInstance(this);

  // Start socket connection
  AbstractController::getController(m_strId)->initCommunicationManager();
  // Run Thread managing communication
  std::thread communicationThread(
      CommunicationManager::communicationManagerTask,
      static_cast<void*>(&m_strId));
  communicationThread.detach();

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
  Navigation::step(m_strId);

  m_uiCurrentStep++;
}

/****************************************/
/****************************************/

void CCrazyflieSensing::Reset() {}

/****************************************/
/****************************************/

CCrazyflieSensing::~CCrazyflieSensing() {
  AbstractController::getController()->state = State::kDead;
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
