#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utils/vector3d.h"

constexpr float kDefaultX = 0.01f;
constexpr float kDefaultY = -9.03f;
constexpr float kDefaultZ = 154.18f;

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

TEST(validateVector3D, additionAssignementOperator) {
  Vector3D vec1 = Vector3D(kDefaultX, 0.0f, 0.0f);
  Vector3D vec2 = Vector3D(kDefaultX, kDefaultY, kDefaultZ);
  Vector3D vec3 = vec1;
  vec1 += vec2;
  EXPECT_EQ(vec1, vec2 + vec3);
}

TEST(validateVector3D, notEqualOperator) {
  Vector3D vec1 = Vector3D(0.0f, 0.0f, 0.0f);
  Vector3D vec2 = Vector3D(kDefaultX, kDefaultY, kDefaultZ);
  EXPECT_NE(vec1, vec2);
}

TEST(validateVector3D, isAlmostEqual) {
  Vector3D vec1 = Vector3D(kDefaultX, kDefaultY, kDefaultZ);
  Vector3D vec2 = Vector3D(kDefaultX, kDefaultY, kDefaultZ);

  EXPECT_TRUE(vec1.isAlmostEqual(vec2));

  vec1 += Vector3D(kDefaultX, 0.0f, 0.0f);
  EXPECT_TRUE(vec1.isAlmostEqual(vec2));

  vec1 += Vector3D(0.0f, 0.0f, kDefaultZ);
  EXPECT_FALSE(vec1.isAlmostEqual(vec2));
}

TEST(validateVector3D, distanceTo) {
  Vector3D vec1 = Vector3D(0.0f, 3.0f, 4.0f);
  Vector3D vec2 = Vector3D(0.0f, 0.0f, 0.0f);
  EXPECT_EQ(vec1.distanceTo(vec2), 5.0f);
}