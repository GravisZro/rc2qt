#include "pe_resource_decoder.h"
#include "imageio.h"
#include "pefile.hpp"
#include "pe_constants.hpp"
#include "pe_containers.hpp"
#include "rc_constants.h"
#include "utils.h"

#include <sstream>
#include <string_view>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <array>
#include <cstring>
#include <fstream>
#include <format>
#include <utility>
#include <unordered_map>

namespace
{
  class byte_reader
  {
  public:
    byte_reader(std::span<const uint8_t> data)
      : m_data(data), m_pos(0) {}

    bool eof() const { return m_pos >= m_data.size(); }
    size_t& pos() { return m_pos; }
    const size_t& pos() const { return m_pos; }
    size_t remaining() const { return m_data.size() - m_pos; }

    uint8_t peek_u8(size_t offset = 0) const
    {
      size_t idx = m_pos + offset;
      if (idx >= m_data.size())
        return 0;
      return m_data[idx];
    }

    uint8_t read_u8()
    {
      if (m_pos >= m_data.size())
        throw std::runtime_error("byte_reader: out of bounds reading uint8_t");
      return m_data[m_pos++];
    }

    uint16_t read_u16()
    {
      if (m_pos + 2 > m_data.size())
        throw std::runtime_error("byte_reader: out of bounds reading uint16_t");
      uint16_t val = m_data[m_pos] | (static_cast<uint16_t>(m_data[m_pos + 1]) << 8);
      m_pos += 2;
      return val;
    }

    uint32_t read_u32()
    {
      if (m_pos + 4 > m_data.size())
        throw std::runtime_error("byte_reader: out of bounds reading uint32_t");
      uint32_t val = m_data[m_pos]
        | (static_cast<uint32_t>(m_data[m_pos + 1]) << 8)
        | (static_cast<uint32_t>(m_data[m_pos + 2]) << 16)
        | (static_cast<uint32_t>(m_data[m_pos + 3]) << 24);
      m_pos += 4;
      return val;
    }

    std::string read_u16_string()
    {
      std::u16string result;
      while (m_pos + 1 < m_data.size())
      {
        uint16_t ch = read_u16();
        if (ch == 0)
          break;
        result += static_cast<char16_t>(ch);
      }
      return utf16_to_utf8(result);
    }

    void skip(size_t n) { m_pos = std::min(m_pos + n, m_data.size()); }

    std::span<const uint8_t> slice(size_t offset, size_t len) const
    {
      if (offset + len > m_data.size())
        return {};
      return m_data.subspan(offset, len);
    }

  private:
    static std::string utf16_to_utf8(const std::u16string& utf16)
    {
      std::string result;
      for (char16_t ch : utf16)
      {
        if (ch < 0x80)
        {
          result += static_cast<char>(ch);
        }
        else if (ch < 0x800)
        {
          result += static_cast<char>(0xC0 | (ch >> 6));
          result += static_cast<char>(0x80 | (ch & 0x3F));
        }
        else
        {
          result += static_cast<char>(0xE0 | (ch >> 12));
          result += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
          result += static_cast<char>(0x80 | (ch & 0x3F));
        }
      }
      return result;
    }

    std::span<const uint8_t> m_data;
    size_t m_pos;
  };

  struct dialog_control
  {
    uint32_t help_id = 0;
    uint32_t ex_style = 0;
    uint32_t style = 0;
    int16_t x = 0;
    int16_t y = 0;
    int16_t cx = 0;
    int16_t cy = 0;
    uint32_t id = 0;
    std::string class_name;
    std::string text;
    bool is_extended = false;
  };

  struct dialog_header
  {
    bool is_extended = false;
    uint32_t help_id = 0;
    uint32_t ex_style = 0;
    uint32_t style = 0;
    uint16_t count = 0;
    int16_t x = 0;
    int16_t y = 0;
    int16_t cx = 0;
    int16_t cy = 0;
    std::string menu;
    std::string class_name;
    std::string title;
    uint16_t font_size = 0;
    uint16_t font_weight = 0;
    bool font_italic = false;
    uint8_t font_charset = 0;
    std::string font_name;
  };

  struct [[gnu::packed]] vs_version_t
  {
    uint16_t high;
    uint16_t low;
  };

  struct [[gnu::packed]] vs_fixedfileinfo_t
  {
    uint32_t dwSignature;
    vs_version_t dwStrucVersion;
    vs_version_t dwFileVersionMS;
    vs_version_t dwFileVersionLS;
    vs_version_t dwProductVersionMS;
    vs_version_t dwProductVersionLS;
    uint32_t dwFileFlagsMask;
    uint32_t dwFileFlags;
    uint32_t dwFileOS;
    uint32_t dwFileType;
    uint32_t dwFileSubtype;
    uint32_t dwFileDateMS;
    uint32_t dwFileDateLS;
  };

  struct [[gnu::packed]] vs_versioninfo_t
  {
    uint16_t            wLength;
    uint16_t            wValueLength;
    uint16_t            wType;
    char16_t            szKey[16];
    uint16_t            Padding1;
    vs_fixedfileinfo_t  Value;
    // uint16_t            Padding2;
    // uint16_t            Children;
    // The child blocks (StringFileInfo/VarFileInfo) begin immediately after
    // Value, which ends on a DWORD boundary (header = 92 bytes).
  };

