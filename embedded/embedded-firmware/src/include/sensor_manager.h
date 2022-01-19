#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <cstdint>

#include <directions.h>

class SensorManager {
 public:
  /**
   * @brief Get the distance between the drone and the wall in a certain
   * direction.
   *
   * @return The distance between the drone and the wall in the specified
   * direction -1 if invalid
   */
  float getSensorReadings(bridge::Direction);

  /**
   * @brief Set the LED on the drone to a certain color
   *
   * @param color The LED's color.
   * The first 8 bits should be used for the blue value
   * The next 8 bits should represent the green value
   * The next 8 bits should represent the red value
   * The other 8 bits are not used.
   */
  void setDELColor(uint32_t color);
};

#endif
