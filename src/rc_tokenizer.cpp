#include "rc_tokenizer.h"
#include "rc_helpers.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <array>
#include <stdexcept>

namespace rc
{
  static std::string utf16le_to_utf8(const std::string& input)
  {
    std::string result;
    result.reserve(input.size() / 2);
    const auto* data = reinterpret_cast<const unsigned char*>(input.data());
    size_t len = input.size();
    size_t i = 0;

    if(len >= 2 && data[0] == 0xFF && data[1] == 0xFE)
      i = 2;

    while(i + 1 < len)
    {
      uint16_t ch = data[i] | (static_cast<uint16_t>(data[i + 1]) << 8);
      i += 2;
      if(ch == 0)
        continue;
      if(ch < 0x80)
        result.push_back(static_cast<char>(ch));
      else if(ch < 0x800)
      {
        result.push_back(static_cast<char>(0xC0 | (ch >> 6)));
        result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
      }
      else
      {
        result.push_back(static_cast<char>(0xE0 | (ch >> 12)));
        result.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
      }
    }
    return result;
  }

  static std::string cp1252_to_utf8(const std::string& input)
  {
    static const std::array<uint16_t, 256> cp1252_to_utf16 =
    {
      0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D,
      0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B,
      0x001C, 0x001D, 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029,
      0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
      0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
      0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050, 0x0051, 0x0052, 0x0053,
      0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061,
      0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
      0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D,
      0x007E, 0x007F, 0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039,
      0x0152, 0x008D, 0x017D, 0x008F, 0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122,
      0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178, 0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
      0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5,
      0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3,
      0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1,
      0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
      0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED,
      0x00EE, 0x00EF, 0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB,
      0x00FC, 0x00FD, 0x00FE, 0x00FF,
    };

    std::string result;
    result.reserve(input.size() * 2);
    for(char letter : input)
    {
      uint16_t n = cp1252_to_utf16[uint8_t(letter)];
      result.push_back(n & 0xff);
      result.push_back(n >> 8);
    }
    return utf16le_to_utf8(result);
  }



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
      std::string upper = rc::to_upper(value);

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
