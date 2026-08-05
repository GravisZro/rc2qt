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

// PREVIOUS read_string -- replaced with rc.exe-compliant version per
// https://www.ryanliptak.com/blog/every-rc-exe-bug-quirk-probably/#that-s-not-my-a
// Kept (commented) for reference; details on what the new version
// implements are in the function body below.
#if 0
static std::string read_string_prev(const std::string& input, size_t& pos, size_t& line)
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
#endif

static std::string read_string(const std::string& input, size_t& pos, size_t& line)
{
  // Compliant with rc.exe string-literal escape handling per Ryan Liptak's
  // "That's not my \a" section (#that-s-not-my-a). Notable rules:
  //   * Supported escapes: \a, \n, \r, \t, \nnn (octal, up to 3 digits),
  //     \xhh (hex, up to 2 digits). \a is 0x08 (Backspace), NOT C's 0x07.
  //     \a and \t are case-insensitive; \n and \r are case-sensitive.
  //   * Unsupported escapes (e.g. \b, \e, \f, \v, \?, \k, \N, \T...) are
  //     preserved literally -- both the backslash and the character end up
  //     in the parsed string (\k -> \k).
  //
  // Note: rc.exe's quote-escaping (`""` to embed a quote) and the special
  // case for `\"` are documented in adjacent sections of the article and
  // are intentionally NOT changed here, to preserve existing tokenizer
  // behaviour at the raw-lex level.
  std::string value;
  while(pos < input.size())
  {
    char c = input[pos++];
    if(c == '"')
      break;
    if(c == '\\')
    {
      if(pos >= input.size())
      {
        value.push_back('\\');
        break;
      }
      char esc = input[pos++];
      switch(esc)
      {
        case 'a':
        case 'A':
          value.push_back('\x08');
          break;
        case 'n':
          value.push_back('\n');
          break;
        case 'r':
          value.push_back('\r');
          break;
        case 't':
        case 'T':
          value.push_back('\t');
          break;
        case '\\':
          value.push_back('\\');
          break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        {
          // rc.exe octal escape: up to three octal digits, one byte,
          // accumulated modulo 256 (clamped to a byte for .res storage).
          unsigned int v = static_cast<unsigned int>(esc - '0');
          for(int n = 0; n < 2 && pos < input.size() &&
                         input[pos] >= '0' && input[pos] <= '7'; ++n)
            v = (v << 3) | static_cast<unsigned int>(input[pos++] - '0');
          value.push_back(static_cast<char>(v & 0xFF));
          break;
        }
        case 'x':
        {
          // rc.exe hex escape: up to two hex digits, one byte.
          // If no hex digits follow, the escape is unsupported -> keep
          // the backslash and 'x' literally, matching the general rule.
          unsigned int v = 0;
          int n = 0;
          while(n < 2 && pos < input.size())
          {
            char d = input[pos];
            unsigned int dv;
            if(d >= '0' && d <= '9')      dv = static_cast<unsigned int>(d - '0');
            else if(d >= 'a' && d <= 'f') dv = 10u + static_cast<unsigned int>(d - 'a');
            else if(d >= 'A' && d <= 'F') dv = 10u + static_cast<unsigned int>(d - 'A');
            else break;
            v = (v << 4) | dv;
            ++pos;
            ++n;
          }
          if(n == 0)
          {
            value.push_back('\\');
            value.push_back('x');
          }
          else
            value.push_back(static_cast<char>(v & 0xFF));
          break;
        }
        case '"':
          // Backslash-quote is treated as a literal `"`; the enclosing
          // string is terminated by the next unescaped `"`. Preserved from
          // prior tokenizer behaviour; rc.exe's full special case (where
          // the `\` drops out and `""` becomes an escaped quote) is left
          // for a future change -- see the quote-escaping section.
          value.push_back('"');
          break;
        default:
          // Unrecognized escape: keep backslash and character literally,
          // e.g. `\b`, `\e`, `\f`, `\v`, `\?`, `\k`, `\N`, `\R`, `\X`...
          value.push_back('\\');
          value.push_back(esc);
          break;
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
      if(!tokens.empty() && tokens.back() != (token_type::comma | token_type::pipe))
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

    if(c == '0' && pos + 1 < src.size() && (src[pos + 1] == 'o' || src[pos + 1] == 'O'))
    {
      pos += 2;
      std::string value = "0o";
      while(pos < src.size() && src[pos] >= '0' && src[pos] <= '7')
      {
        value.push_back(src[pos]);
        ++pos;
      }
      tokens.push_back({token_type::integer_literal, value, line});
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
