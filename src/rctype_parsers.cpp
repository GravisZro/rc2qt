#include "rctype_parsers.h"

#include <algorithm>
#include <array>
#include <string>
#include <set>

#define Q2(x) #x
#define Q1(x) Q2(x)
#define RC_ERROR(message) \
    "file: " Q1(__FILE__) "\n" \
    "line: " Q1(__LINE__) "\n" \
    "RC parser error: " message


namespace parsers
{
// Convert 16 bit code point to UTF-8 string.
// see Table 3-6 : http://www.unicode.org/versions/Unicode6.2.0/ch03.pdf#page=42
static void append_utf16(std::string& dest, uint16_t data) noexcept
{
  if (data <= 0x007F) // one byte value
    dest.push_back(char(data));
  else if (data <= 0x07FF) // two byte value
  {
    dest.push_back(0xC0 + ((data & 0x07C0) >> 6));
    dest.push_back(0x80 +  (data & 0x003F));
  }
  else // three byte value
  {
    dest.push_back(0xE0 + ((data & 0xF000) >> 12));
    dest.push_back(0x80 + ((data & 0x0FC0) >>  6));
    dest.push_back(0x80 +  (data & 0x003F));
  }
}

std::string parse_string(byte_istream& input)
{
  std::string value;
  uint8_t current = 0;
  while(current = input.get(),
        current && current != '"' &&
        input.good())
  {
    if(current == '\\') // escape sequence expected
    {
      current = input.get();
      if(input.good())
      {
        switch (current)
        {
          case 'u': // unicode escape symbol \u???? - value range: 0 to 65535
          {
            std::array<uint8_t, 4> sequence;
            input.read(sequence.data(), sizeof(sequence));
            if(input.good() || !std::all_of(std::begin(sequence), std::end(sequence), ::isxdigit)) // IF exceeds End Of String OR NOT all digits are hexadecimal
              throw RC_ERROR("End Of String found while decoding UTF-16 OR hexadecimal escape sequence");
            append_utf16(value, std::stoi(std::string(std::begin(sequence), std::end(sequence)), nullptr, 16));
            break;
          }

        case '"':
          value.push_back('\\');
          value.push_back(current);
          break; // quote

        // normally escaped symbols
        case '/': value.push_back('/'); break; // slash (escaping mandatory?)
        case '\\':value.push_back('\\'); break; // backslash
        case 'b': value.push_back('\b'); break; // backspace
        case 'f': value.push_back('\f'); break; // feed
        case 'r': value.push_back('\r'); break; // return (to line start)
        case 'n': value.push_back('\n'); break; // newline
        case 't': value.push_back('\t'); break; // tab
        case 'v': value.push_back('\v'); break; // vertical tab
        case 'a': value.push_back('\a'); break; // audible bell

        default: // some other escape character or unexpected symbol
          value.push_back('\\');
          value.push_back(current);
          break;
        }
      }
      else
        value.push_back(current);
    }
  }
  return value;
}

std::string read_field(byte_istream& input)
{
  std::string value;
  uint8_t current = 0;
  std::set<uint8_t> field_ends = { ' ', ',', '\n' };
  do
  {
    current = input.get();
    if(current == '"')
      value += parse_string(input);
    else
      value.push_back(current);
  } while(!field_ends.contains(current) && input.good());
  return value;
}

constexpr byte_istream& operator>>(byte_istream& input, bool& fs)
{
  return input;
}

/*
    "BOOLEAN" :     { "choice" : { "literal" : "TRUE", "literal" : "FALSE" } },
    "text" :        { "type" : "string",    },
    "x" :           { "type" : "int16_t",   },
    "y" :           { "type" : "int16_t",   },
    "width" :       { "type" : "uint16_t",  },
    "height" :      { "type" : "uint16_t",  },
    "filename" :    { "type" : "string",    },
    "idvalue" :     { "type" : "uint16_t"   },
    "pointsize" :   { "type" : "uint16_t"   },
    "typeface" :    { "type" : "string"     },
    "weight" :      { "type" : "uint16_t"   },
    "italic" :      { "type" : "BOOLEAN"    },
    "charset" :     { "type" : "textual_id" },
    "WORD" :        { "type" : "uint16_t"   },
    "DWORD" :
    {
      "type" : "uint32_t",
      "option" : { "literal" : "L" }
    },
*/
} // namespace parsers
