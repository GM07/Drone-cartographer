#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>

#include "gmock/gmock.h"

class StubCCIQuadrotorPositionActuator : public CCI_QuadRotorPositionActuator {
 public:
  virtual ~StubCCIQuadrotorPositionActuator(){};
  virtual void SetRelativePosition(const CVector3& c_pos){};
  virtual void SetAbsoluteYaw(const CRadians& c_yaw){};
  virtual void SetRelativeYaw(const CRadians& c_yaw){};
  virtual void CreateLuaState(lua_State* pt_lua_state){};

  MOCK_METHOD1(SetAbsolutePosition, void(const CVector3&));
};
