#ifndef RC_HELPERS_H
#define RC_HELPERS_H

#include <string>
#include <string_view>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <format>
#include <iostream>
#include <algorithm>

constexpr auto expand_tabs(std::string_view str)
{
  std::string result;
  result.reserve(str.size());
  for (auto c : str)
  {
    if (c == '\t')
      result += ' ', result += ' ';
    else
      result += c;
  }
  return result;
}

template<typename... Args>
static inline void info(std::string_view fmt, Args... args)
{ std::cerr << std::vformat(expand_tabs(fmt), std::make_format_args(args...)) << std::endl << std::flush; }

template<typename... Args>
static inline void print(std::string_view fmt, Args... args)
{ std::cout << std::vformat(expand_tabs(fmt), std::make_format_args(args...)) << std::endl << std::flush; }

// Generic bitwise operators for any enum used as a flag bitmask.
// Allows combining enumerator values, e.g. category_t::a | category_t::b.
template<typename E, std::enable_if_t<std::is_enum_v<E>, bool> = false>
constexpr E operator | (E a, E b) { return E(std::to_underlying(a) | std::to_underlying(b)); }


template<typename E, std::enable_if_t<std::is_enum_v<E>, bool> = false>
constexpr bool operator > (E a, E b) { return std::to_underlying(a) > std::to_underlying(b); }

template<typename E, std::enable_if_t<std::is_enum_v<E>, bool> = false>
constexpr bool operator < (E a, E b) { return std::to_underlying(a) < std::to_underlying(b); }


template<typename E, std::enable_if_t<std::is_enum_v<E>, bool> = false>
constexpr E operator + (E a, E b) { return static_cast<E>(std::to_underlying(a) + std::to_underlying(b)); }

template<typename E, std::enable_if_t<std::is_enum_v<E>, bool> = false>
constexpr E operator - (E a, E b) { return static_cast<E>(std::to_underlying(a) - std::to_underlying(b)); }

constexpr std::string to_upper(const std::string& s)
{
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 /* Obsolete: '|' never skips the high bit and makes to_upper a no-op.
                 [](unsigned char c) { return c | 0x80 ? c : std::toupper(c); }); */
                 [](unsigned char c) { return c & 0x80 ? c : std::toupper(c); });
  return result;
}

int64_t safe_stoi(const std::string& s, int64_t default_value = 0);
uint64_t safe_stoul(const std::string& s, int base = 0, uint64_t default_value = 0);

std::string utf16le_to_utf8(const std::string& input);
std::string cp1252_to_utf8(const std::string& input);

#ifdef HAVE_ICU
void set_codepage(std::string& codepage_num);
void set_codepage(uint32_t codepage);
std::string codepage_to_utf8(const std::string& input);
#endif

bool match_string(const std::string& needle, std::initializer_list<std::string> haystack);
std::string escape_string(const std::string& s);

#endif // RC_HELPERS_H
