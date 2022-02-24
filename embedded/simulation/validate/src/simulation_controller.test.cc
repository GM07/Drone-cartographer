#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

#include "controllers/simulation_controller.h"
#include "stubSocket.h"
#include "stub_cci_positioning_sensor.h"
#include "stub_cci_quadrotor_position_actuator.h"
#include "stub_crazyflie_sensing.h"

using ::testing::_;

TEST(validateSimulationController, receiveMessageShouldNotPullSocketIfEmpty) {
  StubCCrazyflieSensing crazyflie;
  SimulationController controller(&crazyflie);
  boost::asio::io_service service;
  auto socket =
      std::make_unique<boost::asio::local::stream_protocol::StubSocket>(
          service);
  EXPECT_CALL(*socket, receive(_)).Times(0);
  controller.m_socket = std::move(socket);

  const size_t messageSize = 60;
  char message[messageSize];

  controller.receiveMessage(&message, messageSize);
}

TEST(validateSimulationController, logShouldAddSomethingToBuffer) {
  StubCCrazyflieSensing crazyflie;
  SimulationController controller(&crazyflie);
  logBuffer.seekg(0, std::ios::end);
  EXPECT_EQ(logBuffer.tellg(), 0);

  controller.log("sup");

  logBuffer.seekg(0, std::ios::end);
  EXPECT_EQ(logBuffer.tellg(), sizeof("sup"));
}

TEST(validateSimulationController, takeOffShouldDoSomethingIfValidHeight) {
  StubCCrazyflieSensing crazyflie;
  StubCCIQuadrotorPositionActuator propellers;
  StubCCIPositioningSensor positionSensor;
  SimulationController controller(&crazyflie);

  crazyflie.m_pcPropellers = &propellers;
  crazyflie.m_pcPos = &positionSensor;

  EXPECT_CALL(propellers, SetAbsolutePosition(_)).Times(1);

  controller.takeOff(1.0f);
}