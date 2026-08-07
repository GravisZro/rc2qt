#include "rc_generator.h"
#include "rc_ast.h"
#include "rc_constants.h"
#include "utils.h"
#include "xmlhelpers.h"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <cstring>
#include <format>

namespace rc
{
  using namespace xml;

  static pugi::xml_node add_tag(pugi::xml_node parent, const char* const tag_name, const std::string& name)
  {
    pugi::xml_node child = parent.append_child(tag_name);
    child.append_attribute("name").set_value(name.c_str());
    return child;
  }

  static pugi::xml_node add_widget(pugi::xml_node& parent, const std::string& qt_class, const std::string& name)
  {
    pugi::xml_node widget = parent.append_child("widget");
    set_attr(widget, "class", qt_class);
    set_attr(widget, "name", name);
    return widget;
  }

  static void add_property_rect(pugi::xml_node& widget, int x, int y, int width, int height)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", "geometry");
    pugi::xml_node rect = prop.append_child("rect");
    rect.append_child("x").text() = x;
    rect.append_child("y").text() = y;
    rect.append_child("width").text() = width;
    rect.append_child("height").text() = height;
  }

  static void add_property_string(pugi::xml_node& widget, const std::string& name, const std::string& value)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", name);
    pugi::xml_node str_node = prop.append_child("string");
    std::string safe_value;
    for(char c : value)
    {
      if(c == '\0')
        safe_value += "\\0";
      else
        safe_value += c;
    }
    str_node.text() = safe_value.c_str();
  }

  static void add_property_bool(pugi::xml_node& widget, const std::string& name, bool value)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", name);
    prop.append_child("bool").text() = value ? "true" : "false";
  }

  static void add_property_int(pugi::xml_node& widget, const std::string& name, int value)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", name);
    prop.append_child("number").text() = value;
  }

  static void add_property_size(pugi::xml_node& widget, const std::string& name, int width, int height)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", name);
    pugi::xml_node size = prop.append_child("size");
    size.append_child("width").text() = width;
    size.append_child("height").text() = height;
  }

  static void add_property_set(pugi::xml_node& widget, const std::string& name, const std::string& value)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", name);
    prop.append_child("set").text() = value.c_str();
  }

  static void add_property_enum(pugi::xml_node& widget, const std::string& name, const std::string& value)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", name);
    prop.append_child("enum").text() = value.c_str();
  }

  static void add_property_font(pugi::xml_node& widget, const std::string& family, int pointsize, bool bold, bool italic)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", "font");
    pugi::xml_node font = prop.append_child("font");
    font.append_child("family").text() = family.c_str();
    font.append_child("pointsize").text() = pointsize;
    font.append_child("bold").text() = bold ? "true" : "false";
    font.append_child("italic").text() = italic ? "true" : "false";
  }

  static std::string map_keyword_to_widget(const std::string& keyword)
  {
    static const std::map<std::string, std::string> keyword_map =
    {
      { "PUSHBUTTON", "QPushButton" },
      { "DEFPUSHBUTTON", "QPushButton" },
      { "CHECKBOX", "QCheckBox" },
      { "AUTOCHECKBOX", "QCheckBox" },
      { "AUTO3STATE", "QCheckBox" },
      { "STATE3", "QCheckBox" },
      { "RADIOBUTTON", "QRadioButton" },
      { "AUTORADIOBUTTON", "QRadioButton" },
      { "GROUPBOX", "QGroupBox" },
      { "LTEXT", "QLabel" },
      { "CTEXT", "QLabel" },
      { "RTEXT", "QLabel" },
      { "ICON", "QLabel" },
      { "EDITTEXT", "QLineEdit" },
      { "LISTBOX", "QListWidget" },
      { "COMBOBOX", "QComboBox" },
      { "SCROLLBAR", "QScrollBar" },
      { "PUSHBOX", "QPushButton" },
    };

    auto it = keyword_map.find(keyword);
    if (it == keyword_map.end())
      return "";
    return it->second;
  }

  static bool has_style(const style_expr& style, const std::string& flag)
  {
    for(const auto& nf : style.not_flags)
    {
      if(nf == flag)
        return false;
    }

    if(style.first == flag)
      return true;

    for(const auto& [op, name] : style.ops)
    {
      if(name == flag)
        return true;
    }

    if(style.resolved_value >= 0)
    {
      const auto& reg = constant_registry::instance();
      int64_t val = reg.resolve(flag);
      if(val >= 0 && (style.resolved_value & val) == val)
        return true;
    }

    return false;
  }


  static std::string map_class_to_widget(const std::string& class_name, const style_expr& style)
  {
    std::string upper = to_upper(class_name);

    /* Ordinal aliases for the standard control classes delegate to the
       same logic as their named counterparts. */
    if (upper == "#128")
      upper = "BUTTON";
    else if (upper == "#129")
      upper = "EDIT";

    static const std::map<std::string, std::string> class_map =
    {
      { "STATIC", "QLabel" },
      { "LISTBOX", "QListWidget" },
      { "COMBOBOX", "QComboBox" },
      { "COMBOBOXEX32", "QComboBox" },
      { "SCROLLBAR", "QScrollBar" },
      { "SYSTABCONTROL32", "QTabWidget" },
      { "SYSTREEVIEW32", "QTreeWidget" },
      { "SYSLISTVIEW32", "QTableWidget" },
      { "MSCTLS_PROGRESS32", "QProgressBar" },
      { "MSCTLS_TRACKBAR32", "QSlider" },
      { "MSCTLS_UPDOWN32", "QSpinBox" },
      { "SYSDATETIMEPICK32", "QDateTimeEdit" },
      { "SYSMONTHCAL32", "QCalendarWidget" },
      { "SYSLINK", "QLabel" },
      { "TOOLBARWINDOW32", "QToolBar" },
      { "REBARWINDOW32", "QToolBar" },
      { "TOOLTIPS_CLASS32", "QWidget" },
      { "#130", "QLabel" },
      { "#131", "QListWidget" },
      { "#132", "QScrollBar" },
      { "#133", "QComboBox" },
      { "#32774", "QPushButton" },
      { "#32768", "QWidget" },
      { "SYSANIMATE32", "QLabel" },
      { "SYSPAGER", "QStackedWidget" },
      { "RICHEDIT", "QTextEdit" },
      { "RICHEDIT20A", "QTextEdit" },
      { "RICHEDIT20W", "QTextEdit" },
      { "RICHEDIT50W", "QTextEdit" },
      { "MSCTLS_STATUSBAR32", "QStatusBar" },
      { "SYSHEADER32", "QHeaderView" },
      { "MSCTLS_HOTKEY32", "QWidget" },
      { "NATIVEFONTCTL", "QWidget" },
    };

    /* The BUTTON and EDIT classes depend on their style flags. */
    if (upper == "BUTTON")
    {
      if (has_style(style, "BS_GROUPBOX"))
        return "QGroupBox";
      if (has_style(style, "BS_CHECKBOX") ||
          has_style(style, "BS_AUTOCHECKBOX") ||
          has_style(style, "BS_AUTO3STATE") ||
          has_style(style, "BS_3STATE"))
        return "QCheckBox";
      if (has_style(style, "BS_RADIOBUTTON") ||
          has_style(style, "BS_AUTORADIOBUTTON"))
        return "QRadioButton";
      return "QPushButton";
    }

    if (upper == "EDIT")
    {
      if (has_style(style, "ES_MULTILINE"))
        return "QTextEdit";
      return "QLineEdit";
    }

    if (auto it = class_map.find(upper);
        it != class_map.end())
      return it->second;

    return "QWidget";
  }


generator::generator(void)
{
#ifndef HAVE_QT
  if(FT_Init_FreeType(&m_ft_library))
    throw std::runtime_error("Failed to initialize FreeType library!");
#endif
}


generator::~generator(void)
{
#ifndef HAVE_QT
  if(m_ft_face)
    FT_Done_Face(m_ft_face);
  if(m_ft_library)
    FT_Done_FreeType(m_ft_library);
#endif
}

