#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>

/////////////////////////////////////////////////////
const argos::CCI_PositioningSensor::SReading&
argos::CCI_PositioningSensor::GetReading() const {
  static argos::CCI_PositioningSensor::SReading reading{CVector3(),
                                                        CQuaternion()};
  return reading;
}