#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "components/drone.h"
#include "controllers/firmware_controller.h"
#include "mock_functions.h"
#include "stub_firmware_sensors.h"
#include "utils/led.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

TEST(ValidateEmbeddedFirmware, setLEDStateShouldRegisterAndRun) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, ledseqRun(_)).Times(1);
  Drone::getEmbeddedDrone().getController()->identify();

  delete mock;
}

TEST(ValidateEmbeddedFirmware, receiveMessage) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, appchannelReceiveDataPacket(_, _, _)).Times(1);

  std::array<char, 10> buffer;
  Drone::getEmbeddedDrone().getController()->receiveMessage(buffer.data(),
                                                            buffer.max_size());

  delete mock;
}

TEST(ValidateEmbeddedFirmware, sendMessage) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, appchannelSendDataPacket(_, _)).Times(1);

  std::array<char, 10> buffer;
  buffer.fill('\0');
  Drone::getEmbeddedDrone().getController()->sendMessage(buffer.data(),
                                                         buffer.size());

  delete mock;
}

TEST(ValidateEmbeddedFirmware, checkIfDroneCrashed) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, supervisorIsTumbled())
      .Times(2)
      .WillOnce(Return(false))
      .WillRepeatedly(Return(true));

  EXPECT_FALSE(Drone::getEmbeddedDrone().getController()->isDroneCrashed());
  EXPECT_TRUE(Drone::getEmbeddedDrone().getController()->isDroneCrashed());

  delete mock;
}

TEST(ValidateEmbeddedFirmware, isTrajectoryFinished) {
  mock = new FunctionsMock;
  constexpr point_t kPointRef1{0, 1, 1, 1};
  constexpr point_t kPointRef2{0, 0, 0, 0};
  EXPECT_CALL(*mock, estimatorKalmanGetEstimatedPos)
      .Times(2)
      .WillOnce([&](point_t* point) { *point = kPointRef1; })
      .WillRepeatedly([&](point_t* point) { *point = kPointRef2; });

  Drone::getEmbeddedDrone().getController()->m_targetPosition =
      Vector3D(1.0F, 1.0F, 1.0F);

  EXPECT_TRUE(
      Drone::getEmbeddedDrone().getController()->isTrajectoryFinished());
  EXPECT_FALSE(
      Drone::getEmbeddedDrone().getController()->isTrajectoryFinished());

  delete mock;
}

TEST(ValidateEmbeddedFirmware, getCurrentLocation) {
  mock = new FunctionsMock;
  constexpr point_t kPointRef{0, 0, 1, 0};
  EXPECT_CALL(*mock, estimatorKalmanGetEstimatedPos)
      .Times(1)
      .WillOnce([&](point_t* point) { *point = kPointRef; });

  EXPECT_EQ(Drone::getEmbeddedDrone().getController()->getCurrentLocation(),
            Vector3D(0, 1, 0));

  delete mock;
}

TEST(ValidateEmbeddedFirmware, setVelocity) {
  mock = new FunctionsMock;
  EXPECT_CALL(*mock, commanderSetSetpoint(_, _)).Times(1);

  Vector3D vec;
  Drone::getEmbeddedDrone().getController()->setVelocity(vec, 3);

  delete mock;
}

TEST(ValidateEmbeddedFirmware, takeoff) {
  mock = new FunctionsMock;

  constexpr point_t kPointRef{0, 0, 1, 0};
  constexpr float kHeight = 1.0F;
  EXPECT_CALL(*mock, estimatorKalmanGetEstimatedPos)
      .Times(AtLeast(1))
      .WillRepeatedly([&](point_t* point) { *point = kPointRef; });
  EXPECT_CALL(*mock, commanderSetSetpoint(_, 3)).Times(1);

  Drone::getEmbeddedDrone().getController()->takeOff(kHeight);
  Drone::getEmbeddedDrone().step();

  delete mock;
}

TEST(ValidateEmbeddedFirmware, land) {
  mock = new FunctionsMock;

  constexpr point_t kPointRef{0, 0, 1, 1};

  EXPECT_CALL(*mock, estimatorKalmanGetEstimatedPos)
      .Times(AtLeast(1))
      .WillRepeatedly([&](point_t* point) { *point = kPointRef; });
  EXPECT_CALL(*mock, commanderSetSetpoint(_, 3)).Times(1);

  Drone::getEmbeddedDrone().getController()->land();
  Drone::getEmbeddedDrone().step();
  delete mock;
}

TEST(ValidateEmbeddedFirmware, updateSensorsData) {
  mock = new FunctionsMock;
  FirmwareController controller;
  std::unique_ptr<StubFirmwareSensors> sensor =
      std::make_unique<StubFirmwareSensors>();

  EXPECT_CALL(*sensor, getFrontDistance())
      .WillOnce(Return(0))
      .WillOnce(Return(1));
  EXPECT_CALL(*sensor, getLeftDistance())
      .WillOnce(Return(0))
      .WillOnce(Return(1));
  EXPECT_CALL(*sensor, getBackDistance())
      .WillOnce(Return(0))
      .WillOnce(Return(1));
  EXPECT_CALL(*sensor, getRightDistance())
      .WillOnce(Return(0))
      .WillOnce(Return(1));

  EXPECT_CALL(*sensor, getPosX()).Times(2);
  EXPECT_CALL(*sensor, getPosY()).Times(2);
  EXPECT_CALL(*sensor, getBatteryLevel(_)).Times(2);

  controller.m_abstractSensors = std::move(sensor);

  controller.updateSensorsData();
  ControllerData firstData = controller.m_data;

  controller.updateSensorsData();
  ControllerData secondData = controller.m_data;

  EXPECT_TRUE(firstData != secondData);

  delete mock;
}
