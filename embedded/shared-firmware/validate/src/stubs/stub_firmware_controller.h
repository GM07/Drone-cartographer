#include <gmock/gmock.h>

#include "controllers/firmware_controller.h"

class StubController : public FirmwareController {
 public:
  StubController() = default;

  MOCK_METHOD1(takeOff, void(float));
  MOCK_METHOD0(land, void());
  MOCK_METHOD1(blinkLED, void(LED));
  MOCK_METHOD2(sendMessage, void(void*, size_t));
  MOCK_METHOD2(receiveMessage, size_t(void*, size_t));
  MOCK_CONST_METHOD0(isTrajectoryFinished, bool());
};