bool generator::generate_all(const rc_file& file, const std::string& output_dir, const std::string& res_dir_name)
{
  collect_global_data(file);

  std::vector<std::string> generated_files;
  std::set<std::string> used_short_ids;

  bool created_output_dir = false;

  int dialog_index = 0;

  for(const auto& res : file.resources)
  {
    if(!std::holds_alternative<dialog_data>(res.data))
      continue;

    const auto& dd = std::get<dialog_data>(res.data);

    if(!created_output_dir)
    {
      std::filesystem::create_directories(std::filesystem::path(output_dir) / res_dir_name);
      created_output_dir = true;
    }

    std::string short_id = res.id;
    if(short_id.size() > 4 && short_id.substr(0, 4) == "IDD_")
      short_id = short_id.substr(4);
    for(char& c : short_id)
      c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    for(char& c : short_id)
    {
      if(!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-')
        c = '_';
    }

    if(short_id.empty())
      short_id = std::format("dialog_{}", dialog_index);

    ++dialog_index;

    std::string unique_short_id = short_id;
    int suffix = 2;
    while(used_short_ids.count(unique_short_id))
      unique_short_id = std::format("{}_{}", short_id, suffix++);
    used_short_ids.insert(unique_short_id);

    std::filesystem::path filename = std::filesystem::path(output_dir) / res_dir_name / (unique_short_id + ".ui");

    m_name_counts.clear();
    m_action_counter = 0;
    m_menubar_node = pugi::xml_node();


    pugi::xml_document doc;
    pugi::xml_node ui = doc.append_child("ui");
    set_attr(ui, "version", "4.0");
    ui.append_child("class").text() = "Form";

    write_dialog(ui, res);

    pugi::xml_node root_widget = ui.child("widget");

    std::string menu_id;
    if(const dialog_stmt* stmt = find_statement(dd, "MENU"))
      menu_id = stmt->id_value;

    for(const auto& menu_res : file.resources)
    {
      if(!std::holds_alternative<menu_data>(menu_res.data))
        continue;

      if(menu_id.empty())
        continue;

      if(menu_res.id == menu_id)
      {
        write_menu(root_widget, menu_res);
      }
      else
      {
        const auto& reg = constant_registry::instance();
        int64_t dialog_menu_val = reg.resolve(menu_id);
        int64_t menu_res_val = reg.resolve(menu_res.id);
        if(dialog_menu_val >= 0 && menu_res_val >= 0 && dialog_menu_val == menu_res_val)
          write_menu(root_widget, menu_res);
      }
    }

    write_actions(root_widget, file);

    if(doc.save_file(filename.generic_string().c_str(), "  "))
      generated_files.push_back(filename.generic_string());
  }

  return !generated_files.empty();
}

void generator::collect_global_data(const rc_file& file)
{
  m_accelerator_map.clear();
  m_string_table_map.clear();
  m_menu_text_map.clear();
  m_menu_disabled_map.clear();
  m_menu_checked_map.clear();
  m_dlginit_map.clear();
  m_ds_control_dialogs.clear();
  m_name_counts.clear();
    m_action_counter = 0;

  m_menubar_node = pugi::xml_node();

  for(const auto& res : file.resources)
  {
    if(res.type == "ACCELERATORS" &&
       std::holds_alternative<std::vector<accelerator_entry>>(res.data))
    {
      const auto& accels = std::get<std::vector<accelerator_entry>>(res.data);
      for(const auto& a : accels)
      {
        if(!a.id.empty())
        {
          std::string qt_key;

          std::string event_upper = to_upper(a.event);

          bool has_ctrl = false;
          bool has_shift = false;
          bool has_alt = false;

          for(const auto& mod : a.modifiers)
          {
            std::string mod_upper = to_upper(mod);
            if(mod_upper == "CONTROL" || mod_upper == "CTRL")
              has_ctrl = true;
            else if(mod_upper == "SHIFT")
              has_shift = true;
            else if(mod_upper == "ALT")
              has_alt = true;
          }

          if(has_ctrl) qt_key += "Ctrl+";
          if(has_alt) qt_key += "Alt+";
          if(has_shift) qt_key += "Shift+";

          qt_key += map_vk_to_qt(a.event);

          m_accelerator_map[a.id] = qt_key;
        }
      }
    }

    if(res.type == "STRINGTABLE" &&
       std::holds_alternative<std::vector<string_table_entry>>(res.data))
    {
      const auto& strings = std::get<std::vector<string_table_entry>>(res.data);
      for(const auto& s : strings)
      {
        if(!s.id.empty())
          m_string_table_map[s.id] = s.value;
      }
    }

    if(res.type == "DLGINIT" &&
       std::holds_alternative<std::vector<dlginit_entry>>(res.data))
    {
      const auto& items = std::get<std::vector<dlginit_entry>>(res.data);
      for(const auto& item : items)
      {
        if(item.message == 0x0401 || // LB_ADDSTRING
           item.message == 0x0402 || // LB_INSERTSTRING
           item.message == 0x0403 || // CB_ADDSTRING
           item.message == 0x0404)   // CB_INSERTSTRING
          m_dlginit_map[item.control_id].push_back(item.text);
      }
    }
  }

  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<menu_data>(res.data))
    {
      const auto& md = std::get<menu_data>(res.data);
      std::function<void(const std::vector<menu_entry>&)> collect_texts = [&](const std::vector<menu_entry>& entries)
      {
        for(const auto& entry : entries)
        {
          if(std::holds_alternative<menu_item>(entry.item))
          {
            const auto& mi = std::get<menu_item>(entry.item);
            if(!mi.id.empty() && mi.text != "-")
            {
              std::string display_text = strip_accelerator(mi.text);
              if(!display_text.empty())
                m_menu_text_map[mi.id] = display_text;

              for(const auto& f : mi.flags)
              {
                std::string f_upper = to_upper(f);
                if(f_upper == "GRAYED" || f_upper == "INACTIVE" || f_upper == "MFS_GRAYED" || f_upper == "MFS_UNHILITE")
                  m_menu_disabled_map[mi.id] = true;
                if(f_upper == "CHECKED" || f_upper == "MFS_CHECKED")
                  m_menu_checked_map[mi.id] = true;
              }
            }
          }
          else if(std::holds_alternative<std::shared_ptr<popup>>(entry.item))
          {
            auto sub = std::get<std::shared_ptr<popup>>(entry.item);
            for(const auto& f : sub->flags)
            {
              std::string f_upper = to_upper(f);
              if(f_upper == "GRAYED" || f_upper == "INACTIVE" || f_upper == "MFS_GRAYED")
                m_menu_disabled_map[sub->text] = true;
            }
            collect_texts(sub->entries);
          }
        }
      };
      collect_texts(md.entries);
    }
  }

  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<dialog_data>(res.data))
    {
      const auto& dd = std::get<dialog_data>(res.data);
      bool is_ds_control = false;
      for(const auto& stmt : dd.statements)
      {
        std::string kw_upper = to_upper(stmt.keyword);
        if(kw_upper == "STYLE" && has_style(stmt.value, "DS_CONTROL"))
          is_ds_control = true;
      }
      if(is_ds_control)
        m_ds_control_dialogs[res.id] = &dd;
    }
  }
}

bool generator::generate_qrc(const rc_file& file, const std::string& output_path, const std::vector<std::string>& ui_paths)
{
  pugi::xml_document doc;

  pugi::xml_node qresource = doc.append_child("RCC");
  pugi::xml_node res_node = qresource.append_child("qresource");

  std::filesystem::path qrc_path_full = std::filesystem::absolute(output_path);
  std::filesystem::path qrc_dir_fs;
  if(qrc_path_full.has_parent_path() && !qrc_path_full.parent_path().empty())
    qrc_dir_fs = std::filesystem::absolute(qrc_path_full.parent_path());
  else
    qrc_dir_fs = std::filesystem::current_path();

  for(const auto& up : ui_paths)
  {
    if(!up.empty())
    {
      pugi::xml_node file_node = res_node.append_child("file");
      std::string normalized = up;
      std::replace(normalized.begin(), normalized.end(), '\\', '/');
      std::filesystem::path abs_path = std::filesystem::absolute(normalized);
      std::filesystem::path rel = abs_path.lexically_relative(qrc_dir_fs);
      std::string rel_str = rel.empty() ? normalized : rel.generic_string();
      file_node.text() = rel_str.c_str();
    }
  }

  for(const auto& res : file.resources)
  {
    if(res.filename.empty())
      continue;

    if(match_string(res.type, { "BITMAP", "ICON", "CURSOR", "RT_MANIFEST", "REGISTRY", "TYPELIB", "DATA"}))
    {
      pugi::xml_node file_node = res_node.append_child("file");
      set_attr(file_node, "alias", res.id);
      // Paths in an .rc file are relative to the .rc file's own directory,
      // so the filename must be emitted verbatim rather than resolved
      // against the current working directory (which std::filesystem
      // absolute() would do) or rewritten relative to the .qrc location.
      std::string rel_str = res.filename;
      std::replace(rel_str.begin(), rel_str.end(), '\\', '/');
      file_node.text() = rel_str.c_str();
    }
  }

  return doc.save_file(output_path.c_str(), "  ");
}

