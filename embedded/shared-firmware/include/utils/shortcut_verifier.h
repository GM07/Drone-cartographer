#ifndef SHORTCUT_VERIFIER_H
#define SHORTCUT_VERIFIER_H

#include "utils/vector3d.h"

class ShortcutVerifier {
 public:
  Vector3D source{};
  Vector3D intersectionPosition{};
  bool hasFirstIntersection{false};
  bool hasSecondIntersection{false};
  bool isPathIntersectionValid{false};

  void reset() {
    hasFirstIntersection = false;
    hasSecondIntersection = false;
    isPathIntersectionValid = false;
    source = Vector3D();
    intersectionPosition = Vector3D();
  }
};

#endif
