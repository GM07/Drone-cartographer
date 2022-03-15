#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "stub_firmware_controller.h"

TEST(ValidateDrone, droneShouldNotThrow) {
  EXPECT_NO_THROW(({
    Drone *drone = new Drone(std::dynamic_pointer_cast<AbstractController>(
        std::make_shared<StubController>()));
    delete drone;
  }));
}