void generator::write_dialog(pugi::xml_node& parent, const resource& res)
{
  if(!std::holds_alternative<dialog_data>(res.data))
    return;
  const auto& dd = std::get<dialog_data>(res.data);
  std::string dialog_name = res.id;

  pugi::xml_node widget = parent.append_child("widget");
  set_attr(widget, "class", "QDialog");
  set_attr(widget, "name", dialog_name);

  setup_dialog_font(dd);

  std::vector<int> groupbox_indices;
  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    const auto& ctrl = dd.controls[i];
    std::string qt_class = map_keyword_to_widget(ctrl.keyword);
    if(qt_class.empty() && ctrl.keyword == "CONTROL")
      qt_class = map_class_to_widget(ctrl.class_name, ctrl.style);
    if(qt_class == "QGroupBox")
      groupbox_indices.push_back(static_cast<int>(i));
  }

  std::vector<int> parent_groupbox(dd.controls.size(), -1);
  for(int gi : groupbox_indices)
  {
    const auto& gb = dd.controls[gi];
    int16_t gb_x = gb.x;
    int16_t gb_y = gb.y;
    uint16_t gb_w = gb.width;
    uint16_t gb_h = gb.height;

    for(size_t i = 0; i < dd.controls.size(); ++i)
    {
      if(static_cast<int>(i) == gi)
        continue;
      if(parent_groupbox[i] >= 0)
        continue;

      const auto& ctrl = dd.controls[i];
      int16_t cx = ctrl.x + static_cast<int16_t>(ctrl.width / 2);
      int16_t cy = ctrl.y + static_cast<int16_t>(ctrl.height / 2);

      if(cx >= gb_x && cx < gb_x + static_cast<int16_t>(gb_w) &&
         cy >= gb_y && cy < gb_y + static_cast<int16_t>(gb_h))
      {
        parent_groupbox[i] = gi;
      }
    }
  }

  std::vector<std::string> qt_classes(dd.controls.size());
  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    const auto& ctrl = dd.controls[i];
    std::string qt_class = map_keyword_to_widget(ctrl.keyword);
    if(qt_class.empty() && ctrl.keyword == "CONTROL")
      qt_class = map_class_to_widget(ctrl.class_name, ctrl.style);
    if(qt_class.empty())
      qt_class = "QWidget";
    qt_classes[i] = qt_class;
  }

  size_t gb_count = groupbox_indices.size();
  std::vector<std::vector<control>> gb_children(gb_count);
  std::vector<std::vector<std::string>> gb_child_classes(gb_count);
  std::vector<std::vector<size_t>> gb_child_indices(gb_count);
  std::vector<std::vector<control_layout>> gb_child_layout(gb_count);
  std::vector<int> groupbox_extra_height(gb_count, 0);

  std::vector<bool> taken(dd.controls.size(), false);
  for(size_t k = 0; k < gb_count; ++k)
  {
    int gi = groupbox_indices[k];
    taken[gi] = true;

    const auto& gb = dd.controls[gi];
    for(size_t i = 0; i < dd.controls.size(); ++i)
    {
      if(parent_groupbox[i] != gi)
        continue;
      if(taken[i])
        continue;

      const auto& ctrl = dd.controls[i];

      control relative = ctrl;
      relative.x = ctrl.x - gb.x;
      relative.y = ctrl.y - gb.y - 4;

      gb_children[k].push_back(relative);
      gb_child_classes[k].push_back(qt_classes[i]);
      gb_child_indices[k].push_back(i);
      taken[i] = true;
    }

    layout_control_sizes(gb_children[k], gb_child_classes[k], gb_child_layout[k]);

    int gb_h_px = dlu_to_pixel_y(gb.height);
    int max_bottom_rel = 0;
    for(size_t j = 0; j < gb_children[k].size(); ++j)
    {
      int rel_bottom = dlu_to_pixel_y(gb_children[k][j].y) +
                       gb_child_layout[k][j].y_shift_px +
                       gb_child_layout[k][j].height_px;
      if(rel_bottom > max_bottom_rel)
        max_bottom_rel = rel_bottom;
    }
    if(max_bottom_rel > gb_h_px)
      groupbox_extra_height[k] = max_bottom_rel - gb_h_px;
  }

  std::vector<control> top_level;
  std::vector<std::string> top_classes;
  std::vector<int> top_extra;
  std::vector<size_t> top_indices;
  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    if(parent_groupbox[i] != -1)
      continue;

    int extra = 0;
    for(size_t k = 0; k < gb_count; ++k)
    {
      if(groupbox_indices[k] == static_cast<int>(i))
      {
        extra = groupbox_extra_height[k];
        break;
      }
    }

    top_level.push_back(dd.controls[i]);
    top_classes.push_back(qt_classes[i]);
    top_extra.push_back(extra);
    top_indices.push_back(i);
  }

  std::vector<control_layout> top_layout;
  layout_control_sizes(top_level, top_classes, top_layout, &top_extra);

  int dialog_ph = dlu_to_pixel_y(dd.height);
  int growth = 0;
  for(size_t j = 0; j < top_level.size(); ++j)
  {
    int bottom = dlu_to_pixel_y(top_level[j].y) + top_layout[j].y_shift_px + top_layout[j].height_px;
    if(bottom - dialog_ph > growth)
      growth = bottom - dialog_ph;
  }
  if(growth < 0)
    growth = 0;

  write_dialog_properties(widget, dd, growth);

  std::vector<int> top_shift(dd.controls.size(), 0);
  for(size_t j = 0; j < top_indices.size(); ++j)
    top_shift[top_indices[j]] = top_layout[j].y_shift_px;

  std::vector<bool> written(dd.controls.size(), false);

  for(size_t k = 0; k < gb_count; ++k)
  {
    int gi = groupbox_indices[k];
    write_control(widget, dd.controls[gi], dialog_name, top_shift[gi], groupbox_extra_height[k]);
    written[gi] = true;

    pugi::xml_node gb_widget = widget.last_child();

    for(size_t j = 0; j < gb_children[k].size(); ++j)
    {
      write_control(gb_widget, gb_children[k][j], dialog_name, gb_child_layout[k][j].y_shift_px);
      written[gb_child_indices[k][j]] = true;
    }
  }

  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    if(!written[i])
    {
      write_control(widget, dd.controls[i], dialog_name, top_shift[i]);
      written[i] = true;
    }
  }
}

void generator::setup_dialog_font(const dialog_data& dd)
{
  std::string original_font_name = "MS Sans Serif";
  if(const dialog_stmt* stmt = find_statement(dd, "FONT"))
    original_font_name = stmt->text_value;

  std::string font_name = substitute_font(original_font_name);
  int font_size = 8;
  int font_weight = -1;
  bool font_italic = false;

  if(const dialog_stmt* font_stmt = find_statement(dd, "FONT"))
  {
    if(font_stmt->value.resolved_value >= 0)
      font_size = static_cast<int>(font_stmt->value.resolved_value);
    else if(font_stmt->numeric_value > 0)
      font_size = static_cast<int>(font_stmt->numeric_value);

    if(font_stmt->numeric_value2 > 0)
      font_weight = static_cast<int>(font_stmt->numeric_value2);

    font_italic = font_stmt->italic;
  }

  m_original_font_name = original_font_name;
  m_mapped_font_name = font_name;
  m_font_size = font_size > 0 ? font_size : 8;
  m_font_weight = font_weight;
  m_font_italic = font_italic;

  set_current_font(m_mapped_font_name, m_font_size, m_font_weight, m_font_italic);
}

void generator::write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd, int extra_height)
{
  int px = dlu_to_pixel_x(dd.x);
  int py = dlu_to_pixel_y(dd.y);
  int pw = dlu_to_pixel_x(dd.width);
  int ph = dlu_to_pixel_y(dd.height) + extra_height;
  add_property_rect(widget, px, py, pw, ph);

  std::string caption;
  if(const dialog_stmt* stmt = find_statement(dd, "CAPTION"))
    caption = stmt->text_value;
  if(!caption.empty())
    add_property_string(widget, "windowTitle", caption);

  bool font_bold = false;
  if(m_font_size > 0)
    add_property_font(widget, m_mapped_font_name, m_font_size, font_bold, m_font_italic);

  std::vector<std::string> flags;
  bool is_fixed_size = false;

  if(const dialog_stmt* stmt = find_statement(dd, "STYLE"))
  {
    const style_expr& style = stmt->value;

    if(has_style(style, "WS_MINIMIZEBOX"))
      flags.push_back("Qt::WindowMinimizeButtonHint");

    if(has_style(style, "WS_MAXIMIZEBOX"))
      flags.push_back("Qt::WindowMaximizeButtonHint");

    if(has_style(style, "DS_MODALFRAME"))
    {
      flags.push_back("Qt::Dialog");
      flags.push_back("Qt::WindowCloseButtonHint");
      is_fixed_size = true;
    }

    if(has_style(style, "WS_VISIBLE"))
      add_property_bool(widget, "visible", true);

    add_property_bool(widget, "enabled", !has_style(style, "WS_DISABLED"));
  }

  if(const dialog_stmt* stmt = find_statement(dd, "EXSTYLE"))
  {
    const style_expr& style = stmt->value;
    if(has_style(style, "WS_EX_CONTEXTHELP"))
      flags.push_back("Qt::WindowContextHelpButtonHint");

    if(has_style(style, "WS_EX_TOPMOST"))
      flags.push_back("Qt::WindowStaysOnTopHint");

    if(has_style(style, "WS_EX_TOOLWINDOW"))
      flags.push_back("Qt::Tool");

    if(has_style(style, "WS_EX_DLGMODALFRAME"))
    {
      flags.push_back("Qt::Dialog");
      flags.push_back("Qt::WindowCloseButtonHint");
      is_fixed_size = true;
    }
  }

  if(!flags.empty())
  {
    std::string combined = flags[0];
    for(size_t i = 1; i < flags.size(); ++i)
      combined += "|" + flags[i];
    add_property_set(widget, "windowFlags", combined);
  }

  if(is_fixed_size)
  {
    int pw = dlu_to_pixel_x(dd.width);
    int ph = dlu_to_pixel_y(dd.height) + extra_height;
    add_property_size(widget, "minimumSize", pw, ph);
    add_property_size(widget, "maximumSize", pw, ph);
  }



}

