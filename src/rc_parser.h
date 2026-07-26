#ifndef RC_PARSER_H
#define RC_PARSER_H

#include "rc_ast.h"
#include "rc_tokenizer.h"

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
  const std::vector<token>& tokens;
  size_t pos = 0;

  const token& current() const;
  const token& peek(size_t offset = 1) const;
  token advance();
  bool match(token_type type);
  bool match_id(const std::string& value);
  bool match_id_ci(const std::string& value);
  void skip_newlines();
  std::string to_upper(const std::string& s);

  bool is_resource_type(const std::string& s);
  bool is_attribute(const std::string& s);
  bool is_control_keyword(const std::string& s);
  bool is_dialog_statement(const std::string& s);

  resource parse_resource();
  void parse_simple_resource(resource& res);
  void parse_dialog_resource(resource& res);
  void parse_dialogex_resource(resource& res);
  void parse_menu_resource(resource& res);
  void parse_toolbar_resource(resource& res);
  void parse_accelerator_resource(resource& res);
  void parse_stringtable_resource(resource& res);
  void parse_versioninfo_resource(resource& res);
  void parse_rcdata_resource(resource& res);
  void parse_dlginit_resource(resource& res);
  void parse_unknown_resource(resource& res);

  style_expr parse_style_expr();
  control parse_control();
  void parse_menu_body(std::vector<menu_entry>& entries);
  popup* parse_popup();
  menu_item parse_menu_item();
  dialog_stmt parse_dialog_statement();
  void skip_begin_end();
  std::string parse_resource_id();
};

}

#endif // RC_PARSER_H
