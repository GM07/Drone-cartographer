#ifndef STUB_CCI_POSITIONING_SENSOR_H
#define STUB_CCI_POSITIONING_SENSOR_H

#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>

namespace argos {

class StubCCIPositioningSensor : public CCI_PositioningSensor {
 public:
  virtual ~StubCCIPositioningSensor() = default;
};

}  // namespace argos

#endif