  static_assert(sizeof(vs_fixedfileinfo_t) == 52,
    "VS_FIXEDFILEINFO must be 52 bytes");
  static_assert(sizeof(vs_versioninfo_t) == 92,
    "VS_VERSION_INFO header must be 92 bytes");

  static const char* class_from_ordinal(uint16_t ordinal)
  {
    switch (ordinal)
    {
      case 0x0080: return "BUTTON";
      case 0x0081: return "EDIT";
      case 0x0082: return "STATIC";
      case 0x0083: return "LISTBOX";
      case 0x0084: return "SCROLLBAR";
      case 0x0085: return "COMBOBOX";
      default: return "UNKNOWN";
    }
  }

  static std::string read_ord_or_string(byte_reader& reader)
  {
    uint16_t first = reader.read_u16();
    if (first == 0xFFFF)
    {
      uint16_t ordinal = reader.read_u16();
      return std::format("#{}", ordinal);
    }
    else if (first == 0x0000)
    {
      return "";
    }
    else
    {
      std::u16string str;
      str += static_cast<char16_t>(first);
      while (true)
      {
        uint16_t ch = reader.read_u16();
        if (ch == 0)
          break;
        str += static_cast<char16_t>(ch);
      }
      std::string result;
      for (char16_t ch : str)
        result += static_cast<char>(ch < 128 ? ch : '?');
      return result;
    }
  }

  static std::string read_unicode_string(byte_reader& reader)
  {
    std::string result;
    while (true)
    {
      uint16_t ch = reader.read_u16();
      if (ch == 0)
        break;
      result += static_cast<char>(ch < 128 ? ch : '?');
    }
    return result;
  }

  static std::string format_style(uint32_t style, rc::category_t style_type)
  {
    std::string result;
    const auto& reg = rc::constant_registry::instance();

    for (uint32_t bit = 1; bit; bit <<= 1)
    {
      if (style & bit)
      {
        std::string name = reg.resolve(style_type, bit);
        if (!name.empty())
        {
          if (!result.empty())
            result += " | ";
          result += name;
        }
      }
    }

    return result;
  }



  static std::string resource_id_string(uint32_t id)
  {
    return std::format("ID_{}", id);
  }

  static dialog_header parse_dialog_header(byte_reader& reader, bool is_extended)
  {
    dialog_header hdr;
    hdr.is_extended = is_extended;

    if (is_extended)
    {
      hdr.help_id = reader.read_u32();
    }

    hdr.ex_style = reader.read_u32();
    hdr.style = reader.read_u32();
    hdr.count = reader.read_u16();
    hdr.x = static_cast<int16_t>(reader.read_u16());
    hdr.y = static_cast<int16_t>(reader.read_u16());
    hdr.cx = static_cast<int16_t>(reader.read_u16());
    hdr.cy = static_cast<int16_t>(reader.read_u16());

    hdr.menu = read_ord_or_string(reader);
    hdr.class_name = read_ord_or_string(reader);
    hdr.title = read_unicode_string(reader);

    if (hdr.style & 0x40)
    {
      hdr.font_size = reader.read_u16();
      hdr.font_weight = reader.read_u16();
      hdr.font_italic = reader.read_u8();
      hdr.font_charset = reader.read_u8();
      hdr.font_name = read_unicode_string(reader);
    }

    return hdr;
  }

  static dialog_control parse_dialog_control(byte_reader& reader, bool is_extended)
  {
    dialog_control ctrl;
    ctrl.is_extended = is_extended;

    if (is_extended)
    {
      ctrl.help_id = reader.read_u32();
    }

    ctrl.ex_style = reader.read_u32();
    ctrl.style = reader.read_u32();
    ctrl.x = static_cast<int16_t>(reader.read_u16());
    ctrl.y = static_cast<int16_t>(reader.read_u16());
    ctrl.cx = static_cast<int16_t>(reader.read_u16());
    ctrl.cy = static_cast<int16_t>(reader.read_u16());

    if (is_extended)
    {
      ctrl.id = reader.read_u32();
    }
    else
    {
      ctrl.id = reader.read_u16();
    }

    std::string class_str = read_ord_or_string(reader);
    if (class_str == "#32770")
      ctrl.class_name = "Dialog";
    else if (class_str == "#32768")
      ctrl.class_name = "SysListView32";
    else if (class_str == "#32769")
      ctrl.class_name = "SysTreeView32";
    else if (class_str == "#32771")
      ctrl.class_name = "SysTabControl32";
    else if (class_str == "#32772")
      ctrl.class_name = "SysProgressBar";
    else if (class_str.size() > 1 && class_str[0] == '#')
    {
      char* end = nullptr;
      long ordinal = std::strtol(class_str.c_str() + 1, &end, 10);
      if (end && *end == '\0' && ordinal > 0)
      {
        const char* ordinal_name = class_from_ordinal(
          static_cast<uint16_t>(ordinal));
        if (strcmp(ordinal_name, "UNKNOWN") != 0)
          ctrl.class_name = ordinal_name;
        else
          ctrl.class_name = class_str;
      }
      else
        ctrl.class_name = class_str;
    }
    else
      ctrl.class_name = class_str;

    ctrl.text = read_ord_or_string(reader);

    uint16_t data_len = reader.read_u16();
    if (data_len > 0)
      reader.skip(data_len);

    return ctrl;
  }

