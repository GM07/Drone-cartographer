#include "sensors/simulation_sensors.h"

SimulationSensors::SimulationSensors(CCrazyflieSensing* ccrazyflieSensing)
    : m_ccrazyflieSensing(ccrazyflieSensing) {}

float SimulationSensors::getFrontDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kFront);
  return iterDistRead->second;
}

float SimulationSensors::getLeftDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kLeft);
  return iterDistRead->second;
}

float SimulationSensors::getRightDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kRight);
  return iterDistRead->second;
}

float SimulationSensors::getBackDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kBack);
  return iterDistRead->second;
}

float SimulationSensors::getPosX() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetX();
}

float SimulationSensors::getPosY() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetY();
}

float SimulationSensors::getPosZ() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetZ();
}

float SimulationSensors::getBatteryLevel() const {
  return m_ccrazyflieSensing->m_pcBattery->GetReading().AvailableCharge;
}
