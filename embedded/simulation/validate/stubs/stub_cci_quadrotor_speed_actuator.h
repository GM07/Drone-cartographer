#ifndef STUB_CCI_QUADROTOR_SPEED_ACTUATOR
#define STUB_CCI_QUADROTOR_SPEED_ACTUATOR

#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_speed_actuator.h>

#include "gmock/gmock.h"

using ::argos::CCI_QuadRotorSpeedActuator;
using ::argos::CRadians;
using ::argos::CVector3;

class StubCCIQuadrotorSpeedActuator : public CCI_QuadRotorSpeedActuator {
 public:
  virtual ~StubCCIQuadrotorSpeedActuator(){};
  virtual void SetRotationalSpeed(const CRadians& c_speed){};

  MOCK_METHOD1(SetLinearVelocity, void(const CVector3&));
};

#endif