#include "rc_parser.h"
#include "utils.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <ranges>
#include <map>
#include <functional>
#include <utility>

namespace rc
{

enum const_flags : uint16_t
{
  resource_type   = 0x0001,
  attribute_type  = 0x0002,
  control_keyword = 0x0004,
  dialog_keyword  = 0x0008,
  popup_flag      = 0x0010,
  has_text_flag   = 0x0020,
};

static const std::map<std::string_view, const_flags> const_flag_data
{
    // resource types
  { "ACCELERATORS", const_flags::resource_type },
  { "BITMAP",       const_flags::resource_type },
  { "CURSOR",       const_flags::resource_type },
  { "DIALOG",       const_flags::resource_type },
  { "DIALOGEX",     const_flags::resource_type },
  { "FONT",         const_flags::resource_type | const_flags::dialog_keyword },
  { "HTML",         const_flags::resource_type },
  { "ICON",         const_flags::resource_type | const_flags::control_keyword | const_flags::has_text_flag },
  { "MENU",         const_flags::resource_type | const_flags::dialog_keyword },
  { "MENUEX",       const_flags::resource_type },
  { "MESSAGETABLE", const_flags::resource_type },
  { "POPUP",        const_flags::resource_type },
  { "RCDATA",       const_flags::resource_type },
  { "STRINGTABLE",  const_flags::resource_type },
  { "TOOLBAR",      const_flags::resource_type },
  { "VERSIONINFO",  const_flags::resource_type },
  { "TEXTINCLUDE",  const_flags::resource_type },
  { "REGISTRY",     const_flags::resource_type },
  { "DLGINIT",      const_flags::resource_type },
  { "GUIDELINES",   const_flags::resource_type },
  { "DESIGNINFO",   const_flags::resource_type },
  { "DLGINCLUDE",   const_flags::resource_type },

    // resource attribute types
  { "PRELOAD",      const_flags::attribute_type },
  { "LOADONCALL",   const_flags::attribute_type },
  { "DISCARDABLE",  const_flags::attribute_type },
  { "MOVEABLE",     const_flags::attribute_type },
  { "PURE",         const_flags::attribute_type },
  { "IMPURE",       const_flags::attribute_type },
  { "SHARED",       const_flags::attribute_type },
  { "NONSHARED",    const_flags::attribute_type },
  { "FIXED",        const_flags::attribute_type },

    // controls
  { "AUTO3STATE",       const_flags::control_keyword | const_flags::has_text_flag },
  { "AUTOCHECKBOX",     const_flags::control_keyword | const_flags::has_text_flag },
  { "AUTORADIOBUTTON",  const_flags::control_keyword | const_flags::has_text_flag },
  { "CHECKBOX",         const_flags::control_keyword | const_flags::has_text_flag },
  { "COMBOBOX",         const_flags::control_keyword },
  { "CONTROL",          const_flags::control_keyword },
  { "CTEXT",            const_flags::control_keyword | const_flags::has_text_flag },
  { "DEFPUSHBUTTON",    const_flags::control_keyword | const_flags::has_text_flag },
  { "EDITTEXT",         const_flags::control_keyword },
  { "GROUPBOX",         const_flags::control_keyword | const_flags::has_text_flag },
  { "LISTBOX",          const_flags::control_keyword },
  { "LTEXT",            const_flags::control_keyword | const_flags::has_text_flag },
  { "PUSHBOX",          const_flags::control_keyword | const_flags::has_text_flag },
  { "PUSHBUTTON",       const_flags::control_keyword | const_flags::has_text_flag },
  { "RADIOBUTTON",      const_flags::control_keyword | const_flags::has_text_flag },
  { "RTEXT",            const_flags::control_keyword | const_flags::has_text_flag },
  { "SCROLLBAR",        const_flags::control_keyword },
  { "STATE3",           const_flags::control_keyword | const_flags::has_text_flag },

    // dialog options
  { "CAPTION",          const_flags::dialog_keyword },
  { "CLASS",            const_flags::dialog_keyword },
  { "EXSTYLE",          const_flags::dialog_keyword },
  { "LANGUAGE",         const_flags::dialog_keyword },
  { "STYLE",            const_flags::dialog_keyword },
  { "VERSION",          const_flags::dialog_keyword },
  { "CHARACTERISTICS",  const_flags::dialog_keyword },

  { "GRAYED",           const_flags::popup_flag },
  { "INACTIVE",         const_flags::popup_flag },
  { "CHECKED",          const_flags::popup_flag },
  { "MENUBARBREAK",     const_flags::popup_flag },
  { "MENUBREAK",        const_flags::popup_flag },
  { "RIGHTBREAK",       const_flags::popup_flag },
  { "MF_GRAYED",        const_flags::popup_flag },
  { "MF_ENABLED",       const_flags::popup_flag },
  { "MF_CHECKED",       const_flags::popup_flag },
  { "MF_UNCHECKED",     const_flags::popup_flag },
  { "MF_MENUBARBREAK",  const_flags::popup_flag },
  { "MF_MENUBREAK",     const_flags::popup_flag },
  { "MF_POPUP",         const_flags::popup_flag },
  { "MF_SEPARATOR",     const_flags::popup_flag },
  { "MF_STRING",        const_flags::popup_flag },
  { "MF_HELP",          const_flags::popup_flag },
  { "MF_END",           const_flags::popup_flag },
};

static bool has_flag(const std::string& s, const_flags flag)
{
  return const_flag_data.contains(s) && (const_flag_data.at(s) & flag);
}

parser::parser(const std::vector<token>& t) : m_tokens(t) {}

const token& parser::current() const
{
  if(m_pos >= m_tokens.size())
    return m_tokens.back();
  return m_tokens[m_pos];
}

const token& parser::peek(size_t offset) const
{
  size_t idx = m_pos + offset;
  if(idx < m_tokens.size())
    return m_tokens[idx];
  if(!m_tokens.empty())
    return m_tokens.back();
  static const token eof_token { token_type::eof, "", 0 };
  return eof_token;
}

token parser::advance()
{
  if(m_pos >= m_tokens.size())
    return { token_type::eof, "", 0 };
  return m_tokens[m_pos++];
}

void parser::advance_to_end(void)
{
  while(current() != token_type::newline_end_eof)
    advance();
}

int16_t parser::next16(void)
{
  return static_cast<int16_t>(safe_stoi(next_val()));
}

uint16_t parser::nextu16(void)
{
  return static_cast<uint16_t>(safe_stoi(next_val()));
}

bool parser::is_current_type_attribute(void) const
{
  return current() == token_type::identifier &&
         has_flag(current(), const_flags::attribute_type);
}

bool parser::is_current_identifier(const std::string& identifier)
{
  return current() == token_type::identifier &&
         current() == identifier;
}

// fixed statement keywords of the VERSIONINFO resource
static const std::initializer_list<std::string> version_fixed_keywords =
{
  "FILEVERSION", "PRODUCTVERSION", "FILEFLAGSMASK",
  "FILEFLAGS", "FILEOS", "FILETYPE", "FILESUBTYPE",
};

// number suffix tokens left over from literals such as "0x3fL"
static const std::initializer_list<std::string> version_number_suffixes =
{
  "L", "U", "UL", "LU", "LL", "ULL",
};


bool parser::match(token_type type)
{
  if(current() == type)
  {
    ++m_pos;
    return true;
  }
  return false;
}

bool parser::match_id(const std::string& value)
{
  if(is_current_identifier(value))
  {
    ++m_pos;
    return true;
  }
  return false;
}

void parser::skip_newlines()
{
  while(match(token_type::newline));
}


std::string parser::parse_resource_id()
{
  return current() == token_type::str_num_ident ? next_val() : "";
}

style_expr parser::parse_style_expr()
{
  style_expr expr;

  if(is_current_identifier("NOT"))
  {
    advance();
    skip_newlines();
    std::string flag;
    if(current() == token_type::num_or_ident)
      flag = next_val();
    if(!flag.empty())
      expr.not_flags.push_back(flag);
    return expr;
  }

  if(current() == token_type::num_or_ident)
    expr.first = next_val();

  while(current() == token_type::pipe ||
        (current() == token_type::identifier &&
         match_string(current(), { "||", "OR" })))
  {
    std::string op = next_val();
    skip_newlines();

    if(is_current_identifier("NOT"))
    {
      advance();
      skip_newlines();
      std::string flag;
      if(current() == token_type::num_or_ident)
        flag = next_val();
      if(!flag.empty())
        expr.not_flags.push_back(flag);
      skip_newlines();
      continue;
    }

    std::string val;
    if(current() == token_type::num_or_ident)
      val = next_val();
    else
      break;
    skip_newlines();
    expr.ops.push_back({op, val});
  }

  return expr;
}

control parser::parse_control()
{
  control ctrl;
  ctrl.keyword = next_val();

  if(to_upper(ctrl.keyword) == "CONTROL")
  {
    if(current() == token_type::string_literal)
      ctrl.text = next_val();
    if(match(token_type::comma))
    {
      ctrl.id = parse_resource_id();
      if(match(token_type::comma) &&
         current() == token_type::string_literal)
        ctrl.class_name = next_val();
      if(match(token_type::comma))
        ctrl.style = parse_style_expr();
      if(match(token_type::comma))
        ctrl.x = next16();
      if(match(token_type::comma))
        ctrl.y = next16();
      if(match(token_type::comma))
        ctrl.width = next16();
      if(match(token_type::comma))
        ctrl.height = next16();
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.ext_style = parse_style_expr();
      }
    }
  }
  else
  {
    if(has_flag(ctrl.keyword, const_flags::has_text_flag))
    {
      if(current() == token_type::str_num_ident)
        ctrl.text = next_val();
      if(match(token_type::comma))
        ctrl.id = parse_resource_id();
    }
    else
    {
      skip_newlines();
      ctrl.id = parse_resource_id();
    }

    if(match(token_type::comma))
      ctrl.x = next16();
    if(match(token_type::comma))
      ctrl.y = next16();
    if(match(token_type::comma))
      ctrl.width = next16();
    if(match(token_type::comma))
      ctrl.height = next16();
    if(match(token_type::comma))
      ctrl.style = parse_style_expr();
    if(match(token_type::comma))
      ctrl.ext_style = parse_style_expr();
  }

