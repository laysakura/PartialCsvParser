/**
 * csv-parser-cplusplus is available in: https://code.google.com/p/csv-parser-cplusplus
 */

#include <csv_parser/csv_parser.hpp>
#include <iostream>
#include <cstdlib>
#include "benchmark.hpp"
#include "cmdline_options.hpp"


inline void help_exit(int argc, char * argv[]) {
  std::cerr << argv[0] << " [-h] -c N_EXPECTED_COLUMNS -f FILENAME" << std::endl;
  exit(2);
}

int main(int argc, char * argv[]) {
  // command line options
  if (cmdline_option_exists(argv, argv + argc, "-h")) help_exit(argc, argv);

  const char * n_expected_columns_str = get_cmdline_option(argv, argv + argc, "-c");
  if (!n_expected_columns_str) help_exit(argc, argv);
  const size_t n_expected_columns = std::atoi(n_expected_columns_str);

  const char * filepath = get_cmdline_option(argv, argv + argc, "-f");
  if (!filepath) help_exit(argc, argv);

  // setup CSV parser
  csv_parser file_parser;
  const char field_terminator = ',';
  const char line_terminator  = '\n';
  const char enclosure_char   = '"';
  file_parser.init(filepath);
  file_parser.set_enclosed_char(enclosure_char, ENCLOSURE_NONE);  // Same as PartialCsvParser
  file_parser.set_field_term_char(field_terminator);
  file_parser.set_line_term_char(line_terminator);

  // parse and calculate the number of columns
  size_t n_total_columns = 0;
  BENCH_START;
  while(file_parser.has_more_rows()) n_total_columns += file_parser.get_row().size();
  BENCH_STOP("parse");

  // check the answer
  if (n_total_columns == n_expected_columns) {
    std::cout << "OK. Parsed " << n_total_columns << " columns." << std::endl;
    return 0;
  }
  else {
    std::cout << "NG. Parsed " << n_total_columns << " columns, while " << n_expected_columns << " columns are expected." << std::endl;
    return 1;
  }
}
