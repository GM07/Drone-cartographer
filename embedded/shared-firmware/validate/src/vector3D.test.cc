#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils/vector3d.h"

const float kDefaultX = 0.01f;
const float kDefaultY = -9.03f;
const float kDefaultZ = 154.18f;

TEST(validateVector3D, plusOperator) {
  Vector3D vec1(kDefaultX, kDefaultY, kDefaultZ);
  Vector3D vec2(kDefaultX, kDefaultY, kDefaultZ);
  EXPECT_EQ(vec1 + vec2,
            Vector3D(2.0f * kDefaultX, 2.0f * kDefaultY, 2.0f * kDefaultZ));
}

TEST(validateVector3D, minusOperator) {
  Vector3D vec1(kDefaultX, kDefaultY, kDefaultZ);
  Vector3D vec2(kDefaultX, kDefaultY, kDefaultZ);
  EXPECT_EQ(vec1 - vec2, Vector3D(0.0f, 0.0f, 0.0f));
}

TEST(validateVector3D, multiplyOperator) {
  const float kZ = 154.0f;
  const int32_t KMultiplier = 3;
  Vector3D vec1(kDefaultX, kDefaultY, kZ);
  EXPECT_EQ(vec1 * KMultiplier,
            Vector3D(KMultiplier * kDefaultX, KMultiplier * kDefaultY,
                     KMultiplier * kZ));
}

TEST(validateVector3D, divisionOperator) {
  Vector3D vec1(kDefaultX, kDefaultY, kDefaultZ);
  const int32_t kDivider = 2;
  EXPECT_EQ(vec1 / kDivider,
            Vector3D(kDefaultX / kDivider, kDefaultY / kDivider,
                     kDefaultZ / kDivider));
}

TEST(validateVector3D, shouldDefaultConstruct) {
  EXPECT_EQ(Vector3D(), Vector3D(0.0f, 0.0f, 0.0f));
}