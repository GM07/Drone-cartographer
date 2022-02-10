#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils/vector3d.h"

TEST(validateVector3D, plusOperator) {
  Vector3D vec1(0.01f, -9.03f, 154.18f);
  Vector3D vec2(0.01f, -9.03f, 154.18f);
  EXPECT_EQ(vec1 + vec2, Vector3D(0.02f, -18.06f, 308.36f));
}

TEST(validateVector3D, minusOperator) {
  Vector3D vec1(0.01f, -9.03f, 154.18f);
  Vector3D vec2(0.01f, -9.03f, 154.18f);
  EXPECT_EQ(vec1 - vec2, Vector3D(0.0f, 0.0f, 0.0f));
}

TEST(validateVector3D, multiplyOperator) {
  Vector3D vec1(0.01f, -9.03f, 154.0f);
  EXPECT_EQ(vec1 * 3, Vector3D(0.03f, -27.09f, 462.0f));
}

TEST(validateVector3D, divisionOperator) {
  Vector3D vec1(0.01f, -9.03f, 154.18f);
  EXPECT_EQ(vec1 / 2, Vector3D(0.005f, -4.515, 77.09));
}

TEST(validateVector3D, shouldDefaultConstruct) {
  EXPECT_EQ(Vector3D(), Vector3D(0.0f, 0.0f, 0.0f));
}