static std::string get_suffix(const std::string& id)
{
  const auto prefixes = {"IDD_", "DLG_", "IDC_", "IDM_"};
  std::string upper = to_upper(id);
  if(std::ranges::any_of(prefixes, [&upper](auto& prefix) { return upper.starts_with(prefix); }))
    upper = upper.substr(4);

  return upper;
}

std::set<std::string> generator::id_words(const std::string& id) const
{
  std::set<std::string> words;
  std::string upper = get_suffix(id);

  std::string word;
  for(char c : upper)
  {
    if(c == '_')
    {
      if(word.size() >= 3)
        words.insert(word);
      word.clear();
    }
    else
    {
      word += c;
    }
  }
  if(word.size() >= 3)
    words.insert(word);

  return words;
}

bool generator::share_common_word(const std::string& id1, const std::string& id2) const
{
  auto words1 = id_words(id1);
  auto words2 = id_words(id2);

  for(const auto& w : words1)
  {
    if(words2.count(w))
      return true;
  }

  std::string upper1 = get_suffix(id1);
  std::string upper2 = get_suffix(id2);

  if(words1.contains(upper2))
    return true;
  if(words2.contains(upper1))
    return true;

  return false;
}

void generator::write_control(pugi::xml_node& parent, const control& ctrl, const std::string& dialog_name, int y_shift_px, int extra_height_px)
{
  std::string qt_class = map_keyword_to_widget(ctrl.keyword);

  if(qt_class.empty() && ctrl.keyword == "CONTROL")
    qt_class = map_class_to_widget(ctrl.class_name, ctrl.style);

  if(qt_class.empty())
    qt_class = "QWidget";

  /* EDITTEXT keywords map to QLineEdit, but a multiline edit control
     (ES_MULTILINE) must become a QTextEdit, mirroring the EDIT class
     handling in map_class_to_widget(). */
  if(qt_class == "QLineEdit" && has_style(ctrl.style, "ES_MULTILINE"))
    qt_class = "QTextEdit";

  std::string name_id = ctrl.id;
  auto resolved_id = constant_registry::instance().resolve(ctrl.id);
  bool is_numeric_id = !ctrl.id.empty() &&
    std::all_of(ctrl.id.begin(), ctrl.id.end(), [](char c) {
      return std::isdigit(static_cast<unsigned char>(c));
    });
  if(is_numeric_id && resolved_id < 0 && !ctrl.text.empty())
  {
    std::string text_id;
    for(char c : ctrl.text)
    {
      if(std::isalnum(static_cast<unsigned char>(c)))
        text_id += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
      else if(c == ' ')
        text_id += '_';
    }
    if(!text_id.empty())
      name_id = "static_" + text_id;
  }
  std::string name = unique_name(name_id);
  pugi::xml_node widget = add_widget(parent, qt_class, name);

  int ctrl_w_dlu = ctrl.width;
  int ctrl_h_dlu = ctrl.height;
  ensure_text_fits(ctrl.text, ctrl_w_dlu, ctrl_h_dlu, widget, qt_class);

  int px = dlu_to_pixel_x(ctrl.x);
  int py = dlu_to_pixel_y(ctrl.y);
  int pw = dlu_to_pixel_x(ctrl_w_dlu);
  int ph = dlu_to_pixel_y(ctrl_h_dlu);
  apply_combo_dropdown_height(widget, ctrl, qt_class == "QComboBox", ph);

  py += y_shift_px;
  int min_h = min_height_px(qt_class);
  if(ph < min_h)
    ph = min_h;
  ph += extra_height_px;

  add_property_rect(widget, px, py, pw, ph);

  if(!ctrl.text.empty())
  {
    if(qt_class == "QGroupBox")
      add_property_string(widget, "title", ctrl.text);
    else
      add_property_string(widget, "text", ctrl.text);
  }

  bool visible = !has_style(ctrl.style, "WS_HIDDEN");
  for(const auto& nf : ctrl.style.not_flags)
  {
    if(nf == "WS_VISIBLE")
      visible = false;
  }
  if(visible)
    add_property_bool(widget, "visible", true);
  else
    add_property_bool(widget, "visible", false);

  bool enabled = !has_style(ctrl.style, "WS_DISABLED");
  add_property_bool(widget, "enabled", enabled);

  if(qt_class == "QPushButton")
  {
    bool def = (ctrl.keyword == "DEFPUSHBUTTON") ||
               has_style(ctrl.style, "BS_DEFPUSHBUTTON");
    add_property_bool(widget, "default", def);
    add_property_bool(widget, "autoDefault", def);
  }

  if(qt_class == "QLabel")
  {
    std::string align = "Qt::AlignLeft";
    if(ctrl.keyword == "CTEXT")
      align = "Qt::AlignHCenter";
    else if(ctrl.keyword == "RTEXT")
      align = "Qt::AlignRight";
    else if(has_style(ctrl.style, "SS_CENTER"))
      align = "Qt::AlignHCenter";
    else if(has_style(ctrl.style, "SS_RIGHT"))
      align = "Qt::AlignRight";
    add_property_set(widget, "alignment", align);
  }

  if(qt_class == "QLineEdit")
  {
    if(has_style(ctrl.style, "ES_READONLY"))
      add_property_bool(widget, "readOnly", true);
    if(has_style(ctrl.style, "ES_PASSWORD"))
      add_property_enum(widget, "echoMode", "QLineEdit::Password");
  }

  if(qt_class == "QTextEdit")
  {
    if(has_style(ctrl.style, "ES_READONLY"))
      add_property_bool(widget, "readOnly", true);
    if(has_style(ctrl.style, "ES_WANTRETURN"))
      add_property_bool(widget, "tabChangesFocus", false);
    if(has_style(ctrl.style, "ES_AUTOVSCROLL"))
      add_property_enum(widget, "verticalScrollBarPolicy", "Qt::ScrollBarAsNeeded");
    if(has_style(ctrl.style, "ES_AUTOHSCROLL"))
      add_property_enum(widget, "horizontalScrollBarPolicy", "Qt::ScrollBarAsNeeded");
  }

  if(qt_class == "QGroupBox")
    add_property_bool(widget, "flat", false);

  if(qt_class == "QComboBox")
  {
    if(has_style(ctrl.style, "CBS_DROPDOWNLIST"))
      add_property_bool(widget, "editable", false);
    else if(has_style(ctrl.style, "CBS_DROPDOWN"))
      add_property_bool(widget, "editable", true);

    auto it = m_dlginit_map.find(ctrl.id);
    if(it != m_dlginit_map.end())
    {
      for(const auto& item_text : it->second)
      {
        pugi::xml_node item_node = widget.append_child("item");
        add_property_string(item_node, "text", item_text);
      }
    }
  }

  if(qt_class == "QSlider")
  {
    if(has_style(ctrl.style, "TBS_VERT"))
      add_property_enum(widget, "orientation", "Qt::Vertical");
    else
      add_property_enum(widget, "orientation", "Qt::Horizontal");

    if(has_style(ctrl.style, "TBS_NOTICKS"))
      add_property_enum(widget, "tickPosition", "QSlider::NoTicks");
    else if(has_style(ctrl.style, "TBS_BOTH"))
      add_property_enum(widget, "tickPosition", "QSlider::TicksBothSides");
    else if(has_style(ctrl.style, "TBS_TOP") || has_style(ctrl.style, "TBS_LEFT"))
      add_property_enum(widget, "tickPosition", "QSlider::TicksAbove");
    else
      add_property_enum(widget, "tickPosition", "QSlider::TicksBelow");

    if(has_style(ctrl.style, "TBS_AUTOTICKS"))
      add_property_int(widget, "tickInterval", 1);
  }

  if(qt_class == "QTableWidget")
  {
    if(has_style(ctrl.style, "LVS_REPORT"))
      add_property_enum(widget, "selectionBehavior", "QAbstractItemView::SelectRows");
    if(has_style(ctrl.style, "LVS_SINGLESEL"))
      add_property_enum(widget, "selectionMode", "QAbstractItemView::SingleSelection");
    if(has_style(ctrl.style, "LVS_SORTASCENDING"))
      add_property_bool(widget, "sortingEnabled", true);
  }

  if(qt_class == "QTreeWidget")
  {
    if(has_style(ctrl.style, "TVS_HASBUTTONS"))
      add_property_bool(widget, "rootIsDecorated", true);
    if(has_style(ctrl.style, "TVS_SHOWSELALWAYS"))
      add_property_enum(widget, "selectionMode", "QAbstractItemView::SingleSelection");
  }

  if(qt_class == "QProgressBar")
  {
    if(has_style(ctrl.style, "PBS_VERTICAL"))
      add_property_enum(widget, "orientation", "Qt::Vertical");
    else
      add_property_enum(widget, "orientation", "Qt::Horizontal");
    add_property_int(widget, "minimum", 0);
    add_property_int(widget, "maximum", 100);
    add_property_int(widget, "value", 0);
  }

  if(qt_class == "QSpinBox")
  {
    if(has_style(ctrl.style, "UDS_WRAP"))
      add_property_bool(widget, "wrapping", true);
    add_property_int(widget, "minimum", 0);
    add_property_int(widget, "maximum", 99);
    add_property_int(widget, "value", 0);
  }

  if(qt_class == "QDateTimeEdit")
  {
    if(has_style(ctrl.style, "DTS_UPDOWN"))
      add_property_enum(widget, "buttonSymbols", "QAbstractSpinBox::UpDownArrows");
    else
      add_property_enum(widget, "buttonSymbols", "QAbstractSpinBox::NoButtons");
  }

  if(qt_class == "QScrollBar")
  {
    if(has_style(ctrl.style, "SBS_VERT"))
      add_property_enum(widget, "orientation", "Qt::Vertical");
    else
      add_property_enum(widget, "orientation", "Qt::Horizontal");
    add_property_int(widget, "minimum", 0);
    add_property_int(widget, "maximum", 100);
    add_property_int(widget, "value", 0);
  }

  if(qt_class == "QTabWidget")
  {
    if(has_style(ctrl.style, "TCS_BUTTONS"))
      add_property_enum(widget, "tabPosition", "QTabWidget::North");

    int tab_idx = 0;
    for(const auto& [dlg_id, dlg_ptr] : m_ds_control_dialogs)
    {
      if(!dlg_ptr)
        continue;

      if(!dialog_name.empty() && !share_common_word(dialog_name, dlg_id))
        continue;

      const auto& dd = *dlg_ptr;

      std::string tab_title;
      std::string dlg_id_upper = to_upper(dlg_id);

      for(const auto& stmt : dd.statements)
      {
        std::string kw_upper = to_upper(stmt.keyword);
        if(kw_upper == "CAPTION")
        {
          tab_title = stmt.text_value;
          break;
        }
      }
      if(tab_title.empty())
      {
        tab_title = dlg_id;
        for(const auto& prefix : {"IDD_", "DLG_", "IDC_"})
        {
          if(tab_title.size() > strlen(prefix) && tab_title.substr(0, strlen(prefix)) == prefix)
          {
            tab_title = tab_title.substr(strlen(prefix));
            break;
          }
        }
        for(char& c : tab_title)
          c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        for(char& c : tab_title)
        {
          if(c == '_')
            c = ' ';
        }
      }

      std::string tab_name = std::format("tabPage{}", tab_idx);

      pugi::xml_node tab_widget = add_widget(widget, "QWidget", tab_name);

      pugi::xml_node tab_attr = tab_widget.append_child("attribute");
      set_attr(tab_attr, "name", "title");
      tab_attr.append_child("string").text() = tab_title.c_str();

      pugi::xml_node tab_prop = tab_widget.append_child("property");
      set_attr(tab_prop, "name", "geometry");
      pugi::xml_node tab_rect = tab_prop.append_child("rect");
      tab_rect.append_child("x").text() = 0;
      tab_rect.append_child("y").text() = 0;
      tab_rect.append_child("width").text() = dlu_to_pixel_x(dd.width);
      tab_rect.append_child("height").text() = dlu_to_pixel_y(dd.height);

      std::vector<std::string> child_classes(dd.controls.size());
      for(size_t i = 0; i < dd.controls.size(); ++i)
      {
        const auto& child_ctrl = dd.controls[i];
        std::string child_class = map_keyword_to_widget(child_ctrl.keyword);
        if(child_class.empty() && child_ctrl.keyword == "CONTROL")
          child_class = map_class_to_widget(child_ctrl.class_name, child_ctrl.style);
        if(child_class.empty())
          child_class = "QWidget";
        child_classes[i] = child_class;
      }

      std::vector<control_layout> child_layout;
      layout_control_sizes(dd.controls, child_classes, child_layout);

      for(size_t i = 0; i < dd.controls.size(); ++i)
      {
        const auto& child_ctrl = dd.controls[i];
        const std::string& child_class = child_classes[i];

        std::string child_name = unique_name(child_ctrl.id);
        pugi::xml_node child_widget = add_widget(tab_widget, child_class, child_name);

        int child_w_dlu = child_ctrl.width;
        int child_h_dlu = child_ctrl.height;
        ensure_text_fits(child_ctrl.text, child_w_dlu, child_h_dlu, child_widget, child_class);

        int cx = dlu_to_pixel_x(child_ctrl.x);
        int cy = dlu_to_pixel_y(child_ctrl.y) + child_layout[i].y_shift_px;
        int cw = dlu_to_pixel_x(child_w_dlu);
        int ch = dlu_to_pixel_y(child_h_dlu);
        apply_combo_dropdown_height(child_widget, child_ctrl, child_class == "QComboBox", ch);

        int child_min_h = min_height_px(child_class);
        if(ch < child_min_h)
          ch = child_min_h;

        add_property_rect(child_widget, cx, cy, cw, ch);

        if(!child_ctrl.text.empty())
        {
          if(child_class == "QGroupBox")
            add_property_string(child_widget, "title", child_ctrl.text);
          else
            add_property_string(child_widget, "text", child_ctrl.text);
        }
      }

      ++tab_idx;
    }
  }

  if(qt_class == "QListWidget")
  {
    if(has_style(ctrl.style, "LBS_MULTIPLE") || has_style(ctrl.style, "LBS_EXTENDED"))
      add_property_enum(widget, "selectionMode", "QAbstractItemView::ExtendedSelection");
    else
      add_property_enum(widget, "selectionMode", "QAbstractItemView::SingleSelection");

    auto it = m_dlginit_map.find(ctrl.id);
    if(it != m_dlginit_map.end())
    {
      for(const auto& item_text : it->second)
      {
        pugi::xml_node item_node = widget.append_child("item");
        add_property_string(item_node, "text", item_text);
      }
    }
  }
}

