#ifndef RC_HELPERS_H
#define RC_HELPERS_H

#include <string>
#include <initializer_list>
#include <type_traits>

namespace rc
{
  // Generic bitwise operators for any enum used as a flag bitmask.
  // Allows combining enumerator values, e.g. category_t::a | category_t::b.
  template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
  constexpr Enum operator |(Enum lhs, Enum rhs)
  {
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) |
                             static_cast<std::underlying_type_t<Enum>>(rhs));
  }

  template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
  constexpr Enum operator &(Enum lhs, Enum rhs)
  {
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) &
                             static_cast<std::underlying_type_t<Enum>>(rhs));
  }

  template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
  constexpr bool operator ==(Enum lhs, Enum rhs)
  {
    return static_cast<std::underlying_type_t<Enum>>(lhs) ==
           static_cast<std::underlying_type_t<Enum>>(rhs);
  }

  template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
  constexpr bool operator <(Enum lhs, Enum rhs)
  {
    return static_cast<std::underlying_type_t<Enum>>(lhs) <
           static_cast<std::underlying_type_t<Enum>>(rhs);
  }

  std::string to_upper(const std::string& s);

  //bool upper_contains(const std::string& needle, std::initializer_list<std::string> haystack);
  int64_t safe_stoi(const std::string& s, int64_t default_value = 0);
  uint64_t safe_stoul(const std::string& s, int base = 0, uint64_t default_value = 0);

  std::string utf16le_to_utf8(const std::string& input);
  std::string cp1252_to_utf8(const std::string& input);

  bool match_string(const std::string& needle, std::initializer_list<std::string> haystack);
  std::string escape_string(const std::string& s);
}

#endif // RC_HELPERS_H
