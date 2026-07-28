#include "pe_resource_decoder.h"
#include "pefile.hpp"
#include "pe_constants.hpp"
#include "pe_containers.hpp"

#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <functional>

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
        throw std::runtime_error("byte_reader: out of bounds reading u8");
      return m_data[m_pos++];
    }

    uint16_t read_u16()
    {
      if (m_pos + 2 > m_data.size())
        throw std::runtime_error("byte_reader: out of bounds reading u16");
      uint16_t val = m_data[m_pos] | (static_cast<uint16_t>(m_data[m_pos + 1]) << 8);
      m_pos += 2;
      return val;
    }

    uint32_t read_u32()
    {
      if (m_pos + 4 > m_data.size())
        throw std::runtime_error("byte_reader: out of bounds reading u32");
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
      return "#" + std::to_string(ordinal);
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

  static std::string format_style(uint32_t style, bool is_extended, bool is_dialog)
  {
    std::vector<std::string> flags;

    if (is_dialog)
    {
      if (style & 0x00000001) flags.push_back("DS_MODALFRAME");
      if (style & 0x00000002) flags.push_back("DS_ABSALIGN");
      if (style & 0x00000004) flags.push_back("DS_SETFOREGROUND");
      if (style & 0x00000008) flags.push_back("DS_NOFAILCREATE");
      if (style & 0x00000010) flags.push_back("DS_SETFONT");
      if (style & 0x00000020) flags.push_back("DS_FIXEDSYS");
      if (style & 0x00000040) flags.push_back("DS_CONTROL");
      if (style & 0x00000080) flags.push_back("DS_CENTER");
      if (style & 0x00000100) flags.push_back("DS_CENTERMOUSE");
      if (style & 0x00000200) flags.push_back("DS_CONTEXTHELP");
      if (style & 0x00010000) flags.push_back("WS_POPUP");
      if (style & 0x00040000) flags.push_back("WS_CAPTION");
      if (style & 0x00080000) flags.push_back("WS_SYSMENU");
      if (style & 0x00100000) flags.push_back("WS_THICKFRAME");
      if (style & 0x00200000) flags.push_back("WS_MINIMIZEBOX");
      if (style & 0x00400000) flags.push_back("WS_MAXIMIZEBOX");
      if (style & 0x01000000) flags.push_back("WS_VISIBLE");
      if (style & 0x02000000) flags.push_back("WS_DISABLED");
      if (style & 0x04000000) flags.push_back("WS_CLIPSIBLINGS");
      if (style & 0x10000000) flags.push_back("WS_CHILD");
    }
    else
    {
      if (style & 0x00000001) flags.push_back("BS_PUSHBUTTON");
      if (style & 0x00000002) flags.push_back("BS_DEFPUSHBUTTON");
      if (style & 0x00000003) flags.push_back("BS_CHECKBOX");
      if (style & 0x00000004) flags.push_back("BS_AUTOCHECKBOX");
      if (style & 0x00000005) flags.push_back("BSRadioButton");
      if (style & 0x00000007) flags.push_back("BS_3STATE");
      if (style & 0x00000008) flags.push_back("BS_AUTO3STATE");
      if (style & 0x00000009) flags.push_back("BS_GROUPBOX");
      if (style & 0x0000000A) flags.push_back("BS_AUTORADIOBUTTON");
      if (style & 0x0000000B) flags.push_back("BS_PUSHBOX");
      if (style & 0x0000000C) flags.push_back("BS_OWNERDRAW");
      if (style & 0x0000000F) flags.push_back("BS_TYPEMASK");
      if (style & 0x00000010) flags.push_back("BS_LEFTTEXT");
      if (style & 0x00000020) flags.push_back("BS_ICON");
      if (style & 0x00000040) flags.push_back("BS_BITMAP");
      if (style & 0x00000080) flags.push_back("BS_AUTORADIOBUTTON");
      if (style & 0x00000200) flags.push_back("BS_FLAT");

      if (style & 0x00000000) flags.push_back("ES_LEFT");
      if (style & 0x00000001) flags.push_back("ES_CENTER");
      if (style & 0x00000002) flags.push_back("ES_RIGHT");
      if (style & 0x00000004) flags.push_back("ES_MULTILINE");
      if (style & 0x00000008) flags.push_back("ES_UPPERCASE");
      if (style & 0x00000010) flags.push_back("ES_LOWERCASE");
      if (style & 0x00000020) flags.push_back("ES_PASSWORD");
      if (style & 0x00000040) flags.push_back("ES_AUTOVSCROLL");
      if (style & 0x00000080) flags.push_back("ES_AUTOHSCROLL");
      if (style & 0x00000100) flags.push_back("ES_NOHIDESEL");
      if (style & 0x00001000) flags.push_back("ES_OEMCONVERT");
      if (style & 0x00002000) flags.push_back("ES_READONLY");
      if (style & 0x00004000) flags.push_back("ES_WANTRETURN");
      if (style & 0x00010000) flags.push_back("ES_NUMBER");

      if (style & 0x00000001) flags.push_back("SS_LEFT");
      if (style & 0x00000002) flags.push_back("SS_CENTER");
      if (style & 0x00000003) flags.push_back("SS_RIGHT");
      if (style & 0x00000004) flags.push_back("SS_ICON");
      if (style & 0x00000005) flags.push_back("SS_BLACKRECT");
      if (style & 0x00000006) flags.push_back("SS_GRAYRECT");
      if (style & 0x00000007) flags.push_back("SS_WHITERECT");
      if (style & 0x00000008) flags.push_back("SS_BLACKFRAME");
      if (style & 0x00000009) flags.push_back("SS_GRAYFRAME");
      if (style & 0x0000000A) flags.push_back("SS_WHITEFRAME");
      if (style & 0x0000000D) flags.push_back("SS_SIMPLE");
      if (style & 0x0000000E) flags.push_back("SS_LEFTNOWORDWRAP");
      if (style & 0x00000010) flags.push_back("SS_OWNERDRAW");
      if (style & 0x00000030) flags.push_back("SS_TYPEMASK");
      if (style & 0x00000100) flags.push_back("SS_NOPREFIX");
      if (style & 0x00000200) flags.push_back("SS_NOTIFY");
      if (style & 0x00000400) flags.push_back("SS_CENTERIMAGE");
      if (style & 0x00000800) flags.push_back("SS_RIGHTJUST");
      if (style & 0x00001000) flags.push_back("SS_REALSIZECONTROL");
      if (style & 0x00002000) flags.push_back("SS_SUNKEN");

      if (style & 0x00000001) flags.push_back("LBS_NOTIFY");
      if (style & 0x00000002) flags.push_back("LBS_SORT");
      if (style & 0x00000008) flags.push_back("LBS_NOSEL");
      if (style & 0x00000010) flags.push_back("LBS_MULTIPLESEL");
      if (style & 0x00000020) flags.push_back("LBS_OWNERDRAWFIXED");
      if (style & 0x00000040) flags.push_back("LBS_OWNERDRAWVARIABLE");
      if (style & 0x00000080) flags.push_back("LBS_HASSTRINGS");
      if (style & 0x00000100) flags.push_back("LBS_USETABSTOPS");
      if (style & 0x00000200) flags.push_back("LBS_NOINTEGRALHEIGHT");
      if (style & 0x00000400) flags.push_back("LBS_MULTICOLUMN");
      if (style & 0x00000800) flags.push_back("LBS_WANTKEYBOARDINPUT");
      if (style & 0x00001000) flags.push_back("LBS_EXTENDEDSEL");
      if (style & 0x00002000) flags.push_back("LBS_DISABLENOSCROLL");
      if (style & 0x00010000) flags.push_back("LBS_NODATA");
      if (style & 0x00020000) flags.push_back("LBS_NOSEL");

      if (style & 0x00000001) flags.push_back("CBS_SIMPLE");
      if (style & 0x00000002) flags.push_back("CBS_DROPDOWN");
      if (style & 0x00000003) flags.push_back("CBS_DROPDOWNLIST");
      if (style & 0x00000004) flags.push_back("CBS_OWNERDRAWFIXED");
      if (style & 0x00000008) flags.push_back("CBS_OWNERDRAWVARIABLE");
      if (style & 0x00000010) flags.push_back("CBS_AUTOHSCROLL");
      if (style & 0x00000020) flags.push_back("CBS_OEMCONVERT");
      if (style & 0x00000040) flags.push_back("CBS_SORT");
      if (style & 0x00000080) flags.push_back("CBS_HASSTRINGS");
      if (style & 0x00000100) flags.push_back("CBS_NOINTEGRALHEIGHT");
      if (style & 0x00000200) flags.push_back("CBS_SIMPLE");
      if (style & 0x00000400) flags.push_back("CBS_DISABLENOSCROLL");
      if (style & 0x00001000) flags.push_back("CBS_UPPERCASE");
      if (style & 0x00002000) flags.push_back("CBS_LOWERCASE");

      if (style & 0x00000001) flags.push_back("SBS_HORZ");
      if (style & 0x00000002) flags.push_back("SBS_VERT");
      if (style & 0x00000004) flags.push_back("SBS_TOPALIGN");
      if (style & 0x00000008) flags.push_back("SBS_LEFTALIGN");
      if (style & 0x00000010) flags.push_back("SBS_NOABBREVIATIONS");
      if (style & 0x00000020) flags.push_back("SBS_DISABLENOSCROLL");

      if (style & 0x01000000) flags.push_back("WS_VISIBLE");
      if (style & 0x02000000) flags.push_back("WS_DISABLED");
      if (style & 0x04000000) flags.push_back("WS_TABSTOP");
      if (style & 0x10000000) flags.push_back("WS_GROUP");
    }

    std::string result;
    for (size_t i = 0; i < flags.size(); i++)
    {
      if (i > 0)
        result += " | ";
      result += flags[i];
    }
    return result;
  }

  static std::string format_ex_style(uint32_t ex_style)
  {
    std::vector<std::string> flags;
    if (ex_style & 0x00000100) flags.push_back("WS_EX_DLGMODALFRAME");
    if (ex_style & 0x00000004) flags.push_back("WS_EX_NOPARENTNOTIFY");
    if (ex_style & 0x00000080) flags.push_back("WS_EX_TOOLWINDOW");
    if (ex_style & 0x00000040) flags.push_back("WS_EX_APPWINDOW");
    if (ex_style & 0x00000200) flags.push_back("WS_EX_WINDOWEDGE");
    if (ex_style & 0x00000008) flags.push_back("WS_EX_TOPMOST");
    if (ex_style & 0x00080000) flags.push_back("WS_EX_LAYERED");
    if (ex_style & 0x00000020) flags.push_back("WS_EX_CLIENTEDGE");
    if (ex_style & 0x00000010) flags.push_back("WS_EX_CONTEXTHELP");
    if (ex_style & 0x00040000) flags.push_back("WS_EX_CONTROLPARENT");
    if (ex_style & 0x00000400) flags.push_back("WS_EX_RIGHTSCROLLBAR");
    if (ex_style & 0x00000001) flags.push_back("WS_EX_RIGHT");
    if (ex_style & 0x00000002) flags.push_back("WS_EX_RTLREADING");
    if (ex_style & 0x00002000) flags.push_back("WS_EX_STATICEDGE");
    if (ex_style & 0x00010000) flags.push_back("WS_EX_LAYERED");
    if (ex_style & 0x00000010) flags.push_back("WS_EX_CONTEXTHELP");

    std::string result;
    for (size_t i = 0; i < flags.size(); i++)
    {
      if (i > 0)
        result += " | ";
      result += flags[i];
    }
    return result;
  }

  static std::string escape_rc_string(const std::string& s)
  {
    std::string result;
    for (char c : s)
    {
      if (c == '"')
        result += "\"\"";
      else if (c == '\\')
        result += "\\\\";
      else if (c == '\t')
        result += "\\t";
      else if (c == '\n')
        result += "\\n";
      else
        result += c;
    }
    return result;
  }

  static std::string resource_id_string(uint32_t id)
  {
    return "ID_" + std::to_string(id);
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
    std::string cls = ctrl.class_name;
    std::transform(cls.begin(), cls.end(), cls.begin(), ::toupper);

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
        style_val = "0x" + ([](uint32_t v) {
          char buf[16];
          snprintf(buf, sizeof(buf), "%x", v);
          return std::string(buf);
        })(rest_style);

      std::string line = "    CONTROL \"" + escape_rc_string(ctrl.text)
        + "\", " + id_str
        + ", \"" + ctrl.class_name + "\"";

      if (style_val.empty())
        line += ", 0";
      else
        line += ", " + style_val;

      line += ", " + std::to_string(ctrl.x) + ", " + std::to_string(ctrl.y)
        + ", " + std::to_string(ctrl.cx) + ", " + std::to_string(ctrl.cy);

      if (ctrl.ex_style)
        line += ", " + format_ex_style(ctrl.ex_style);

      return line;
    }

    std::string line = "    " + keyword + " \"" + escape_rc_string(ctrl.text)
      + "\", " + id_str
      + ", " + std::to_string(ctrl.x) + ", " + std::to_string(ctrl.y)
      + ", " + std::to_string(ctrl.cx) + ", " + std::to_string(ctrl.cy);

    uint32_t control_type = 0;
    std::string cls_upper = ctrl.class_name;
    std::transform(cls_upper.begin(), cls_upper.end(), cls_upper.begin(), ::toupper);
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
    out << "STYLE " << format_style(hdr.style, false, true);
    out << "\n";
  }

  if (hdr.ex_style)
  {
    out << "EXSTYLE " << format_ex_style(hdr.ex_style);
    out << "\n";
  }

  if (!hdr.title.empty())
    out << "CAPTION \"" << escape_rc_string(hdr.title) << "\"\n";

  if (hdr.font_size > 0)
  {
    out << "FONT " << hdr.font_size << ", \"" << escape_rc_string(hdr.font_name) << "\"";
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
  out << resource_id << " MENU";

  if (version == 1)
    out << " EX";

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
          out << indent << "POPUP \"" << escape_rc_string(text) << "\"";
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
          out << indent << "MENUITEM \"" << escape_rc_string(text) << "\", " << u_id;
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
        out << indent << "POPUP \"" << escape_rc_string(text) << "\"";
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
        out << indent << "MENUITEM \"" << escape_rc_string(text) << "\", " << id;
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
      key_str = std::to_string(event);
    else if (flags & 0x02)
      key_str = "\"" + std::string(1, static_cast<char>(event)) + "\"";
    else
      key_str = std::to_string(event);

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
      << ", \"" << escape_rc_string(str) << "\"\n";
  }

  out << "END\n";
  return out.str();
}

