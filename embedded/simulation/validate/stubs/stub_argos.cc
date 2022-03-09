#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/math/rng.h>

#include "crazyflie_sensing.h"

/**
 * @file argos.cc
 * File used to mock argos functions :(
 */

namespace argos {

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
void CCI_QuadRotorPositionActuator::CreateLuaState(lua_State*) {}
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
