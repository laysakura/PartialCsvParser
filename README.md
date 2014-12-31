# PartialCsvParser

[![Build Status](https://travis-ci.org/laysakura/PartialCsvParser.svg?branch=master)](https://travis-ci.org/laysakura/PartialCsvParser)

[PartialCsvParser](https://github.com/laysakura/PartialCsvParser) is a C++ CSV parser.

This parser is meant to be created to **parse a CSV file in parallel**.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [Installation](#installation)
- [Features](#features)
- [Examples](#examples)
  - [Simplest example: Parse and print a CSV file](#simplest-example-parse-and-print-a-csv-file)
  - [More examples](#more-examples)
- [Anti-features](#anti-features)
- [Reference manual](#reference-manual)
- [Parser behaviors](#parser-behaviors)
  - [All lines of CSV file are parsed exactly once](#all-lines-of-csv-file-are-parsed-exactly-once)
- [For developers](#for-developers)
  - [How to run test cases](#how-to-run-test-cases)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


## Installation

PartialCsvParser is a **Single-header library** distributed under public domain.

Just copy [PartialCsvParser.hpp](./include/PartialCsvParser.hpp) into your include path and include it.
You can also `git add` the header file to your repository, and even modify it.

I appreciate your pull requests if you make some improvements :)


## Features

- Pretty good single-thread & multi-thread performance.
    - Following graphs show sequential performance comparison with other CSV parsers and scalability evaluation.
      Check [benchmark/](./benchmark) for more detailed explanation on performance.

      ![Comparison of CSV parser's performance](https://docs.google.com/spreadsheets/d/1ZqmajL9r4aXAvk_7rp3j7KdLWN71-IbWhVtxB6HpSw4/pubchart?oid=1550764323&format=image)
      ![Scalability on clokoap100](https://docs.google.com/spreadsheets/d/1ZqmajL9r4aXAvk_7rp3j7KdLWN71-IbWhVtxB6HpSw4/pubchart?oid=1943811886&format=image)

- Input CSV from both files and memories.

- Simple interface working with STL (Standard Template Library).

- Column separator (`,` by default) and line separator (`\n` by default) are customizable.
    - Also usable for TSV parsing.

- Parses both CSV with header line and without it.

- UTF-8 support.

- Range in a file can be specified to parse part of a CSV file.
    - Data-parallelism is easily realized by creating threads with different range.


## Examples

Some examples are available in [example/](./example) directory.

You can also build and run them quickly.

```bash
$ cd example/
$ cmake . && make
$ ./00_parse_with_1parser
```

### Simplest example: Parse and print a CSV file

[example/00_parse_with_1parser.cpp](./example/00_parse_with_1parser.cpp)

```c++
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
```

Output:

```
$ ./00_parse_with_1parser
Headers:
Country Name    Style

Got a row: Japan        Shonan Gold     Fruit Beer
Got a row: Scotland     Punk IPA        IPA
Got a row: Germany      Franziskaner    Hefe-Weissbier
```

### More examples

- [Parses a CSV file in parallel](./example/01_parse_with_2parsers_threaded.cpp)
- [UTF-8 TSV from memory](./example/03_parse_tsv_from_memory.cpp)


## Anti-features

- Parsing only. No support to write out a CSV file.

- Multi-byte line separator like CRLF is not supported.
    - This may be easily fixed, thanks :D

- **Enclosure character (usually `"`) is not supported**.
    - The following CSV file is recognized to have 2-row and 2-column,
      while it should 1-row and 3-column if `"` is treated as enclosure character.

      ```csv
      aaa,"bbb
      ccccccc",ddd
      ```

    - The reason: Say you are a parser.  You have the range starting with 3rd 'c'.
      You see `"` in front of you.  Is that open-quote or close-quote?
      You cannot tell without parsing from the beginning of file.


## Reference manual

[Reference manual](http://laysakura.github.io/PartialCsvParser) powered by Doxygen is available.


## Parser behaviors

### All lines of CSV file are parsed exactly once

[PCP::PartialCsvParser::PartialCsvParser()](http://laysakura.github.io/PartialCsvParser/class_p_c_p_1_1_PartialCsvParser.html#aaf19c1d20e4337cea84596d22eb915ed)
takes 2 offsets: `parse_from` and `parse_to`.

If you have multiple threads and each of them holds different part of `[parse_from, parse_to]`,
CSV file is parsed in parallel.

It is assured that all lines of CSV file are parsed exactly once if all instances of parsers' `[parse_from, parse_to]` ranges
cover `[` [PCP::CsvConfig::body_offset()](http://laysakura.github.io/PartialCsvParser/class_p_c_p_1_1_csv_config.html#aa28eae4c19dda51ea055973ae2cdd84e) `,` [PCP::CsvConfig::filesize()](http://laysakura.github.io/PartialCsvParser/class_p_c_p_1_1_csv_config.html#a596c6599cb72bf93a5cc4a7f4b07fadf) `- 1]`
without gaps and overlaps (See the following diagram).

```
header1,header2 \n aaaaaaaa,bbbbbbbbbb \n ccccccccc,dddd \n
                   ^                                      ^
                   body_offset                            filesize - 1

                   <----------><-----------><------------->
                     parser1      parser2       parser3
```

## For developers

### How to run test cases

1. Get Google Test.

  ```bash
  $ wget https://googletest.googlecode.com/files/gtest-1.7.0.zip
  $ unzip gtest-1.7.0.zip
  $ mv gtest-1.7.0 /path/to/PartialCsvParser/contrib/gtest
```

2. Build test cases executables.

  ```bash
  $ cd test
  $ cmake . && make
  ```

3. Run unit tests and integrated tests.

  ```bash
  $ ./run_unit_test
  $ ./run_integrated_test
  ```

## Author

Sho Nakatani, a.k.a. [laysakura](https://github.com/laysakura)


## LICENSE

This project is distributed under public domain.

See [UNLICENSE](./UNLICENSE) for more detailed explanation.
