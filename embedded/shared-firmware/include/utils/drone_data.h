#ifndef DRONE_DATA_H
#define DRONE_DATA_H

#include <array>

#include "utils/vector3d.h"

constexpr size_t kHeaderSize = 5;

struct DroneData {
  DroneData() = default;
  ~DroneData() = default;
  DroneData(const DroneData& data) = default;
  DroneData& operator=(const DroneData& other) = default;
  DroneData& operator=(const DroneData&& other) = delete;
  DroneData(const DroneData&& other) = delete;

  Vector3D m_direction;
  float m_distanceFromTakeoff{0};
  float m_range{0};
  std::array<char, kHeaderSize> m_magicHeader{{'D', 'R', 'E', 'A', 'M'}};
  size_t m_id{0};

 public:
  DroneData transformReference(const float orientation) {
    DroneData copy(*this);
    copy.m_direction = copy.m_direction.rotate(orientation);
    return copy;
  }

} __attribute__((packed));

#endif
