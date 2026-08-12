#include "utils.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <array>
#include <ranges>
#include <vector>

#ifdef HAVE_ICU
#include <unicode/utypes.h>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>
#endif

constexpr std::string to_lower(const std::string& s)
{
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return c & 0x80 ? c : std::tolower(c); });
  return result;
}


namespace
{

bool is_expression_text(const std::string& s)
{
  bool found = false;
  for(size_t i = 0; i < s.size(); ++i)
  {
    if(s[i] == '(' || (i > 0 && strchr("+-&|", s[i]) != nullptr))
      found = true;
  }
  return found;
}

int64_t eval_expr(const std::string& text, size_t& pos);

// Parses an integer literal at pos: 0x/0X hexadecimal, 0o/0O octal, or
// decimal. Returns 0 without advancing pos when no literal is present.
// std::stoll is always given an explicit base: a base of 0 would auto-detect
// octal for any leading zero, which conflicts with RC decimal literals.
int64_t parse_number(const std::string& s, size_t& pos)
{
  size_t start = pos;
  if(pos + 1 < s.size() && s[pos] == '0' &&
     (s[pos + 1] == 'x' || s[pos + 1] == 'X'))
  {
    pos += 2;
    size_t digits = pos;
    while(pos < s.size() && std::isxdigit(static_cast<unsigned char>(s[pos])))
      ++pos;
    if(pos > digits)
      return std::stoll(s.substr(start, pos - start), nullptr, 16);
    pos = start;
  }
  if(pos + 1 < s.size() && s[pos] == '0' &&
     (s[pos + 1] == 'o' || s[pos + 1] == 'O'))
  {
    pos += 2;
    size_t digits = pos;
    while(pos < s.size() && s[pos] >= '0' && s[pos] <= '7')
      ++pos;
    if(pos > digits)
      return std::stoll(s.substr(digits, pos - digits), nullptr, 8);
    pos = start;
  }
  while(pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos])))
    ++pos;
  if(pos == start)
    return 0;
  return std::stoll(s.substr(start, pos - start), nullptr, 10);
}

