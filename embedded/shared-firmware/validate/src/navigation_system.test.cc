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
              isTrajectoryFinished())
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
              isTrajectoryFinished())
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
  EXPECT_CALL(*controller, isTrajectoryFinished())
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
  EXPECT_CALL(*controller, isTrajectoryFinished())
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

TEST(ValidateNavigationSystem, wallAvoidanceWithSensorsEverywhere) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();

  controller->m_data.front = 25;
  controller->m_data.back = 25;
  controller->m_data.left = 25;
  controller->m_data.right = 25;

  Drone drone(controller);

  drone.m_normal = Vector3D(0, 0, 0);
  drone.wallAvoidance();

  EXPECT_FLOAT_EQ(drone.m_normal.m_x, 0);
  EXPECT_FLOAT_EQ(drone.m_normal.m_y, 0);
  EXPECT_FLOAT_EQ(drone.m_normal.m_z, 0);
}

TEST(ValidateNavigationSystem, wallAvoidanceWithSensors) {
  std::shared_ptr<StubController> controller =
      std::make_shared<StubController>();

  controller->m_data.front = 25;
  controller->m_data.back = 0;
  controller->m_data.left = 25;
  controller->m_data.right = 0;

  Drone drone(controller);

  drone.m_normal = Vector3D(-1, -1, 0);

  drone.wallAvoidance();

  EXPECT_FLOAT_EQ(drone.m_normal.m_x, 0);
  EXPECT_FLOAT_EQ(drone.m_normal.m_y, 0);
  EXPECT_FLOAT_EQ(drone.m_normal.m_z, 0);
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