  return ctrl;
}

menu_item parser::parse_menu_item()
{
  menu_item item;
  item.text = next_val();
  if(match(token_type::comma))
    item.id = parse_resource_id();
  while(match(token_type::comma))
  {
    if(current() == token_type::identifier &&
       !has_flag(current(), const_flags::control_keyword) &&
        !match_string(current(), { "POPUP", "END" }) &&
        current() != token_type::newline_eof)
    {
      item.flags.push_back(next_val());
    }
    else
    {
      break;
    }
  }
  return item;
}


popup* parser::parse_popup()
{
  auto is_flag = [this]()
  {
    return current() == token_type::identifier &&
           current() != token_type::newline_eof &&
           !match_string(current(), { "BEGIN", "END" });
  };

  auto* pp = new popup();
  advance();
  if(current() == token_type::string_literal)
    pp->text = next_val();
  if(match(token_type::comma))
  {
    if(current() == token_type::num_or_ident &&
       !has_flag(current(), const_flags::popup_flag))
      pp->id = next_val();

    while(is_flag())
      pp->flags.push_back(next_val());
  }
  while(is_flag())
    pp->flags.push_back(next_val());
  skip_newlines();
  if(match(token_type::begin))
  {
    parse_menu_body(pp->entries);
    match(token_type::end);
  }
  return pp;
}

