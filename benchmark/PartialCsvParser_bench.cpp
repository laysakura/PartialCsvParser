#include <PartialCsvParser.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include "benchmark.hpp"
#include "cmdline_options.hpp"


typedef struct parser_thread_arg_t {
  PCP::partial_csv_t partial_csv;
  size_t n_columns;
} parser_thread_arg_t;

void * partial_parse(parser_thread_arg_t * arg) {
  // instantiate parser
  PCP::PartialCsvParser parser(*arg->partial_csv.csv_config, arg->partial_csv.parse_from, arg->partial_csv.parse_to);

  // parse & count-up columns
  std::vector<std::string> row;
  while (!(row = parser.get_row()).empty()) arg->n_columns += row.size();

  return NULL;
}

inline void help_exit(int argc, char * argv[]) {
  std::cerr << argv[0] << " [-h] -p N_THREADS -c N_EXPECTED_COLUMNS -f FILENAME" << std::endl;
  exit(2);
}

int main(int argc, char * argv[]) {
  // command line options
  if (cmdline_option_exists(argv, argv + argc, "-h")) help_exit(argc, argv);

  const char * n_threads_str = get_cmdline_option(argv, argv + argc, "-p");
  if (!n_threads_str) help_exit(argc, argv);
  const size_t n_threads = std::atoi(n_threads_str);

  const char * n_expected_columns_str = get_cmdline_option(argv, argv + argc, "-c");
  if (!n_expected_columns_str) help_exit(argc, argv);
  const size_t n_expected_columns = std::atoi(n_expected_columns_str);

  const char * filepath = get_cmdline_option(argv, argv + argc, "-f");
  if (!filepath) help_exit(argc, argv);

  // instantiate CsvConfig
  BENCH_START;
  PCP::CsvConfig csv_config(filepath, false);
  BENCH_STOP("mmap(2)+madvise(2) file");

  // setup range each thread parse
  size_t size_per_thread = (csv_config.filesize() - csv_config.body_offset()) / n_threads;
  std::vector<parser_thread_arg_t> parser_thread_args(n_threads);
  for (size_t i = 0; i < n_threads; ++i) {
    parser_thread_arg_t & parser_thread_arg = parser_thread_args[i];
    parser_thread_arg.n_columns = 0;

    PCP::partial_csv_t & partial_csv = parser_thread_arg.partial_csv;
    partial_csv.csv_config = &csv_config;
    partial_csv.parse_from = csv_config.body_offset() + i * size_per_thread;
    partial_csv.parse_to = csv_config.body_offset() + (i + 1) * size_per_thread - 1;
  }

  // create threads
  std::vector<pthread_t> tids(n_threads);
  BENCH_START;
  for (size_t i = 0; i < n_threads; ++i)
      pthread_create(&tids[i], NULL, (void *(*)(void *))partial_parse, &parser_thread_args[i]);
  // join threads
  for (size_t i = 0; i < n_threads; ++i)
      pthread_join(tids[i], NULL);
  BENCH_STOP("join parsing threads");

  // calculate total number of columns
  size_t n_total_columns = 0;
  for (size_t i = 0; i < n_threads; ++i) n_total_columns += parser_thread_args[i].n_columns;

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