void generator::apply_combo_dropdown_height(pugi::xml_node& widget, const control& ctrl, bool is_combo, int& height_px)
{
  bool is_dropdown = is_combo &&
    (has_style(ctrl.style, "CBS_DROPDOWN") ||
     has_style(ctrl.style, "CBS_DROPDOWNLIST"));
  if(is_dropdown)
  {
    // The COMBOBOX height parameter is the fully-expanded height (dropdown list
    // open). A drop-down combo box is collapsed to a one-line field; the list
    // extent is preserved through maxVisibleItems.
    constexpr int kComboClosedDlu = 14;
    constexpr int kComboItemDlu = 8;
    int closed_height = dlu_to_pixel_y(kComboClosedDlu);
    int item_height = dlu_to_pixel_y(kComboItemDlu);
    if(item_height > 0)
    {
      int max_visible = (height_px - closed_height) / item_height;
      if(max_visible < 1)
        max_visible = 1;
      add_property_int(widget, "maxVisibleItems", max_visible);
    }
    height_px = closed_height;
  }
}

int generator::min_height_px(const std::string& qt_class)
{
  static const std::map<std::string, int> min_height_map =
  {
    { "QCheckBox", 15 },
  };

  auto it = min_height_map.find(qt_class);
  if (it == min_height_map.end())
    return 0;
  return it->second;
}

int generator::vertical_margin_px(const std::string& qt_class)
{
  static const std::map<std::string, int> vertical_margin_map =
  {
    { "QCheckBox", 18 },
  };

  auto it = vertical_margin_map.find(qt_class);
  if (it == vertical_margin_map.end())
    return 0;
  return it->second;
}