void parser::parse_menu_body(std::vector<menu_entry>& entries)
{
  while(skip_newlines(), current() != token_type::end_eof)
  {
    if(to_upper(current()) == "POPUP")
    {
      std::shared_ptr<popup> pp(parse_popup());
      menu_entry me;
      me.item = pp;
      entries.push_back(me);
    }
    else if(current() == "MENUITEM")
    {
      advance();
      if(current() == "SEPARATOR")
      {
        advance();
        menu_item sep;
        sep.text = "SEPARATOR";
        menu_entry me;
        me.item = sep;
        entries.push_back(me);
      }
      else
      {
        menu_item mi = parse_menu_item();
        menu_entry me;
        me.item = mi;
        entries.push_back(me);
      }
    }
    else
    {
      advance();
    }
    skip_newlines();
  }
}

dialog_stmt parser::parse_dialog_statement()
{
  dialog_stmt stmt;
  stmt.keyword = next_val();
  std::string upper = to_upper(stmt.keyword);

  if(upper == "STYLE" || upper == "EXSTYLE")
    stmt.value = parse_style_expr();
  else if(upper == "CAPTION")
  {
    if(current() == token_type::string_literal)
      stmt.text_value = next_val();
  }
  else if(upper == "FONT")
  {
    if(current() == token_type::integer_literal)
      stmt.numeric_value = nextu16();
    if(match(token_type::comma))
    {
      if(current() == token_type::string_literal)
        stmt.text_value = next_val();
      if(match(token_type::comma))
      {
        if(current() == token_type::int_or_ident)
          stmt.numeric_value2 = nextu16();
        if(match(token_type::comma) &&
           current() == token_type::int_or_ident)
          stmt.italic = nextu16();
      }
    }
  }
  else if(upper == "CLASS")
  {
    if(current() == token_type::string_literal)
      stmt.text_value = next_val();
    else
      stmt.text_value = parse_resource_id();
  }
  else if(upper == "LANGUAGE")
  {
    stmt.text_value = parse_resource_id();
    if(match(token_type::comma))
      stmt.second_text = parse_resource_id();
  }
  else if(upper == "VERSION" ||
          upper == "CHARACTERISTICS")
    stmt.value = parse_style_expr();
  else if(upper == "MENU")
  {
    stmt.id_value = parse_resource_id();
  }

  return stmt;
}

