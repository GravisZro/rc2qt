#ifndef RC_GENERATOR_H
#define RC_GENERATOR_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <filesystem>

#include <pugixml.hpp>

#ifdef HAVE_QT
# include <QFont>
#else
# include <ft2build.h>
# include <freetype/freetype.h>
#endif

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
  generator(void);
  ~generator(void);
  bool generate_all(const rc_file& file, const std::string& output_dir, const std::string& res_dir_name);
  bool generate_qrc(const rc_file& file, const std::string& output_path, const std::vector<std::string>& ui_paths = {});

private:
  struct text_fit_info
  {
    int width_dlu = 0;
    int height_dlu = 0;
    bool word_wrap = false;
  };

  struct control_layout
  {
    int y_shift_px = 0;
    int height_px = 0;
  };

  void collect_global_data(const rc_file& file);

  void write_dialog(pugi::xml_node& parent, const resource& res);
  void setup_dialog_font(const dialog_data& dd);
  void write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd, int extra_height = 0);
  void write_control(pugi::xml_node& parent, const control& ctrl, const std::string& dialog_name, int y_shift_px = 0);
  void apply_combo_dropdown_height(pugi::xml_node& widget, const control& ctrl, bool is_combo, int& height_px);

  text_fit_info fit_text(const std::string& text, int width_dlu, int height_dlu, const std::string& widget_class);
  static int min_height_px(const std::string& qt_class);
  void layout_control_sizes(const std::vector<control>& controls,
                            const std::vector<std::string>& qt_classes,
                            std::vector<control_layout>& layout);

  std::set<std::string> id_words(const std::string& id) const;
  bool share_common_word(const std::string& id1, const std::string& id2) const;
  void write_menu(pugi::xml_node& parent, const resource& res);
  void write_menu_entries(pugi::xml_node& menu_node, const std::vector<menu_entry>& entries);
  void write_toolbar(pugi::xml_node& parent, const resource& res);
  void write_actions(pugi::xml_node& parent, const rc_file& file);

  std::string unique_name(const std::string& id);
  std::string map_vk_to_qt(const std::string& vk_code);

  int dlu_to_pixel_x(int dlu) const
    { return static_cast<int>(dlu * m_dlu_x_factor); }

  int dlu_to_pixel_y(int dlu) const
    { return static_cast<int>(dlu * m_dlu_y_factor); }

  std::pair<int, int> text_dimensions(const std::string& text);

  std::vector<std::string> wrap_text(const std::string& text, int width_dlu);

  void ensure_text_fits(const std::string& text, int& width_dlu, int& height_dlu,
                        pugi::xml_node& widget, const std::string& widget_class);

  void set_current_font(const std::string& font_name, int font_size, int weight, bool italic);

  const dialog_stmt* find_statement(const dialog_data& dd, const std::string& keyword) const;
  bool has_dialog_flag(const dialog_data& dd, const std::string& keyword, const std::string& flag) const;

  std::string strip_accelerator(const std::string& text) const;
  std::string substitute_font(const std::string& font_name);
  bool load_font_substitution_list(const std::filesystem::path filepath);

  std::map<std::string, int> m_name_counts;
  std::map<std::string, std::string> m_accelerator_map;
  std::map<std::string, std::string> m_string_table_map;
  std::map<std::string, std::string> m_menu_text_map;
  std::map<std::string, bool> m_menu_disabled_map;
  std::map<std::string, bool> m_menu_checked_map;
  std::map<std::string, std::vector<std::string>> m_dlginit_map;
  std::map<std::string, const dialog_data*> m_ds_control_dialogs;
  std::map<std::string, std::string> m_substitute_fonts;
  pugi::xml_node m_menubar_node;

  double m_dlu_x_factor = 1.75;
  double m_dlu_y_factor = 1.75;
  int m_action_counter = 0;

  std::string m_original_font_name;
  std::string m_mapped_font_name;
  int m_font_size = 8;
  int m_font_weight = -1;
  bool m_font_italic = false;

  #ifdef HAVE_QT
  QFont m_current_font;
  #else
  FT_Library m_ft_library = nullptr;
  FT_Face m_ft_face = nullptr;
  #endif
};

}

#endif
