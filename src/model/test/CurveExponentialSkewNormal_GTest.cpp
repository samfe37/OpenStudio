/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "ModelFixture.hpp"
#include "../CurveExponentialSkewNormal.hpp"

#include <cmath>

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, CurveExponentialSkewNormal_DefaultConstructors) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  ASSERT_EXIT(
    {
      Model m;
      CurveExponentialSkewNormal curve(m);

      exit(0);
    },
    ::testing::ExitedWithCode(0), "");
}

TEST_F(ModelFixture, CurveExponentialSkewNormal_GetterSetters_evaluate) {

  Model m;
  CurveExponentialSkewNormal curve(m);

  EXPECT_EQ(1.0, curve.coefficient1C1());
  EXPECT_EQ(1.0, curve.coefficient2C2());
  EXPECT_EQ(-1.0, curve.coefficient3C3());
  EXPECT_EQ(1.0, curve.coefficient4C4());

  EXPECT_EQ(-1.0, curve.minimumValueofx());
  EXPECT_EQ(1.0, curve.maximumValueofx());

  double c1 = 1.0;
  double c2 = 2.0;
  double c3 = 3.0;
  double c4 = 4.0;
  double min_x = 0.1;
  double max_x = 3.0;

  auto calc = [c1, c2, c3, c4](double x) {
    double z1 = (x - c1) / c2;  // From E+ source code itself
    // double z2 = (exp(c3 * x) * c4 * x - c1) / c2;
    double z2 = (c4 * x * std::exp(c3 * x) - c1) / c2;
    double z3 = -c1 / c2;

    double numerator = std::exp(-0.5 * std::pow(z1, 2)) * (1.0 + (z2 / std::abs(z2)) * std::erf(std::abs(z2) / std::sqrt(2.0)));
    double denominator = std::exp(-0.5 * std::pow(z3, 2)) * (1.0 + (z3 / std::abs(z3)) * std::erf(std::abs(z3) / std::sqrt(2.0)));

    return numerator / denominator;
  };

  EXPECT_TRUE(curve.setCoefficient1C1(c1));
  EXPECT_TRUE(curve.setCoefficient2C2(c2));
  EXPECT_TRUE(curve.setCoefficient3C3(c3));
  EXPECT_TRUE(curve.setCoefficient4C4(c4));

  EXPECT_EQ(c1, curve.coefficient1C1());
  EXPECT_EQ(c2, curve.coefficient2C2());
  EXPECT_EQ(c3, curve.coefficient3C3());
  EXPECT_EQ(c4, curve.coefficient4C4());

  EXPECT_TRUE(curve.setMinimumValueofx(min_x));
  EXPECT_TRUE(curve.setMaximumValueofx(max_x));
  EXPECT_EQ(min_x, curve.minimumValueofx());
  EXPECT_EQ(max_x, curve.maximumValueofx());

  EXPECT_FALSE(curve.minimumCurveOutput());
  EXPECT_FALSE(curve.maximumCurveOutput());

  // X in range, no output limit
  double x = 0.5;
  EXPECT_DOUBLE_EQ(calc(x), curve.evaluate(x));
  EXPECT_DOUBLE_EQ(3.559526560439694, curve.evaluate(x));

  // x < min_x
  x = 0.05;
  EXPECT_DOUBLE_EQ(calc(min_x), curve.evaluate(x));
  EXPECT_DOUBLE_EQ(1.3575844732109894, curve.evaluate(x));

  // x > max_x
  x = 20.0;
  EXPECT_DOUBLE_EQ(calc(max_x), curve.evaluate(x));
  EXPECT_DOUBLE_EQ(2.2275710165736293, curve.evaluate(x));

  // Set output limits
  double min_output = 1.5;
  double max_output = 2.0;
  EXPECT_TRUE(curve.setMinimumCurveOutput(min_output));
  EXPECT_TRUE(curve.setMaximumCurveOutput(max_output));

  ASSERT_TRUE(curve.minimumCurveOutput());
  ASSERT_TRUE(curve.maximumCurveOutput());
  EXPECT_EQ(min_output, curve.minimumCurveOutput().get());
  EXPECT_EQ(max_output, curve.maximumCurveOutput().get());

  // out < min output
  EXPECT_DOUBLE_EQ(min_output, curve.evaluate(min_x));
  // out > max output
  EXPECT_DOUBLE_EQ(max_output, curve.evaluate(max_x));

  // Wrong number of arguments
  // EXPECT_THROW(curve.evaluate(1.0, 2.0), openstudio::Exception);
  // EXPECT_THROW(curve.evaluate(1.0, 2.0, 3.0), openstudio::Exception);
}
