#ifndef BENCHMARK_BENCHMARK_HPP_
#define BENCHMARK_BENCHMARK_HPP_

#include <iostream>
#include <ctime>
#include <sys/time.h>

inline double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

double _t_start, _t_stop;

#define BENCH_START \
  _t_start = gettimeofday_sec();

#define BENCH_STOP(msg) \
  _t_stop = gettimeofday_sec(); \
  std::cerr << __FILE__ << ":" << __LINE__ << " - " << _t_stop - _t_start << " seconds - " << msg << std::endl;


#endif /* BENCHMARK_BENCHMARK_HPP_ */
