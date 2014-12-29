/**
 * Parses a CSV file and print the contents.
 */

#include <PartialCsvParser.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() {
  PCP::CsvConfig csv_config("english.csv");

  // parse header line
  std::vector<std::string> headers = csv_config.get_headers();
  // print headers
  std::cout << "Headers:" << std::endl;
  for (size_t i = 0; i < headers.size(); ++i)
    std::cout << headers[i] << "\t";
  std::cout << std::endl << std::endl;

  // instantiate parser
  PCP::PartialCsvParser parser(csv_config);  // parses whole body of CSV without range options.

  // parse & print body lines
  std::vector<std::string> row;
  while (!(row = parser.get_row()).empty()) {
    std::cout << "Got a row: ";
    for (size_t i = 0; i < row.size(); ++i)
      std::cout << row[i] << "\t";
    std::cout << std::endl;
  }

  return 0;
}
