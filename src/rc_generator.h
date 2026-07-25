#ifndef RC_GENERATOR_H
#define RC_GENERATOR_H

#include <cstdint>
#include <string>
#include <map>

#include <pugixml.hpp>

namespace rc
{

struct rc_file;
struct resource;
struct dialog_data;
struct control;
struct dialog_stmt;
struct style_expr;

class generator
{
public:
  bool generate(const rc_file& file, const std::string& output_path);

private:
  void write_dialog(pugi::xml_node& parent, const resource& res);
  void write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd);
  void write_control(pugi::xml_node& parent, const control& ctrl);

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

  int dlu_to_pixel_x(int dlu) const;
  int dlu_to_pixel_y(int dlu) const;

  bool has_style(const style_expr& style, const std::string& flag) const;

  std::string find_statement_text(const dialog_data& dd, const std::string& keyword) const;
  int find_statement_numeric(const dialog_data& dd, const std::string& keyword, int default_value = 0) const;

  std::map<std::string, int> name_counts_;
};

}

#endif
