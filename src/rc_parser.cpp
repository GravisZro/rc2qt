#include "rc_parser.h"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace rc
{

parser::parser(const std::vector<token>& t) : tokens(t) {}

const token& parser::current() const
{
  return tokens[pos];
}

const token& parser::peek(size_t offset) const
{
  size_t idx = pos + offset;
  return idx < tokens.size() ? tokens[idx] : tokens.back();
}

token parser::advance()
{
  return tokens[pos++];
}

bool parser::match(token_type type)
{
  if(current().type == type)
  {
    ++pos;
    return true;
  }
  return false;
}

bool parser::match_id(const std::string& value)
{
  if(current().type == token_type::identifier && current().value == value)
  {
    ++pos;
    return true;
  }
  return false;
}

bool parser::match_id_ci(const std::string& value)
{
  if(current().type == token_type::identifier && to_upper(current().value) == to_upper(value))
  {
    ++pos;
    return true;
  }
  return false;
}

bool parser::expect(token_type type)
{
  if(match(type))
    return true;
  std::cerr << "Line " << current().line << ": expected token type " << static_cast<int>(type)
            << " but got '" << current().value << "'" << std::endl;
  return false;
}

bool parser::expect_id(const std::string& value)
{
  if(match_id(value))
    return true;
  std::cerr << "Line " << current().line << ": expected '" << value
            << "' but got '" << current().value << "'" << std::endl;
  return false;
}

void parser::skip_newlines()
{
  while(match(token_type::newline))
    ;
}

std::string parser::to_upper(const std::string& s)
{
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

bool parser::is_resource_type(const std::string& s)
{
  static const std::string types[] = {
    "ACCELERATORS", "BITMAP", "CURSOR", "DIALOG", "DIALOGEX",
    "FONT", "HTML", "ICON", "MENU", "MENUEX", "MESSAGETABLE",
    "POPUP", "RCDATA", "STRINGTABLE", "TOOLBAR", "VERSIONINFO",
    "TEXTINCLUDE", "REGISTRY", "DLGINIT", "GUIDELINES", "DESIGNINFO",
    "DLGINCLUDE"
  };
  std::string upper = to_upper(s);
  for(const auto& t : types)
    if(upper == t)
      return true;
  return false;
}

bool parser::is_attribute(const std::string& s)
{
  static const std::string attrs[] = {
    "PRELOAD", "LOADONCALL", "DISCARDABLE", "MOVEABLE", "PURE",
    "IMPURE", "SHARED", "NONSHARED", "FIXED"
  };
  std::string upper = to_upper(s);
  for(const auto& a : attrs)
    if(upper == a)
      return true;
  return false;
}

bool parser::is_control_keyword(const std::string& s)
{
  static const std::string ctrls[] = {
    "AUTO3STATE", "AUTOCHECKBOX", "AUTORADIOBUTTON", "CHECKBOX",
    "COMBOBOX", "CONTROL", "CTEXT", "DEFPUSHBUTTON", "EDITTEXT",
    "GROUPBOX", "ICON", "LISTBOX", "LTEXT", "PUSHBOX", "PUSHBUTTON",
    "RADIOBUTTON", "RTEXT", "SCROLLBAR", "STATE3"
  };
  std::string upper = to_upper(s);
  for(const auto& c : ctrls)
    if(upper == c)
      return true;
  return false;
}

bool parser::is_dialog_statement(const std::string& s)
{
  static const std::string stmts[] = {
    "CAPTION", "CLASS", "EXSTYLE", "FONT", "LANGUAGE",
    "STYLE", "VERSION", "CHARACTERISTICS", "MENU"
  };
  std::string upper = to_upper(s);
  for(const auto& st : stmts)
    if(upper == st)
      return true;
  return false;
}

bool parser::is_known_id(const std::string& s)
{
  static const std::string known[] = {
    "CHECKED", "GRAYED", "HELP", "INACTIVE", "MENUBARBREAK", "MENUBREAK",
    "VIRTKEY", "ASCII", "ALT", "SHIFT", "CONTROL", "NOINVERT",
    "SEPARATOR", "BUTTON", "POPUP", "MENUITEM",
    "BEGIN", "END",
    "MFS_GRAYED", "MFS_CHECKED", "MFS_ENABLED", "MFS_DEFAULT",
    "MFS_BITMAP", "MFS_UNCHECKED", "MFS_UNHILITE", "MFS_HILITE",
    "MFT_SEPARATOR", "MFT_MENUBARBREAK", "MFT_MENUBREAK",
    "MFT_OWNERDRAW", "MFT_STRING", "MFT_BITMAP", "MFT_POPUP"
  };
  std::string upper = to_upper(s);
  for(const auto& k : known)
    if(upper == k)
      return true;
  return false;
}

std::string parser::parse_resource_id()
{
  if(current().type == token_type::identifier)
    return advance().value;
  if(current().type == token_type::integer_literal)
    return advance().value;
  if(current().type == token_type::hex_literal)
    return advance().value;
  if(current().type == token_type::string_literal)
    return advance().value;
  return "";
}

style_expr parser::parse_style_expr()
{
  style_expr expr;

  if(current().type == token_type::identifier && to_upper(current().value) == "NOT")
  {
    advance();
    skip_newlines();
    std::string flag;
    if(current().type == token_type::identifier)
      flag = advance().value;
    else if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
      flag = advance().value;
    if(!flag.empty())
      expr.not_flags.push_back(flag);
    return expr;
  }

  if(current().type == token_type::identifier)
    expr.first = advance().value;
  else if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
    expr.first = advance().value;

  while(current().type == token_type::pipe ||
        (current().type == token_type::identifier &&
         (to_upper(current().value) == "||" || to_upper(current().value) == "OR")))
  {
    std::string op = advance().value;
    skip_newlines();

    if(current().type == token_type::identifier && to_upper(current().value) == "NOT")
    {
      advance();
      skip_newlines();
      std::string flag;
      if(current().type == token_type::identifier)
        flag = advance().value;
      else if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
        flag = advance().value;
      if(!flag.empty())
        expr.not_flags.push_back(flag);
      skip_newlines();
      continue;
    }

    std::string val;
    if(current().type == token_type::identifier)
      val = advance().value;
    else if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
      val = advance().value;
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
  ctrl.keyword = advance().value;

  if(to_upper(ctrl.keyword) == "CONTROL")
  {
    if(current().type == token_type::string_literal)
      ctrl.text = advance().value;
    if(match(token_type::comma))
    {
      ctrl.id = parse_resource_id();
      if(match(token_type::comma))
      {
        if(current().type == token_type::string_literal)
          ctrl.class_name = advance().value;
      }
      if(match(token_type::comma))
        ctrl.style = parse_style_expr();
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.x = static_cast<int16_t>(std::stoi(advance().value));
      }
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.y = static_cast<int16_t>(std::stoi(advance().value));
      }
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.width = static_cast<uint16_t>(std::stoi(advance().value));
      }
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.height = static_cast<uint16_t>(std::stoi(advance().value));
      }
      if(match(token_type::comma))
        ctrl.ext_style = parse_style_expr();
    }
  }
  else
  {
    std::string kw_upper = to_upper(ctrl.keyword);
    bool keyword_has_text = (kw_upper == "PUSHBUTTON" || kw_upper == "DEFPUSHBUTTON" ||
                             kw_upper == "CHECKBOX" || kw_upper == "AUTOCHECKBOX" ||
                             kw_upper == "AUTO3STATE" || kw_upper == "STATE3" ||
                             kw_upper == "RADIOBUTTON" || kw_upper == "AUTORADIOBUTTON" ||
                             kw_upper == "LTEXT" || kw_upper == "CTEXT" || kw_upper == "RTEXT" ||
                             kw_upper == "GROUPBOX" || kw_upper == "ICON" || kw_upper == "PUSHBOX");

    if(keyword_has_text)
    {
      if(current().type == token_type::string_literal || current().type == token_type::identifier ||
         current().type == token_type::integer_literal || current().type == token_type::hex_literal)
        ctrl.text = advance().value;
      if(match(token_type::comma))
        ctrl.id = parse_resource_id();
    }
    else
    {
      ctrl.id = parse_resource_id();
    }
    if(match(token_type::comma))
    {
      skip_newlines();
      ctrl.x = static_cast<int16_t>(std::stoi(advance().value));
    }
    if(match(token_type::comma))
    {
      skip_newlines();
      ctrl.y = static_cast<int16_t>(std::stoi(advance().value));
    }
    if(match(token_type::comma))
    {
      skip_newlines();
      ctrl.width = static_cast<uint16_t>(std::stoi(advance().value));
    }
    if(match(token_type::comma))
    {
      skip_newlines();
      ctrl.height = static_cast<uint16_t>(std::stoi(advance().value));
    }
    if(match(token_type::comma))
    {
      skip_newlines();
      ctrl.style = parse_style_expr();
    }
    if(match(token_type::comma))
    {
      skip_newlines();
      ctrl.ext_style = parse_style_expr();
    }
  }

  return ctrl;
}

