#include <gmock/gmock.h>

#include "components/drone.h"

class StubDrone : Drone {
 public:
  StubDrone(std::shared_ptr<AbstractController> controller)
      : Drone(controller) {}
  virtual ~StubDrone() = default;

  MOCK_METHOD0(step, void());
};
