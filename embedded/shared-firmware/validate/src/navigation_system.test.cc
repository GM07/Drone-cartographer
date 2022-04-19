#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "stub_firmware_controller.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

TEST(ValidateNavigationSystem, stepTakingOff) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->m_state = State::kTakingOff;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              isAltitudeReached())
      .Times(1);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              isDroneCrashed())
      .Times(testing::AtLeast(0));
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              updateSensorsData())
      .Times(testing::AtLeast(0));
  drone.step();
}

TEST(ValidateNavigationSystem, stepLanding) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->m_state = State::kLanding;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              isAltitudeReached())
      .Times(1);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              isDroneCrashed())
      .Times(testing::AtLeast(0));
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              updateSensorsData())
      .Times(testing::AtLeast(0));
  drone.step();
}

TEST(ValidateNavigationSystem, stateIdle) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->m_state = State::kIdle;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              takeOff(_))
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              land())
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              identify())
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              isDroneCrashed())
      .Times(testing::AtLeast(0));
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              updateSensorsData())
      .Times(testing::AtLeast(0));
  drone.step();
}

TEST(ValidateNavigationSystem, stateExploring) {
  Drone drone(std::dynamic_pointer_cast<AbstractController>(
      std::make_shared<StubController>()));
  drone.getController()->m_state = State::kExploring;
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              takeOff(_))
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              land())
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              identify())
      .Times(0);
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              isDroneCrashed())
      .Times(testing::AtLeast(0));
  EXPECT_CALL(*std::dynamic_pointer_cast<StubController>(drone.getController()),
              updateSensorsData())
      .Times(testing::AtLeast(0));

  drone.step();
  EXPECT_EQ(drone.getController()->m_state, State::kExploring);
}

TEST(ValidateNavigationSystem, stateReturningToBase) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();

  EXPECT_CALL(*controller, setVelocity(_, _, _)).Times(AtLeast(1));
  controller->m_state = State::kReturningToBase;
  Drone drone(controller);
  drone.step();
}

TEST(ValidateNavigationSystem, takingOffFinished) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, takeOff(_)).Times(0);
  EXPECT_CALL(*controller, land()).Times(0);
  EXPECT_CALL(*controller, identify()).Times(0);
  EXPECT_CALL(*controller, isAltitudeReached())
      .Times(2)
      .WillOnce(Return(false))
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*controller, isDroneCrashed()).Times(testing::AtLeast(0));
  EXPECT_CALL(*controller, updateSensorsData()).Times(testing::AtLeast(0));

  Drone drone(controller);
  drone.getController()->m_state = State::kTakingOff;

  drone.step();
  drone.step();

  EXPECT_EQ(drone.getController()->m_state, State::kExploring);
}

TEST(ValidateNavigationSystem, landingFinished) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();
  EXPECT_CALL(*controller, takeOff(_)).Times(0);
  EXPECT_CALL(*controller, land()).Times(0);
  EXPECT_CALL(*controller, identify()).Times(0);
  EXPECT_CALL(*controller, isAltitudeReached())
      .Times(2)
      .WillOnce(Return(false))
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*controller, isDroneCrashed()).Times(testing::AtLeast(0));
  EXPECT_CALL(*controller, updateSensorsData()).Times(testing::AtLeast(0));

  Drone drone(controller);
  drone.getController()->m_state = State::kLanding;

  drone.step();
  drone.step();

  EXPECT_EQ(drone.getController()->m_state, State::kIdle);
}

TEST(ValidateNavigationSystem, getRealSensorsDistance) {
  float sensor1 = 10;
  float sensor2 = -2;
  float sensor3 = -20;
  constexpr float kMastDist = 1500;
  EXPECT_EQ(Drone::getRealSensorDistance(sensor1), sensor1);
  EXPECT_EQ(Drone::getRealSensorDistance(sensor2), 0);
  EXPECT_EQ(Drone::getRealSensorDistance(sensor3), kMastDist);
}
