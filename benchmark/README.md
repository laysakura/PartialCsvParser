<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [PartialCsvParser benchmark](#partialcsvparser-benchmark)
  - [Evaluation settings](#evaluation-settings)
    - [MBA](#mba)
    - [clokoap100](#clokoap100)
    - [Tips: How to check read speed](#tips-how-to-check-read-speed)
    - [Tips: Memory file system on Mac OSX](#tips-memory-file-system-on-mac-osx)
  - [Benchmark results](#benchmark-results)
    - [Comparison with othre libraries](#comparison-with-othre-libraries)
    - [Scalability](#scalability)
  - [Generate benchmark data](#generate-benchmark-data)
  - [Build benchmark executables](#build-benchmark-executables)
  - [Run PartialCsvParser benchmark](#run-partialcsvparser-benchmark)
  - [Run csv-parser-cplusplus benchmark](#run-csv-parser-cplusplus-benchmark)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# PartialCsvParser benchmark

Following benchmarks are taken.

- Scalability of PartialCsvParser.

- Comparison with other CSV parser.
    - [PartialCsvParser](https://github.com/laysakura/PartialCsvParser) v0.1.1
    - [csv-parser-cplusplus](https://code.google.com/p/csv-parser-cplusplus/) v1.0.0


## Evaluation settings

Parses this CSV file and just counts the total number of columns:

- Size: 412 MB
- Number of rows: 8,192,000
- Number of columns: 5
- First 5 rows:

  ```csv
  1,Douglas,Sanchez,dsanchez0@toplist.cz,2.221.14.151
  2,Robert,Hudson,rhudson1@wisc.edu,211.131.138.32
  3,Victor,Taylor,vtaylor2@state.gov,41.251.57.238
  4,Kelly,Harvey,kharvey3@sun.com,69.31.229.204
  5,John,Adams,jadams4@washington.edu,206.51.26.246
  ```

Used 2 environments.

- MBA
    - Macbook Air, a comodity laptop as you know.

- clokoap100
    - Middle class server with both SSD and HDD.

More detailed specification follows.

### MBA

|                                   |               |
|-----------------------------------|---------------|
| CPU clock                         | 1.3 GHz       |
| # CPU sockets                     | 1             |
| # Cores per socket                | 2             |
| # Logical cores per physical core | 2             |
|                                   |               |
| Memory size                       | 8 GB          |
| Memory sequential read            | 2.142 GB/sec  |
| Memory random read                | 1.467 GB/sec  |
| Memory file system                | hfs           |
|                                   |               |
| SSD sequential read               | 492.289 MB/s  |
| SSD random read                   | 11.374 MB/sec |
| SSD file system                   | hfs           |


### clokoap100

|                                   |               |
|-----------------------------------|---------------|
| CPU clock                         | 2.393 GHz      |
| # CPU sockets                     | 2              |
| # Cores per socket                | 4              |
| # Logical cores per physical core | 2              |
|                                   |                |
| Memory size                       | 24 GB          |
| Memory sequential read            | 2.194 GB/sec   |
| Memory random read                | 1.759 GB/sec   |
| Memory file system                | tmpfs          |
|                                   |                |
| SSD sequential read               | 470.114 MB/s   |
| SSD random read                   | 30.350 MB/sec  |
| SSD file system                   | ext3           |
|                                   |                |
| HDD sequential read               | 261.251 MB/sec |
| HDD random read                   | 4.191 MB/sec   |
| HDD file system                   | ext3           |

### Tips: How to check read speed

Used [fio](https://github.com/axboe/fio) to check random/sequential read speed.

```bash
$ vim random-read-mem.fio
[random-read]
rw=randread
size=512m
directory=/dev/shm

$ vim sequential-read-mem.fio
[sequential-read]
rw=read
size=512m
directory=/dev/shm

$ fio random-read-mem.fio
```

Change the `directory` to access to different devices.

### Tips: Memory file system on Mac OSX

Unlike Linux, Mac OSX does not have tmpfs mounted to `/dev/shm`.

You can create and destroy memory file system by the following way.

```bash
$ hdid -nomount ram://2097152  # 512 bytes * 2097152 sectors = 1024 Mbytes
/dev/disk2
$ newfs_hfs /dev/disk2
Initialized /dev/rdisk2 as a 1024 MB case-insensitive HFS Plus volume
$ mkdir /tmp/mnt
$ mount -t hfs /dev/disk2 /tmp/mnt

$ hdiutil detach /dev/disk2
```

## Benchmark results

### Comparison with othre libraries

![Comparison of CSV parser's performance](https://docs.google.com/spreadsheets/d/1ZqmajL9r4aXAvk_7rp3j7KdLWN71-IbWhVtxB6HpSw4/pubchart?oid=1550764323&format=image)

### Scalability

- Scalability on clokoap100

  ![Scalability on clokoap100](https://docs.google.com/spreadsheets/d/1ZqmajL9r4aXAvk_7rp3j7KdLWN71-IbWhVtxB6HpSw4/pubchart?oid=1943811886&format=image)

- Scalability on MBA

  ![Scalability on MBA](https://docs.google.com/spreadsheets/d/1ZqmajL9r4aXAvk_7rp3j7KdLWN71-IbWhVtxB6HpSw4/pubchart?oid=97166348&format=image)


[Raw data](https://docs.google.com/spreadsheets/d/1ZqmajL9r4aXAvk_7rp3j7KdLWN71-IbWhVtxB6HpSw4/edit#gid=2109635011) is available at Google SpreadSheet.

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
/Users/nakatani.sho/git/PartialCsvParser/benchmark/PartialCsvParser_bench.cpp:50 - 0.0186529 seconds - mmap(2) file
/Users/nakatani.sho/git/PartialCsvParser/benchmark/PartialCsvParser_bench.cpp:73 - 3.97924 seconds - join parsing threads
OK. Parsed 20480000 columns.

real    0m4.010s
user    0m13.796s
sys     0m0.172s
```

Check the wall-clock time. 4.010 seconds in this execution.


## Run csv-parser-cplusplus benchmark

```bash
$ time ./csv_parser_cplusplus_bench -c 20480000 -f csv/20480000col.csv
/Users/nakatani.sho/git/PartialCsvParser/benchmark/csv_parser_cplusplus_bench.cpp:42 - 34.0444 seconds - parse
OK. Parsed 20480000 columns.

real    0m34.049s
user    0m33.022s
sys     0m0.498s
```
