CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(PartialCsvParser)

#
# setting variables
SET(PROJ_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
SET(UNIT_TEST_DIR ${PROJ_ROOT_DIR}/test/unit)
SET(INTEGRATED_TEST_DIR ${PROJ_ROOT_DIR}/test/integrated)
SET(GTEST_DIR ${PROJ_ROOT_DIR}/contrib/gtest)
SET(GTEST_SRC
    ${GTEST_DIR}/src/gtest-all.cc
    ${GTEST_DIR}/src/gtest_main.cc
)

#
# compile environments
ADD_DEFINITIONS(
    -DPCP_GTEST
)

SET(CMAKE_CXX_FLAGS "-O0 -g -Wall -std=c++11 ${CMAKE_CXX_FLAGS}")

INCLUDE_DIRECTORIES(
    ${GTEST_DIR}
    ${GTEST_DIR}/include
    ${PROJ_ROOT_DIR}/include
)


#
# unit test
EXECUTE_PROCESS(
    COMMAND find ${UNIT_TEST_DIR} -name "*.cpp"
    COMMAND grep -v CMakeFiles
    COMMAND tr "\n" ";"
    OUTPUT_VARIABLE PCP_UNIT_TEST
)
SET(UNIT_TEST_SOURCE_FILES
    ${GTEST_SRC}
    ${PCP_UNIT_TEST}
)

ADD_EXECUTABLE(run_unit_test ${UNIT_TEST_SOURCE_FILES})
TARGET_LINK_LIBRARIES(run_unit_test pthread)

#
# integrated test
EXECUTE_PROCESS(
    COMMAND find ${INTEGRATED_TEST_DIR} -name "*.cpp"
    COMMAND grep -v CMakeFiles
    COMMAND tr "\n" ";"
    OUTPUT_VARIABLE PCP_INTEGRATED_TEST
)
SET(INTEGRATED_TEST_SOURCE_FILES
    ${GTEST_SRC}
    ${PCP_INTEGRATED_TEST}
)

ADD_EXECUTABLE(run_integrated_test ${INTEGRATED_TEST_SOURCE_FILES})
TARGET_LINK_LIBRARIES(run_integrated_test pthread)