  static std::string classify_control(const dialog_control& ctrl)
  {
    std::string cls = to_upper(ctrl.class_name);

    if (cls == "BUTTON" || cls == "BS_PUSHBUTTON" || cls == "BS_DEFPUSHBUTTON")
    {
      uint32_t type = ctrl.style & 0x0F;
      if (type == 0) return "PUSHBUTTON";
      if (type == 1) return "DEFPUSHBUTTON";
      if (type == 2 || type == 3 || type == 5 || type == 6)
        return (type == 3 || type == 6) ? "AUTOCHECKBOX" : "CHECKBOX";
      if (type == 4 || type == 7 || type == 8)
        return (type == 7 || type == 8) ? "AUTORADIOBUTTON" : "RADIOBUTTON";
      if (type == 9) return "GROUPBOX";
      if (type == 0xB) return "PUSHBOX";
      return "PUSHBUTTON";
    }

    if (cls == "EDIT")
      return "EDITTEXT";

    if (cls == "STATIC")
    {
      uint32_t type = ctrl.style & 0x1F;
      if (type == 4 || type == 5 || type == 6 || type == 7 || type == 8
          || type == 9 || type == 0xA || type == 0xD || type == 0xE)
        return "ICON";
      if (type == 2) return "CTEXT";
      if (type == 3) return "RTEXT";
      return "LTEXT";
    }

    if (cls == "LISTBOX")
      return "LISTBOX";

    if (cls == "SCROLLBAR")
      return "SCROLLBAR";

    if (cls == "COMBOBOX")
      return "COMBOBOX";

    return "CONTROL";
  }

  static std::string decode_control(const dialog_control& ctrl)
  {
    std::string keyword = classify_control(ctrl);
    std::string id_str = resource_id_string(ctrl.id);

    if (keyword == "CONTROL")
    {
      std::string style_str;
      uint32_t visible_style = ctrl.style & 0x10000000;
      uint32_t rest_style = ctrl.style & ~static_cast<uint32_t>(0x10000000);
      std::string style_val;
      if (rest_style)
        style_val = std::format("0x{:08x}", rest_style);

      std::string line = std::format("    CONTROL \"{}\", {}, \"{}\"",
        escape_string(ctrl.text), id_str, ctrl.class_name);

      if (style_val.empty())
        line += ", 0";
      else
        line += ", " + style_val;

      line += std::format(", {}, {}, {}, {}", ctrl.x, ctrl.y, ctrl.cx, ctrl.cy);

      if (ctrl.ex_style)
        line += ", " + format_style(ctrl.ex_style, rc::category_t::extended_style);

      return line;
    }

    std::string line = std::format("    {} \"{}\", {}, {}, {}, {}, {}",
      keyword, escape_string(ctrl.text), id_str, ctrl.x, ctrl.y, ctrl.cx, ctrl.cy);

    uint32_t control_type = 0;
    std::string cls_upper = to_upper(ctrl.class_name);
    if (cls_upper == "BUTTON")
      control_type = ctrl.style & 0x0F;
    else if (cls_upper == "STATIC")
      control_type = ctrl.style & 0x1F;
    else if (cls_upper == "EDIT" || cls_upper == "SCROLLBAR")
      control_type = ctrl.style & 0x0F;
    else if (cls_upper == "LISTBOX" || cls_upper == "COMBOBOX")
      control_type = ctrl.style & 0x0F;

    if (keyword == "EDITTEXT" || keyword == "LISTBOX" || keyword == "COMBOBOX" || keyword == "SCROLLBAR")
    {
      uint32_t style_flags = ctrl.style & 0xFFFF0000;
      if (keyword == "SCROLLBAR")
        style_flags = ctrl.style;
      if (style_flags)
      {
        std::string sf;
        if (keyword == "EDITTEXT")
        {
          if (ctrl.style & 0x0080) sf += "ES_AUTOHSCROLL";
        }
        else if (keyword == "LISTBOX")
        {
          if (ctrl.style & 0x0008) sf += (sf.empty() ? "" : " | ") + std::string("LBS_NOTIFY");
          if (ctrl.style & 0x0002) sf += (sf.empty() ? "" : " | ") + std::string("LBS_SORT");
        }
        else if (keyword == "COMBOBOX")
        {
          uint32_t cbs_type = ctrl.style & 0x0F;
          if (cbs_type == 2) sf += "CBS_DROPDOWN";
          else if (cbs_type == 3) sf += "CBS_DROPDOWNLIST";
          else sf += "CBS_SIMPLE";
          if (ctrl.style & 0x0010) sf += (sf.empty() ? "" : " | ") + std::string("CBS_AUTOHSCROLL");
        }
        if (!sf.empty())
          line += ", " + sf;
      }
      return line;
    }

    return line;
  }
}