void parser::skip_begin_end()
{
  if(!match(token_type::begin))
    return;
  int depth = 1;
  while(depth > 0 && current() != token_type::eof)
  {
    if(current() == token_type::begin)
      ++depth;
    else if(current() == token_type::end)
      --depth;
    if(depth > 0)
      advance();
  }
}

void parser::parse_simple_resource(resource& res)
{
  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  if(current() == token_type::string_literal)
    res.filename = next_val();
  else if(current() == token_type::identifier &&
          !has_flag(current(), const_flags::resource_type))
    res.filename = next_val();
}

void parser::parse_dialog_resource(resource& res)
{
  dialog_data dd;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  if(current() == token_type::number_literal)
    dd.x = static_cast<int16_t>(safe_stoi(next_val()));
  if(match(token_type::comma))
  {
    skip_newlines();
    dd.y = static_cast<int16_t>(safe_stoi(next_val()));
  }
  if(match(token_type::comma))
  {
    skip_newlines();
    dd.width = static_cast<uint16_t>(safe_stoi(next_val()));
  }
  if(match(token_type::comma))
  {
    skip_newlines();
    dd.height = static_cast<uint16_t>(safe_stoi(next_val()));
  }

  if(to_upper(res.type) == "DIALOGEX" &&
     match(token_type::comma))
    dd.help_id = parse_resource_id();

  while(skip_newlines(), current() != token_type::begin_eof)
  {
    if(has_flag(current(), const_flags::dialog_keyword))
      dd.statements.push_back(parse_dialog_statement());
    else
      advance();
  }

  if(match(token_type::begin))
  {
    while(skip_newlines(), current() != token_type::end_eof)
    {
      if(has_flag(current(), const_flags::control_keyword))
        dd.controls.push_back(parse_control());
      else
        advance();
      skip_newlines();
    }
    match(token_type::end);
  }

  /* Controls are processed top to bottom, left to right, so sort them by x
     first and then by y. Stable so controls at identical positions keep their
     RC source order. */
  std::stable_sort(dd.controls.begin(), dd.controls.end(),
    [](const control& a, const control& b)
    {
      if(a.x != b.x)
        return a.x < b.x;
      return a.y < b.y;
    });

  res.data = dd;
}

void parser::parse_menu_resource(resource& res)
{
  menu_data md;

  while(current() == token_type::identifier &&
        (has_flag(current(), const_flags::attribute_type) ||
          to_upper(current()) == "EX"))
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    parse_menu_body(md.entries);
    match(token_type::end);
  }

  res.data = md;
}

void parser::parse_toolbar_resource(resource& res)
{
  toolbar_data td;

  while(current() == token_type::identifier &&
        (has_flag(current(), const_flags::attribute_type) ||
         current() == "EX"))
    res.attributes.push_back(next_val());

  if(current() == token_type::number_literal)
    td.width = static_cast<uint16_t>(safe_stoi(next_val()));
  if(match(token_type::comma))
  {
    skip_newlines();
    td.height = static_cast<uint16_t>(safe_stoi(next_val()));
  }

  skip_newlines();
  if(match(token_type::begin))
  {
    while(skip_newlines(), current() != token_type::end_eof)
    {
      if(to_upper(current()) == "SEPARATOR")
      {
        advance();
        toolbar_entry te;
        te.is_separator = true;
        td.entries.push_back(te);
      }
      else if(to_upper(current()) == "BUTTON")
      {
        advance();
        toolbar_entry te;
        te.id = parse_resource_id();
        td.entries.push_back(te);
      }
      else
      {
        advance();
      }
      skip_newlines();
    }
    match(token_type::end);
  }

  res.data = td;
}

void parser::parse_accelerator_resource(resource& res)
{
  std::vector<accelerator_entry> accels;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    while(skip_newlines(), current() != token_type::end_eof)
    {
      accelerator_entry ae;
      if(current() == token_type::str_num_ident)
        ae.event = next_val();

      if(match(token_type::comma))
        ae.id = parse_resource_id();

      while(match(token_type::comma | token_type::pipe))
      {
        if(current() == token_type::identifier)
          ae.modifiers.push_back(next_val());
      }

      accels.push_back(ae);
      skip_newlines();
    }
    match(token_type::end);
  }

  res.data = accels;
}

