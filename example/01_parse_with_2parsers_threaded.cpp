/**
 * Parses a CSV file and print the contents using 2 threads.
 * Don't care if the output is mixed!
 */

#include <PartialCsvParser.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>

void * partial_parse(PCP::partial_csv_t * partial_csv) {
  // instantiate parser
  PCP::PartialCsvParser parser(*partial_csv->csv_config, partial_csv->parse_from, partial_csv->parse_to);

  // parse & print body lines
  std::vector<std::string> row;
  while (!(row = parser.get_row()).empty()) {
    std::cout << "[thread#" << pthread_self() << "] "<< "Got a row: ";
    for (size_t i = 0; i < row.size(); ++i)
      std::cout << row[i] << "\t";
    std::cout << std::endl;
  }
  return NULL;
}

int main() {
  PCP::CsvConfig csv_config("english.csv");

  // parse header line
  std::vector<std::string> headers = csv_config.get_headers();
  // print headers
  std::cout << "Headers:" << std::endl;
  for (size_t i = 0; i < headers.size(); ++i)
    std::cout << headers[i] << "\t";
  std::cout << std::endl << std::endl;

  // Setup range each thread parse.
  // One thread parses the former part of CSV, the other parses latter.
  size_t half_offset = (csv_config.filesize() - csv_config.body_offset()) / 2;
  PCP::partial_csv_t partial_csv1 = { &csv_config, csv_config.body_offset(), half_offset };
  PCP::partial_csv_t partial_csv2 = { &csv_config, half_offset + 1, csv_config.filesize() - 1 };

  // create threads, join them.
  pthread_t th1, th2;
  pthread_create(&th1, NULL, (void *(*)(void *))partial_parse, &partial_csv1);
  pthread_create(&th2, NULL, (void *(*)(void *))partial_parse, &partial_csv2);
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  return 0;
}
