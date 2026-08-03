#include "rc_parser.h"
#include "utils.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <ranges>
#include <map>
#include <functional>

namespace rc
{

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
  static const token eof_token{token_type::eof, "", 0};
  return eof_token;
}

token parser::advance()
{
  if(m_pos >= m_tokens.size())
    return {token_type::eof, "", 0};
  return m_tokens[m_pos++];
}

int16_t parser::next16(void)
{
  skip_newlines();
  return static_cast<int16_t>(safe_stoi(next_val()));
}

uint16_t parser::nextu16(void)
{
  return static_cast<uint16_t>(safe_stoi(next_val()));
}

static bool match_type(token_type needle, std::initializer_list<token_type> haystack)
{
  return std::ranges::any_of(haystack, [needle](auto token) { return token == needle; });
}

bool parser::is_current_type(token_type token)
  { return current().type == token; }

bool parser::is_current_type(std::initializer_list<token_type> haystack)
{
  return match_type(current().type, haystack);
}

static constexpr std::initializer_list<token_type> tt_II    = { token_type::identifier, token_type::integer_literal };
static constexpr std::initializer_list<token_type> tt_IIH   = { token_type::identifier, token_type::integer_literal,
                                                                token_type::hex_literal };
static constexpr std::initializer_list<token_type> tt_IIHS  = { token_type::identifier, token_type::integer_literal,
                                                                token_type::hex_literal, token_type::string_literal };
static constexpr std::initializer_list<token_type> tt_NEB   = { token_type::newline, token_type::eof, token_type::begin, };
static constexpr std::initializer_list<token_type> tt_NEE   = { token_type::newline, token_type::eof, token_type::end };


bool parser::is_current_string(const std::string& str)
  { return current().value == str; }

bool parser::is_current_string(std::initializer_list<std::string> haystack)
{
  return match_string(current().value, haystack);
}

bool parser::match(token_type type)
{
  if(current().type == type)
  {
    ++m_pos;
    return true;
  }
  return false;
}