std::string decode_versioninfo(
  std::span<const uint8_t> data,
  const std::string& resource_id)
{
  byte_reader reader(data);

  uint16_t total_len = reader.read_u16();
  (void)total_len;
  std::string header = read_unicode_string(reader);

  if (header != "VS_VERSION_INFO")
    return "";

  std::ostringstream out;
  out << resource_id << " VERSIONINFO\n";

  if (reader.pos() % 4 != 0)
    reader.skip((4 - (reader.pos() % 4)) % 4);

  if (reader.remaining() >= 40)
  {
    size_t value_offset = reader.pos();
    uint32_t sig = reader.read_u32();
    if (sig == 0xFEEF04BD)
    {
      uint32_t file_ver_ms = reader.read_u32();
      uint32_t file_ver_ls = reader.read_u32();
      uint32_t prod_ver_ms = reader.read_u32();
      uint32_t prod_ver_ls = reader.read_u32();

      auto hiword = [](uint32_t v) -> uint32_t { return (v >> 16) & 0xFFFF; };
      auto loword = [](uint32_t v) -> uint32_t { return v & 0xFFFF; };

      out << "FILEVERSION "
          << hiword(file_ver_ms) << "," << loword(file_ver_ms) << ","
          << hiword(file_ver_ls) << "," << loword(file_ver_ls) << "\n";
      out << "PRODUCTVERSION "
          << hiword(prod_ver_ms) << "," << loword(prod_ver_ms) << ","
          << hiword(prod_ver_ls) << "," << loword(prod_ver_ls) << "\n";

      reader.skip(48);
    }
    else
    {
      reader.pos() -= 4;
    }
  }

  out << "BEGIN\n";

  std::function<void(byte_reader&, int, size_t)> decode_block;
  decode_block = [&](byte_reader& r, int indent, size_t block_end) -> void
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

      if (r.pos() % 4 != 0)
        r.skip((4 - (r.pos() % 4)) % 4);

      std::string key = read_unicode_string(r);

      if (r.pos() % 4 != 0)
        r.skip((4 - (r.pos() % 4)) % 4);

      std::string pad(indent * 2, ' ');
      size_t next_item = item_start + len;

      bool is_container = (key == "StringFileInfo" || key == "VarFileInfo"
        || key == "StringTable" || key == "VS_VERSION_INFO");

      if (is_container)
      {
        if (key != "VS_VERSION_INFO")
        {
          out << pad << "BLOCK \"" << key << "\"\n";
          out << pad << "BEGIN\n";
        }

        size_t children_end = item_start + len;
        size_t val_end = r.pos() + val_len;
        if (val_end < children_end)
          decode_block(r, indent + (key == "VS_VERSION_INFO" ? 0 : 1), children_end);

        if (key != "VS_VERSION_INFO")
          out << pad << "END\n";
      }
      else if (val_len > 0 && val_len < 1024)
      {
        std::string val = read_unicode_string(r);
        if (val_len > 2 && val.size() == 1 && !val.empty())
        {
          r.pos() -= val.size();
          std::string raw;
          for (uint16_t i = 0; i < val_len; i++)
            raw += static_cast<char>(r.read_u8());
          val = raw;
        }

        if (val.find(',') != std::string::npos
            || (!val.empty() && (std::isdigit(val[0]) || val[0] == '-' || val[0] == '0')))
          out << pad << "VALUE \"" << key << "\", " << val << "\n";
        else
          out << pad << "VALUE \"" << key << "\", \"" << escape_rc_string(val) << "\"\n";
      }

      r.pos() = next_item;
    }
  };

  decode_block(reader, 1, data.size());

  out << "END\n";
  return out.str();
}