namespace pe_decoder
{

std::string decode_dialog(
  std::span<const uint8_t> data,
  const std::string& resource_id)
{
  byte_reader reader(data);

  uint16_t first_word = reader.read_u16();
  bool is_extended = (first_word == 1);
  reader.skip(2);

  dialog_header hdr = parse_dialog_header(reader, is_extended);

  std::ostringstream out;
  out << resource_id << " ";
  if (is_extended)
    out << "DIALOGEX ";
  else
    out << "DIALOG ";

  out << hdr.x << ", " << hdr.y << ", " << hdr.cx << ", " << hdr.cy;
  if (is_extended && hdr.help_id != 0)
    out << ", " << hdr.help_id;
  out << "\n";

  if (hdr.style)
  {
    out << "STYLE " << format_style(hdr.style,
      rc::category_t::dialog_style | rc::category_t::window_style);
    out << "\n";
  }

  if (hdr.ex_style)
  {
    out << "EXSTYLE " << format_style(hdr.ex_style, rc::category_t::extended_style);
    out << "\n";
  }

  if (!hdr.title.empty())
    out << "CAPTION \"" << escape_string(hdr.title) << "\"\n";

  if (hdr.font_size > 0)
  {
    out << "FONT " << hdr.font_size << ", \"" << escape_string(hdr.font_name) << "\"";
    if (hdr.font_weight || hdr.font_italic)
    {
      out << ", " << hdr.font_weight;
      if (hdr.font_italic)
        out << ", " << (hdr.font_italic ? "TRUE" : "FALSE");
    }
    out << "\n";
  }

  out << "BEGIN\n";

  for (uint16_t i = 0; i < hdr.count && !reader.eof(); i++)
  {
    size_t align = is_extended ? 4 : 2;
    size_t cur = reader.pos();
    size_t pad = (align - (cur % align)) % align;
    reader.skip(pad);

    dialog_control ctrl = parse_dialog_control(reader, is_extended);
    out << decode_control(ctrl) << "\n";
  }

  out << "END\n";
  return out.str();
}

std::string decode_menu(
  std::span<const uint8_t> data,
  const std::string& resource_id)
{
  byte_reader reader(data);

  uint16_t version = reader.read_u16();
  uint16_t offset = reader.read_u16();
  (void)offset;

  uint32_t help_id = 0;
  if (version == 1)
    help_id = reader.read_u32();

  std::ostringstream out;
  if (version == 1)
    out << resource_id << " MENUEX";
  else
    out << resource_id << " MENU";

  out << "\nBEGIN\n";

  struct menu_stack_entry
  {
    int depth = 0;
  };
  std::vector<menu_stack_entry> stack;
  int current_depth = 0;

  if (version == 1)
  {
    // Per MS docs, MENUEX_TEMPLATE_HEADER is:
    //   wVersion(WORD=1) + wOffset(WORD=4) + dwHelpId(DWORD) = 8 bytes.
    // wOffset is relative to end of wOffset field (byte 4), so items start at 4+4=8.
    // The reader is already positioned at byte 8 after reading the header above.
    //
    // MENUEX_TEMPLATE_ITEM (ALL items, same format):
    //   dwType(DWORD) + dwState(DWORD) + uId(UINT=DWORD) + wFlags(WORD) + szText(WCHAR[])
    //   = 14 bytes fixed + variable null-terminated text.
    // For popup items: dwHelpId appears at the first DWORD boundary after szText.
    // All items are DWORD-aligned.
    // wFlags bit 0x01 = popup (opens submenu)
    // wFlags bit 0x80 = last item in this menu/submenu
    // Submenu terminator: all-zero fields + empty text

    auto align_dword = [](size_t p) -> size_t
    {
      return (p + 3) & ~static_cast<size_t>(3);
    };

    // Recursive lambda to parse items at a given depth.
    // Uses r.pos() as a cursor; returns position past all consumed data.
    std::function<size_t(byte_reader&, int)> parse_level;
    parse_level = [&](byte_reader& r, int depth) -> size_t
    {
      std::string indent(depth * 2, ' ');

      while (r.remaining() >= 14)
      {
        // Read 14-byte fixed portion
        uint32_t dw_type = r.read_u32();
        uint32_t dw_state = r.read_u32();
        uint32_t u_id = r.read_u32();
        uint16_t w_flags = r.read_u16();

        std::string text = read_unicode_string(r);
        // r.pos() is now past null terminator

        // Check terminator: all-zero fields + empty text
        if (dw_type == 0 && dw_state == 0 && u_id == 0 && w_flags == 0 && text.empty())
        {
          r.pos() = align_dword(r.pos());
          return r.pos();
        }

        bool is_popup = (w_flags & 0x01) != 0;
        bool is_last  = (w_flags & 0x80) != 0;
        bool is_separator = (!is_popup) && (dw_type & 0x0800) != 0;

        // DWORD-align past text. For popup items, dwHelpId sits here.
        r.pos() = align_dword(r.pos());

        if (is_popup)
        {
          // Skip the 4-byte dwHelpId that follows popup text
          uint32_t dh = 0;
          if (r.remaining() >= 4)
            dh = r.read_u32();
          (void)dh;
        }

        // Emit the item
        if (is_separator)
        {
          out << indent << "MENUITEM SEPARATOR\n";
        }
        else if (is_popup)
        {
          out << indent << "POPUP \"" << escape_string(text) << "\"";
          if (u_id != 0)
            out << ", " << u_id;
          if (dw_state & 0x0003)
            out << ", GRAYED";
          if (dw_state & 0x1000)
            out << ", INACTIVE";
          out << "\n";
          out << indent << "BEGIN\n";

          // Recursively parse children
          r.pos() = parse_level(r, depth + 1);

          out << indent << "END\n";
        }
        else
        {
          out << indent << "MENUITEM \"" << escape_string(text) << "\", " << u_id;
          if (dw_state & 0x0003) out << ", GRAYED";
          if (dw_state & 0x0008) out << ", CHECKED";
          if (dw_state & 0x1000) out << ", DEFAULT";
          if (dw_type & 0x0040) out << ", MENUBARBREAK";
          if (dw_type & 0x0080) out << ", MENUBREAK";
          out << "\n";
        }

        if (is_last)
          return r.pos();
      }

      return r.pos();
    };

    parse_level(reader, 0);
  }
  else
  {
    while (!reader.eof())
    {
      if (reader.remaining() < 2)
        break;

      uint16_t option = reader.read_u16();
      if (option == 0)
      {
        if (!stack.empty())
        {
          out << "  END\n";
          stack.pop_back();
          current_depth = stack.empty() ? 0 : stack.back().depth;
        }
        else
        {
          break;
        }
        continue;
      }

      if (reader.remaining() < 2)
        break;

      uint16_t id = reader.read_u16();
      std::string text = read_unicode_string(reader);

      bool is_separator = (option & 0x0800) != 0;
      bool is_popup = (option & 0x0010) != 0;

      std::string indent(current_depth * 2, ' ');

      if (is_separator)
      {
        out << indent << "MENUITEM SEPARATOR\n";
      }
      else if (is_popup)
      {
        out << indent << "POPUP \"" << escape_string(text) << "\"";
        if (option & 0x0001)
          out << ", GRAYED";
        if (option & 0x0002)
          out << ", INACTIVE";
        out << "\n";
        out << indent << "BEGIN\n";
        stack.push_back({current_depth + 1});
        current_depth++;
      }
      else
      {
        out << indent << "MENUITEM \"" << escape_string(text) << "\", " << id;
        if (option & 0x0001) out << ", GRAYED";
        if (option & 0x0008) out << ", CHECKED";
        if (option & 0x0080) out << ", MENUBARBREAK";
        if (option & 0x0100) out << ", MENUBREAK";
        out << "\n";
      }
    }
  }

  while (!stack.empty())
  {
    out << "  END\n";
    stack.pop_back();
  }

  out << "END\n";
  return out.str();
}


static const std::unordered_map<uint16_t, std::string> keyMap =
{
  { 0x10, "VK_SHIFT" },
  { 0x11, "VK_CONTROL" },
  { 0x12, "VK_MENU" },
  { 0x13, "VK_PAUSE" },
  { 0x14, "VK_CAPITAL" },
  { 0x08, "VK_BACK" },
  { 0x09, "VK_TAB" },
  { 0x0D, "VK_RETURN" },
  { 0x1B, "VK_ESCAPE" },
  { 0x20, "VK_SPACE" },
  { 0x21, "VK_PRIOR" },
  { 0x22, "VK_NEXT" },
  { 0x23, "VK_END" },
  { 0x24, "VK_HOME" },
  { 0x25, "VK_LEFT" },
  { 0x26, "VK_UP" },
  { 0x27, "VK_RIGHT" },
  { 0x28, "VK_DOWN" },
  { 0x2C, "VK_SNAPSHOT" },
  { 0x2D, "VK_INSERT" },
  { 0x2E, "VK_DELETE" },
  { 0x2F, "VK_HELP" },
  { 0xA0, "VK_LSHIFT" },
  { 0xA1, "VK_RSHIFT" },
  { 0xA2, "VK_LCONTROL" },
  { 0xA3, "VK_RCONTROL" },
  { 0xA4, "VK_LMENU" },
  { 0xA5, "VK_RMENU" },
  { 0x30, "VK_0" }, { 0x31, "VK_1" }, { 0x32, "VK_2" }, { 0x33, "VK_3" },
  { 0x34, "VK_4" }, { 0x35, "VK_5" }, { 0x36, "VK_6" }, { 0x37, "VK_7" },
  { 0x38, "VK_8" }, { 0x39, "VK_9" },
  { 0x41, "VK_A" }, { 0x42, "VK_B" }, { 0x43, "VK_C" }, { 0x44, "VK_D" },
  { 0x45, "VK_E" }, { 0x46, "VK_F" }, { 0x47, "VK_G" }, { 0x48, "VK_H" },
  { 0x49, "VK_I" }, { 0x4A, "VK_J" }, { 0x4B, "VK_K" }, { 0x4C, "VK_L" },
  { 0x4D, "VK_M" }, { 0x4E, "VK_N" }, { 0x4F, "VK_O" }, { 0x50, "VK_P" },
  { 0x51, "VK_Q" }, { 0x52, "VK_R" }, { 0x53, "VK_S" }, { 0x54, "VK_T" },
  { 0x55, "VK_U" }, { 0x56, "VK_V" }, { 0x57, "VK_W" }, { 0x58, "VK_X" },
  { 0x59, "VK_Y" }, { 0x5A, "VK_Z" },
  { 0x70, "VK_F1" },  { 0x71, "VK_F2" },  { 0x72, "VK_F3" },  { 0x73, "VK_F4" },
  { 0x74, "VK_F5" },  { 0x75, "VK_F6" },  { 0x76, "VK_F7" },  { 0x77, "VK_F8" },
  { 0x78, "VK_F9" },  { 0x79, "VK_F10" }, { 0x7A, "VK_F11" }, { 0x7B, "VK_F12" },
  { 0x7C, "VK_F13" }, { 0x7D, "VK_F14" }, { 0x7E, "VK_F15" }, { 0x7F, "VK_F16" },
  { 0x80, "VK_F17" }, { 0x81, "VK_F18" }, { 0x82, "VK_F19" }, { 0x83, "VK_F20" },
  { 0x84, "VK_F21" }, { 0x85, "VK_F22" }, { 0x86, "VK_F23" }, { 0x87, "VK_F24" },
  { 0x60, "VK_NUMPAD0" }, { 0x61, "VK_NUMPAD1" },
  { 0x62, "VK_NUMPAD2" }, { 0x63, "VK_NUMPAD3" },
  { 0x64, "VK_NUMPAD4" }, { 0x65, "VK_NUMPAD5" },
  { 0x66, "VK_NUMPAD6" }, { 0x67, "VK_NUMPAD7" },
  { 0x68, "VK_NUMPAD8" }, { 0x69, "VK_NUMPAD9" },
  { 0x6A, "VK_MULTIPLY" }, { 0x6B, "VK_ADD" },
  { 0x6C, "VK_SEPARATOR" }, { 0x6D, "VK_SUBTRACT" },
  { 0x6E, "VK_DECIMAL" }, { 0x6F, "VK_DIVIDE" },
};

std::string decode_accelerators(
  std::span<const uint8_t> data,
  const std::string& resource_id)
{
  byte_reader reader(data);

  std::ostringstream out;
  out << resource_id << " ACCELERATORS\nBEGIN\n";

  while (!reader.eof() && reader.remaining() >= 8)
  {
    uint8_t flags = reader.read_u8();
    reader.read_u8();
    uint16_t event = reader.read_u16();
    uint16_t id = reader.read_u16();
    reader.read_u16();

    std::string key_str;
    if (flags & 0x01)
    {
      auto it = keyMap.find(event);
      if (it != keyMap.end())
        key_str = it->second;
      else if (event >= 0x21 && event <= 0x7E)
        key_str = std::format("\"{}\"", static_cast<char>(event));
      else
        key_str = std::format("0x{:X}", event);
    }
    else if (flags & 0x02)
      key_str = std::format("\"{}\"", static_cast<char>(event));
    else
      key_str = std::format("{}", event);

    out << "  " << key_str << ", " << id;

    std::vector<std::string> attr;
    if (flags & 0x01) attr.push_back("VIRTKEY");
    if (flags & 0x02) attr.push_back("ASCII");
    if (flags & 0x08) attr.push_back("NOINVERT");
    if (flags & 0x10) attr.push_back("ALT");
    if (flags & 0x20) attr.push_back("SHIFT");
    if (flags & 0x40) attr.push_back("CONTROL");

    if (!attr.empty())
    {
      out << ", ";
      for (size_t i = 0; i < attr.size(); i++)
      {
        if (i > 0) out << " | ";
        out << attr[i];
      }
    }
    out << "\n";
  }

  out << "END\n";
  return out.str();
}

std::string decode_stringtable(
  std::span<const uint8_t> data,
  uint32_t group_id)
{
  byte_reader reader(data);

  std::ostringstream out;
  out << "STRINGTABLE\nBEGIN\n";

  for (int i = 0; i < 16 && !reader.eof(); i++)
  {
    uint16_t len = reader.read_u16();
    if (len == 0)
      continue;

    std::u16string utf16;
    for (uint16_t j = 0; j < len && !reader.eof(); j++)
      utf16 += static_cast<char16_t>(reader.read_u16());

    std::string str;
    for (char16_t ch : utf16)
      str += static_cast<char>(ch < 128 ? ch : '?');

    uint32_t string_id = (group_id - 1) * 16 + i;
    out << "  " << resource_id_string(string_id)
      << ", \"" << escape_string(str) << "\"\n";
  }

  out << "END\n";
  return out.str();
}

/* Parent container kind that drives how each child block is interpreted. */
enum class version_block_kind
{
  root,          // children are named containers (StringFileInfo/VarFileInfo)
  string_file,   // children are StringTable language blocks (containers)
  string_table,  // children are String blocks (quoted UTF-16 values)
  var_file,      // children are Var blocks (binary WORD-array values)
};

std::string decode_versioninfo(
  std::span<const uint8_t> data,
  const std::string& resource_id)
{
  byte_reader reader(data);

  if (reader.remaining() < sizeof(vs_versioninfo_t))
    return "";

  vs_versioninfo_t header{};
  std::memcpy(&header, data.data(), sizeof(header));
  reader.skip(sizeof(header));

  std::u16string key;
  for (size_t i = 0; i < 16 && header.szKey[i] != 0; i++)
    key += header.szKey[i];

  if (key != u"VS_VERSION_INFO")
    return "";

  std::ostringstream out;
  out << resource_id << " VERSIONINFO\n";

  const vs_fixedfileinfo_t& fixed_info = header.Value;

  if (fixed_info.dwSignature == 0xFEEF04BD)
  {
    out << std::format("FILEVERSION {},{},{},{}\n",
                        fixed_info.dwFileVersionMS.high,
                        fixed_info.dwFileVersionMS.low,
                        fixed_info.dwFileVersionLS.high,
                        fixed_info.dwFileVersionLS.low);
    out << std::format("PRODUCTVERSION {},{},{},{}\n",
                       fixed_info.dwProductVersionMS.high,
                       fixed_info.dwProductVersionMS.low,
                       fixed_info.dwProductVersionLS.high,
                       fixed_info.dwProductVersionLS.low);
  }

  out << "BEGIN\n";

  std::function<void(byte_reader&, int, size_t, version_block_kind)> decode_block;
  decode_block = [&](byte_reader& r, int indent, size_t block_end,
    version_block_kind parent_kind) -> void
  {
    while (r.pos() < block_end && r.remaining() >= 4)
    {
      size_t item_start = r.pos();

      if (r.pos() % 4 != 0)
        r.skip((4 - (r.pos() % 4)) % 4);

      if (r.pos() >= block_end)
        break;

      uint16_t len = r.read_u16();
      uint16_t val_len = r.read_u16();

      if (len == 0 || r.pos() + len > block_end + 1024)
        break;

      /* Every version block carries a WORD wType field between
         wValueLength and szKey. */
      r.skip(2);

      std::string key = read_unicode_string(r);

      if (r.pos() % 4 != 0)
        r.skip((4 - (r.pos() % 4)) % 4);

      std::string pad(indent * 2, ' ');

      /* The block content ends at item_start + wLength; the next block
         starts at the next DWORD boundary. */
      size_t next_item = item_start + len;
      if (next_item % 4 != 0)
        next_item += 4 - (next_item % 4);

      /* A block is a container if its key names a known container, or if
         it is a StringTable language block (8-hex-digit key) sitting
         directly inside a StringFileInfo block. */
      bool is_named_container = (key == "StringFileInfo" ||
                                 key == "VarFileInfo" ||
                                 key == "VS_VERSION_INFO");
      bool is_container = (is_named_container ||
                           parent_kind == version_block_kind::string_file);

      if (is_container)
      {
        if (key != "VS_VERSION_INFO")
        {
          out << pad << "BLOCK \"" << key << "\"\n";
          out << pad << "BEGIN\n";
        }

        version_block_kind child_kind = version_block_kind::root;
        if (key == "StringFileInfo")
          child_kind = version_block_kind::string_file;
        else if (key == "VarFileInfo")
          child_kind = version_block_kind::var_file;
        else if (parent_kind == version_block_kind::string_file)
          child_kind = version_block_kind::string_table;

        size_t children_end = item_start + len;
        size_t val_end = r.pos() + val_len;
        if (val_end < children_end)
          decode_block(r, indent + (key == "VS_VERSION_INFO" ? 0 : 1), children_end,
                       child_kind);

        if (key != "VS_VERSION_INFO")
          out << pad << "END\n";
      }
      else if (parent_kind == version_block_kind::var_file && val_len > 0)
      {
        /* Var block: the value is a binary array of WORDs (language ids). */
        std::string val;
        for (uint16_t i = 0; i + 1 < val_len && r.remaining() >= 2; i += 2)
        {
          if (!val.empty())
            val += ", ";
          val += std::format("0x{:04X}", r.read_u16());
        }
        out << pad << std::format("VALUE \"{}\", {}\n", key, val);
      }
      else if (val_len > 0 && val_len < 1024)
      {
        std::string val = read_unicode_string(r);
        if (val.find(',') != std::string::npos
            || (!val.empty() && (std::isdigit(val[0]) || val[0] == '-' || val[0] == '0')))
          out << pad << std::format("VALUE \"{}\", {}\n", key, val);
        else
          out << pad << std::format("VALUE \"{}\", {}\n", key, escape_string(val));
      }

      r.pos() = next_item;
    }
  };

  decode_block(reader, 1, data.size(), version_block_kind::root);

  out << "END\n";
  return out.str();
}

// ─── Main resource decoder ────────────────────────────────────────────

std::vector<decoded_resource> decode_pe_resources(
  const std::string& pe_path,
  const std::filesystem::path& output_dir)
{
  std::vector<decoded_resource> results;

  pefile::PE pe(pe_path);

  for (const auto& root_dir : pe.resources())
  {
    // Pre-scan: collect all RT_ICON IDs referenced by GROUP_ICON entries
    std::set<uint32_t> grouped_icon_ids;
    for (const auto& te : root_dir.entries)
    {
      if (te.id != 14 || !te.directory)
        continue;
      for (const auto& ie : te.directory->entries)
      {
        std::span<const uint8_t> gd;
        if (ie.data_entry)
          gd = pe.get_data(ie.data_entry->data_rva, ie.data_entry->size);
        else if (ie.directory)
          for (const auto& le : ie.directory->entries)
            if (le.data_entry) { gd = pe.get_data(le.data_entry->data_rva, le.data_entry->size); break; }
        if (gd.empty()) continue;
        byte_reader gr(gd);
        gr.skip(2); gr.read_u16(); uint16_t cnt = gr.read_u16();
        for (uint16_t k = 0; k < cnt && gr.remaining() >= 14; k++)
        {
          gr.skip(8); gr.read_u16(); gr.read_u32(); uint16_t nid = gr.read_u16();
          grouped_icon_ids.insert(nid);
        }
      }
    }

    for (const auto& type_entry : root_dir.entries)
    {
      uint32_t type_id = type_entry.id;

      if (!type_entry.directory)
        continue;

      for (const auto& id_entry : type_entry.directory->entries)
      {
        std::span<const uint8_t> raw_data;

        if (id_entry.data_entry)
        {
          raw_data = pe.get_data(id_entry.data_entry->data_rva, id_entry.data_entry->size);
        }
        else if (id_entry.directory)
        {
          for (const auto& lang_entry : id_entry.directory->entries)
          {
            if (lang_entry.data_entry)
            {
              raw_data = pe.get_data(lang_entry.data_entry->data_rva, lang_entry.data_entry->size);
              break;
            }
          }
        }

        if (raw_data.empty())
          continue;

        std::string res_id;
        if (!id_entry.name.empty())
          res_id = id_entry.name;
        else
          res_id = resource_id_string(id_entry.id);

        static int s_bitmap_idx = 0;
        static int s_icon_idx = 0;

        decoded_resource dr;
        dr.id = res_id;

        switch (type_id)
        {
          case 2:
          {
            dr.type = "BITMAP";
            dr.image_data = imageio::dib_to_bmp(raw_data);
            dr.filename = std::format("image{}.{}", s_bitmap_idx, imageio::is_png_data(raw_data) ? "png" : "bmp");
            dr.id = std::format("image{}", s_bitmap_idx);
            s_bitmap_idx++;
            break;
          }
          case 3:
          {
            // Skip RT_ICON entries that are part of a GROUP_ICON
            if (grouped_icon_ids.count(id_entry.id))
              continue;
            dr.type = "ICON";
            dr.image_data = imageio::ico_to_bmp(raw_data);
            dr.filename = std::format("icon{}.bmp", s_icon_idx);
            dr.id = std::format("icon{}", s_icon_idx);
            s_icon_idx++;
            break;
          }
          case 4:
            dr.type = "MENU";
            dr.rc_text = decode_menu(raw_data, res_id);
            break;
          case 5:
            dr.type = "DIALOG";
            dr.rc_text = decode_dialog(raw_data, res_id);
            break;
          case 6:
            dr.type = "STRINGTABLE";
            dr.rc_text = decode_stringtable(raw_data, id_entry.id);
            break;
          case 9:
            dr.type = "ACCELERATORS";
            dr.rc_text = decode_accelerators(raw_data, res_id);
            break;
          case 14:
          {
            byte_reader grp_reader(raw_data);
            grp_reader.skip(2); // reserved
            uint16_t icon_type = grp_reader.read_u16();
            uint16_t count = grp_reader.read_u16();
            (void)icon_type;

            for (uint16_t i = 0; i < count; i++)
            {
              if (grp_reader.remaining() < 14)
                break;
              uint8_t  w = grp_reader.read_u8();
              uint8_t  h = grp_reader.read_u8();
              /*uint8_t  colors = */ grp_reader.read_u8();
              /*uint8_t  reserved = */ grp_reader.read_u8();
              uint16_t planes = grp_reader.read_u16();
              uint16_t bpp = grp_reader.read_u16();
              uint32_t size = grp_reader.read_u32();
              uint16_t nid = grp_reader.read_u16();
              (void)planes;
              (void)size;

              for (const auto& type_entry2 : root_dir.entries)
              {
                if (type_entry2.id != 3)
                  continue;
                if (!type_entry2.directory)
                  continue;
                for (const auto& id_entry2 : type_entry2.directory->entries)
                {
                  if (id_entry2.id != nid)
                    continue;

                  std::span<const uint8_t> ico_data;
                  if (id_entry2.data_entry)
                    ico_data = pe.get_data(id_entry2.data_entry->data_rva, id_entry2.data_entry->size);
                  else if (id_entry2.directory)
                  {
                    for (const auto& lang_entry2 : id_entry2.directory->entries)
                    {
                      if (lang_entry2.data_entry)
                      {
                        ico_data = pe.get_data(lang_entry2.data_entry->data_rva, lang_entry2.data_entry->size);
                        break;
                      }
                    }
                  }

                  if (!ico_data.empty())
                  {
                    decoded_resource icon_dr;
                    icon_dr.type = "ICON";
                    icon_dr.id = std::format("icon{}", s_icon_idx);
                    icon_dr.image_data = imageio::ico_to_bmp(ico_data);
                    icon_dr.filename =
                        std::format("icon{}_{}x{}_{}bpp.{}",
                                    s_icon_idx, w, h, bpp,
                                    imageio::is_png_data(ico_data) ? "png" : "bmp");
                    s_icon_idx++;
                    results.push_back(std::move(icon_dr));
                  }
                  break;
                }
              }
            }
            continue;
          }
          case 16:
            dr.type = "VERSIONINFO";
            dr.rc_text = decode_versioninfo(raw_data, res_id);
            break;
          default:
            break;
        }

        bool has_rc = !dr.rc_text.empty();
        bool has_img = !dr.image_data.empty();

        if (has_rc || has_img)
        {
          if (dr.type.empty())
          {
            switch (type_id)
            {
              case 2: dr.type = "BITMAP"; break;
              case 3: dr.type = "ICON"; break;
              case 4: dr.type = "MENU"; break;
              case 5: dr.type = "DIALOG"; break;
              case 6: dr.type = "STRINGTABLE"; break;
              case 9: dr.type = "ACCELERATORS"; break;
              case 16: dr.type = "VERSIONINFO"; break;
              default: dr.type = "UNKNOWN"; break;
            }
          }
          results.push_back(std::move(dr));
        }
      }
    }
  }

  return results;
}

}
