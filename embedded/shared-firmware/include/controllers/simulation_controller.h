#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#include "abstractController.h"

class SimulationController : public AbstractController {
 public:
  SimulationController(void* controller)
      : m_ccrazyflieSensing(controller){/**/};

  SimulationController(SimulationController&& other) = delete;
  SimulationController operator=(SimulationController&& other) = delete;
  SimulationController(SimulationController& other) = delete;
  SimulationController operator=(SimulationController& other) = delete;

  Vector3D& getCurrentLocation() override{/**/};

  void setLEDState(LEDColor color, Side side, bool enable,
                   bool blink) override{/**/};

  void goTo(const Vector3D& location, float yaw, float pitch,
            bool isRelative) override{/**/};
  void goTo(const Vector3D& location, bool isRelative) override{/**/};
  void takeoff(float height) override{/**/};
  void land() override{/**/};

  float getDistance(Direction direction) override{/**/};
  float getBatteryLevel() override{/**/};

  void sendP2PMessage(void* message) override{/**/};
  void initCommunicationManager() override{/**/};
  size_t receiveMessage(void* message, size_t size) override { /**/
  }
  void sendMessage(void* message, size_t size) override { /**/
  }

  void log(const std::string&& message) override{/**/};

  void delay(const uint32_t ticks) override{/**/};

 private:
  void* m_ccrazyflieSensing;
};

#endif