bool parser::match_id(const std::string& value)
{
  if(is_current_type(token_type::identifier) && current().value == value)
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


bool parser::is_resource_type(const std::string& s)
{
  return match_string(s,
    { "ACCELERATORS", "BITMAP", "CURSOR", "DIALOG", "DIALOGEX",
      "FONT", "HTML", "ICON", "MENU", "MENUEX", "MESSAGETABLE",
      "POPUP", "RCDATA", "STRINGTABLE", "TOOLBAR", "VERSIONINFO",
      "TEXTINCLUDE", "REGISTRY", "DLGINIT", "GUIDELINES", "DESIGNINFO",
      "DLGINCLUDE" });
}

bool parser::is_attribute(const std::string& s)
{
  return match_string(s,
    { "PRELOAD", "LOADONCALL", "DISCARDABLE", "MOVEABLE", "PURE",
      "IMPURE", "SHARED", "NONSHARED", "FIXED" });
}

bool parser::is_control_keyword(const std::string& s)
{
  return match_string(s,
    { "AUTO3STATE", "AUTOCHECKBOX", "AUTORADIOBUTTON", "CHECKBOX",
      "COMBOBOX", "CONTROL", "CTEXT", "DEFPUSHBUTTON", "EDITTEXT",
      "GROUPBOX", "ICON", "LISTBOX", "LTEXT", "PUSHBOX", "PUSHBUTTON",
      "RADIOBUTTON", "RTEXT", "SCROLLBAR", "STATE3" });
}

bool parser::is_dialog_statement(const std::string& s)
{
  return match_string(s,
    { "CAPTION", "CLASS", "EXSTYLE", "FONT", "LANGUAGE",
      "STYLE", "VERSION", "CHARACTERISTICS", "MENU"});
}


std::string parser::parse_resource_id()
{
  return is_current_type(tt_IIHS) ? next_val() : "";
}

style_expr parser::parse_style_expr()
{
  style_expr expr;

  if(is_current_type(token_type::identifier) && is_current_string("NOT"))
  {
    advance();
    skip_newlines();
    std::string flag;
    if(is_current_type(tt_IIH))
      flag = next_val();
    if(!flag.empty())
      expr.not_flags.push_back(flag);
    return expr;
  }

  if(is_current_type(tt_IIH))
    expr.first = next_val();

  while(is_current_type(token_type::pipe) ||
        (is_current_type(token_type::identifier) &&
          is_current_string({"||","OR"})))
  {
    std::string op = next_val();
    skip_newlines();

    if(is_current_type(token_type::identifier) && is_current_string("NOT"))
    {
      advance();
      skip_newlines();
      std::string flag;
      if(is_current_type(tt_IIH))
        flag = next_val();
      if(!flag.empty())
        expr.not_flags.push_back(flag);
      skip_newlines();
      continue;
    }

    std::string val;
    if(is_current_type(tt_IIH))
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
    if(is_current_type(token_type::string_literal))
      ctrl.text = next_val();
    if(match(token_type::comma))
    {
      ctrl.id = parse_resource_id();
      if(match(token_type::comma))
      {
        if(is_current_type(token_type::string_literal))
          ctrl.class_name = next_val();
      }
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.style = parse_style_expr();
      }
      if(match(token_type::comma))
      {
        skip_newlines();
        ctrl.x = next16();
      }
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
    bool keyword_has_text = match_string(ctrl.keyword,
                                         {"PUSHBUTTON", "DEFPUSHBUTTON", "CHECKBOX", "AUTOCHECKBOX",
                                          "AUTO3STATE", "STATE3", "RADIOBUTTON", "AUTORADIOBUTTON",
                                          "LTEXT", "CTEXT", "RTEXT", "GROUPBOX", "ICON", "PUSHBOX"});

    if(keyword_has_text)
    {
      if(is_current_type(tt_IIHS))
        ctrl.text = next_val();
      if(match(token_type::comma))
        ctrl.id = parse_resource_id();
    }
    else
      ctrl.id = parse_resource_id();

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
  item.text = next_val();
  if(match(token_type::comma))
    item.id = parse_resource_id();
  while(match(token_type::comma))
  {
    if(is_current_type(token_type::identifier) &&
       !is_control_keyword(current().value) &&
       !is_current_string({"POPUP", "END"}) &&
       !is_current_type({token_type::eof, token_type::newline}))
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
    { return is_current_type(token_type::identifier) &&
            !is_current_string({ "BEGIN", "END" }) &&
            !is_current_type({token_type::eof, token_type::newline}); };

  auto* pp = new popup();
  advance();
  if(is_current_type(token_type::string_literal))
    pp->text = next_val();
  if(match(token_type::comma))
  {
    if(is_current_type(tt_IIH) && ! is_current_string({ "GRAYED", "INACTIVE", "CHECKED", "MENUBARBREAK", "MENUBREAK", "RIGHTBREAK", "MF_GRAYED", "MF_ENABLED", "MF_CHECKED", "MF_UNCHECKED", "MF_MENUBARBREAK", "MF_MENUBREAK", "MF_POPUP", "MF_SEPARATOR", "MF_STRING", "MF_HELP", "MF_END" }))
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
  while(skip_newlines(), !is_current_type({token_type::end, token_type::eof}))
  {
    if(to_upper(current().value) == "POPUP")
    {
      std::shared_ptr<popup> pp(parse_popup());
      menu_entry me;
      me.item = pp;
      entries.push_back(me);
    }
    else if(is_current_string("MENUITEM"))
    {
      advance();
      if(is_current_string("SEPARATOR"))
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
    if(is_current_type(token_type::string_literal))
      stmt.text_value = next_val();
  }
  else if(upper == "FONT")
  {
    if(is_current_type(token_type::integer_literal))
      stmt.numeric_value = nextu16();
    if(match(token_type::comma))
    {
      if(is_current_type(token_type::string_literal))
        stmt.text_value = next_val();
      if(match(token_type::comma))
      {
        if(is_current_type({token_type::identifier, token_type::integer_literal}))
          stmt.numeric_value2 = nextu16();
        if(match(token_type::comma))
        {
          if(is_current_type({token_type::identifier, token_type::integer_literal}))
          {
            stmt.italic = nextu16();
          }
        }
      }
    }
  }
  else if(upper == "CLASS")
  {
    if(is_current_type(token_type::string_literal))
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
  while(depth > 0 && current().type != token_type::eof)
  {
    if(is_current_type(token_type::begin))
      ++depth;
    else if(is_current_type(token_type::end))
      --depth;
    if(depth > 0)
      advance();
  }
}

void parser::parse_simple_resource(resource& res)
{
  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  if(is_current_type(token_type::string_literal))
    res.filename = next_val();
  else if(is_current_type(token_type::identifier) && !is_resource_type(current().value))
    res.filename = next_val();
}

void parser::parse_dialog_resource(resource& res)
{
  dialog_data dd;

  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  if(is_current_type({token_type::integer_literal, token_type::hex_literal}))
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
      if(is_current_type({token_type::end, token_type::eof}))
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

  while(is_current_type(token_type::identifier) && (is_attribute(current().value) || to_upper(current().value) == "EX"))
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

  while(is_current_type(token_type::identifier) && (is_attribute(current().value) || to_upper(current().value) == "EX"))
    res.attributes.push_back(next_val());

  if(is_current_type({token_type::integer_literal, token_type::hex_literal}))
    td.width = static_cast<uint16_t>(safe_stoi(next_val()));
  if(match(token_type::comma))
  {
    skip_newlines();
    td.height = static_cast<uint16_t>(safe_stoi(next_val()));
  }

  skip_newlines();
  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(is_current_type({token_type::end, token_type::eof}))
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

  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    while(current().type != token_type::end && current().type != token_type::eof)
    {
      skip_newlines();
      if(is_current_type({token_type::end, token_type::eof}))
        break;

      accelerator_entry ae;
      if(is_current_type(token_type::string_literal))
        ae.event = next_val();
      else       if(is_current_type({token_type::integer_literal, token_type::hex_literal}))
        ae.event = next_val();
      else if(is_current_type(token_type::identifier))
        ae.event = next_val();

      if(match(token_type::comma))
        ae.id = parse_resource_id();

      while(match(token_type::comma) || match(token_type::pipe))
      {
        if(is_current_type(token_type::identifier))
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

  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    while(skip_newlines(), !is_current_type({ token_type::end, token_type::eof}))
    {
      string_table_entry ste;
      ste.id = parse_resource_id();
      if(match(token_type::comma))
      {
        if(is_current_type(token_type::string_literal))
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

  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  while(current().type != token_type::begin && current().type != token_type::eof)
  {
    if(is_current_type(token_type::identifier))
    {
      std::string key = to_upper(next_val());
      std::string val;
      while(is_current_type({token_type::newline, token_type::eof, token_type::identifier}))
      {
        if(!val.empty())
          val += " ";
        val += next_val();
        if(is_current_type(token_type::comma))
        {
          val += next_val();
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
    size_t max_iters = m_tokens.size() * 4;
    size_t iters = 0;
    while(depth > 0 && !is_current_type(token_type::eof) && iters < max_iters)
    {
      ++iters;
      if(is_current_type(token_type::begin))
        ++depth;
      if(is_current_type(token_type::end))
        --depth;

      if(depth >= 2 && is_current_type(token_type::string_literal))
      {
        advance();
      }

      if(depth >= 2 && is_current_type(token_type::identifier) &&
          is_current_string("VALUE"))
      {
        advance();
        if(is_current_type({token_type::string_literal, token_type::identifier}))
        {
          std::string key = next_val();
          std::string val;
          if(match(token_type::comma))
          {
            skip_newlines();
            while(!is_current_type({token_type::newline, token_type::eof}))
            {
              if(!val.empty())
                val += " ";
              val += next_val();
              if(is_current_type(token_type::comma))
              {
                val += next_val();
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

  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  skip_newlines();
  if(match(token_type::begin))
  {
    while(!is_current_type({ token_type::eof, token_type::end }))
    {
      skip_newlines();
      if(is_current_type({ token_type::eof, token_type::end }))
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

  while(is_current_type(token_type::identifier) && is_attribute(current().value))
    res.attributes.push_back(next_val());

  skip_newlines();
  if(!match(token_type::begin))
  {
    res.data = entries;
    return;
  }

  while(!is_current_type({ token_type::eof, token_type::end }))
  {
    skip_newlines();
    if(is_current_type({ token_type::eof, token_type::end }))
      break;

    if(is_current_type({token_type::integer_literal, token_type::hex_literal}))
    {
      std::string num_str = current().value;
      bool is_zero = (num_str == "0");
      if(!is_zero && num_str.size() > 2 && num_str[0] == '0' &&
         (num_str[1] == 'x' || num_str[1] == 'X'))
        is_zero = (num_str == "0x0");

      if(is_zero)
      {
        advance();
        while(!is_current_type(tt_NEE))
          advance();
        continue;
      }
    }

    if(is_current_type(token_type::identifier))
    {
      dlginit_entry entry;
      entry.control_id = next_val();

      if(!match(token_type::comma))
      {
        while(!is_current_type(tt_NEE))
          advance();
        continue;
      }

      skip_newlines();

      if(is_current_type({token_type::hex_literal, token_type::integer_literal}))
      {
        std::string msg = current().value;
        if(msg.size() > 2 && msg[0] == '0' && (msg[1] == 'x' || msg[1] == 'X'))
          entry.message = static_cast<uint16_t>(safe_stoul(msg, 16));
        else
          entry.message = static_cast<uint16_t>(safe_stoul(msg));
        advance();
      }

      while(!is_current_type(tt_NEE))
        advance();

      skip_newlines();

      std::string text_data;
      while(!is_current_type(tt_NEE))
      {
        if(is_current_type(token_type::hex_literal))
        {
          std::string hex = current().value;
          unsigned val = safe_stoul(hex, 16);
          text_data += static_cast<char>(val & 0xFF);
          text_data += static_cast<char>((val >> 8) & 0xFF);
          advance();
          while(is_current_type(token_type::comma))
          {
            advance();
            skip_newlines();
            if(is_current_type(token_type::hex_literal))
            {
              std::string h2 = current().value;
              unsigned v2 = safe_stoul(h2, 16);
              text_data += static_cast<char>(v2 & 0xFF);
              text_data += static_cast<char>((v2 >> 8) & 0xFF);
              advance();
            }
            else if(is_current_type(token_type::string_literal))
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
        else if(is_current_type(token_type::string_literal))
        {
          std::string s = current().value;
          s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
          for(size_t i = 0; i < s.size(); ++i)
            text_data += s[i];
          advance();
        }
        else if(is_current_type(token_type::comma))
        {
          advance();
        }
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
  while(!is_current_type(tt_NEB))
    advance();

  skip_newlines();
  skip_begin_end();
}

resource parser::parse_resource()
{
  resource res;
  res.id = parse_resource_id();
  skip_newlines();

  if(is_current_type(token_type::identifier))
    res.type = next_val();

  std::map<std::string, void (parser::*)(resource&)> funcmap =
  {
    { "DIALOG", &parser::parse_dialog_resource },
    { "DIALOGEX", &parser::parse_dialogex_resource },
    { "MENU", &parser::parse_menu_resource },
    { "MENUEX", &parser::parse_menu_resource },
    { "TOOLBAR", &parser::parse_toolbar_resource },
    { "ACCELERATORS", &parser::parse_accelerator_resource },
    { "STRINGTABLE", &parser::parse_stringtable_resource },
    { "VERSIONINFO", &parser::parse_versioninfo_resource },
    { "RCDATA", &parser::parse_rcdata_resource },
    { "DLGINIT", &parser::parse_dlginit_resource },
    { "BITMAP", &parser::parse_simple_resource },
    { "ICON", &parser::parse_simple_resource },
    { "CURSOR", &parser::parse_simple_resource },
    { "FONT", &parser::parse_simple_resource },
    { "HTML", &parser::parse_simple_resource },
    { "MESSAGETABLE", &parser::parse_simple_resource },
    { "REGISTRY", &parser::parse_simple_resource },
    { "DLGINCLUDE", &parser::parse_simple_resource },
  };

  if(auto name = to_upper(res.type);
     funcmap.contains(name))
    std::bind_front(funcmap.at(name), this)(res);
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
    if(is_current_type(token_type::eof))
      break;

    if(is_current_type(token_type::identifier) && is_current_string("LANGUAGE"))
    {
      advance();
      skip_newlines();
      if(is_current_type(tt_II))
        advance();
      skip_newlines();
      if(is_current_type(token_type::comma))
        advance();
      skip_newlines();
      if(is_current_type(tt_II))
        advance();
      skip_newlines();
      continue;
    }

    if(is_current_type(token_type::identifier) &&
        is_current_string("STRINGTABLE"))
    {
      resource res;
      res.type = next_val();
      parse_stringtable_resource(res);
      file.resources.push_back(res);
      skip_newlines();
      continue;
    }

    if(is_current_type(tt_II))
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
