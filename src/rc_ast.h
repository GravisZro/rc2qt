#ifndef RC_AST_H
#define RC_AST_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace rc
{

struct style_expr
{
  std::string first;
  std::vector<std::pair<std::string, std::string>> ops;
  std::vector<std::string> not_flags;
  int64_t resolved_value = -1;
};

struct menu_item
{
  std::string text;
  std::string id;
  std::vector<std::string> flags;
};

struct popup;

struct menu_entry
{
  std::variant<menu_item, std::shared_ptr<popup>> item;
};

struct popup
{
  std::string text;
  std::string id;
  std::vector<std::string> flags;
  std::vector<menu_entry> entries;
};

struct control
{
  std::string keyword;
  std::string text;
  std::string id;
  std::string class_name;
  style_expr style;
  int16_t x = 0;
  int16_t y = 0;
  uint16_t width = 0;
  uint16_t height = 0;
  style_expr ext_style;
  std::vector<std::string> extra_styles;
};

struct dialog_stmt
{
  std::string keyword;
  style_expr value;
  std::string text_value;
  std::string id_value;
  uint16_t numeric_value = 0;
  uint16_t numeric_value2 = 0;
  std::string second_text;
};

struct dialog_data
{
  int16_t x = 0;
  int16_t y = 0;
  uint16_t width = 0;
  uint16_t height = 0;
  std::string help_id;
  std::vector<dialog_stmt> statements;
  std::vector<control> controls;
};

struct toolbar_entry
{
  bool is_separator = false;
  std::string id;
};

struct toolbar_data
{
  uint16_t width = 0;
  uint16_t height = 0;
  std::vector<toolbar_entry> entries;
};

struct accelerator_entry
{
  std::string event;
  std::string id;
  std::string type;
  std::vector<std::string> modifiers;
};

struct string_table_entry
{
  std::string id;
  std::string value;
};

struct version_info
{
  std::map<std::string, std::string> values;
};

struct menu_data
{
  std::vector<menu_entry> entries;
};

struct empty_data {};

struct dlginit_entry
{
  std::string control_id;
  uint16_t message = 0;
  std::string text;
};

struct resource
{
  std::string id;
  std::string type;
  std::vector<std::string> attributes;

  std::string filename;

  std::variant<
    empty_data,
    dialog_data,
    toolbar_data,
    menu_data,
    std::vector<accelerator_entry>,
    std::vector<string_table_entry>,
    version_info,
    std::vector<dlginit_entry>
  > data;
};

struct rc_file
{
  std::vector<resource> resources;
};

}

#endif // RC_AST_H