std::vector<decoded_resource> decode_pe_resources(
  const std::string& pe_path)
{
  std::vector<decoded_resource> results;

  pefile::PE pe(pe_path);

  for (const auto& root_dir : pe.resources())
  {
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

        std::string rc_text;

        switch (type_id)
        {
          case 4:
            rc_text = decode_menu(raw_data, res_id);
            break;
          case 5:
            rc_text = decode_dialog(raw_data, res_id);
            break;
          case 6:
            rc_text = decode_stringtable(raw_data, id_entry.id);
            break;
          case 9:
            rc_text = decode_accelerators(raw_data, res_id);
            break;
          case 16:
            rc_text = decode_versioninfo(raw_data, res_id);
            break;
          default:
            break;
        }

        if (!rc_text.empty())
        {
          decoded_resource dr;
          dr.id = res_id;
          switch (type_id)
          {
            case 4: dr.type = "MENU"; break;
            case 5: dr.type = "DIALOG"; break;
            case 6: dr.type = "STRINGTABLE"; break;
            case 9: dr.type = "ACCELERATORS"; break;
            case 16: dr.type = "VERSIONINFO"; break;
            default: dr.type = "UNKNOWN"; break;
          }
          dr.rc_text = rc_text;
          results.push_back(std::move(dr));
        }
      }
    }
  }

  return results;
}

}
