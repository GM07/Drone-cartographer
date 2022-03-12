#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#ifdef GTEST
#include "socket.h"
#else
#include <boost/asio.hpp>
#endif
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <mutex>
#include <thread>

#include "controllers/abstract_controller.h"
#include "crazyflie_sensing.h"
#include "utils/semaphore.h"

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
  explicit SimulationController(CCrazyflieSensing* ccrazyflieSensing);
  ~SimulationController() override;

  SimulationController(SimulationController&& other) = delete;
  SimulationController& operator=(SimulationController&& other) = delete;
  SimulationController(const SimulationController& other) = delete;
  SimulationController& operator=(const SimulationController& other) = delete;

  void goTo(const Vector3D& location, bool isRelative) override;
  void takeOff(float height) override;
  void land() override;

  Vector3D getCurrentLocation() const override;
  bool isTrajectoryFinished() const override;

  void initCommunicationManager() override;
  size_t receiveMessage(void* message, size_t size) override;
  void sendMessage(void* message, size_t size) override;
  void sendDroneDataToServerThread();

  void log(const std::string& message) override;
  void blinkLED(LED /*led*/) override;

  void updateSensorsData() override;
  bool isDroneCrashed() const override;

#ifndef GTEST
 private:
#endif

  CCrazyflieSensing* m_ccrazyflieSensing;

  std::unique_ptr<Semaphore> m_controllerDataSem;
  boost::lockfree::queue<ControllerData, boost::lockfree::fixed_sized<true>>
      m_serverDataQueue{1};
  std::unique_ptr<boost::asio::local::stream_protocol::socket> m_dataSocket;
  std::unique_ptr<std::thread> m_sendDataThread;
  std::atomic<bool> m_threadContinueFlag{true};

  std::unique_ptr<boost::asio::local::stream_protocol::socket> m_socket;
};

#endif