void parser::parse_stringtable_resource(resource& res)
{
  std::vector<string_table_entry> entries;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    while(skip_newlines(), current() != token_type::end_eof)
    {
      string_table_entry ste;
      ste.id = parse_resource_id();
      if(match(token_type::comma))
      {
        if(current() == token_type::string_literal)
          ste.value = next_val();
      }
      entries.push_back(ste);
      skip_newlines();
    }
    match(token_type::end);
  }

  res.data = entries;
}

void parser::parse_versioninfo_resource(resource& res)
{
  version_info vi;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  // Fixed statements, e.g. "FILEVERSION 1,0,0,1" or "FILETYPE VFT_DLL".
  skip_newlines();
  while(current() == token_type::identifier &&
        current() == version_fixed_keywords)
  {
    std::string key = to_upper(next_val());
    std::string val;
    bool first = true;
    while(current() == token_type::num_or_ident)
    {
      std::string item = next_val();
      if(current() == token_type::identifier &&
         current() == version_number_suffixes)
        advance();

      if(!first)
        val += ",";
      val += item;
      first = false;
      if(!match(token_type::comma))
        break;
    }
    if(!val.empty())
      vi.fixed[key] = val;
    skip_newlines();
  }

  // Body: "BLOCK \"StringFileInfo\"" / "BLOCK \"VarFileInfo\"" subtrees.
  if(match(token_type::begin))
  {
    while(skip_newlines(), current() != token_type::end_eof)
    {
      if(!match_id("BLOCK"))
      {
        advance();
        continue;
      }
      std::string block_name;
      if(current() == token_type::str_num_ident)
        block_name = next_val();
      skip_newlines();
      if(match(token_type::begin))
      {
        if(to_upper(block_name) == "STRINGFILEINFO")
        {
          // Nested language blocks, each holding VALUE statements.
          while(skip_newlines(), current() != token_type::end_eof)
          {
            if(match_id("BLOCK"))
            {
              version_string_block sb;
              if(current() == token_type::str_num_ident)
                sb.language = next_val();
              skip_newlines();
              if(match(token_type::begin))
              {
                parse_version_value_list(sb.values);
                match(token_type::end);
              }
              vi.string_info.push_back(std::move(sb));
            }
            else
            {
              version_string_block sb;
              parse_version_value_list(sb.values);
              if(!sb.values.empty())
                vi.string_info.push_back(std::move(sb));
            }
          }
          match(token_type::end);
        }
        else
        {
          // VarFileInfo (or any other block): plain VALUE statements.
          parse_version_value_list(vi.var_info);
          match(token_type::end);
        }
      }
    }
    match(token_type::end);
  }

  res.data = vi;
}

void parser::parse_version_value_list(std::map<std::string, std::string>& out)
{
  while(skip_newlines(), current() != token_type::end_eof)
  {
    if(!match_id("VALUE"))
    {
      advance();
      continue;
    }
    std::string key;
    if(current() == token_type::string_or_ident)
      key = next_val();
    if(match(token_type::comma))
    {
      std::string val;
      bool first = true;
      skip_newlines();
      while(current() == token_type::str_num_ident)
      {
        std::string item;
        if(current() == token_type::string_literal)
        {
          item = next_val();
          if(!item.empty() && item.back() == '\0')
            item.pop_back();
        }
        else if(current() == token_type::identifier)
          item = next_val();
        else
        {
          item = next_val();
          if(current() == token_type::identifier &&
             current() == version_number_suffixes)
            advance();
        }
        if(!first)
          val += ", ";
        val += item;
        first = false;
        if(!match(token_type::comma))
          break;
      }
      out[key] = val;
    }
  }
}


void parser::parse_rcdata_resource(resource& res)
{
  version_info vi;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    while(skip_newlines(), current() != token_type::end_eof)
      advance();
    match(token_type::end);
  }

  res.data = vi;
}

