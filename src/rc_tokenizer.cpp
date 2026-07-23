#include "rc_tokenizer.h"

#include <algorithm>
#include <cctype>

namespace rc
{

static bool is_id_start(char c)
{
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '#';
}

static bool is_id_char(char c)
{
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-';
}

static std::string read_string(const std::string& input, size_t& pos, size_t& line)
{
  std::string value;
  while(pos < input.size())
  {
    char c = input[pos++];
    if(c == '"')
      break;
    if(c == '\\')
    {
      if(pos >= input.size())
        break;
      char esc = input[pos++];
      switch(esc)
      {
        case 'n': value.push_back('\n'); break;
        case 't': value.push_back('\t'); break;
        case 'r': value.push_back('\r'); break;
        case '\\': value.push_back('\\'); break;
        case '"': value.push_back('"'); break;
        case '0': value.push_back('\0'); break;
        default: value.push_back(esc); break;
      }
    }
    else
    {
      if(c == '\n')
        ++line;
      value.push_back(c);
    }
  }
  return value;
}

static std::string read_hex(const std::string& input, size_t& pos)
{
  std::string value = "0x";
  while(pos < input.size())
  {
    char c = input[pos];
    if(std::isxdigit(static_cast<unsigned char>(c)))
    {
      value.push_back(c);
      ++pos;
    }
    else
      break;
  }
  return value;
}

static std::string read_number(const std::string& input, size_t& pos)
{
  std::string value;
  while(pos < input.size())
  {
    char c = input[pos];
    if(std::isdigit(static_cast<unsigned char>(c)))
    {
      value.push_back(c);
      ++pos;
    }
    else
      break;
  }
  return value;
}

static std::string read_identifier(const std::string& input, size_t& pos)
{
  std::string value;
  while(pos < input.size() && is_id_char(input[pos]))
  {
    value.push_back(input[pos]);
    ++pos;
  }
  return value;
}

std::vector<token> tokenize(const std::string& input)
{
  std::vector<token> tokens;
  size_t pos = 0;
  size_t line = 1;

  while(pos < input.size())
  {
    char c = input[pos];

    if(c == '\n')
    {
      tokens.push_back({token_type::newline, "\\n", line});
      ++pos;
      ++line;
      continue;
    }

    if(std::isspace(static_cast<unsigned char>(c)))
    {
      ++pos;
      continue;
    }

    if(c == '/' && pos + 1 < input.size() && input[pos + 1] == '/')
    {
      while(pos < input.size() && input[pos] != '\n')
        ++pos;
      continue;
    }

    if(c == '/' && pos + 1 < input.size() && input[pos + 1] == '*')
    {
      pos += 2;
      while(pos + 1 < input.size())
      {
        if(input[pos] == '*' && input[pos + 1] == '/')
        {
          pos += 2;
          break;
        }
        if(input[pos] == '\n')
          ++line;
        ++pos;
      }
      continue;
    }

    if(c == '#')
    {
      while(pos < input.size() && input[pos] != '\n')
        ++pos;
      continue;
    }

    if(c == '"')
    {
      ++pos;
      std::string value = read_string(input, pos, line);
      tokens.push_back({token_type::string_literal, value, line});
      continue;
    }

    if(c == '0' && pos + 1 < input.size() && (input[pos + 1] == 'x' || input[pos + 1] == 'X'))
    {
      pos += 2;
      std::string value = read_hex(input, pos);
      tokens.push_back({token_type::hex_literal, value, line});
      continue;
    }

    if(std::isdigit(static_cast<unsigned char>(c)))
    {
      std::string value = read_number(input, pos);
      tokens.push_back({token_type::integer_literal, value, line});
      continue;
    }

    if(c == ',')
    {
      tokens.push_back({token_type::comma, ",", line});
      ++pos;
      continue;
    }

    if(c == '|')
    {
      tokens.push_back({token_type::pipe, "|", line});
      ++pos;
      continue;
    }

    if(is_id_start(c))
    {
      std::string value = read_identifier(input, pos);

      std::string upper = value;
      std::transform(upper.begin(), upper.end(), upper.begin(),
                     [](unsigned char c) { return std::toupper(c); });

      if(upper == "BEGIN")
        tokens.push_back({token_type::begin, value, line});
      else if(upper == "END")
        tokens.push_back({token_type::end, value, line});
      else
        tokens.push_back({token_type::identifier, value, line});
      continue;
    }

    ++pos;
  }

  tokens.push_back({token_type::eof, "", line});
  return tokens;
}

}
