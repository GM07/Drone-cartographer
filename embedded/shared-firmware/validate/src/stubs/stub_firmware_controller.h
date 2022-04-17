#include <gmock/gmock.h>

#include <unordered_map>

#include "controllers/firmware_controller.h"

class StubController : public FirmwareController {
 public:
  StubController() = default;

  MOCK_METHOD1(takeOff, void(float));
  MOCK_METHOD0(land, void());
  MOCK_METHOD0(identify, void());
  MOCK_CONST_METHOD2(sendMessage, void(void*, size_t));
  MOCK_CONST_METHOD2(receiveMessage, size_t(void*, size_t));
  MOCK_METHOD0(updateSensorsData, void());
  MOCK_CONST_METHOD0(isDroneCrashed, bool());
  MOCK_METHOD2(setVelocity, void(const Vector3D&, float));
  MOCK_CONST_METHOD0(isTrajectoryFinished, bool());
  MOCK_CONST_METHOD2(sendP2PMessage, void(void*, size_t));
  MOCK_CONST_METHOD1(receiveP2PMessage,
                     void(std::unordered_map<size_t, DroneData>&));
  MOCK_CONST_METHOD0(getId, size_t());
  MOCK_CONST_METHOD0(getMinCollisionAvoidanceDistance, float());
  MOCK_CONST_METHOD0(getMaxCollisionAvoidanceDistance, float());
  MOCK_CONST_METHOD0(hasLowBattery, bool());
  MOCK_METHOD3(setVelocity, void(const Vector3D&, float, bool));
};
