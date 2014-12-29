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

TEST_F(PartialCsvParserErrorTest, csv_with_header_has_different_number_of_columns) {
  CsvConfig csv_config("fixture/Invalid_DifferentNumberOfColumns.csv");
  std::vector<std::string> headers = csv_config.headers();
  EXPECT_EQ("a", headers[0]); EXPECT_EQ("a", headers[1]); EXPECT_EQ("a", headers[2]);

  PartialCsvParser parser(csv_config);
  std::vector<std::string> row;

  row = parser.get_row();
  EXPECT_EQ("b", row[0]); EXPECT_EQ("b", row[1]); EXPECT_EQ("b", row[1]);
  EXPECT_THROW(parser.get_row(), PCPCsvError);
}

TEST_F(PartialCsvParserErrorTest, csv_without_header_has_different_number_of_columns) {
  CsvConfig csv_config("fixture/Invalid_DifferentNumberOfColumns.csv", false);
  PartialCsvParser parser(csv_config);
  std::vector<std::string> row;

  row = parser.get_row();
  EXPECT_EQ("a", row[0]); EXPECT_EQ("a", row[1]); EXPECT_EQ("a", row[1]);
  row = parser.get_row();
  EXPECT_EQ("b", row[0]); EXPECT_EQ("b", row[1]); EXPECT_EQ("b", row[1]);
  EXPECT_THROW(parser.get_row(), PCPCsvError);
}
