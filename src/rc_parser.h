#ifndef RC_PARSER_H
#define RC_PARSER_H

#include "rc_ast.h"
#include "rc_tokenizer.h"

#include <map>
#include <string>
#include <vector>

namespace rc
{

class parser
{
public:
  parser(const std::vector<token>& tokens);
  rc_file parse();

private:
  const token& current() const;
  const token& peek(size_t offset = 1) const;
  token advance();
  std::string next_val(void) { return advance().value; }
  int16_t next16(void);
  uint16_t nextu16(void);
  bool is_current_type_attribute(void) const;
  bool is_current_identifier(const std::string& identifier);

  bool match(token_type type);
  bool match_id(const std::string& value);
  void skip_newlines();

  resource parse_resource();
  void parse_simple_resource(resource& res);
  void parse_dialog_resource(resource& res);
  void parse_menu_resource(resource& res);
  void parse_toolbar_resource(resource& res);
  void parse_accelerator_resource(resource& res);
  void parse_stringtable_resource(resource& res);
  void parse_versioninfo_resource(resource& res);
  void parse_version_value_list(std::map<std::string, std::string>& out);
  void parse_rcdata_resource(resource& res);
  void parse_dlginit_resource(resource& res);
  void parse_designinfo_resource(resource& res);
  void parse_textinclude_resource(resource& res);
  void parse_typelib_resource(resource& res);
  void parse_unused_resource(resource& res);
  void parse_user_resource(resource& res);
  void decode_binary_resource(resource& res);

  style_expr parse_style_expr();
  control parse_control();
  void parse_menu_body(std::vector<menu_entry>& entries);
  popup* parse_popup();
  menu_item parse_menu_item();
  dialog_stmt parse_dialog_statement();
  void skip_begin_end();
  std::string parse_resource_id();

  const std::vector<token>& m_tokens;
  size_t m_pos = 0;
};

}

#endif // RC_PARSER_H
