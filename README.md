# PartialCsvParser

[![Build Status](https://travis-ci.org/laysakura/partial_csv_parser.svg?branch=master)](https://travis-ci.org/laysakura/partial_csv_parser)

[PartialCsvParser](https://github.com/laysakura/partial_csv_parser) is a C++ CSV parser.

This parser is meant to be created to **parse a CSV file in parallel**.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [PartialCsvParser](#partialcsvparser)
  - [Features](#features)
  - [Examples](#examples)
    - [Simplest example: Parse and print a CSV file](#simplest-example-parse-and-print-a-csv-file)
    - [Multi-thread example: Parses a CSV file in parallel](#multi-thread-example-parses-a-csv-file-in-parallel)
  - [Anti-features](#anti-features)
  - [Reference manual](#reference-manual)
  - [Parser behaviors](#parser-behaviors)
    - [All lines of CSV file are parsed exactly once](#all-lines-of-csv-file-are-parsed-exactly-once)
  - [For developers](#for-developers)
    - [How to run test cases](#how-to-run-test-cases)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


## Features

- **Single-header library**.
    - Just copy [PartialCsvParser.hpp](./include/PartialCsvParser.hpp) into your project and include it.
    - You can freely add the header file to your repository, and even modify it.
        - I appreciate your pull requests if you make some improvements :)

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

### Multi-thread example: Parses a CSV file in parallel

[example/01_parse_with_2parsers_threaded.cpp](./example/01_parse_with_2parsers_threaded.cpp)

```c++
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
  PCP::PartialCsvParser parser(partial_csv->csv_config, partial_csv->parse_from, partial_csv->parse_to);

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
  PCP::partial_csv_t partial_csv1 = { csv_config, csv_config.body_offset(), half_offset };
  PCP::partial_csv_t partial_csv2 = { csv_config, half_offset + 1, csv_config.filesize() - 1 };

  // create threads, join them.
  pthread_t th1, th2;
  pthread_create(&th1, NULL, (void *(*)(void *))partial_parse, &partial_csv1);
  pthread_create(&th2, NULL, (void *(*)(void *))partial_parse, &partial_csv2);
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  return 0;
}
```

Lucky output:

```
$ ./01_parse_with_2parsers_threaded
Headers:
Country Name    Style

[thread#0x104fb5000] Got a row: Japan   Shonan Gold     Fruit Beer
[thread#0x105038000] Got a row: Scotland        Punk IPA        IPA
[thread#0x105038000] Got a row: Germany Franziskaner    Hefe-Weissbier
```

Unlucky output:

```
$ ./01_parse_with_2parsers_threaded
Headers:
Country Name    Style

[thread#[thread#0x01x0140a433000] Got a ro9wb:0 0S0c0o]t lGaontd        aP row: Japan   Shonan Gold     Fruit Beer
unk IPA IPA
[thread#0x104a33000] Got a row: Germany Franziskaner    Hefe-Weissbier
```

Since `partial_parse()` invocations are not ordered, printed result may be such a mess.


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

[Reference manual](http://laysakura.github.io/partial_csv_parser) powered by Doxygen is available.


## Parser behaviors

### All lines of CSV file are parsed exactly once

[PCP::PartialCsvParser::PartialCsvParser()](http://laysakura.github.io/partial_csv_parser/class_p_c_p_1_1_partial_csv_parser.html#aaf19c1d20e4337cea84596d22eb915ed)
takes 2 offsets: `parse_from` and `parse_to`.

If you have multiple threads and each of them holds different part of `[parse_from, parse_to]`,
CSV file is parsed in parallel.

It is assured that all lines of CSV file are parsed exactly once if all instances of parsers' `[parse_from, parse_to]` ranges
cover `[` [PCP::CsvConfig::body_offset()](http://laysakura.github.io/partial_csv_parser/class_p_c_p_1_1_csv_config.html#aa28eae4c19dda51ea055973ae2cdd84e) `,` [PCP::CsvConfig::filesize()](http://laysakura.github.io/partial_csv_parser/class_p_c_p_1_1_csv_config.html#a596c6599cb72bf93a5cc4a7f4b07fadf) `- 1]`
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
  $ mv gtest-1.7.0 /path/to/partial_csv_parser/contrib/gtest
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
