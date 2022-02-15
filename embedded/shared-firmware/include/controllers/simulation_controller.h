#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#include <boost/asio.hpp>

#include "controllers/abstract_controller.h"
#include "crazyflie_sensing.h"

inline std::stringstream logBuffer;
inline std::mutex logBufferMutex;

/***
 * This variable must absolutely stay inline here
 * Since the sockets are created using multiple threads
 * We need to make sure that this object stays alive
 * for as long as possible. Otherwise we end up with the
 * destructor of this object called before the destructor
 * of the threads hence a core dump.
 */
inline boost::asio::io_service io_service;

class SimulationController : public AbstractController {
 public:
  SimulationController(CCrazyflieSensing* ccrazyflieSensing);

  SimulationController(SimulationController&& other) = delete;
  SimulationController operator=(SimulationController&& other) = delete;
  SimulationController(SimulationController& other) = delete;
  SimulationController operator=(SimulationController& other) = delete;

  Vector3D& getCurrentLocation() override{/**/};

  void setLEDState(LED led, bool enable, bool blink) override;

  void goTo(const Vector3D& location, float yaw, float pitch,
            bool isRelative) override{/**/};
  void goTo(const Vector3D& location, bool isRelative) override{/**/};
  void takeOff(float height) override;
  void land() override;

  float getDistance(Direction direction) override{/**/};
  float getBatteryLevel() override{/**/};

  void sendP2PMessage(void* message) override{/**/};
  void initCommunicationManager() override;
  size_t receiveMessage(void* message, size_t size) override;
  void sendMessage(void* message, size_t size) override;

  void log(const std::string& message) override;

  void delay(const uint32_t ticks) override{/**/};

  void setSimulationDroneInstance(CCrazyflieSensing* ccrazyflieSensing);

 private:
  CCrazyflieSensing* m_ccrazyflieSensing;
  std::unique_ptr<boost::asio::local::stream_protocol::socket> m_socket;
};

#endif
