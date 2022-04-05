#ifndef CONTROLLER_DATA_H
#define CONTROLLER_DATA_H

struct __attribute__((__packed__)) ControllerData {
 public:
  [[nodiscard]] inline bool operator==(const ControllerData& other) const {
    return (front == other.front && left == other.left && back == other.back &&
            right == other.right && posX == other.posX && posY == other.posY &&
            batteryLevel == other.batteryLevel && state == other.state);
  }

  [[nodiscard]] inline bool operator!=(const ControllerData& other) const {
    return !(*this == other);
  }

  float front;
  float left;
  float back;
  float right;
  float posX;
  float posY;
  float batteryLevel;
  uint8_t state;
};

#endif












