#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#ifdef GTEST
#include "socket.h"
#else
#include <boost/asio.hpp>
#endif

#include <atomic>
#include <mutex>
#include <optional>
#include <thread>

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
  explicit SimulationController(CCrazyflieSensing* ccrazyflieSensing);
  ~SimulationController() override;

  SimulationController(SimulationController&& other) = delete;
  SimulationController& operator=(SimulationController&& other) = delete;
  SimulationController(const SimulationController& other) = delete;
  SimulationController& operator=(const SimulationController& other) = delete;

  void setVelocity(const Vector3D& direction, float speed) override;
  inline void stopMotors() override{/**/};

  [[nodiscard]] Vector3D getCurrentLocation() const override;
  [[nodiscard]] bool isTrajectoryFinished() const override;

  void initCommunicationManager() override;

  size_t receiveMessage(void* message, size_t size) const override;
  void sendMessage(void* message, size_t size) const override;
  void sendDroneDataToServerThread();

  void sendP2PMessage(void* message, size_t size) override;
  void receiveP2PMessage(
      std::unordered_map<size_t, DroneData>* p2pData) override;

  void log(const std::string& message) override;
  void identify() override;
  [[nodiscard]] size_t getId() const override;

  void updateSensorsData() override;
  [[nodiscard]] float getMinCollisionAvoidanceDistance() const override;
  [[nodiscard]] float getMaxCollisionAvoidanceDistance() const override;
  [[nodiscard]] inline bool isDroneCrashed() const override { return false; };

#ifndef GTEST
 private:
#endif

  CCrazyflieSensing* m_ccrazyflieSensing;

  std::optional<ControllerData> m_controllerData;
  std::mutex m_controllerDataMutex;
  std::unique_ptr<boost::asio::local::stream_protocol::socket> m_dataSocket;
  std::unique_ptr<boost::asio::local::stream_protocol::socket> m_socket;
  std::unique_ptr<std::thread> m_sendDataThread;

  std::atomic_bool m_threadContinueFlag{true};
};

#endif
