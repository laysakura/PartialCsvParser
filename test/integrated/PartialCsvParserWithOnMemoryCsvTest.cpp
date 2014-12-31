#include <gtest/gtest.h>
#include <PartialCsvParser.hpp>

using namespace PCP;

class PartialCsvParserWithOnMemoryCsvTest : public ::testing::Test {
protected:
  PartialCsvParserWithOnMemoryCsvTest() {}

  virtual void SetUp() {}
};

TEST_F(PartialCsvParserWithOnMemoryCsvTest, NullTerminatedString) {
  const char * const csv =
    "101,102\n"
    "201,202\n";

  Memory::CsvConfig csv_config(csv, false);
  PartialCsvParser parser(csv_config);

  std::vector<std::string> row;

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("101", row[0]);
  EXPECT_EQ("102", row[1]);

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("201", row[0]);
  EXPECT_EQ("202", row[1]);

  EXPECT_TRUE((row = parser.get_row()).empty());
}

TEST_F(PartialCsvParserWithOnMemoryCsvTest, StringWithLength) {
  const char * const csv =
    "101,102\n"
    "201,202_this_should_be_ignored\n";

  Memory::CsvConfig csv_config(15UL, csv, false);
  PartialCsvParser parser(csv_config);

  std::vector<std::string> row;

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("101", row[0]);
  EXPECT_EQ("102", row[1]);

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("201", row[0]);
  EXPECT_EQ("202", row[1]);

  EXPECT_TRUE((row = parser.get_row()).empty());
}
