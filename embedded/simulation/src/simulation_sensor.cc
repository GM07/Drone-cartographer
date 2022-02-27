#include "sensors/simulation_sensor.h"

SimulationSensor::SimulationSensor(CCrazyflieSensing* ccrazyflieSensing)
    : m_ccrazyflieSensing(ccrazyflieSensing) {}

float SimulationSensor::getFrontDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  return iterDistRead->second;
}

float SimulationSensor::getLeftDistance() const {
  constexpr size_t kIterOffset = 1;
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, kIterOffset);
  return iterDistRead->second;
}

float SimulationSensor::getRightDistance() const {
  constexpr size_t kIterOffset = 3;
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, kIterOffset);
  return iterDistRead->second;
}

float SimulationSensor::getBackDistance() const {
  constexpr size_t kIterOffset = 2;
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, kIterOffset);
  return iterDistRead->second;
}

float SimulationSensor::getPosX() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetX();
}

float SimulationSensor::getPosY() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetY();
}

float SimulationSensor::getPosZ() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetZ();
}

float SimulationSensor::getBatteryLevel() const {
  return m_ccrazyflieSensing->m_pcBattery->GetReading().AvailableCharge;
}