void generator::layout_control_sizes(const std::vector<control>& controls,
                                     const std::vector<std::string>& qt_classes,
                                     std::vector<control_layout>& layout,
                                     const std::vector<int>* extra_heights)
{
  layout.assign(controls.size(), control_layout{});

  if(controls.empty())
    return;

  std::vector<int> py(controls.size());
  std::vector<int> ph(controls.size());
  std::vector<int> extra(controls.size());
  std::vector<int> min_h(controls.size());
  std::vector<int> margin(controls.size());

  for(size_t i = 0; i < controls.size(); ++i)
  {
    const control& ctrl = controls[i];
    const std::string& qt_class = qt_classes[i];

    int height_dlu = ctrl.height;
    try
    {
      int width_dlu = ctrl.width;
      text_fit_info info = fit_text(ctrl.text, width_dlu, height_dlu, qt_class);
      height_dlu = info.height_dlu;
    }
    catch(const std::runtime_error&)
    {
      height_dlu = ctrl.height;
    }

    ph[i] = dlu_to_pixel_y(height_dlu);
    if(qt_class == "QComboBox" &&
       (has_style(ctrl.style, "CBS_DROPDOWN") || has_style(ctrl.style, "CBS_DROPDOWNLIST")))
      ph[i] = dlu_to_pixel_y(14);

    if(extra_heights != nullptr && i < extra_heights->size())
      extra[i] = (*extra_heights)[i];

    py[i] = dlu_to_pixel_y(ctrl.y);
    min_h[i] = min_height_px(qt_class);
    margin[i] = vertical_margin_px(qt_class);
    layout[i].height_px = std::max(ph[i], min_h[i]) + extra[i];
  }

  std::vector<size_t> order(controls.size());
  for(size_t i = 0; i < controls.size(); ++i)
    order[i] = i;

  std::sort(order.begin(), order.end(), [&](size_t a, size_t b)
  {
    if(py[a] != py[b])
      return py[a] < py[b];
    return controls[a].x < controls[b].x;
  });

  std::vector<std::pair<int, int>> events;
  for(size_t p = 0; p < order.size(); ++p)
  {
    size_t i = order[p];
    if(margin[i] > 0)
    {
      size_t q = p + 1;
      while(q < order.size() && py[order[q]] <= py[i])
        ++q;
      if(q < order.size())
      {
        int gap = py[order[q]] - py[i];
        if(gap < margin[i])
          events.push_back({ py[order[q]], margin[i] - gap });
      }
    }

    int expansion = std::max(0, min_h[i] - ph[i]) + extra[i];
    if(expansion > 0)
      events.push_back({ py[i] + ph[i], expansion });
  }
  std::sort(events.begin(), events.end());

  int running = 0;
  size_t event_index = 0;
  for(size_t idx : order)
  {
    while(event_index < events.size() && events[event_index].first <= py[idx])
    {
      running += events[event_index].second;
      ++event_index;
    }
    layout[idx].y_shift_px = running;
  }
}

std::string generator::unique_name(const std::string& id)
{
  std::string base = id.empty() ? "widget" : id;
  bool is_numeric = !base.empty() &&
    (std::all_of(base.begin(), base.end(), [](char c) {
      return std::isdigit(static_cast<unsigned char>(c)) || c == '-';
    }));
  if(is_numeric)
    base = "widget_" + base;
  int& count = m_name_counts[base];
  if(count == 0)
  {
    count = 1;
    return base;
  }
  count++;
  return std::format("{}_{}", base, count);
}



#ifdef HAVE_QT
# include <QFontDatabase>
# include <QFontMetrics>

// based upon https://jeffpar.github.io/kbarchive/kb/145/Q145994/
void generator::set_current_font(const std::string& font_name, int font_size, int weight, bool italic)
{
  if(QFontDatabase db; !db.families().contains(QString::fromStdString(font_name), Qt::CaseInsensitive))
    { throw std::runtime_error(std::format("Font \"{}\" not found!", font_name)); }

  m_current_font = QFont(QString::fromStdString(font_name), font_size, weight, italic);
  QFontMetrics fm(m_current_font);

  QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  m_dlu_x_factor = static_cast<double>(fm.horizontalAdvance(alphabet)) / 52.0 / 4.00;
  m_dlu_y_factor = static_cast<double>(fm.height()) / 8.0;
}

std::pair<int, int> generator::text_dimensions(const std::string& text)
{
  QFontMetrics fm(m_current_font);

  // Convert the measured pixels into dialog units using the same base-unit
  // factors that dlu_to_pixel_x/y are derived from, so that the result can be
  // compared directly against the DLU box sizes from the RC file.
  int width_dlu = qCeil(static_cast<double>(fm.horizontalAdvance(QString::fromStdString(text))) / m_dlu_x_factor);
  int height_dlu = qCeil(static_cast<double>(fm.height()) / m_dlu_y_factor);
  return { width_dlu, height_dlu };
}
#elif HAVE_FREETYPE
# include <ft2build.h>
# include <freetype/freetype.h>
# include <fontconfig/fontconfig.h>
# include <cmath>

// based upon https://jeffpar.github.io/kbarchive/kb/145/Q145994/
void generator::set_current_font(const std::string &font_name, int font_size, int weight, bool italic)
{
  if (!FcInit())
    throw std::runtime_error("Failed to initialize Fontconfig!");

  FcPattern *pat = FcPatternCreate();
  FcPatternAddString(pat, FC_FAMILY, reinterpret_cast<const FcChar8 *>(font_name.c_str()));

  int fc_weight = FC_WEIGHT_MEDIUM; // normal
  if (weight >= 700) // bold
    fc_weight = FC_WEIGHT_BOLD;
  else if (weight <= 300) // light
    fc_weight = FC_WEIGHT_LIGHT;
  FcPatternAddInteger(pat, FC_WEIGHT, fc_weight);

  FcPatternAddInteger(pat, FC_SLANT, italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
  FcConfigSubstitute(nullptr, pat, FcMatchPattern);
  FcDefaultSubstitute(pat);

  FcResult result;
  FcPattern *match = FcFontMatch(nullptr, pat, &result);
  FcPatternDestroy(pat);

  if (!match)
  {
    FcFini();
    throw std::runtime_error(std::format("Font \"{}\" not found via Fontconfig!", font_name));
  }

  FcChar8 *file_path = nullptr;
  if (FcPatternGetString(match, FC_FILE, 0, &file_path) != FcResultMatch)
  {
    FcPatternDestroy(match);
    FcFini();
    throw std::runtime_error(std::format("Could not extract file path for font \"{}\"", font_name));
  }

  std::string path_str(reinterpret_cast<char *>(file_path));
  FcPatternDestroy(match);
  FcFini();

  // 2. Load the font face using the FreeType library initialized in the constructor
  if(m_ft_face)
  {
    FT_Done_Face(m_ft_face);
    m_ft_face = nullptr;
  }
  if(FT_New_Face(m_ft_library, path_str.c_str(), 0, &m_ft_face))
    throw std::runtime_error(std::format("Failed to load font file: {}", path_str));

  // Set font size (FreeType takes size in 26.6 fractional pixels, or pixel sizes directly)
  // Assuming font_size represents point size at 96 DPI: pixels = point_size * 96 / 72
  int pixel_height = (font_size * 96 + 36) / 72;
  if(FT_Set_Pixel_Sizes(m_ft_face, 0, pixel_height))
  {
    FT_Done_Face(m_ft_face);
    m_ft_face = nullptr;
    throw std::runtime_error("Failed to set pixel size for FreeType face.");
  }

  // 3. Extract metrics equivalent to QFontMetrics
  // ft_face->size->metrics.height is in 26.6 fractional pixels (shift right by 6)
  double font_height = static_cast<double>(m_ft_face->size->metrics.height >> 6);
  m_dlu_y_factor = font_height / 8.0;

  // Calculate horizontal advance for the alphabet string
  std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  double total_width = 0.0;

  // Ensure glyph loader knows to load metrics
  for(char c : alphabet)
  {
    FT_UInt glyph_index = FT_Get_Char_Index(m_ft_face, static_cast<FT_ULong>(c));
    // Load glyph metrics (with no rasterization bitmap overhead needed for width)
    if(FT_Load_Glyph(m_ft_face, glyph_index, FT_LOAD_DEFAULT) == 0)
      total_width += static_cast<double>(m_ft_face->glyph->advance.x >> 6);
  }

  double avgCharWidth = total_width / 52.0;
  m_dlu_x_factor = avgCharWidth / 4.0;

  // 4. The font face is kept alive in m_ft_face for text_dimensions();
  //    it is released in the destructor
}

std::pair<int, int> generator::text_dimensions(const std::string& text)
{
  if(m_ft_face == nullptr)
    throw std::runtime_error("A Font must be set before using generator::text_dimensions");

  double total_width = 0.0;
  for(char c : text)
  {
    FT_UInt glyph_index = FT_Get_Char_Index(m_ft_face, static_cast<FT_ULong>(c));
    if(FT_Load_Glyph(m_ft_face, glyph_index, FT_LOAD_DEFAULT) == 0)
      total_width += static_cast<double>(m_ft_face->glyph->advance.x >> 6);
  }

  double font_height = static_cast<double>(m_ft_face->size->metrics.height >> 6);
  // Convert the measured pixels into dialog units using the same base-unit
  // factors that dlu_to_pixel_x/y are derived from, so that the result can
  // be compared directly against the DLU box sizes from the RC file.
  int width_dlu = static_cast<int>(std::ceil(total_width / m_dlu_x_factor));
  int height_dlu = static_cast<int>(std::ceil(font_height / m_dlu_y_factor));
  return { width_dlu, height_dlu };
}

#else
# error No support included for font evaluation
#endif

static bool supports_word_wrap(const std::string& widget_class)
{
  // Only QLabel has a wordWrap property; QAbstractButton-derived widgets
  // (QPushButton, QCheckBox, QRadioButton) do not support wrapping.
  return widget_class == "QLabel";
}

std::vector<std::string> generator::wrap_text(const std::string& text, int width_dlu)
{
  std::vector<std::string> lines;
  std::string current;
  std::istringstream stream(text);
  std::string word;

  while(stream >> word)
  {
    std::string candidate = current.empty() ? word : current + " " + word;
    if(text_dimensions(candidate).first <= width_dlu)
    {
      current = candidate;
    }
    else
    {
      if(!current.empty())
      {
        lines.push_back(current);
        current = word;
      }
      else
      {
        lines.push_back(word);
      }
    }
  }

  if(!current.empty())
    lines.push_back(current);

  return lines;
}

generator::text_fit_info generator::fit_text(const std::string& text, int width_dlu, int height_dlu,
                                             const std::string& widget_class)
{
  if(text.empty())
    return { width_dlu, height_dlu, false };

  const auto [mapped_w, mapped_h] = text_dimensions(text);
  if(mapped_w <= width_dlu && mapped_h <= height_dlu)
    return { width_dlu, height_dlu, false };

  // The mapped (substituted) font may be wider than the original RC font.
  // Check whether the text fits inside the bounding box when wrapped with the
  // original font; if it does, the original layout relies on multi-line text
  // and the widget is reflowed to the mapped font with word wrapping enabled.
  // Otherwise the bounding box is expanded to the single-line dimensions of
  // the mapped font.
  bool wrapped_in_original = false;
  bool can_word_wrap = supports_word_wrap(widget_class);

  try
  {
    set_current_font(m_original_font_name, m_font_size, m_font_weight, m_font_italic);

    std::vector<std::string> original_lines = wrap_text(text, width_dlu);
    int original_line_height = text_dimensions(text).second;
    int original_height = static_cast<int>(original_lines.size()) * original_line_height;
    wrapped_in_original = original_height <= height_dlu;
  }
  catch(const std::runtime_error&)
  {
    // The original font cannot be resolved; fall back to the mapped font below.
  }

  set_current_font(m_mapped_font_name, m_font_size, m_font_weight, m_font_italic);

  if(wrapped_in_original && can_word_wrap)
  {
    std::vector<std::string> lines = wrap_text(text, width_dlu);
    int line_height = text_dimensions(text).second;

    int max_line_width = 0;
    for(const auto& line : lines)
      max_line_width = std::max(max_line_width, text_dimensions(line).first);

    if(max_line_width > width_dlu)
      width_dlu = max_line_width;

    int needed_height = static_cast<int>(lines.size()) * line_height;
    if(needed_height > height_dlu)
      height_dlu = needed_height;

    return { width_dlu, height_dlu, true };
  }

  if(mapped_w > width_dlu)
    width_dlu = mapped_w;
  if(mapped_h > height_dlu)
    height_dlu = mapped_h;

  return { width_dlu, height_dlu, false };
}

void generator::ensure_text_fits(const std::string& text, int& width_dlu, int& height_dlu,
                                 pugi::xml_node& widget, const std::string& widget_class)
{
  if(text.empty())
    return;

  text_fit_info info = fit_text(text, width_dlu, height_dlu, widget_class);
  width_dlu = info.width_dlu;
  height_dlu = info.height_dlu;

  if(info.word_wrap)
  {
    add_property_bool(widget, "wordWrap", true);
    return;
  }

  const auto [final_w, final_h] = text_dimensions(text);
  if(final_w > width_dlu || final_h > height_dlu)
  {
    throw std::runtime_error(std::format(
      "Text \"{}\" needs {}x{} DLU but its bounding box is {}x{} DLU",
      text, final_w, final_h, width_dlu, height_dlu));
  }
}

const dialog_stmt* generator::find_statement(const dialog_data& dd, const std::string& keyword) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
      return &s;
  }
  return nullptr;
}

