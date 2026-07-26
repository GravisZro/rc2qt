#ifndef RC_GENERATOR_H
#define RC_GENERATOR_H

#include <cstdint>
#include <string>
#include <map>
#include <vector>

#include <pugixml.hpp>

namespace rc
{

struct rc_file;
struct resource;
struct dialog_data;
struct control;
struct dialog_stmt;
struct style_expr;
struct menu_data;
struct menu_entry;
struct popup;
struct menu_item;
struct toolbar_data;
struct accelerator_entry;
struct string_table_entry;

class generator
{
public:
  bool generate(const rc_file& file, const std::string& output_path);
  bool generate_all(const rc_file& file, const std::string& output_dir, const std::string& rc_basename);
  bool generate_qrc(const rc_file& file, const std::string& output_path, const std::vector<std::string>& ui_paths = {});
  bool generate_qrc(const rc_file& file, const std::string& output_path, const std::string& ui_path = "");

private:
  void collect_global_data(const rc_file& file);
  bool generate_single_dialog(const rc_file& file, const resource& res, const std::string& output_path);

  void write_dialog(pugi::xml_node& parent, const resource& res);
  void write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd);
  void write_control(pugi::xml_node& parent, const control& ctrl);
  void write_menu(pugi::xml_node& parent, const resource& res);
  void write_menu_entries(pugi::xml_node& menu_node, const std::vector<menu_entry>& entries, int& action_counter);
  void write_toolbar(pugi::xml_node& parent, const resource& res);
  void write_actions(pugi::xml_node& parent, const rc_file& file);

  pugi::xml_node add_widget(pugi::xml_node& parent, const std::string& qt_class, const std::string& name);
  void add_property_rect(pugi::xml_node& widget, int x, int y, int width, int height);
  void add_property_string(pugi::xml_node& widget, const std::string& name, const std::string& value);
  void add_property_bool(pugi::xml_node& widget, const std::string& name, bool value);
  void add_property_int(pugi::xml_node& widget, const std::string& name, int value);
  void add_property_set(pugi::xml_node& widget, const std::string& name, const std::string& value);
  void add_property_enum(pugi::xml_node& widget, const std::string& name, const std::string& value);
  void add_property_font(pugi::xml_node& widget, const std::string& family, int pointsize, bool bold, bool italic);

  std::string map_keyword_to_widget(const std::string& keyword);
  std::string map_class_to_widget(const std::string& class_name, const style_expr& style);
  std::string unique_name(const std::string& id);
  std::string map_vk_to_qt(const std::string& vk_code);

  int dlu_to_pixel_x(int dlu) const;
  int dlu_to_pixel_y(int dlu) const;
  void set_dlu_factors(const std::string& font_family, int font_size);

  bool has_style(const style_expr& style, const std::string& flag) const;

  std::string find_statement_text(const dialog_data& dd, const std::string& keyword) const;
  int find_statement_numeric(const dialog_data& dd, const std::string& keyword, int default_value = 0) const;
  int find_statement_numeric2(const dialog_data& dd, const std::string& keyword, int default_value = 0) const;
  const style_expr* find_statement_style(const dialog_data& dd, const std::string& keyword) const;
  bool has_dialog_flag(const dialog_data& dd, const std::string& keyword, const std::string& flag) const;

  std::string strip_accelerator(const std::string& text) const;
  std::string map_ms_font(const std::string& ms_font);

  std::map<std::string, int> name_counts_;
  std::map<std::string, std::string> accelerator_map_;
  std::map<std::string, std::string> string_table_map_;
  std::map<std::string, std::string> menu_text_map_;
  std::map<std::string, bool> menu_disabled_map_;
  std::map<std::string, bool> menu_checked_map_;
  std::map<std::string, std::vector<std::string>> dlginit_map_;
  std::map<std::string, const dialog_data*> ds_control_dialogs_;
  pugi::xml_node menubar_node_;
  double dlu_x_factor_ = 1.75;
  double dlu_y_factor_ = 1.75;
};

}

#endif
