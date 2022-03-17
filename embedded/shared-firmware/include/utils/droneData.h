#ifndef DRONEDATA_H
#define DRONEDATA_H

#include "utils/vector3d.h"

struct DroneData {
  Vector3D direction;
  float distanceFromTakeoff = 0;
  float range = 0;
  size_t id = 0;

  DroneData() = default;

  explicit DroneData(const DroneData* data) {
    direction =
        Vector3D(data->direction.m_x, data->direction.m_y, data->direction.m_z);
    distanceFromTakeoff = data->distanceFromTakeoff;
    id = data->id;
    range = data->range;
  }
} __attribute__((packed));

#endif