bool generator::has_dialog_flag(const dialog_data& dd, const std::string& keyword, const std::string& flag) const
{
  const dialog_stmt* stmt = find_statement(dd, keyword);
  if(stmt && has_style(stmt->value, flag))
    return true;
  return false;
}

void generator::write_menu(pugi::xml_node& parent, const resource& res)
{
  if(!std::holds_alternative<menu_data>(res.data))
    return;
  const auto& md = std::get<menu_data>(res.data);

  if(!m_menubar_node)
  {
    m_menubar_node = parent.append_child("widget");
    set_attr(m_menubar_node, "class", "QMenuBar");
    set_attr(m_menubar_node, "name", "menubar");

    pugi::xml_node geom = m_menubar_node.append_child("property");
    set_attr(geom, "name", "geometry");
    pugi::xml_node rect = geom.append_child("rect");
    rect.append_child("x").text() = 0;
    rect.append_child("y").text() = 0;
    rect.append_child("width").text() = 800;
    rect.append_child("height").text() = 22;
  }

  for(const auto& entry : md.entries)
  {
    if(std::holds_alternative<std::shared_ptr<popup>>(entry.item))
    {
      auto popup_ptr = std::get<std::shared_ptr<popup>>(entry.item);
      std::string menu_name = "menu" + popup_ptr->text;

      std::string cleaned = menu_name;
      cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), [](unsigned char c) { return !std::isalnum(c); }), cleaned.end());
      if(!cleaned.empty() && std::isdigit(static_cast<unsigned char>(cleaned[0])))
        cleaned = "m" + cleaned;
      menu_name = unique_name(cleaned);

      pugi::xml_node menu = m_menubar_node.append_child("widget");
      set_attr(menu, "class", "QMenu");
      set_attr(menu, "name", menu_name);

      add_property_string(menu, "title", popup_ptr->text);
      write_menu_entries(menu, popup_ptr->entries);

      add_tag(m_menubar_node, "addaction", menu_name);
    }
  }
}

void generator::write_menu_entries(pugi::xml_node& menu_node, const std::vector<menu_entry>& entries)
{
  for(const auto& entry : entries)
  {
    if(std::holds_alternative<menu_item>(entry.item))
    {
      const auto& mi = std::get<menu_item>(entry.item);

      if(mi.text == "-" || mi.text.empty())
        add_tag(menu_node, "addaction", "separator");
      else
      {
        std::string action_name = mi.id.empty()
          ? std::format("action{}", m_action_counter++)
          : mi.id;
        add_tag(menu_node, "addaction", action_name);
      }
    }
    else if(std::holds_alternative<std::shared_ptr<popup>>(entry.item))
    {
      auto sub = std::get<std::shared_ptr<popup>>(entry.item);

      std::string sub_name = "menu" + sub->text;
      std::string cleaned = sub_name;
      cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), [](unsigned char c) { return !std::isalnum(c); }), cleaned.end());
      if(!cleaned.empty() && std::isdigit(static_cast<unsigned char>(cleaned[0])))
        cleaned = "m" + cleaned;
      sub_name = unique_name(cleaned);

      pugi::xml_node sub_menu = menu_node.append_child("widget");
      set_attr(sub_menu, "class", "QMenu");
      set_attr(sub_menu, "name", sub_name);

      add_property_string(sub_menu, "title", sub->text);
      write_menu_entries(sub_menu, sub->entries);
      add_tag(menu_node, "addaction", sub_name);
    }
  }
}

void generator::write_toolbar(pugi::xml_node& parent, const resource& res)
{
  const auto& td = std::get<toolbar_data>(res.data);
  std::string tb_name = "toolBar";
  if(!res.id.empty())
    tb_name = res.id;

  pugi::xml_node toolbar = parent.append_child("widget");
  set_attr(toolbar, "class", "QToolBar");
  set_attr(toolbar, "name", tb_name);

  for(const auto& entry : td.entries)
  {
    if(entry.is_separator)
      add_tag(toolbar, "addaction", "separator");
    else if(!entry.id.empty())
      add_tag(toolbar, "addaction", entry.id);
  }
}

void generator::write_actions(pugi::xml_node& parent, const rc_file& file)
{
  std::map<std::string, bool> actions_defined;

  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<menu_data>(res.data))
    {
      const auto& md = std::get<menu_data>(res.data);
      std::function<void(const std::vector<menu_entry>&)> collect = [&](const std::vector<menu_entry>& entries)
      {
        for(const auto& entry : entries)
          if(std::holds_alternative<std::shared_ptr<popup>>(entry.item))
            collect(std::get<std::shared_ptr<popup>>(entry.item)->entries);
          else if(std::holds_alternative<menu_item>(entry.item))
            if(const auto& mi = std::get<menu_item>(entry.item);
               !mi.id.empty() && mi.text != "-")
              actions_defined[mi.id] = true;
      };
      collect(md.entries);
    }

    if(std::holds_alternative<toolbar_data>(res.data))
    {
      const auto& td = std::get<toolbar_data>(res.data);
      for(const auto& entry : td.entries)
      {
        if(!entry.is_separator && !entry.id.empty())
          actions_defined[entry.id] = true;
      }
    }
  }

  for(const auto& [id, _] : actions_defined)
  {
    auto action = add_tag(parent, "action", id);

    std::string display_text = id;
    auto text_it = m_menu_text_map.find(id);
    if(text_it != m_menu_text_map.end())
      display_text = text_it->second;

    add_property_string(action, "text", display_text);

    auto acc_it = m_accelerator_map.find(id);
    if(acc_it != m_accelerator_map.end() && !acc_it->second.empty())
      add_property_string(action, "shortcut", acc_it->second);

    auto str_it = m_string_table_map.find(id);
    if(str_it != m_string_table_map.end() && !str_it->second.empty())
      add_property_string(action, "toolTip", str_it->second);

    auto dis_it = m_menu_disabled_map.find(id);
    if(dis_it != m_menu_disabled_map.end() && dis_it->second)
      add_property_bool(action, "enabled", false);

    auto chk_it = m_menu_checked_map.find(id);
    if(chk_it != m_menu_checked_map.end() && chk_it->second)
      add_property_bool(action, "checkable", true);
  }
}