menu_item parser::parse_menu_item()
{
  menu_item item;
  item.text = advance().value;
  if(match(token_type::comma))
    item.id = parse_resource_id();
  while(match(token_type::comma))
  {
    if(current().type == token_type::identifier &&
       !is_control_keyword(current().value) &&
       to_upper(current().value) != "POPUP" &&
       to_upper(current().value) != "END" &&
       current().type != token_type::eof &&
       current().type != token_type::newline)
    {
      item.flags.push_back(advance().value);
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
  auto* pp = new popup();
  advance();
  if(current().type == token_type::string_literal)
    pp->text = advance().value;
  if(match(token_type::comma))
  {
    auto is_popup_flag = [](const std::string& s) -> bool {
      auto u = s;
      for(auto& c : u) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
      return u == "GRAYED" || u == "INACTIVE" || u == "CHECKED" ||
             u == "MENUBARBREAK" || u == "MENUBREAK" || u == "RIGHTBREAK";
    };

    if(current().type == token_type::integer_literal ||
       current().type == token_type::hex_literal ||
       (current().type == token_type::identifier && !is_popup_flag(current().value)))
    {
      pp->id = advance().value;
    }
    while(current().type == token_type::identifier &&
          to_upper(current().value) != "BEGIN" &&
          to_upper(current().value) != "END" &&
          current().type != token_type::newline &&
          current().type != token_type::eof)
    {
      pp->flags.push_back(advance().value);
    }
  }
  while(current().type == token_type::identifier &&
        to_upper(current().value) != "BEGIN" &&
        to_upper(current().value) != "END" &&
        current().type != token_type::newline &&
        current().type != token_type::eof)
  {
    pp->flags.push_back(advance().value);
  }
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
  while(current().type != token_type::end && current().type != token_type::eof)
  {
    skip_newlines();
    if(current().type == token_type::end || current().type == token_type::eof)
      break;

    if(to_upper(current().value) == "POPUP")
    {
      std::shared_ptr<popup> pp(parse_popup());
      menu_entry me;
      me.item = pp;
      entries.push_back(me);
    }
    else if(to_upper(current().value) == "MENUITEM")
    {
      advance();
      if(to_upper(current().value) == "SEPARATOR")
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
  stmt.keyword = advance().value;
  std::string upper = to_upper(stmt.keyword);

  if(upper == "STYLE" || upper == "EXSTYLE")
  {
    stmt.value = parse_style_expr();
  }
  else if(upper == "CAPTION")
  {
    if(current().type == token_type::string_literal)
      stmt.text_value = advance().value;
  }
  else if(upper == "FONT")
  {
    if(current().type == token_type::integer_literal)
      stmt.numeric_value = static_cast<uint16_t>(std::stoi(advance().value));
    if(match(token_type::comma))
    {
      if(current().type == token_type::string_literal)
        stmt.text_value = advance().value;
      if(match(token_type::comma))
      {
        if(current().type == token_type::identifier || current().type == token_type::integer_literal)
          stmt.numeric_value2 = static_cast<uint16_t>(std::stoi(advance().value, nullptr, 0));
      }
    }
  }
  else if(upper == "CLASS")
  {
    if(current().type == token_type::string_literal)
      stmt.text_value = advance().value;
    else
      stmt.text_value = parse_resource_id();
  }
  else if(upper == "LANGUAGE")
  {
    stmt.text_value = parse_resource_id();
    if(match(token_type::comma))
      stmt.second_text = parse_resource_id();
  }
  else if(upper == "VERSION")
  {
    stmt.value = parse_style_expr();
  }
  else if(upper == "CHARACTERISTICS")
  {
    stmt.value = parse_style_expr();
  }
  else if(upper == "MENU")
  {
    stmt.id_value = parse_resource_id();
  }

  return stmt;
}

void parser::parse_dialog_body(dialog_data& dd)
{
  while(current().type != token_type::end && current().type != token_type::eof)
  {
    skip_newlines();
    if(current().type == token_type::end || current().type == token_type::eof)
      break;

    if(to_upper(current().value) == "MENUITEM" && peek().type == token_type::identifier &&
       to_upper(peek().value) == "SEPARATOR")
    {
      advance();
      advance();
    }
    else if(is_dialog_statement(current().value))
    {
      dd.statements.push_back(parse_dialog_statement());
    }
    else if(is_control_keyword(current().value))
    {
      dd.controls.push_back(parse_control());
    }
    else
    {
      advance();
    }
    skip_newlines();
  }
}

void parser::skip_begin_end()
{
  if(!match(token_type::begin))
    return;
  int depth = 1;
  while(depth > 0 && current().type != token_type::eof)
  {
    if(current().type == token_type::begin)
      ++depth;
    if(current().type == token_type::end)
      --depth;
    advance();
  }
}

void parser::parse_simple_resource(resource& res)
{
  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  if(current().type == token_type::string_literal)
    res.filename = advance().value;
  else if(current().type == token_type::identifier && !is_resource_type(current().value))
    res.filename = advance().value;

  if(to_upper(res.type) == "TOOLBAR" && match(token_type::comma))
  {
    // toolbar width, height before BEGIN
  }
}

void parser::parse_dialog_resource(resource& res)
{
  dialog_data dd;

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
    dd.x = static_cast<int16_t>(std::stoi(advance().value));
  if(match(token_type::comma))
  {
    skip_newlines();
    dd.y = static_cast<int16_t>(std::stoi(advance().value));
  }
  if(match(token_type::comma))
  {
    skip_newlines();
    dd.width = static_cast<uint16_t>(std::stoi(advance().value));
  }
  if(match(token_type::comma))
  {
    skip_newlines();
    dd.height = static_cast<uint16_t>(std::stoi(advance().value));
  }

  if(to_upper(res.type) == "DIALOGEX")
  {
    if(match(token_type::comma))
      dd.help_id = parse_resource_id();
  }

  skip_newlines();

  while(current().type != token_type::begin && current().type != token_type::eof)
  {
    if(is_dialog_statement(current().value))
      dd.statements.push_back(parse_dialog_statement());
    else
      advance();
    skip_newlines();
  }

  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(current().type == token_type::end || current().type == token_type::eof)
        break;
      if(is_control_keyword(current().value))
        dd.controls.push_back(parse_control());
      else
        advance();
      skip_newlines();
    }
    match(token_type::end);
  }

  res.data = dd;
}

void parser::parse_dialogex_resource(resource& res)
{
  parse_dialog_resource(res);
}

void parser::parse_menu_resource(resource& res)
{
  menu_data md;

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

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

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
    td.width = static_cast<uint16_t>(std::stoi(advance().value));
  if(match(token_type::comma))
  {
    skip_newlines();
    td.height = static_cast<uint16_t>(std::stoi(advance().value));
  }

  skip_newlines();
  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(current().type == token_type::end || current().type == token_type::eof)
        break;

      if(to_upper(current().value) == "SEPARATOR")
      {
        advance();
        toolbar_entry te;
        te.is_separator = true;
        td.entries.push_back(te);
      }
      else if(to_upper(current().value) == "BUTTON")
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

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  skip_newlines();
  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(current().type == token_type::end || current().type == token_type::eof)
        break;

      accelerator_entry ae;
      if(current().type == token_type::string_literal)
        ae.event = advance().value;
      else if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
        ae.event = advance().value;
      else if(current().type == token_type::identifier)
        ae.event = advance().value;

      if(match(token_type::comma))
        ae.id = parse_resource_id();

      while(match(token_type::comma))
      {
        if(current().type == token_type::identifier)
          ae.modifiers.push_back(advance().value);
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

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  skip_newlines();
  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(current().type == token_type::end || current().type == token_type::eof)
        break;

      string_table_entry ste;
      ste.id = parse_resource_id();
      if(match(token_type::comma))
      {
        if(current().type == token_type::string_literal)
          ste.value = advance().value;
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

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  while(current().type != token_type::begin && current().type != token_type::eof)
  {
    if(current().type == token_type::identifier)
    {
      std::string key = to_upper(advance().value);
      std::string val;
      while(current().type != token_type::newline &&
            current().type != token_type::eof &&
            current().type != token_type::identifier)
      {
        if(!val.empty())
          val += " ";
        val += advance().value;
        if(current().type == token_type::comma)
        {
          val += advance().value;
          if(!val.empty())
            val += " ";
        }
      }
      if(!val.empty())
        vi.values[key] = val;
    }
    else
    {
      advance();
    }
  }

  skip_newlines();
  if(match(token_type::begin))
  {
    int depth = 1;
    while(depth > 0 && current().type != token_type::eof)
    {
      if(current().type == token_type::begin)
        ++depth;
      if(current().type == token_type::end)
        --depth;

      if(depth >= 2 && current().type == token_type::string_literal)
      {
        std::string block_name = advance().value;
        (void)block_name;
      }

      if(depth >= 2 && current().type == token_type::identifier &&
         to_upper(current().value) == "VALUE")
      {
        advance();
        if(current().type == token_type::string_literal)
        {
          std::string key = advance().value;
          std::string val;
          if(match(token_type::comma))
          {
            skip_newlines();
            while(current().type != token_type::newline &&
                  current().type != token_type::eof)
            {
              if(!val.empty())
                val += " ";
              val += advance().value;
              if(current().type == token_type::comma)
              {
                val += advance().value;
                if(!val.empty())
                  val += " ";
              }
            }
          }
          std::string full_key = key;
          vi.values[full_key] = val;
        }
      }

      if(depth > 0)
        advance();
    }
    match(token_type::end);
  }

  res.data = vi;
}

void parser::parse_rcdata_resource(resource& res)
{
  version_info vi;

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  skip_newlines();
  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(current().type == token_type::end || current().type == token_type::eof)
        break;
      advance();
    }
    match(token_type::end);
  }

  res.data = vi;
}

void parser::parse_dlginit_resource(resource& res)
{
  std::vector<dlginit_entry> entries;

  while(current().type == token_type::identifier && is_attribute(current().value))
    res.attributes.push_back(advance().value);

  skip_newlines();
  if(!match(token_type::begin))
  {
    res.data = entries;
    return;
  }

  while(current().type != token_type::end && current().type != token_type::eof)
  {
    skip_newlines();
    if(current().type == token_type::end || current().type == token_type::eof)
      break;

    if(current().type == token_type::integer_literal || current().type == token_type::hex_literal)
    {
      std::string num_str = current().value;
      bool is_zero = (num_str == "0");
      if(!is_zero && num_str.size() > 2 && num_str[0] == '0' &&
         (num_str[1] == 'x' || num_str[1] == 'X'))
        is_zero = (num_str == "0x0");

      if(is_zero)
      {
        advance();
        while(current().type != token_type::newline && current().type != token_type::end &&
              current().type != token_type::eof)
          advance();
        continue;
      }
    }

    if(current().type == token_type::identifier)
    {
      dlginit_entry entry;
      entry.control_id = advance().value;

      if(!match(token_type::comma))
      {
        while(current().type != token_type::newline && current().type != token_type::end &&
              current().type != token_type::eof)
          advance();
        continue;
      }

      skip_newlines();

      if(current().type == token_type::hex_literal || current().type == token_type::integer_literal)
      {
        std::string msg = current().value;
        if(msg.size() > 2 && msg[0] == '0' && (msg[1] == 'x' || msg[1] == 'X'))
          entry.message = static_cast<uint16_t>(std::stoul(msg, nullptr, 16));
        else
          entry.message = static_cast<uint16_t>(std::stoul(msg, nullptr, 0));
        advance();
      }

      while(current().type != token_type::newline && current().type != token_type::end &&
            current().type != token_type::eof)
        advance();

      skip_newlines();

      std::string text_data;
      while(current().type != token_type::end && current().type != token_type::eof &&
            current().type != token_type::identifier)
      {
        if(current().type == token_type::hex_literal)
        {
          std::string hex = current().value;
          unsigned val = std::stoul(hex, nullptr, 16);
          text_data += static_cast<char>(val & 0xFF);
          text_data += static_cast<char>((val >> 8) & 0xFF);
          advance();
          while(current().type == token_type::comma)
          {
            advance();
            skip_newlines();
            if(current().type == token_type::hex_literal)
            {
              std::string h2 = current().value;
              unsigned v2 = std::stoul(h2, nullptr, 16);
              text_data += static_cast<char>(v2 & 0xFF);
              text_data += static_cast<char>((v2 >> 8) & 0xFF);
              advance();
            }
            else if(current().type == token_type::string_literal)
            {
              std::string s2 = current().value;
              s2.erase(std::remove(s2.begin(), s2.end(), '"'), s2.end());
              for(size_t j = 0; j < s2.size(); ++j)
                text_data += s2[j];
              advance();
            }
            else
              break;
          }
        }
        else if(current().type == token_type::string_literal)
        {
          std::string s = current().value;
          s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
          for(size_t i = 0; i < s.size(); ++i)
            text_data += s[i];
          advance();
        }
        else if(current().type == token_type::comma)
        {
          advance();
        }
        else
          break;
      }

      size_t last = text_data.find_last_not_of('\0');
      if(last != std::string::npos)
        text_data = text_data.substr(0, last + 1);
      else if(!text_data.empty())
        text_data = text_data.substr(0, text_data.size());

      entry.text = text_data;
      entries.push_back(entry);
    }
    else
    {
      while(current().type != token_type::newline && current().type != token_type::end &&
            current().type != token_type::eof)
        advance();
    }
  }

  match(token_type::end);
  res.data = entries;
}

void parser::parse_unknown_resource(resource& res)
{
  while(current().type != token_type::newline && current().type != token_type::begin &&
        current().type != token_type::eof)
    advance();

  skip_newlines();
  skip_begin_end();
}

resource parser::parse_resource()
{
  resource res;
  res.id = parse_resource_id();
  skip_newlines();

  if(current().type == token_type::identifier)
    res.type = advance().value;

  std::string upper = to_upper(res.type);

  if(upper == "DIALOG")
    parse_dialog_resource(res);
  else if(upper == "DIALOGEX")
    parse_dialogex_resource(res);
  else if(upper == "MENU" || upper == "MENUEX")
    parse_menu_resource(res);
  else if(upper == "TOOLBAR")
    parse_toolbar_resource(res);
  else if(upper == "ACCELERATORS")
    parse_accelerator_resource(res);
  else if(upper == "STRINGTABLE")
    parse_stringtable_resource(res);
  else if(upper == "VERSIONINFO")
    parse_versioninfo_resource(res);
  else if(upper == "RCDATA")
    parse_rcdata_resource(res);
  else if(upper == "DLGINIT")
    parse_dlginit_resource(res);
  else if(upper == "BITMAP" || upper == "ICON" || upper == "CURSOR" ||
          upper == "FONT" || upper == "HTML" || upper == "MESSAGETABLE" ||
          upper == "REGISTRY" || upper == "DLGINCLUDE")
    parse_simple_resource(res);
  else
    parse_unknown_resource(res);

  return res;
}

rc_file parser::parse()
{
  rc_file file;

  while(current().type != token_type::eof)
  {
    skip_newlines();
    if(current().type == token_type::eof)
      break;

    if(current().type == token_type::identifier && to_upper(current().value) == "LANGUAGE")
    {
      advance();
      skip_newlines();
      if(current().type == token_type::integer_literal || current().type == token_type::identifier)
        advance();
      skip_newlines();
      if(current().type == token_type::comma)
        advance();
      skip_newlines();
      if(current().type == token_type::integer_literal || current().type == token_type::identifier)
        advance();
      skip_newlines();
      continue;
    }

    if(current().type == token_type::identifier &&
       to_upper(current().value) == "STRINGTABLE")
    {
      resource res;
      res.type = advance().value;
      parse_stringtable_resource(res);
      file.resources.push_back(res);
      skip_newlines();
      continue;
    }

    if(current().type == token_type::identifier ||
       current().type == token_type::integer_literal)
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
