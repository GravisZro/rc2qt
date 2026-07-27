#ifndef RC_HELPERS_H
#define RC_HELPERS_H

#include <string>

namespace rc
{
  std::string to_upper(const std::string& s);

  //bool upper_contains(const std::string& needle, std::initializer_list<std::string> haystack);
  int64_t safe_stoi(const std::string& s, int64_t default_value = 0);
  uint64_t safe_stoul(const std::string& s, int base = 0, uint64_t default_value = 0);

}

#endif // RC_HELPERS_H
