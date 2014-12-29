#include <gtest/gtest.h>
#include <tuple>
#include <cstring>
#include <PartialCsvParser.hpp>

using namespace PCP;

class _get_current_line_Test :
  public ::testing::TestWithParam<std::tuple<const char *, size_t, const char *> >
{};

TEST_P(_get_current_line_Test, get_correct_line)
{
  const char * const text = std::get<0>(GetParam());
  size_t current_pos = std::get<1>(GetParam());
  const char * const expected_got_line = std::get<2>(GetParam());

  const char * line = 0;
  size_t line_length = 0;
  ASSERT_NO_THROW(_get_current_line(text, std::strlen(text), current_pos, '\n', &line, &line_length));
  EXPECT_EQ(expected_got_line, std::string(line, line_length));
}

const char * const text_NL_terminated = "aa\nbbb\nc\n";
//                                       012 3456 78
INSTANTIATE_TEST_CASE_P(text_NL_terminated, _get_current_line_Test, ::testing::Values(
  std::make_tuple(text_NL_terminated, 0UL, "aa"),
  std::make_tuple(text_NL_terminated, 1UL, "aa"),
  std::make_tuple(text_NL_terminated, 2UL, "aa"),
  std::make_tuple(text_NL_terminated, 3UL, "bbb"),
  std::make_tuple(text_NL_terminated, 4UL, "bbb"),
  std::make_tuple(text_NL_terminated, 5UL, "bbb"),
  std::make_tuple(text_NL_terminated, 6UL, "bbb"),
  std::make_tuple(text_NL_terminated, 7UL, "c"),
  std::make_tuple(text_NL_terminated, 8UL, "c")
));

const char * const text_NULL_char_terminated = "aa\nbbb\nc";
//                                              012 3456 7
INSTANTIATE_TEST_CASE_P(text_NULL_char_terminated, _get_current_line_Test, ::testing::Values(
  std::make_tuple(text_NULL_char_terminated, 0UL, "aa"),
  std::make_tuple(text_NULL_char_terminated, 1UL, "aa"),
  std::make_tuple(text_NULL_char_terminated, 2UL, "aa"),
  std::make_tuple(text_NULL_char_terminated, 3UL, "bbb"),
  std::make_tuple(text_NULL_char_terminated, 4UL, "bbb"),
  std::make_tuple(text_NULL_char_terminated, 5UL, "bbb"),
  std::make_tuple(text_NULL_char_terminated, 6UL, "bbb"),
  std::make_tuple(text_NULL_char_terminated, 7UL, "c")
));

const char * const text_with_empty_line = "\nbbb\n\n";
//                                         0 1234 5
INSTANTIATE_TEST_CASE_P(text_with_empty_line, _get_current_line_Test, ::testing::Values(
  std::make_tuple(text_with_empty_line, 0UL, ""),
  std::make_tuple(text_with_empty_line, 1UL, "bbb"),
  std::make_tuple(text_with_empty_line, 2UL, "bbb"),
  std::make_tuple(text_with_empty_line, 3UL, "bbb"),
  std::make_tuple(text_with_empty_line, 4UL, "bbb"),
  std::make_tuple(text_with_empty_line, 5UL, "")
));


class _split_Test :
  public ::testing::TestWithParam<std::tuple<const char *, std::vector<std::string> > >
{};

TEST_P(_split_Test, get_correct_split_strings)
{
  const char * const str = std::get<0>(GetParam());
  std::vector<std::string> expected_split_strings = std::get<1>(GetParam());

  std::vector<std::string> split_strings = std::get<1>(GetParam());
  ASSERT_NO_THROW(split_strings = _split(str, std::strlen(str), ','));
  ASSERT_EQ(expected_split_strings, split_strings);
}

#define STR_ARRAY(...) \
  []() -> std::vector<std::string> { std::vector<std::string> v = { __VA_ARGS__ }; return v; }()

INSTANTIATE_TEST_CASE_P(_, _split_Test, ::testing::Values(
  std::make_tuple("aa,bbb,c", STR_ARRAY("aa", "bbb", "c")),
  std::make_tuple(",bbb,,", STR_ARRAY("", "bbb", "", "")),
  std::make_tuple("", STR_ARRAY(""))
));
