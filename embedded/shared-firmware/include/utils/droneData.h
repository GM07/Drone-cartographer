#ifndef DRONEDATA_H
#define DRONEDATA_H

#include "utils/vector3d.h"

struct DroneData {
  Vector3D direction;
  float distanceFromTakeoff;
  float range;
  size_t id;

  DroneData() = default;

  DroneData(const DroneData* data) {
    direction =
        Vector3D(data->direction.m_x, data->direction.m_y, data->direction.m_z);
    distanceFromTakeoff = data->distanceFromTakeoff;
    id = data->id;
    range = data->range;
  }
};

#endif