// Reads a single operand: an optional unary sign followed by an integer
// literal (decimal, 0x hex, or 0o octal) or a parenthesized subexpression.
int64_t eval_term(const std::string& text, size_t& pos)
{
  int sign = 1;
  while(pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
    ++pos;
  if(pos < text.size() && (text[pos] == '+' || text[pos] == '-'))
  {
    if(text[pos] == '-')
      sign = -1;
    ++pos;
    while(pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
      ++pos;
  }
  if(pos < text.size() && text[pos] == '(')
  {
    ++pos;
    int64_t value = eval_expr(text, pos);
    while(pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
      ++pos;
    if(pos < text.size() && text[pos] == ')')
      ++pos;
    return sign * value;
  }
  size_t start = pos;
  int64_t value = parse_number(text, pos);
  if(pos == start)
    return 0;
  return sign * value;
}

// Evaluates binary operators left-to-right with equal precedence, matching
// the RC resource compiler. A closing parenthesis ends the expression.
int64_t eval_expr(const std::string& text, size_t& pos)
{
  int64_t value = eval_term(text, pos);
  for(;;)
  {
    while(pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
      ++pos;
    if(pos >= text.size())
      break;
    char op = text[pos];
    if(strchr("+-&|", op) == nullptr)
      break;
    ++pos;
    int64_t rhs = eval_term(text, pos);
    switch(op)
    {
      case '+':
        value += rhs;
        break;
      case '-':
        value -= rhs;
        break;
      case '&':
        value &= rhs;
        break;
      case '|':
        value |= rhs;
        break;
    }
  }
  return value;
}

}

int64_t safe_stoi(const std::string& s, int64_t default_value)
{
  if(is_expression_text(s))
    return evaluate_expression(s, default_value);
  try
  {
    size_t pos = 0;
    int sign = 1;
    if(!s.empty() && (s[0] == '+' || s[0] == '-'))
    {
      if(s[0] == '-')
        sign = -1;
      pos = 1;
    }
    size_t start = pos;
    int64_t value = parse_number(s, pos);
    if(pos == start)
      return default_value;
    return sign * value;
  }
  catch(const std::exception&)
  {
    return default_value;
  }
}

int64_t evaluate_expression(const std::string& text, int64_t default_value)
{
  size_t pos = 0;
  try
  {
    int64_t result = eval_expr(text, pos);
    if(pos == 0)
      return default_value;
    return result;
  }
  catch(const std::exception&)
  {
    return default_value;
  }
}

uint64_t safe_stoul(const std::string& s, int base, uint64_t default_value)
{
  try
  {
    return std::stoull(s, nullptr, base);
  }
  catch(const std::exception&)
  {
    return default_value;
  }
}

static std::string g_codepage;

void set_codepage(std::string& codepage)
{
  uint64_t val = safe_stoul(codepage, 0, UINT64_MAX);
  if(val == UINT64_MAX)
    g_codepage = to_lower(codepage);
  else if(val > UINT32_MAX)
    throw std::runtime_error(std::format("set_codepage::codepage_num cannot exceed {}. Got \"{}\"", UINT32_MAX, val));
  else
    g_codepage = std::format("cp{}", codepage);
}

#ifdef HAVE_ICU
std::string codepage_to_utf8(const char* codepage, const std::string& input)
{
  std::vector<char> dest(input.size() * 4 + 1, '\0'); // for worst case scenario

  UErrorCode status = U_ZERO_ERROR;
  // Perform the conversion directly from source encoding to UTF-8
  // ucnv_convert acts as a convenience function that pivots through Unicode internally.
  ucnv_convert(
      "utf-8",        // Target encoding name
      codepage,       // Source encoding name (e.g., "cp932", "cp1252")
      dest.data(),    // Output buffer
      dest.size(),    // Output buffer capacity in bytes
      input.c_str(),  // Input string
      -1,             // Input string length (-1 if null-terminated)
      &status         // ICU error code
      );

  if(U_FAILURE(status))
    throw std::runtime_error(__FILE__ ": Something went wrong with the codepage conversion!");

  return std::string(dest.data());
}

std::string codepage_to_utf8(const std::string& input)
{
  if(g_codepage.empty())
    throw std::runtime_error("You must call set_codepage() before calling codepage_to_utf8()");
  return codepage_to_utf8(g_codepage.c_str(), input);
}

struct icu_detector_t
{
  icu_detector_t(void)
  {
    detector = ucsdet_open(&status);
    if (U_FAILURE(status))
      throw std::runtime_error("ICU ucsdet_open() returned and error");
  }
  ~icu_detector_t(void)
  {
    if(detector != nullptr)
    {
      ucsdet_close(detector);
      detector = nullptr;
    }
  }

  std::string detect(const std::string& input)
  {
    ucsdet_setText(detector, input.data(), static_cast<int32_t>(input.size()), &status);
    if (U_FAILURE(status))
      throw std::runtime_error("ICU ucsdet_setText() returned an error");

    const UCharsetMatch* match = ucsdet_detect(detector, &status);
    if (U_FAILURE(status) || !match)
      throw std::runtime_error("ICU ucsdet_detect() returned an error");

    std::string name = to_lower(ucsdet_getName(match, &status));
    if (U_FAILURE(status))
      throw std::runtime_error("ICU ucsdet_getName() returned an error");

    int32_t confidence = ucsdet_getConfidence(match, &status);
    if (U_FAILURE(status))
      throw std::runtime_error("ICU ucsdet_getConfidence() returned an error");

    if(confidence <= 50 &&
        (name == "us-ascii" ||
         name == "macroman" ||
         name == "ibm850" ||
         name == "cp850" ||
         name == "iso-8859-15" ||
         name == "iso-8859-1"))
      name = "cp1252"; // this is a more likely match

    return name;
  }

  void reset(void)
  {
    status = U_ZERO_ERROR;
  }

  UErrorCode status = U_ZERO_ERROR;
  UCharsetDetector* detector = nullptr;
};

std::string detect_codepage(const std::string& input)
{
  static icu_detector_t d;
  d.reset();
  return d.detect(input);
}

#else

std::string utf16le_to_utf8(const std::string& input)
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

std::string cp1252_to_utf8(const std::string& input)
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

std::string codepage_to_utf8(const std::string& input)
{
  if(g_codepage.empty())
    throw std::runtime_error("You must call set_codepage() before calling codepage_to_utf8()");

  if(g_codepage == "utf-8" ||
     g_codepage == "cp65001")
    return input;

  if(g_codepage == "cp1200" ||
     g_codepage == "utf-16le")
    return utf16le_to_utf8(input);

  if(g_codepage != "cp1252")
    return cp1252_to_utf8(input);

  throw std::runtime_error("ICU support is disabled. The only code pages supported are 1200 (UTF-16LE) and 1252 (Windows-1252)");
}

std::string detect_codepage(const std::string& input)
{
  const auto* data = reinterpret_cast<const unsigned char*>(input.data());
  if(input.size() >= 4)
  {
    if(data[0] == 0xFF && data[1] == 0xFE && data[2] == 0x00 && data[3] == 0x00)
      return "12000"; // UTF-32LE
    else if(data[0] == 0x00 && data[1] == 0x00 && data[2] == 0xFE && data[3] == 0xFF)
      return "12001"; // UTF-32BE
  }

  if(input.size() >= 2)
  {
    else if(data[0] == 0xFE && data[1] == 0xFF)
      return "1201"; // UTF16-LE
    else if(data[0] == 0xFF && data[1] == 0xFE)
      return "1200"; // UTF16-LE
    else
    {
      size_t nulls = 0;
      size_t check = std::min(input.size(), static_cast<size_t>(64));
      for(size_t i = 1; i < check; i += 2)
        if(data[i] == 0)
          ++nulls;
      if(nulls > check / 4)
        return "1201"; // UTF16-LE
    }
  }
  return "1252"; // default is 1252
}
#endif

bool match_string(const std::string& needle, std::initializer_list<std::string> haystack)
{
  auto upper = to_upper(needle);
  return std::ranges::any_of(haystack, [upper](auto str) { return to_upper(str) == upper; });
}

std::string escape_string(const std::string& s)
{
  std::string result;
  result.reserve(s.size()); // Pre-allocate to reduce reallocations

  for (char c : s)
  {
    switch (c)
    {
      case '"':  result += "\\\""; break;
      case '\\': result += "\\\\"; break;
      case '\t': result += "\\t";  break;
      case '\n': result += "\\n";  break;
      default:   result += c;      break;
    }
  }
  return result;
}
