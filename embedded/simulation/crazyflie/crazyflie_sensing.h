/*
 * AUTHORS: Carlo Pinciroli <cpinciro@ulb.ac.be>
 *          Pierre-Yves Lajoie <lajoie.py@gmail.com>
 *
 * An example crazyflie drones sensing.
 *
 * This controller is meant to be used with the XML file:
 *    experiments/foraging.argos
 */

#ifndef CRAZYFLIE_SENSING_H
#define CRAZYFLIE_SENSING_H

/*
 * Include some necessary headers.
 */
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the crazyflie distance sensor */
#include <argos3/plugins/robots/crazyflie/control_interface/ci_crazyflie_distance_scanner_sensor.h>
/* Definition of the crazyflie position actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
/* Definition of the crazyflie position actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_speed_actuator.h>
/* Definition of the crazyflie position sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
/* Definition of the crazyflie range and bearing actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
/* Definition of the crazyflie range and bearing sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
/* Definition of the crazyflie battery sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>
/* Definitions for random number generation */
#include <argos3/core/utility/math/rng.h>

#ifdef GTEST
#include "socket.h"
#else
#include <boost/asio.hpp>
#endif

#include <mutex>
#include <thread>

#include "components/drone.h"

/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CCrazyflieSensing : public argos::CCI_Controller {
 public:
  /* Class constructor. */
  CCrazyflieSensing();
  CCrazyflieSensing(const CCrazyflieSensing& other) = delete;
  CCrazyflieSensing(CCrazyflieSensing&& other) = delete;
  CCrazyflieSensing operator=(const CCrazyflieSensing& other) = delete;
  CCrazyflieSensing operator=(CCrazyflieSensing&& other) = delete;
  /* Class destructor. */
  ~CCrazyflieSensing() override;

  /*
   * This function initializes the controller.
   * The 't_node' variable points to the <parameters> section in the XML
   * file in the <controllers><footbot_foraging_controller> section.
   */
  void Init(argos::TConfigurationNode& t_node) override;

  /*
   * This function is called once every time step.
   * The length of the time step is set in the XML file.
   */
  void ControlStep() override;

  /*
   * This function resets the controller to its state right after the
   * Init().
   * It is called when you press the reset button in the GUI.
   */
  void Reset() override;

  /*
   * Called to cleanup what done by Init() when the experiment finishes.
   * In this example controller there is no need for clean anything up,
   * so the function could have been omitted. It's here just for
   * completeness.
   */
  void Destroy() override {}

  static void printLogs();

  /* Pointer to the crazyflie distance sensor */
  argos::CCI_CrazyflieDistanceScannerSensor* m_pcDistance{nullptr};

  /* Pointer to the position actuator */
  // argos::CCI_QuadRotorPositionActuator* m_pcPropellers{nullptr};

  /* Pointer to the speed actuator */
  argos::CCI_QuadRotorSpeedActuator* m_speedPropellers{nullptr};

  /* Pointer to the range and bearing actuator */
  argos::CCI_RangeAndBearingActuator* m_pcRABA{nullptr};

  /* Pointer to the range and bearing sensor */
  argos::CCI_RangeAndBearingSensor* m_pcRABS{nullptr};

  /* Pointer to the positioning sensor */
  argos::CCI_PositioningSensor* m_pcPos{nullptr};

  /* Pointer to the battery sensor */
  argos::CCI_BatterySensor* m_pcBattery{nullptr};

  /* The random number generator */
  argos::CRandom::CRNG* m_pcRNG{nullptr};

  /* Current step */
  uint32_t m_uiCurrentStep{0};

#ifndef GTEST
 private:
#endif
  void attemptSocketConnection();
  void threadTasksWrapper();

  std::unique_ptr<std::thread> m_communicationThread;
  Drone m_drone;
};

#endif
