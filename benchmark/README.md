# PartialCsvParser benchmark

Following benchmarks are taken.

- Scalability of PartialCsvParser.

- Comparison with other CSV parser.
    - [csv-parser-cplusplus](https://code.google.com/p/csv-parser-cplusplus/)


## Benchmark results


You can also conduct experiments with your own environments following the instructions below.


## Generate benchmark data

```bash
$ ../script/generate-benchmark-data.sh 12  # you can use bigger or smaller data
$ ls csv/
20480000col.csv 5000col.csv
```

## Build benchmark executables

Internet connection is necessary because Makefile internally invoke `wget` to get other libraries to compare the performance with.

```bash
$ cmake . && make
```

## Run PartialCsvParser benchmark

With 4 threads, for example.

```bash
$ time ./PartialCsvParser_bench -p 4 -c 20480000 -f csv/20480000col.csv
/Users/nakatani.sho/git/partial_csv_parser/benchmark/PartialCsvParser_bench.cpp:50 - 0.0186529 seconds - mmap(2) file
/Users/nakatani.sho/git/partial_csv_parser/benchmark/PartialCsvParser_bench.cpp:73 - 3.97924 seconds - join parsing threads
OK. Parsed 20480000 columns.

real    0m4.010s
user    0m13.796s
sys     0m0.172s
```

Check the wall-clock time. 4.010 seconds in this execution.


## Run csv-parser-cplusplus benchmark

```bash
$ time ./csv_parser_cplusplus_bench -c 20480000 -f csv/20480000col.csv
/Users/nakatani.sho/git/partial_csv_parser/benchmark/csv_parser_cplusplus_bench.cpp:42 - 34.0444 seconds - parse
OK. Parsed 20480000 columns.

real    0m34.049s
user    0m33.022s
sys     0m0.498s
```
