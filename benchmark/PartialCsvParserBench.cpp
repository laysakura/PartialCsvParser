#include <PartialCsvParser.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <pthread.h>

static std::unordered_map<pthread_t, size_t> n_columns;

void * partial_parse(PCP::partial_csv_t * partial_csv) {
  // initialize counter
  pthread_t tid = pthread_self();
  n_columns[tid] = 0;

  // instantiate parser
  PCP::PartialCsvParser parser(*partial_csv->csv_config, partial_csv->parse_from, partial_csv->parse_to);

  // parse & count-up columns
  std::vector<std::string> row;
  while (!(row = parser.get_row()).empty()) n_columns[tid] += row.size();

  return NULL;
}

int main() {
  const char * filepath = "in.csv";
  const size_t n_threads = 1;

  PCP::CsvConfig csv_config(filepath, false);

  // Setup range each thread parse.
  size_t size_per_thread = (csv_config.filesize() - csv_config.body_offset()) / n_threads;
  std::vector<PCP::partial_csv_t> partial_csvs(n_threads);
  for (size_t i = 0; i < n_threads; ++i) {
    PCP::partial_csv_t & partial_csv = partial_csvs[i];
    partial_csv.csv_config = &csv_config;
    partial_csv.parse_from = csv_config.body_offset() + i * size_per_thread;
    partial_csv.parse_to = csv_config.body_offset() + (i + 1) * size_per_thread - 1;
  }

  // create threads
  std::vector<pthread_t> tids(n_threads);
  for (size_t i = 0; i < n_threads; ++i)
      pthread_create(&tids[i], NULL, (void *(*)(void *))partial_parse, &partial_csvs[i]);
  // join threads
  for (size_t i = 0; i < n_threads; ++i)
      pthread_join(tids[i], NULL);

  // calculate total number of columns
  size_t n_total_columns = 0;
  for (size_t i = 0; i < n_threads; ++i) n_total_columns += n_columns[tids[i]];

  std::cout << n_total_columns << std::endl;

  return 0;
}
