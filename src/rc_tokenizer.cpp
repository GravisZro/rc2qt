#include "rc_tokenizer.h"
#include "utils.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <array>
#include <stdexcept>

namespace rc
{

static bool is_id_start(char c)
{
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '#' || c == '~';
}

static bool is_id_char(char c)
{
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.' || c == '~';
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
  std::string converted;
  if(input.size() >= 2)
  {
    const auto* data = reinterpret_cast<const unsigned char*>(input.data());

    if(input.size() >= 4)
    {
      bool is_utf32le = (data[0] == 0xFF && data[1] == 0xFE &&
                         data[2] == 0x00 && data[3] == 0x00);
      bool is_utf32be = (data[0] == 0x00 && data[1] == 0x00 &&
                         data[2] == 0xFE && data[3] == 0xFF);
      if(is_utf32le || is_utf32be)
        throw std::runtime_error("UTF-32 encoded .rc files are not supported");
    }

    bool is_utf16be = (data[0] == 0xFE && data[1] == 0xFF);
    if(is_utf16be)
      throw std::runtime_error("UTF-16 BE encoded .rc files are not supported");

    bool is_utf16le = (data[0] == 0xFF && data[1] == 0xFE);
    if(!is_utf16le)
    {
      size_t nulls = 0;
      size_t check = std::min(input.size(), static_cast<size_t>(64));
      for(size_t i = 1; i < check; i += 2)
        if(data[i] == 0)
          ++nulls;
      if(nulls > check / 4)
        is_utf16le = true;
    }
    if(is_utf16le)
      converted = utf16le_to_utf8(input);
    else
      converted = cp1252_to_utf8(input);
  }

  const std::string& src = converted;
  std::vector<token> tokens;
  size_t pos = 0;
  size_t line = 1;

  while(pos < src.size())
  {
    char c = src[pos];

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

    if(c == '/' && pos + 1 < src.size() && src[pos + 1] == '/')
    {
      while(pos < src.size() && src[pos] != '\n')
        ++pos;
      continue;
    }

    if(c == '/' && pos + 1 < src.size() && src[pos + 1] == '*')
    {
      pos += 2;
      while(pos + 1 < src.size())
      {
        if(src[pos] == '*' && src[pos + 1] == '/')
        {
          pos += 2;
          break;
        }
        if(src[pos] == '\n')
          ++line;
        ++pos;
      }
      continue;
    }

    if(c == '#')
    {
      while(pos < src.size() && src[pos] != '\n')
        ++pos;
      continue;
    }

    if(c == ';')
    {
      while(pos < src.size() && src[pos] != '\n')
        ++pos;
      continue;
    }

    if(c == '"')
    {
      ++pos;
      std::string value = read_string(src, pos, line);
      tokens.push_back({token_type::string_literal, value, line});
      continue;
    }

    if(c == '0' && pos + 1 < src.size() && (src[pos + 1] == 'x' || src[pos + 1] == 'X'))
    {
      pos += 2;
      std::string value = read_hex(src, pos);
      tokens.push_back({token_type::hex_literal, value, line});
      continue;
    }

    if(std::isdigit(static_cast<unsigned char>(c)))
    {
      std::string value = read_number(src, pos);
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

    if(c == '{')
    {
      tokens.push_back({token_type::begin, "{", line});
      ++pos;
      continue;
    }

    if(c == '}')
    {
      tokens.push_back({token_type::end, "}", line});
      ++pos;
      continue;
    }

    if(is_id_start(c))
    {
      std::string value = read_identifier(src, pos);
      std::string upper = to_upper(value);

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
