CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(PartialCsvParser_benchmarks)

#
# setting variables
SET(PROJ_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
SET(BENCHMARK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

#
# compile environments
SET(CMAKE_CXX_FLAGS "-O2 -g -Wall ${CMAKE_CXX_FLAGS}")

INCLUDE_DIRECTORIES(
    ${PROJ_ROOT_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/csv_parser_cplusplus/include
)


#
# Build PartialCsvParser benchmark
ADD_EXECUTABLE(PartialCsvParser_bench PartialCsvParser_bench.cpp)
TARGET_LINK_LIBRARIES(PartialCsvParser_bench pthread)


#
# Get csv-parser-cplusplus
EXECUTE_PROCESS(COMMAND wget -O contrib/libcsv_parser++-1.0.0.tar.bz2 https://csv-parser-cplusplus.googlecode.com/files/libcsv_parser%2B%2B-1.0.0.tar.bz2)
EXECUTE_PROCESS(COMMAND tar xvf contrib/libcsv_parser++-1.0.0.tar.bz2 -C contrib/)
EXECUTE_PROCESS(COMMAND mv contrib/libcsv_parser++-1.0.0 contrib/csv_parser_cplusplus)

#
# Build csv-parser-cplusplus benchmark
ADD_EXECUTABLE(csv_parser_cplusplus_bench csv_parser_cplusplus_bench.cpp contrib/csv_parser_cplusplus/csv_parser.cpp)
