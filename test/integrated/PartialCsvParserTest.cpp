#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <PartialCsvParser.hpp>

using namespace PCP;

class PartialCsvParserTest : public ::testing::Test {
protected:
  PartialCsvParserTest() {}

  virtual void SetUp() {}
};

TEST_F(PartialCsvParserTest, 1worker_WithHeader_2col_3line_WithoutQuote_WithLastNL) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.csv");
  std::vector<std::string> headers = csv_config.headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

  PartialCsvParser parser(csv_config);

  std::vector<std::string> row;

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("101", row[0]);
  EXPECT_EQ("102", row[1]);

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("201", row[0]);
  EXPECT_EQ("202", row[1]);

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_EQ(PartialCsvParser::NO_MORE_ROW, parser.get_row());
}

TEST_F(PartialCsvParserTest, 1worker_WithHeader_2col_3line_WithoutQuote_WithoutLastNL) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithoutLastNL.csv");
  std::vector<std::string> headers = csv_config.headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

  PartialCsvParser parser(csv_config);

  std::vector<std::string> row;

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("101", row[0]);
  EXPECT_EQ("102", row[1]);

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("201", row[0]);
  EXPECT_EQ("202", row[1]);

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_EQ(PartialCsvParser::NO_MORE_ROW, parser.get_row());
}

TEST_F(PartialCsvParserTest, 1worker_WithoutHeader_2col_3line_WithoutQuote_WithLastNL) {
  CsvConfig csv_config("fixture/WithoutHeader_2col_3line_WithoutQuote_WithLastNL.csv", false);
  PartialCsvParser parser(csv_config);

  std::vector<std::string> row;

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("101", row[0]);
  EXPECT_EQ("102", row[1]);

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("201", row[0]);
  EXPECT_EQ("202", row[1]);

  EXPECT_NE(PartialCsvParser::NO_MORE_ROW, row = parser.get_row());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_EQ(PartialCsvParser::NO_MORE_ROW, parser.get_row());
}
