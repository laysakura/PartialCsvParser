/**
 * Parses a TSV without header from null-terminated memory and print the contents.
 */

#include <PartialCsvParser.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() {
  const char * const tsv =
    "Monday\tTuesday\tWednesday\tThursday\tFriday\tSaturday\tSunday\n"
    "月曜\t火曜\t水曜\t木曜\t金曜\t土曜\t日曜\n";
  PCP::Memory::CsvConfig csv_config(tsv, false, '\t');

  // instantiate parser
  PCP::PartialCsvParser parser(csv_config);

  // parse & print favorite day of week
  std::vector<std::string> row;
  while (!(row = parser.get_row()).empty())
    std::cout << row[5] << " is my favorite!" << std::endl;

  return 0;
}