void parser::parse_dlginit_resource(resource& res)
{
  std::vector<dlginit_entry> entries;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  skip_newlines();
  if(!match(token_type::begin))
  {
    res.data = entries;
    return;
  }

  while(skip_newlines(), current() != token_type::end_eof)
  {
    if(current() == token_type::number_literal)
    {
      std::string num_str = current();
      bool is_zero = (num_str == "0");
      if(!is_zero && num_str.size() > 2 && num_str[0] == '0' &&
         (num_str[1] == 'x' || num_str[1] == 'X'))
        is_zero = (num_str == "0x0");

      if(is_zero)
      {
        advance();
        advance_to_end();
        continue;
      }
    }

    if(current() != token_type::identifier)
      throw std::runtime_error("Expected identifier!");

    dlginit_entry entry;
    entry.control_id = next_val();

    if(!match(token_type::comma))
    {
      advance_to_end();
      continue;
    }

    skip_newlines();

    if(current() == token_type::number_literal)
    {
      std::string msg = current();
      if(msg.size() > 2 && msg[0] == '0' && (msg[1] == 'x' || msg[1] == 'X'))
        entry.message = static_cast<uint16_t>(safe_stoul(msg, 16));
      else
        entry.message = static_cast<uint16_t>(safe_stoul(msg));
      advance();
    }

    advance_to_end();
    skip_newlines();

    std::string text_data;
    while(current() != token_type::newline_end_eof)
    {
      if(current() == token_type::hex_literal)
      {
        std::string hex = current();
        unsigned val = safe_stoul(hex, 16);
        text_data += static_cast<char>(val & 0xFF);
        text_data += static_cast<char>((val >> 8) & 0xFF);
        while(advance(), current() == token_type::comma)
        {
          advance();
          skip_newlines();
          if(current() == token_type::hex_literal)
          {
            std::string h2 = current();
            unsigned v2 = safe_stoul(h2, 16);
            text_data += static_cast<char>(v2 & 0xFF);
            text_data += static_cast<char>((v2 >> 8) & 0xFF);
          }
          else if(current() == token_type::string_literal)
          {
            std::string s2 = current();
            s2.erase(std::remove(s2.begin(), s2.end(), '"'), s2.end());
            for(size_t j = 0; j < s2.size(); ++j)
              text_data += s2[j];
          }
          else
            break;
        }
      }
      else if(current() == token_type::string_literal)
      {
        std::string s = current();
        s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
        for(size_t i = 0; i < s.size(); ++i)
          text_data += s[i];
        advance();
      }
      else if(current() == token_type::comma)
        advance();
      else
        break;
    }

    size_t last = text_data.find_last_not_of('\0');
    if(last != std::string::npos)
      text_data = text_data.substr(0, last + 1);
    else
      text_data.clear();

    entry.text = text_data;
    entries.push_back(entry);
  }

  match(token_type::end);
  res.data = entries;
}


/* === INFORMATION FOR IMPLEMENTING A DESIGNINFO PARSER ===

"DESIGNINFO", "the DESIGNINFO block is adata utilized to manage layout margins, snap lines, and guide coordinates for top dialog boxes"

"LEFTMARGIN", "Defines the inner bounding absolute x-coordinate (in dialog units) relative to the left of the dialog frame, typically acting as padding boundaries for controls."
"RIGHTMARGIN", "Defines the inner bounding absolute x-coordinate (in dialog units) relative to the right of the dialog frame, typically acting as padding boundaries for controls."

"TOPMARGIN", "Defines the inner bounding absolute y-coordinate (in dialog units) from the top edge of the window."
"BOTTOMMARGIN", "Defines the inner bounding absolute y-coordinate (in dialog units) from the bottom edge of the window."

"VERTGUIDE", "Custom vertical alignment snap-lines added explicitly by developers inside the Visual Studio Dialog Editor to line up multiple controls horizontally."
"HORZGUIDE", "Custom horizontal alignment snap-lines used to align controls vertically across rows."

"VERTGUIDE", "Defines a vertical alignment reference line placed at a specific coordinate (in dialog units) across the dialog. This allows multiple controls (like text boxes or buttons) to snap to a shared vertical axis."
"HORZGUIDE", "Defines a horizontal alignment reference line at a specific coordinate. Used to keep rows or groups of controls level with one another."

### Translation Mapping

| MFC DESIGNINFO | Parameter | Qt .ui Equivalent Concept |
| :--- | :--- | :--- |
| LEFTMARGIN | 7 | Layout property `<property name="leftMargin"><number>7</number></property>` |
| RIGHTMARGIN | 278 | Total width calculation or spacer/layout constraint |
| TOPMARGIN | 7 | Layout property `<property name="topMargin"><number>7</number></property>` |
| BOTTOMMARGIN | 196 | Total height calculation or spacer/layout constraint |
| VERTGUIDE | 140 / 210 | Explicit column alignments (can be ignored) |
| HORZGUIDE | 50 / 100 | Explicit row alignments (can be ignored) |

Example:
GUIDELINES DESIGNINFO DISCARDABLE
BEGIN
 IDD_COMPLEX_DIALOG, DIALOG
 BEGIN
     LEFTMARGIN, 7
     RIGHTMARGIN, 278
     VERTGUIDE, 140
     VERTGUIDE, 210
     TOPMARGIN, 7
     BOTTOMMARGIN, 196
     HORZGUIDE, 50
     HORZGUIDE, 100
 END

 IDD_MAIN_DIALOG, DIALOG
 BEGIN
     LEFTMARGIN, 7
     RIGHTMARGIN, 313
     TOPMARGIN, 7
     BOTTOMMARGIN, 183
 END
END


Comments have been added to identify translated values

<widget class="QDialog" name="AboutBox">
<property name="geometry">
 <rect>
   <x>0</x>
   <y>0</y>
   <width>170</width>
   <height>62</height>
 </rect>
</property>
<layout class="QVBoxLayout" name="verticalLayout">
 <!-- Translates to LEFTMARGIN, 7 -->
 <property name="leftMargin">
   <number>7</number>
   </property>
 <!-- Translates to TOPMARGIN, 7 -->
 <property name="topMargin">
   <number>7</number>
 </property>
 <!-- Translates to calculated Right Margin (170 - 163 = 7) -->
 <property name="rightMargin">
   <number>7</number>
 </property>
 <!-- Translates to BOTTOMMARGIN (e.g., 62 - 55 = 7) -->
 <property name="bottomMargin">
   <number>7</number>
 </property>

 <!-- Child widgets go here -->
</layout>
</widget>

 */
