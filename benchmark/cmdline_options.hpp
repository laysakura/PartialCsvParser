#ifndef BENCHMARK_CMDLINE_OPTIONS_HPP_
#define BENCHMARK_CMDLINE_OPTIONS_HPP_

#include <algorithm>

inline char * get_cmdline_option(char ** begin, char ** end, const std::string & option) {
  char ** it = std::find(begin, end, option);
  if (it != end && ++it != end) return *it;
  return 0;
}

inline bool cmdline_option_exists(char ** begin, char ** end, const std::string & option) {
  return std::find(begin, end, option) != end;
}


#endif /* BENCHMARK_CMDLINE_OPTIONS_HPP_ */
