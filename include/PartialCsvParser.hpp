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
#include <cstring>
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
#define STRERROR_THROW(err_class, msg) \
  { \
    throw err_class(std::string("Fatal from PartialCsvParser ") + msg + ": " + strerror(errno)); \
  }
#define PERROR_ABORT(msg) \
  { \
    std::perror((std::string("Fatal from PartialCsvParser ") + std::string(msg)).c_str()); \
    std::abort(); \
  }

namespace PCP {

// Exception classes
class PCPError : public std::runtime_error {
public:
  PCPError(const std::string &cause)
  : std::runtime_error(cause)
  {}
};


// Utility functions
inline size_t _filesize(int opened_fd) throw(PCPError) {
  struct stat st;
  if (fstat(opened_fd, &st) != 0) STRERROR_THROW(PCPError, "while getting stat(2) of file");
  return st.st_size;
}

inline void _get_line_common_assert(
  const char * const text,
  size_t text_length_byte,
  size_t current_pos)
{
  ASSERT(text);
  ASSERT(text_length_byte >= 1);
  ASSERT(0 <= current_pos);
  ASSERT(current_pos <= text_length_byte - 1);
}

/**
 * Find a line including specified current_pos.
 * @param text Original text to find a line from.
 * @param text_length_byte Byte length of the original text.
 * @param current_pos Current position taking 0 ~ (text_length - 1).
 * @param line_terminator Character to terminate a line.
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
inline void _get_current_line(
  const char * const text,
  size_t text_length_byte,
  size_t current_pos,
  char line_terminator,
  /* out */
  const char ** line,
  size_t * line_length_byte)
{
  _get_line_common_assert(text, text_length_byte, current_pos);

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

/**
 * Find next line of specified current_pos.
 * @param text Original text to find a line from.
 * @param text_length_byte Byte length of the original text.
 * @param current_pos Current position taking 0 ~ (text_length - 1).
 * @param line_terminator Character to terminate a line.
 * @param line Pointer of start of the next line will be output.
 * @param line_length_byte Byte length of next line (not including line terminator) will be output.
 * @return false if current_pos is at the last line. true otherwise.
 *
   @verbatim
   \n aaaaaaaaaaa \n bbbbbbbbbbbbb \n  cccccccccc \0
                     ^           ^  ^  ^
                    (1)         (2)(3)(4)
   @endverbatim
 *
 * If current_pos is at between (1) and (2), next line will point at (4).
 * If current_pos is at (3), next line will also point at (4).
 * If curretn_pos is at (4), next line does not exist. Return false.
 *
   @verbatim
   \n ddddddddd \n eeeeeeeeeee \n \0
                   ^            ^
                  (1)          (2)
   @endverbatim
 *
 * Consider null character just after line terminator.
 * If current_pos is at between (1) and (2), next line will be "".
 */
inline bool _get_next_line(
  const char * const text,
  size_t text_length_byte,
  size_t current_pos,
  char line_terminator,
  /* out */
  const char ** line,
  size_t * line_length_byte)
{
  _get_line_common_assert(text, text_length_byte, current_pos);

  // \n aaaaaaaaaaaa \n
  //    ^            ^
  //    start        end
  const char *line_start = text + current_pos, *line_end;

  // search line_start
  while (line_start < text + text_length_byte && *line_start != line_terminator) {
    ++line_start;
    if (line_start == text + text_length_byte) return false;
  }
  ++line_start;
  // search line_end
  line_end = line_start;
  while (line_end < text + text_length_byte && *line_end != line_terminator) ++line_end;

  *line = line_start;
  *line_length_byte = line_end - line_start;
  return true;
}

inline std::vector<std::string> _split(const char * const str, size_t len, char delimiter) {
  ASSERT(str);
  ASSERT(len >= 0);

  std::vector<std::string> ret;  // NRVO optimization may prevent copy when returning this local variable.

  const char *p_beg = str, *p_end = str;
  while (p_end - str < len) {
    // come to delimiter
    if (*p_end == delimiter) {
      ret.push_back(std::string(p_beg, p_end - p_beg));
      p_beg = p_end + 1;
    }
    ++p_end;
  }
  // come to end of str
  ret.push_back(std::string(p_beg, p_end - p_beg));
  return ret;
}


class CsvConfig;
class PartialCsvParser;

class CsvConfig {
public:
  CsvConfig(
    const char * const filepath,
    bool has_header_line = true,
    char field_terminator = ',',
    char line_terminator = '\n',
    char enclosure_char = '"')
  throw(PCPError)
  : filepath(filepath), has_header_line(has_header_line),
    field_terminator(field_terminator), line_terminator(line_terminator),
    enclosure_char(enclosure_char),
    header_length(CsvConfig::HEADER_LENGTH_NOT_CALCULATED)
  {
    if ((fd = open(filepath, O_RDONLY)) == -1)
      STRERROR_THROW(PCPError, std::string("while open ") + filepath);
    csv_size = _filesize(fd);
    if ((csv_text = static_cast<const char *>(mmap(NULL, csv_size, PROT_READ, MAP_PRIVATE, fd, 0))) == (void*)-1)
      STRERROR_THROW(PCPError, std::string("while mmap ") + filepath);
  }

  ~CsvConfig() {
    if (munmap((void*)csv_text, csv_size) != 0) PERROR_ABORT("while munmap");
    if (close(fd) != 0) PERROR_ABORT("while closing file descriptor");
  }

  /**
   * Return the size of CSV file.
   */
  inline size_t filesize() const { return csv_size; }

  /**
   * Return the pointer of CSV content.
   */
  inline const char * const content() const { return csv_text; }

  /**
   * Return the offset where CSV body (excluding header line) starts from.
   */
  inline size_t body_offset() {
    if (!has_header_line) return 0;
    if (header_length != HEADER_LENGTH_NOT_CALCULATED) return header_length + 1;
    headers();  // calculate header_length
    return header_length;
  }

  /**
   * Return header string array.
   * has_header_line flag must be set true in constructor.
   */
  inline std::vector<std::string> headers() {
    ASSERT(has_header_line);
    std::vector<std::string> header;  // NRVO optimization may prevent copy when returning this local variable.

    const char * line = 0;
    _get_current_line(csv_text, csv_size, 0, line_terminator, &line, &header_length);

    return _split(line, header_length, field_terminator);
  }

  inline const char get_field_terminator() const { return field_terminator; }
  inline const char get_line_terminator() const { return line_terminator; }
  inline const char get_enclosure_char() const { return enclosure_char; }

private:
  const char * const filepath;
  const bool has_header_line;
  const char field_terminator;
  const char line_terminator;
  const char enclosure_char;

  int fd;
  size_t csv_size;
  const char * csv_text;

  size_t header_length;

  static const size_t HEADER_LENGTH_NOT_CALCULATED = -1;

  PREVENT_CLASS_DEFAULT_METHODS(CsvConfig);
};


class PartialCsvParser {
public:
  static const std::vector<std::string> NO_MORE_ROW;

  /**
   * Constructor.
   * @param csv_config Instance of CsvConfig.
   * @param read_from CSV file's <em>approximate</em> offset to start parsing. Must be no less than offset of CSV body.
   *   read_from = READ_FROM_BODY_BEGINNING has the same meaning with read_from = body_offset().
   * @param read_to CSV file's <em>approximate</em> offset to stop parsing. Must be greater than read_from and no greater than filesize().
   *   read_to = READ_TO_FILE_END has the same meaning with read_to = filesize() - 1.
   *
   * In order to fully parse CSV lines without overlaps, read_from and read_to are interpreted with the following strategy.
   *
     @verbatim
     <-------> means range from read_from to read_to.
     @endverbatim
   *
     @verbatim
     (beginning of CSV)  aaaaaaaaaaaaaaaa \0
                         <---><-----><-->
                          (1)   (2)   (3)
     @endverbatim
   *
   * In this severe edge case, only (1), <b>who covers the beginning of line</b>, parses line "aaaaaaaaaaaaaaaa" to prevent overlap.
   *
   *
     @verbatim
     (beginning of CSV)  aaaaaaaaaaaaa \n bbbbbbbbbbb \0
                         <---><-------->  <--------->
                          (1)    (2)          (3)
     @endverbatim
   *
   * In this case, (1) parses "aaaaaaaaaaaaa", (2) parses no line, and (3) parses "bbbbbbbbbbb".
   *
     @verbatim
     (beginning of CSV)  aaaaaaaaaaaaa \n bbbbbbbbbbb \0
                         <---><-----------><-------->
                          (1)      (2)         (3)
     @endverbatim
   *
   * In this case, (1) parses "aaaaaaaaaaaaa", (2) parses "bbbbbbbbbbb", and (3) parses no line.
   *
   * In short, <b>partial parser who covers the beginning of a line parses the line</b>.
   */
  PartialCsvParser(
    CsvConfig & csv_config,
    size_t read_from = READ_FROM_BODY_BEGINNING,
    size_t read_to = READ_TO_FILE_END)
  : csv_config(csv_config), read_from(read_from), read_to(read_to)
  {
    if (read_from == READ_FROM_BODY_BEGINNING) this->read_from = csv_config.body_offset();
    if (read_to == READ_TO_FILE_END) this->read_to = csv_config.filesize() - 1;
    cur_pos = this->read_from;
    ASSERT(csv_config.body_offset() <= this->read_from);
    ASSERT(this->read_from < this->read_to);
    ASSERT(this->read_to < csv_config.filesize());
  }

  ~PartialCsvParser() {}

  inline std::vector<std::string> get_row() {
    while (cur_pos <= read_to) {
      const char * line;
      size_t line_length;
      _get_current_line(csv_config.content(), csv_config.filesize(), cur_pos, csv_config.get_line_terminator(), &line, &line_length);

      // cur_pos exactly is the beginning of current line.
      //
      // (\n or beginning of CSV file)  aaaaaaaaaaaaaa \n
      //                                <----...
      //                                cur_pos
      //
      // Parse "aaaaaaaaaaaaaa" and move cur_pos to the beginning of the next line.
      if (csv_config.content() + cur_pos == line) {
        cur_pos += line_length + 1;  // +1 is from line_delimitor
        return _split(line, line_length, csv_config.get_field_terminator());
      }

      // read_to is at the same line with cur_pos.
      //
      // (\n or beginning of CSV file)  aaaaaaaaaaaaaa \n
      //                                    <---------->
      //                                    cur_pos    read_to
      if (csv_config.content() + read_to <= line + line_length + 1)  // +1 is from line_delimitor
        return NO_MORE_ROW;

      // read_to is beyond the same line with cur_pos.
      //
      // (\n or beginning of CSV file)  aaaaaaaaaaaaaa \n bbbbbbbbbb
      //                                    <--------------...
      //                                    cur_pos
      //
      // Move cur_pos to the beginning of the next line.
      if (csv_config.content() + read_to > line + line_length + 1)  // +1 is from line_delimitor
        cur_pos += line_length + 1;  // +1 is from line_delimitor
    }
    return NO_MORE_ROW;
  }

private:
  static const size_t READ_FROM_BODY_BEGINNING = -1;
  static const size_t READ_TO_FILE_END = -1;

  CsvConfig & csv_config;
  size_t read_from, read_to;
  size_t cur_pos;

  PREVENT_CLASS_DEFAULT_METHODS(PartialCsvParser);
};
const std::vector<std::string> PartialCsvParser::NO_MORE_ROW = std::vector<std::string>(0);

}

#endif /* INCLUDE_PARTIALCSVPARSER_HPP_ */