void parser::parse_designinfo_resource(resource& res)
{
  designinfo_data data;

  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  skip_newlines();
  if(!match(token_type::begin))
  {
    res.data = data;
    return;
  }

  while(skip_newlines(), current() != token_type::end_eof)
  {
    if(current() != token_type::num_or_ident)
    {
      advance_to_end();
      continue;
    }

    designinfo_dialog dialog;
    dialog.id = next_val();

    if(!match(token_type::comma))
    {
      advance_to_end();
      continue;
    }

    skip_newlines();

    if(current() == token_type::identifier)
      dialog.type = next_val();

    skip_newlines();
    if(!match(token_type::begin))
      continue;

    while(skip_newlines(), current() != token_type::end_eof)
    {
      if(current() != token_type::identifier)
      {
        advance_to_end();
        continue;
      }

      designinfo_guide guide;
      guide.key = to_upper(next_val());

      if(!match(token_type::comma))
      {
        advance_to_end();
        continue;
      }

      skip_newlines();

      if(current() == token_type::num_or_ident)
      {
        guide.value = next_val();
        guide.numeric_value = safe_stoi(guide.value);
      }

      dialog.guides.push_back(guide);

      advance_to_end();
    }

    match(token_type::end);
    data.dialogs.push_back(dialog);
  }

  match(token_type::end);
  res.data = data;
}

void parser::parse_textinclude_resource(resource& res)
{
  // TODO: implement header file parser to map new "#define" identifiers to numeric values
  parse_unused_resource(res);
}

void parser::parse_typelib_resource(resource& res)
{
  parse_unused_resource(res);
}

void parser::parse_unused_resource([[maybe_unused]] resource& res)
{
  while(current() != token_type::newline_eof)
    advance();
  skip_newlines();
  skip_begin_end();
}

void parser::decode_binary_resource(resource& res)
{
  // TODO: implement calling binary decoders
  while(is_current_type_attribute())
    advance();
  res.filename = next_val();
  info("Decoding binary resource:\n\tID: {}\n\ttype: {}\n\tfilename: {}", res.id, res.type, res.filename);

  skip_newlines();
  skip_begin_end();
}

void parser::parse_user_resource(resource& res)
{
  while(is_current_type_attribute())
    res.attributes.push_back(next_val());

  skip_newlines();
  if(current() != token_type::begin)
  {
    // File form: nameID typeID filename
    res.filename = next_val();
    info("Saving user resource:\n\tID: {}\n\ttype: {}\n\tfilename: {}", res.id, res.type, res.filename);
    parse_unused_resource(res);
  }
  else // Inline form: nameID typeID { raw-data }
    parse_user_data_block(res);
}

void parser::parse_user_data_block(resource& res)
{
  user_data ud;

  if(match(token_type::begin))
  {
    skip_newlines();
    while(current() != token_type::end_eof)
    {
      user_data_entry entry;

      if(current() == token_type::string_literal)
      {
        entry.value = next_val();
        entry.is_string = true;
      }
      else if(current() == token_type::identifier &&
              to_upper(current()) == "L" &&
              peek(1) == token_type::string_literal)
      {
        // Wide-character string, e.g. L"text"
        advance();
        entry.value = next_val();
        entry.is_string = true;
        entry.is_wide = true;
      }
      else if(current() == token_type::num_or_ident)
      {
        entry.value = next_val();
        if(current() == token_type::identifier &&
           current() == version_number_suffixes)
        {
          // "L" suffix stores the integer as a DWORD
          advance();
          entry.is_dword = true;
        }
      }
      else
      {
        advance();
        continue;
      }

      ud.items.push_back(std::move(entry));
      match(token_type::comma);
      skip_newlines();
    }
    match(token_type::end);
  }

  res.data = ud;
}

