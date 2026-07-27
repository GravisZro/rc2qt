#include "rc_helpers.h"

#include <algorithm>
#include <cctype>

namespace rc
{
  std::string to_upper(const std::string& s)
  {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return c | 0x80 ? c : std::toupper(c); });
    return result;
  }
/*
  bool upper_contains(const std::string& needle, std::initializer_list<std::string> haystack)
  {
    std::string upper = rc::to_upper(needle);
    for(const auto& hey : haystack)
      if(upper == hey)
        return true;
    return false;
  }
  */

  int64_t safe_stoi(const std::string& s, int64_t default_value)
  {
    try
    {
      if(!s.empty() && (s[0] == '+' || s[0] == '-'))
        return std::stoll(s);
      if(s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        return std::stoll(s, nullptr, 16);
      return std::stoll(s);
    }
    catch(const std::exception&)
    {
      return default_value;
    }
  }

  uint64_t safe_stoul(const std::string& s, int base, uint64_t default_value)
  {
    try
    {
      return std::stoull(s, nullptr, base);
    }
    catch(const std::exception&)
    {
      return default_value;
    }
  }
}
