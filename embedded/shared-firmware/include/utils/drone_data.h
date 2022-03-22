#ifndef DRONE_DATA_H
#define DRONE_DATA_H

#include "utils/vector3d.h"

struct DroneData {
  DroneData() = default;
  DroneData(const DroneData& data) = default;

  Vector3D m_direction;
  float m_distanceFromTakeoff{0};
  float m_range{0};
  size_t m_id{0};
} __attribute__((packed));

#endif
