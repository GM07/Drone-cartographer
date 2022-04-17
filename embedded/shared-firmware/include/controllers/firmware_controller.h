#ifndef FIRMWARE_CONTROLLER_H
#define FIRMWARE_CONTROLLER_H

#include <queue>

#include "controllers/abstract_controller.h"

extern "C" {
#include "FreeRTOS.h"
#include "ledseq.h"
#include "radiolink.h"
}

class FirmwareController : public AbstractController {
 public:
  FirmwareController();
  ~FirmwareController() override = default;
  FirmwareController(FirmwareController&& other) = delete;
  FirmwareController& operator=(FirmwareController&& other) = delete;
  FirmwareController(const FirmwareController& other) = delete;
  FirmwareController& operator=(const FirmwareController& other) = delete;

  void stopMotors() const override;
  void setVelocity(const Vector3D& direction, float speed,
                   bool /*bodyReference*/) override;

  [[nodiscard]] Vector3D getCurrentLocation() const override;
  [[nodiscard]] bool isTrajectoryFinished() const override {
    return Math::areAlmostEqual(getCurrentLocation(), m_targetPosition,
                                kRealTrajectoryFinishedTreshold);
  }
  [[nodiscard]] inline bool isAltitudeReached() const override {
    return Math::areAlmostEqual(getCurrentLocation().m_z, m_targetPosition.m_z,
                                kRealTrajectoryFinishedTreshold);
  };

  size_t receiveMessage(void* message, size_t size) const override;
  void sendMessage(void* message, size_t size) const override;

  void sendP2PMessage(void* message, size_t size) override;
  void receiveP2PMessage(
      std::unordered_map<size_t, DroneData>* p2pData) override{};

  void identify() override;
  [[nodiscard]] size_t getId() const override;

  void updateSensorsData() override;
  [[nodiscard]] float getMinCollisionAvoidanceDistance() const override;
  [[nodiscard]] float getMaxCollisionAvoidanceDistance() const override;
  [[nodiscard]] bool isDroneCrashed() const override;

  [[nodiscard]] inline float getSegmentOrientation() const override {
    return 0.0F;
  };

  void initCommunicationManager() override{/**/};
  void log(const std::string& message) override{/**/};

 private:
  float m_height;
  ledseqContext_t m_seqLED{};
};
#endif
