#ifndef DRONEDATA_H
#define DRONEDATA_H

#include "utils/vector3d.h"

struct DroneData {
  Vector3D direction;
  float distanceFromTakeoff;
  size_t id;

  DroneData() = default;

  DroneData(const DroneData* data) {
    direction =
        Vector3D(data->direction.m_x, data->direction.m_y, data->direction.m_z);
    distanceFromTakeoff = data->distanceFromTakeoff;
  }
};

#endif