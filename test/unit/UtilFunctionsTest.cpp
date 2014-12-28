#include <gtest/gtest.h>
#include <utility>
#include <cstring>
#include <PartialCsvParser.hpp>

using namespace PCP;

class _get_current_line_from_text_with_last_nl_Test : public ::testing::TestWithParam<std::pair<size_t, const char *> > {
protected:
  const char * const text_with_last_nl;
  _get_current_line_from_text_with_last_nl_Test()
  : text_with_last_nl("aa\nbbb\nc\n")
    //                 012 3456 78
  {}
};

TEST_P(_get_current_line_from_text_with_last_nl_Test, get_correct_line)
{
  const char * line = 0;
  size_t line_length = 0;
  ASSERT_NO_THROW(_get_current_line(text_with_last_nl, std::strlen(text_with_last_nl), GetParam().first, &line, &line_length));
  EXPECT_EQ(GetParam().second, std::string(line, line_length));
}
INSTANTIATE_TEST_CASE_P(_, _get_current_line_from_text_with_last_nl_Test, ::testing::Values(
  std::make_pair(0UL, "aa"),
  std::make_pair(1UL, "aa"),
  std::make_pair(2UL, "aa"),
  std::make_pair(3UL, "bbb"),
  std::make_pair(4UL, "bbb"),
  std::make_pair(5UL, "bbb"),
  std::make_pair(6UL, "bbb"),
  std::make_pair(7UL, "c"),
  std::make_pair(8UL, "c")
));
