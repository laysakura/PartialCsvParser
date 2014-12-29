#include <gtest/gtest.h>
#include <PartialCsvParser.hpp>

using namespace PCP;

class PartialCsvParserErrorTest : public ::testing::Test {
protected:
  PartialCsvParserErrorTest() {}

  virtual void SetUp() {}
};

TEST_F(PartialCsvParserErrorTest, wrong_filepath) {
  EXPECT_THROW(CsvConfig("fixture/no_such_file.csv"), PCPError);
}
