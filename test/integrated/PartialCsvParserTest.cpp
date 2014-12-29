#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <PartialCsvParser.hpp>

using namespace PCP;

class PartialCsvParserTest : public ::testing::Test {
protected:
  PartialCsvParserTest() {}

  virtual void SetUp() {}
};

TEST_F(PartialCsvParserTest, 1worker_WithHeader_2col_3line_WithoutQuote_WithLastNL) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.csv");
  std::vector<std::string> headers = csv_config.get_headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

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

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_TRUE((row = parser.get_row()).empty());
}

TEST_F(PartialCsvParserTest, 1worker_WithHeader_2col_3line_WithoutQuote_WithLastNL_ignore_header) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.csv");

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

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_TRUE((row = parser.get_row()).empty());
}

TEST_F(PartialCsvParserTest, 1worker_WithHeader_2col_3line_WithoutQuote_WithoutLastNL) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithoutLastNL.csv");
  std::vector<std::string> headers = csv_config.get_headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

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

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_TRUE(parser.get_row().empty());
}

TEST_F(PartialCsvParserTest, 1worker_WithoutHeader_2col_3line_WithoutQuote_WithLastNL) {
  CsvConfig csv_config("fixture/WithoutHeader_2col_3line_WithoutQuote_WithLastNL.csv", false);

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

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_TRUE(parser.get_row().empty());
}

TEST_F(PartialCsvParserTest, 1worker_WithHeader_2col_3line_WithoutQuote_WithLastNL_tsv) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.tsv", true, '\t');
  std::vector<std::string> headers = csv_config.get_headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

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

  EXPECT_FALSE((row = parser.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_TRUE(parser.get_row().empty());
}

TEST_F(PartialCsvParserTest, 2worker_WithHeader_2col_3line_WithoutQuote_WithLastNL) {
  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.csv");
  std::vector<std::string> headers = csv_config.get_headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

  size_t parser1_from = csv_config.body_offset();
  size_t parser1_to = parser1_from + (csv_config.filesize() - csv_config.body_offset()) / 2;
  size_t parser2_from = parser1_to + 1;
  size_t parser2_to = csv_config.filesize() - 1;

  PartialCsvParser parser1(csv_config, parser1_from, parser1_to);
  PartialCsvParser parser2(csv_config, parser2_from, parser2_to);

  std::vector<std::string> row;

  EXPECT_FALSE((row = parser1.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("101", row[0]);
  EXPECT_EQ("102", row[1]);

  EXPECT_FALSE((row = parser1.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("201", row[0]);
  EXPECT_EQ("202", row[1]);

  EXPECT_TRUE(parser1.get_row().empty());

  EXPECT_FALSE((row = parser2.get_row()).empty());
  EXPECT_EQ(2, row.size());
  EXPECT_EQ("301", row[0]);
  EXPECT_EQ("302", row[1]);

  EXPECT_TRUE(parser2.get_row().empty());
}


class ParseWithManyParserTest :
  public ::testing::TestWithParam<size_t>
{};

TEST_P(ParseWithManyParserTest, each_parser_parses_n_bytes)
{
  const size_t n = GetParam();

  CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.csv");
  std::vector<std::string> headers = csv_config.get_headers();
  EXPECT_EQ("col1", headers[0]);
  EXPECT_EQ("col2", headers[1]);

  std::vector< std::vector<std::string> > rows;

  for (size_t offset = csv_config.body_offset(); offset <= csv_config.filesize() - 1; offset += n) {
    PartialCsvParser parser(csv_config, offset, std::min(offset + n - 1, csv_config.filesize() - 1));
    std::vector<std::string> row;
    while (!(row = parser.get_row()).empty()) rows.push_back(row);
  }

  EXPECT_EQ(3, rows.size());
  EXPECT_EQ("101", rows[0][0]); EXPECT_EQ("102", rows[0][1]);
  EXPECT_EQ("201", rows[1][0]); EXPECT_EQ("202", rows[1][1]);
  EXPECT_EQ("301", rows[2][0]); EXPECT_EQ("302", rows[2][1]);
}

CsvConfig csv_config("fixture/WithHeader_2col_3line_WithoutQuote_WithLastNL.csv");
INSTANTIATE_TEST_CASE_P(_, ParseWithManyParserTest, ::testing::Range(1UL, csv_config.filesize()));
