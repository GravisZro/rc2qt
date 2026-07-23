#ifndef RC_TOKENIZER_H
#define RC_TOKENIZER_H

#include <cstdint>
#include <string>
#include <vector>

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
};

std::vector<token> tokenize(const std::string& input);

}

#endif // RC_TOKENIZER_H