std::string generator::map_vk_to_qt(const std::string& vk_code)
{
  std::string key = to_upper(vk_code);

  if(key.size() >= 3 && key[0] == '0' && key[1] == 'X')
  {
    std::string hex_str = key.substr(2);
    try
    {
      unsigned long val = std::stoul(hex_str, nullptr, 16);
      if(val >= 0x20 && val <= 0x7E)
        return std::string(1, static_cast<char>(val));
      return "";
    }
    catch(...)
    {
    }
    return "";
  }

  static const std::unordered_map<std::string, std::string> keyMap =
  {
    // Modifiers
    { "CONTROL", "Ctrl" },
    { "VK_CONTROL", "Ctrl" },
    { "VK_LCONTROL", "Ctrl" },
    { "VK_RCONTROL", "Ctrl" },
    { "VK_SHIFT", "Shift" },
    { "VK_LSHIFT", "Shift" },
    { "VK_RSHIFT", "Shift" },
    { "VK_MENU", "Alt" },
    { "VK_LMENU", "Alt" },
    { "VK_RMENU", "Alt" },

    // --- Alphanumeric Keys ---
    { "VK_BACK",     "Backspace" },
    { "VK_TAB",      "Tab" },
    { "VK_RETURN",   "Return" },
    { "VK_ESCAPE",   "Esc" },
    { "VK_SPACE",    "Space" },
    { " ",           "Space" }, // tricky!

    // --- Navigation & Editing ---
    { "VK_PRIOR",    "PgUp" },
    { "VK_NEXT",     "PgDown" },
    { "VK_END",      "End" },
    { "VK_HOME",     "Home" },
    { "VK_LEFT",     "Left" },
    { "VK_UP",       "Up" },
    { "VK_RIGHT",    "Right" },
    { "VK_DOWN",     "Down" },
    { "VK_SNAPSHOT", "Print" },
    { "VK_INSERT",   "Ins" },
    { "VK_DELETE",   "Del" },

    // --- Why would you bind these? ---
    { "VK_HELP",     "Help" },
    { "VK_PAUSE",    "Pause" },
    { "VK_CAPITAL",  "Caps Lock" },

    // --- Letters (A-Z) ---
    { "VK_A", "A" }, { "VK_B", "B" }, { "VK_C", "C" }, { "VK_D", "D" },
    { "VK_E", "E" }, { "VK_F", "F" }, { "VK_G", "G" }, { "VK_H", "H" },
    { "VK_I", "I" }, { "VK_J", "J" }, { "VK_K", "K" }, { "VK_L", "L" },
    { "VK_M", "M" }, { "VK_N", "N" }, { "VK_O", "O" }, { "VK_P", "P" },
    { "VK_Q", "Q" }, { "VK_R", "R" }, { "VK_S", "S" }, { "VK_T", "T" },
    { "VK_U", "U" }, { "VK_V", "V" }, { "VK_W", "W" }, { "VK_X", "X" },
    { "VK_Y", "Y" }, { "VK_Z", "Z" },

    // --- Numbers (0-9) ---
    { "VK_0", "0" }, { "VK_1", "1" }, { "VK_2", "2" }, { "VK_3", "3" },
    { "VK_4", "4" }, { "VK_5", "5" }, { "VK_6", "6" }, { "VK_7", "7" },
    { "VK_8", "8" }, { "VK_9", "9" },

    // --- Function Keys (F1-F24) ---
    { "VK_F1",  "F1" },  { "VK_F2",  "F2" },  { "VK_F3",  "F3" },  { "VK_F4",  "F4" },
    { "VK_F5",  "F5" },  { "VK_F6",  "F6" },  { "VK_F7",  "F7" },  { "VK_F8",  "F8" },
    { "VK_F9",  "F9" },  { "VK_F10", "F10" }, { "VK_F11", "F11" }, { "VK_F12", "F12" },
    { "VK_F13", "F13" }, { "VK_F14", "F14" }, { "VK_F15", "F15" }, { "VK_F16", "F16" },
    { "VK_F17", "F17" }, { "VK_F18", "F18" }, { "VK_F19", "F19" }, { "VK_F20", "F20" },
    { "VK_F21", "F21" }, { "VK_F22", "F22" }, { "VK_F23", "F23" }, { "VK_F24", "F24" },

    // --- Numpad Keys ---
    { "VK_NUMPAD0", "Num+0" },
    { "VK_NUMPAD1", "Num+1" },
    { "VK_NUMPAD2", "Num+2" },
    { "VK_NUMPAD3", "Num+3" },
    { "VK_NUMPAD4", "Num+4" },
    { "VK_NUMPAD5", "Num+5" },
    { "VK_NUMPAD6", "Num+6" },
    { "VK_NUMPAD7", "Num+7" },
    { "VK_NUMPAD8", "Num+8" },
    { "VK_NUMPAD9", "Num+9" },
    { "VK_MULTIPLY", "*" },
    { "VK_ADD",      "+" },
    { "VK_SEPARATOR","." },
    { "VK_SUBTRACT", "-" },
    { "VK_DECIMAL",  "." },
    { "VK_DIVIDE",   "/" },


/* === mapped to physical keys ===
  VK_STARTKEY
  VK_CONTEXTKEY
    { "VK_OEM_PLUS",   "+" },    // '+' any country
    { "VK_OEM_COMMA",  "," },    // ',' any country
    { "VK_OEM_MINUS",  "-" },    // '-' any country
    { "VK_OEM_PERIOD", "." },    // '.' any country

    { "VK_OEM_1",      ";" },    // OEM specific (usually ';:')
    { "VK_OEM_2",      "/" },    // OEM specific (usually '/?')
    { "VK_OEM_3",      "`" },    // OEM specific (usually '`~')
    { "VK_OEM_4",      "[" },    // OEM specific (usually '[{')
    { "VK_OEM_5",      "\\" },   // OEM specific (usually '\|')
    { "VK_OEM_6",      "]" },    // OEM specific (usually ']}')
    { "VK_OEM_7",      "'" },    // OEM specific (usually '"'')
*/
  };

  if (auto it = keyMap.find(key); it != keyMap.end())
    return it->second;

  // directly mapped ASCII characters
  if(key.size() == 1 && key[0] >= 0x21 && key[0] <= 0x7E)
    return key;

  if(key.size() == 2 && key[0] == '^' && key[1] >= 'A' && key[1] <= 'Z')
    return std::format("Ctrl+{}", key[1]);

  std::set<std::string> unmappable =
  {
    "VK_RWIN",
    "VK_LWIN",
    "VK_STARTKEY",
    "VK_CONTEXTKEY",
    "VK_OEM_PLUS",
    "VK_OEM_COMMA",
    "VK_OEM_MINUS",
    "VK_OEM_PERIOD",
    "VK_OEM_1",
    "VK_OEM_2",
    "VK_OEM_3",
    "VK_OEM_4",
    "VK_OEM_5",
    "VK_OEM_6",
    "VK_OEM_7",
  };

  if(unmappable.contains(key))
    throw std::runtime_error(std::format("Unmappable key for .ui resource files: {}", key));

  throw std::runtime_error(std::format("Unsupported key: {}", key));
}

std::string generator::strip_accelerator(const std::string& text) const
{
  std::string::size_type tab_pos = text.find('\t');
  if(tab_pos != std::string::npos)
    return text.substr(0, tab_pos);
  return text;
}

std::string generator::substitute_font(const std::string& font_name)
{
  static const std::map<std::string, std::string> font_map = {
    {"Arial", "Liberation Sans"},
    {"Microsoft Sans Serif", "Liberation Sans"},
    {"MS Sans Serif", "Liberation Sans"},
    {"MS Shell Dlg", "Liberation Sans"},
    {"MS Shell Dlg 2", "Liberation Sans"},
    {"Tahoma", "Liberation Sans"},
    {"Segoe UI", "Liberation Sans"},
    {"Verdana", "Liberation Sans"},
    {"Times New Roman", "Liberation Serif"},
    {"Courier New", "Liberation Mono"},
    {"Courier", "Liberation Mono"},
    {"Consolas", "Liberation Mono"},
    {"Lucida Console", "Liberation Mono"},
    {"Calibri", "Carlito"},
    {"Georgia", "Liberation Serif"},
    {"Small Fonts", "Liberation Sans"},
    {"System", "Liberation Sans"},
    {"Roman", "Liberation Serif"},
    {"Script", "Liberation Sans"},
    {"Modern", "Liberation Mono"},
  };

  auto it = font_map.find(font_name);
  if(it != font_map.end())
    return it->second;
  return font_name;
}

bool generator::load_font_substitution_list(const std::filesystem::path filepath)
{

  return false;
}

}


