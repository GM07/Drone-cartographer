/*
 * Include some necessary headers.
 */
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the crazyflie distance sensor */
#include <argos3/plugins/robots/crazyflie/control_interface/ci_crazyflie_distance_scanner_sensor.h>
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
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/math/rng.h>

#include "crazyflie_sensing.h"

/**
 * @file argos.cc
 * File used to mock argos functions :(
 */

namespace argos {

///////////////////////////////////////////////
CByteArray::CByteArray(const unsigned char*, unsigned long){};

///////////////////////////////////////////////
void CCI_RangeAndBearingActuator::SetData(const CByteArray&) {}

///////////////////////////////////////////////
const std::vector<CCI_RangeAndBearingSensor::SPacket>&
CCI_RangeAndBearingSensor::GetReadings() const {}

///////////////////////////////////////////////
CCI_Controller::~CCI_Controller() = default;  // Let's hope this is fine.

///////////////////////////////////////////////
CRandom::CRNG* CRandom::CreateRNG(const std::string& str_category) {}

///////////////////////////////////////////////
CARGoSLog LOG(std::cout, SLogColor(), false);

///////////////////////////////////////////////
void CCI_CrazyflieDistanceScannerSensor::CreateLuaState(lua_State*) {}
const CCI_CrazyflieDistanceScannerSensor::TReadingsMap&
CCI_CrazyflieDistanceScannerSensor::GetReadingsMap() {
  return m_tReadingsMap;
}
void CCI_CrazyflieDistanceScannerSensor::ReadingsToLuaState(lua_State*) {}
void CCI_QuadRotorSpeedActuator::CreateLuaState(lua_State*) {}
void CCI_RangeAndBearingActuator::CreateLuaState(lua_State*) {}
void CCI_RangeAndBearingSensor::CreateLuaState(lua_State*) {}
void CCI_RangeAndBearingSensor::ReadingsToLuaState(lua_State*) {}
void CCI_PositioningSensor::CreateLuaState(lua_State*) {}
void CCI_PositioningSensor::ReadingsToLuaState(lua_State*) {}
void CCI_BatterySensor::CreateLuaState(lua_State*) {}
void CCI_BatterySensor::ReadingsToLuaState(lua_State*) {}
const CCI_BatterySensor::SReading& CCI_BatterySensor::GetReading() const {
  return m_sReading;
}

}  // namespace argos
