#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "utils/math.h"
#include "utils/vector3d.h"

constexpr float kDefaultX = 0.005f;
constexpr float kDefaultY = -9.03f;
constexpr float kDefaultZ = 154.18f;

TEST(validateVector3D, shouldNotThrow) {
  EXPECT_NO_THROW(({
    Vector3D* vec1 = new Vector3D();
    delete vec1;
  }));
}

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

TEST(validateVector3D, areAlmostEqual) {
  Vector3D vec1 = Vector3D(kDefaultX, kDefaultY, kDefaultZ);
  Vector3D vec2 = Vector3D(kDefaultX, kDefaultY, kDefaultZ);

  EXPECT_TRUE(areAlmostEqual(vec1, vec2));

  vec1 += Vector3D(kDefaultX, 0.0f, 0.0f);
  EXPECT_TRUE(areAlmostEqual(vec1, vec2));

  vec1 += Vector3D(0.0f, 0.0f, kDefaultZ);
  EXPECT_FALSE(areAlmostEqual(vec1, vec2));
}

TEST(validateVector3D, distanceTo) {
  Vector3D vec1 = Vector3D(0.0f, 3.0f, 4.0f);
  Vector3D vec2 = Vector3D(0.0f, 0.0f, 0.0f);
  EXPECT_EQ(vec1.distanceTo(vec2), 5.0f);
}

TEST(validateVector3D, reflect) {
  Vector3D vec1 = Vector3D(1.0f, 1.0f, 0.0f);
  Vector3D vec2 = Vector3D(0.0f, -1.0f, 0.0f);
  Vector3D result = Vector3D(1.0f, 0.0f, 0.0f);
  Vector3D output = vec2.reflect(vec1);
  EXPECT_TRUE(areAlmostEqual(output, result));
}

TEST(ValidateVector3D, distanceXY) {
  Vector3D vec1(0, 0, 0);
  Vector3D vec2(3, 4, 25);
  EXPECT_FLOAT_EQ(vec2.distanceToXY(vec1), 5);
}

TEST(ValidateVector3D, operatorMinusEqual) {
  Vector3D vec1(5, 5, 5);
  Vector3D vec2(1, 3, 4);
  Vector3D copy(vec1);
  vec1 -= vec2;
  EXPECT_EQ(vec1, copy - vec2);
}

TEST(ValidateVector3D, toString) {
  Vector3D vec1(23, 32, 33);
  EXPECT_NE(vec1.toString().find("23"), std::string::npos);
  EXPECT_NE(vec1.toString().find("32"), std::string::npos);
  EXPECT_NE(vec1.toString().find("33"), std::string::npos);
}

TEST(ValidateVector3D, areSameDirection) {
  Vector3D vec1(1, 2, 0);
  Vector3D vec2(-1, 2, 0);
  EXPECT_TRUE(Vector3D::areSameDirection(vec1, vec2));
}

TEST(ValidateVector3D, constructorX) {
  EXPECT_EQ(Vector3D::x(1), Vector3D(1, 0, 0));
}

TEST(ValidateVector3D, constructorY) {
  EXPECT_EQ(Vector3D::y(1), Vector3D(0, 1, 0));
}

TEST(ValidateVector3D, constructorZ) {
  EXPECT_EQ(Vector3D::z(1), Vector3D(0, 0, 1));
}