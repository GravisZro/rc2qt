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

enum class token_type : uint32_t
{
  string_literal  = 0x00000001,
  integer_literal = 0x00000002,
  hex_literal     = 0x00000004,
  expression      = 0x00000400,
  number_literal  = integer_literal | hex_literal | expression,
  identifier      = 0x00000008,
  string_or_ident = string_literal | identifier,
  int_or_ident    = integer_literal | identifier,
  num_or_ident    = number_literal | identifier,
  str_num_ident   = string_or_ident | number_literal,
  comma           = 0x00000010,
  pipe            = 0x00000020,
  begin           = 0x00000040,
  end             = 0x00000080,
  newline         = 0x00000100,
  eof             = 0x00000200,
  begin_eof        = begin | eof,
  end_eof          = end | eof,
  newline_eof      = newline | eof,
  newline_end_eof = newline | end | eof,
};

struct token
{
  token_type type;
  std::string value;
  size_t line = 0;

  constexpr operator token_type(void) const { return type; }
  constexpr operator std::string(void) const { return value; }

  template<typename T>
  constexpr bool operator != (T other) const { return !operator ==(other); }

  constexpr bool operator == (const token_type haystack) const
    { return std::to_underlying(haystack) & std::to_underlying(type); }

  constexpr bool operator == (const std::string& other_value) const { return other_value == to_upper(value); }

  constexpr bool operator == (std::initializer_list<std::string> haystack) const
    { return std::ranges::any_of(haystack, [upper = to_upper(value)](auto str) { return str == upper; }); }

};

std::vector<token> tokenize(const std::string& input, std::string code_page = "");

}

#endif // RC_TOKENIZER_H
