#include <PartialCsvParser.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <pthread.h>

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

int main() {
  const char * filepath = "in.csv";
  const size_t n_threads = 4;

  PCP::CsvConfig csv_config(filepath, false);

  // Setup range each thread parse.
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
  for (size_t i = 0; i < n_threads; ++i)
      pthread_create(&tids[i], NULL, (void *(*)(void *))partial_parse, &parser_thread_args[i]);
  // join threads
  for (size_t i = 0; i < n_threads; ++i)
      pthread_join(tids[i], NULL);

  // calculate total number of columns
  size_t n_total_columns = 0;
  for (size_t i = 0; i < n_threads; ++i) n_total_columns += parser_thread_args[i].n_columns;

  std::cout << n_total_columns << std::endl;

  return 0;
}
