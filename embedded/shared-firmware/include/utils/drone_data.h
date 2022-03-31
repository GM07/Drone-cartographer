#ifndef DRONE_DATA_H
#define DRONE_DATA_H

#include <vector>

#include "utils/vector3d.h"

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
  std::array<char, 5> m_magicHeader{{'D', 'R', 'E', 'A', 'M'}};
  size_t m_id{0};
} __attribute__((packed));

#endif
