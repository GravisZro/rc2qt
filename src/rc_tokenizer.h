#ifndef RC_TOKENIZER_H
#define RC_TOKENIZER_H

#include <cstdint>
#include <string>
#include <string_view>
#include <initializer_list>
#include <vector>
#include <ranges>
#include <algorithm>
#include "utils.h"

namespace rc
{

enum class token_type
{
  string_literal,
  integer_literal,
  hex_literal,
  identifier,
  comma,
  pipe,
  begin,
  end,
  newline,
  eof,
};

struct token
{
  token_type type;
  std::string value;
  size_t line = 0;

  template<typename T>
  constexpr bool operator != (T other) const { return !operator ==(other); }

  constexpr bool operator == (const token_type other_type) const { return other_type == type; }
  constexpr bool operator == (const std::string& other_value) const { return other_value == to_upper(value); }

  constexpr bool operator == (std::initializer_list<std::string> haystack) const
    { return std::ranges::any_of(haystack, [upper = to_upper(value)](auto str) { return str == upper; }); }

  constexpr bool operator == (std::initializer_list<token_type> haystack) const
    { return std::ranges::any_of(haystack, [this](auto token) { return token == type; }); }
};

std::vector<token> tokenize(const std::string& input);

}

#endif // RC_TOKENIZER_H
