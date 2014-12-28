#include <gtest/gtest.h>
#include <PartialCsvParser.hpp>

class PartialCsvParserTest : public ::testing::Test {
protected:
  PartialCsvParserTest() {}

  virtual void SetUp() {}
};

TEST_F(PartialCsvParserTest, test01) {
  EXPECT_EQ(1, 1);
}
