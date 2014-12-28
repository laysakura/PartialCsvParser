/*
 * PartialCsvParser.hpp
 *
 *  Created on: 2014/12/28
 *      Author: nakatani.sho
 */

#ifndef INCLUDE_PARTIALCSVPARSER_HPP_
#define INCLUDE_PARTIALCSVPARSER_HPP_

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

// Prevent default class methods
#define PREVENT_DEFAULT_CONSTRUCTOR(klass) \
  private: klass();
#define PREVENT_COPY_CONSTRUCTOR(klass) \
  private: klass(const klass&);
#define PREVENT_OBJECT_ASSIGNMENT(klass) \
  private: klass& operator=(const klass&);
#define PREVENT_CLASS_DEFAULT_METHODS(klass) \
  PREVENT_DEFAULT_CONSTRUCTOR(klass); \
  PREVENT_COPY_CONSTRUCTOR(klass); \
  PREVENT_OBJECT_ASSIGNMENT(klass); \


// Assertion also usable with Google Test
#ifdef PCP_GTEST
#include <stdexcept>
#include <sstream>
#include <string>

class PCPAssertionFailed : public std::runtime_error {
public:
  PCPAssertionFailed(const std::string &cause)
  : std::runtime_error(cause)
  {}
};

#define ASSERT(cond) \
  if (!(cond)) { \
    std::ostringstream ss; \
    ss << __FILE__ << ":" << __LINE__ << " (in " << __FUNCTION__ << "())" << " PCPAssertionFailed: " << #cond; \
    throw PCPAssertionFailed(ss.str()); \
  }

#else /* PCP_GTEST */
#include <cassert>

#define ASSERT(cond) \
  std::assert(cond);

#endif /* PCP_GTEST */


// Macros for error cases
#define PERROR_ABORT(msg) \
  { \
    std::perror((std::string("Fatal from PartialCsvParser ") + std::string(msg)).c_str()); \
    std::abort(); \
  }

namespace PCP {

// Utility functions
inline size_t _filesize(int opened_fd) {
  struct stat st;
  if (fstat(opened_fd, &st) != 0) PERROR_ABORT("while getting stat(2) of file");
  return st.st_size;
}

/**
 * Find a line including specified current_pos.
 * @param text Original text to find a line from.
 * @param text_length_byte Byte length of the original text.
 * @param current_pos Current position taking 0 ~ (text_length - 1).
 * @param line Pointer of start of the current line will be output.
 * @param line_length_byte Byte length of current line (not including line terminator) will be output.
 *
   @verbatim
   \n aaaaaaaaaaa \n bbbbbbbbbbbbb \n cccccccccc \n
                     ^           ^  ^
                    (1)         (2)(3)
   @endverbatim
 *
 * If current_pos is at between (1) and (2), line will point at (1).
 * If current_pos is at (3), line will also point at (1).
 *
   @verbatim
   \n ddddddddd \n eeeeeeeeeee \0
                   ^            ^
                  (1)          (2)
   @endverbatim
 *
 * Consider null character.
 * If current_pos is at between (1) and (2), line will point at (1).
 */
void _get_current_line(
  const char * const text,
  size_t text_length_byte,
  size_t current_pos,
  /* out */
  const char ** line,
  size_t * line_length_byte,
  /* optional */
  char line_terminator = '\n')
{
  ASSERT(text);
  ASSERT(text_length_byte >= 1);
  ASSERT(0 <= current_pos && current_pos <= text_length_byte - 1);

  // \n aaaaaaaaaaaa \n
  //    ^            ^
  //    start        end
  const char *line_start = text + current_pos, *line_end = text + current_pos;

  // search line_start
  while (line_start > text && *(line_start - 1) != line_terminator) --line_start;
  // search line_end
  while (line_end < text + text_length_byte && *line_end != line_terminator) ++line_end;

  *line = line_start;
  *line_length_byte = line_end - line_start;
}

class PartialCsvParser;

class CsvConfig {
public:
  CsvConfig(
    const char * const filepath,
    bool has_header_line = true,
    char field_terminator = ',',
    char line_terminator = '\n',
    char enclosure_char = '"')
  : filepath(filepath), has_header_line(has_header_line),
    field_terminator(field_terminator), line_terminator(line_terminator),
    enclosure_char(enclosure_char)
  {
    if ((fd = open(filepath, O_RDONLY)) == -1)
      PERROR_ABORT((std::string("while opening ") + filepath).c_str());  // TODO 例外を投げる
    csv_size = _filesize(fd);
    csv_text = static_cast<const char *>(mmap(NULL, csv_size, PROT_READ, MAP_PRIVATE, fd, 0));

    std::printf("size=%d\n\n%s\n", csv_size, csv_text);
    // reads header
  }

  ~CsvConfig() {
    if (munmap((void*)csv_text, csv_size) != 0) PERROR_ABORT("while munmap");
    if (close(fd) != 0) PERROR_ABORT("while closing file descriptor");
  }

  size_t filesize() const { return csv_size; }

  size_t body_offset() const;

  std::vector<std::string> headers() const {
    std::vector<std::string> header;  // NRVO optimization may prevent copy when returning this local variable.
    return header;
  }

  PartialCsvParser & generate_partial_parser(size_t read_from, size_t read_to) {
  }

private:
  const char * const filepath;
  const bool has_header_line;
  const char field_terminator;
  const char line_terminator;
  const char enclosure_char;

  int fd;
  size_t csv_size;
  const char * csv_text;

  PREVENT_CLASS_DEFAULT_METHODS(CsvConfig);
};

class PartialCsvParser {
public:
  PartialCsvParser(size_t read_from, size_t read_to) {}

  ~PartialCsvParser() {}

  std::vector<std::string> get_row() {
    std::vector<std::string> row;  // NRVO optimization may prevent copy when returning this local variable.
    return row;
  }

  bool has_more_rows() const {}

  PREVENT_CLASS_DEFAULT_METHODS(PartialCsvParser);
};

}

#endif /* INCLUDE_PARTIALCSVPARSER_HPP_ */
