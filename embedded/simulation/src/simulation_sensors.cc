#include "sensors/simulation_sensors.h"

SimulationSensors::SimulationSensors(CCrazyflieSensing* ccrazyflieSensing)
    : m_ccrazyflieSensing(ccrazyflieSensing) {}

[[nodiscard]] float SimulationSensors::getFrontDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kFront);
  return iterDistRead->second;
}

[[nodiscard]] float SimulationSensors::getLeftDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kLeft);
  return iterDistRead->second;
}

[[nodiscard]] float SimulationSensors::getRightDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kRight);
  return iterDistRead->second;
}

[[nodiscard]] float SimulationSensors::getBackDistance() const {
  auto iterDistRead =
      m_ccrazyflieSensing->m_pcDistance->GetReadingsMap().begin();

  std::advance(iterDistRead, ReadingsMapSensorIndex::kBack);
  return iterDistRead->second;
}

[[nodiscard]] float SimulationSensors::getPosX() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetX();
}

[[nodiscard]] float SimulationSensors::getPosY() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetY();
}

[[nodiscard]] float SimulationSensors::getPosZ() const {
  return m_ccrazyflieSensing->m_pcPos->GetReading().Position.GetZ();
}

[[nodiscard]] float SimulationSensors::getBatteryLevel() const {
  constexpr float kToPercentage = 100.0F;
  return static_cast<float>(
             m_ccrazyflieSensing->m_pcBattery->GetReading().AvailableCharge) *
         kToPercentage;
}