resource parser::parse_resource()
{
  resource res;
  res.id = parse_resource_id();
  skip_newlines();

  if(current() == token_type::identifier)
    res.type = next_val();
  else if(current() == token_type::number_literal)
    res.type = next_val();

  std::map<std::string, void (parser::*)(resource&)> funcmap =
  {
    { "DIALOG",           &parser::parse_dialog_resource },
    { "DIALOGEX",         &parser::parse_dialog_resource },
    { "MENU",             &parser::parse_menu_resource },
    { "MENUEX",           &parser::parse_menu_resource },
    { "TOOLBAR",          &parser::parse_toolbar_resource },
    { "ACCELERATORS",     &parser::parse_accelerator_resource },
    { "STRINGTABLE",      &parser::parse_stringtable_resource },
    { "VERSIONINFO",      &parser::parse_versioninfo_resource },
    { "RCDATA",           &parser::parse_rcdata_resource },
    { "DLGINIT",          &parser::parse_dlginit_resource },
    { "BITMAP",           &parser::parse_simple_resource },
    { "ICON",             &parser::parse_simple_resource },
    { "CURSOR",           &parser::parse_simple_resource },
    { "FONT",             &parser::parse_simple_resource },
    { "HTML",             &parser::parse_simple_resource },
    { "MESSAGETABLE",     &parser::parse_simple_resource },
    { "REGISTRY",         &parser::parse_simple_resource },
    { "DLGINCLUDE",       &parser::parse_simple_resource },
    { "DESIGNINFO",       &parser::parse_designinfo_resource },
    { "TYPELIB",          &parser::parse_typelib_resource },
    { "TEXTINCLUDE",      &parser::parse_textinclude_resource },
    { "RT_ACCELERATOR",   &parser::decode_binary_resource },
    { "RT_ANICURSOR",     &parser::decode_binary_resource },
    { "RT_ANIICON",       &parser::decode_binary_resource },
    { "RT_BITMAP",        &parser::decode_binary_resource },
    { "RT_CURSOR",        &parser::decode_binary_resource },
    { "RT_DIALOG",        &parser::decode_binary_resource },
    { "RT_DLGINCLUDE",    &parser::decode_binary_resource },
    { "RT_FONT",          &parser::decode_binary_resource },
    { "RT_FONTDIR",       &parser::decode_binary_resource },
    { "RT_GROUP_CURSOR",  &parser::decode_binary_resource },
    { "RT_GROUP_ICON",    &parser::decode_binary_resource },
    { "RT_HTML",          &parser::decode_binary_resource },
    { "RT_ICON",          &parser::decode_binary_resource },
    { "RT_MANIFEST",      &parser::decode_binary_resource },
    { "RT_MENU",          &parser::decode_binary_resource },
    { "RT_MESSAGETABLE",  &parser::decode_binary_resource },
    { "RT_PLUGPLAY",      &parser::decode_binary_resource },
    { "RT_RCDATA",        &parser::decode_binary_resource },
    { "RT_STRING",        &parser::decode_binary_resource },
    { "RT_VERSION",       &parser::decode_binary_resource },
    { "RT_VXD",           &parser::decode_binary_resource },
  };

  if(auto name = to_upper(res.type);
     funcmap.contains(name))
    std::bind_front(funcmap.at(name), this)(res);
  else
    parse_user_resource(res);

  return res;
}

rc_file parser::parse()
{
  rc_file file;

  while(current().type != token_type::eof)
  {
    skip_newlines();
    if(current() == token_type::eof)
      break;

    if(is_current_identifier("LANGUAGE"))
    {
      advance();
      skip_newlines();
      if(current() == token_type::int_or_ident)
        advance();
      skip_newlines();
      if(current() == token_type::comma)
        advance();
      skip_newlines();
      if(current() == token_type::int_or_ident)
        advance();
      skip_newlines();
      continue;
    }

    if(is_current_identifier("STRINGTABLE"))
    {
      resource res;
      res.type = next_val();
      parse_stringtable_resource(res);
      file.resources.push_back(res);
      skip_newlines();
      continue;
    }

    if(current() == token_type::int_or_ident)
    {
      resource res = parse_resource();
      file.resources.push_back(res);
      skip_newlines();
    }
    else
    {
      advance();
    }
  }

  return file;
}

}
