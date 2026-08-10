#include "rc_generator.h"
#include "rc_types.h"
#include "rc_constants.h"
#include "utils.h"
#include "xmlhelpers.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <cstring>
#include <format>
#include <stdexcept>

#ifdef HAVE_QT
# include <QtMath>
# include <QFontDatabase>
# include <QFontMetrics>
#elif HAVE_FREETYPE
# include <ft2build.h>
# include <freetype/freetype.h>
# include <fontconfig/fontconfig.h>
#endif

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

  static void add_property_sizepolicy(pugi::xml_node& widget, const std::string& htype, const std::string& vtype)
  {
    pugi::xml_node prop = widget.append_child("property");
    set_attr(prop, "name", "sizePolicy");
    pugi::xml_node policy = prop.append_child("sizepolicy");
    set_attr(policy, "hsizetype", htype);
    set_attr(policy, "vsizetype", vtype);
    policy.append_child("horstretch").text() = 0;
    policy.append_child("verstretch").text() = 0;
  }

  /* Widget classes that should grow when their container is resized. Everything
     else keeps a Preferred size, so a dialog opens close to its RC size. */
  static bool layout_class_stretches(const std::string& qt_class)
  {
    static const std::set<std::string> stretch_classes =
    {
      "QTextEdit", "QPlainTextEdit", "QListWidget", "QListView",
      "QTableWidget", "QTableView", "QTreeWidget", "QTreeView",
      "QScrollArea", "QWidget", "QOpenGLWidget", "QGLWidget",
    };
    return stretch_classes.count(qt_class) != 0;
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

  static bool is_dropdown_combo(const control& ctrl)
  {
    return has_style(ctrl.style, "CBS_DROPDOWN") ||
           has_style(ctrl.style, "CBS_DROPDOWNLIST");
  }

  /* A drop-down COMBOBOX height in the resource file is the fully-expanded
     height (dropdown list open); the collapsed control is one line high. */
  static int combo_closed_height_dlu(const control& ctrl)
  {
    if(!is_dropdown_combo(ctrl))
      return -1;
    return 14;
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

  /* Resolve the Qt widget class for a control. This is the single source of
     truth used by both the writer and the layout pass so that metrics (e.g.
     minimumSizeHint enforcement) always match the emitted widget class. */
  static std::string widget_class_for_control(const control& ctrl)
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

    return qt_class;
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

#ifdef HAVE_QT
    if(m_collect_verify)
    {
      render::verify_input input;
      input.name = res.id;
      input.doc = std::move(doc);
      input.targets = std::move(m_verify_targets);
      input.dialog_width = dlu_to_pixel_x(dd.width);
      input.dialog_height = dlu_to_pixel_y(dd.height);
      m_verify_targets.clear();
      m_verify_inputs.push_back(std::move(input));
      continue;
    }
#endif

    if(doc.save_file(filename.generic_string().c_str(), "  "))
      generated_files.push_back(filename.generic_string());
  }

#ifdef HAVE_QT
  if(m_collect_verify)
    return !m_verify_inputs.empty();
#endif
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
  if(m_use_layouts)
  {
    write_dialog_layout(parent, res);
    return;
  }
  write_dialog_absolute(parent, res);
}

void generator::write_dialog_absolute(pugi::xml_node& parent, const resource& res)
{
  if(!std::holds_alternative<dialog_data>(res.data))
    return;
  const auto& dd = std::get<dialog_data>(res.data);
  std::string dialog_name = res.id;

  pugi::xml_node widget = parent.append_child("widget");
  set_attr(widget, "class", "QDialog");
  set_attr(widget, "name", dialog_name);

  setup_dialog_font(dd);

  std::vector<std::string> qt_classes(dd.controls.size());
  std::vector<int> groupbox_indices;
  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    qt_classes[i] = widget_class_for_control(dd.controls[i]);
    if(qt_classes[i] == "QGroupBox")
      groupbox_indices.push_back(static_cast<int>(i));
  }

   /* Containment is geometry based and therefore independent of the order the
      controls are listed in the RC file. A non-groupbox control belongs to the
      smallest groupbox enclosing its centre point. GROUPBOX controls may nest
      inside other groupboxes: a box is a child of the smallest groupbox that
      contains its rect (allowing a small overshoot past the right or bottom
      edge, see below), and only when that box is strictly larger, which keeps
      the parent relation acyclic. */
  std::vector<int> parent_groupbox = compute_parent_groupbox(dd.controls, qt_classes);

  /* A groupbox node owns the controls nested directly inside it, in RC order
     including any that appear after the box, laid out relative to the box.
     Nested groupboxes form the same structure recursively. The whole hierarchy
     is built before any XML is emitted. */
  struct groupbox_node
  {
    int rc_index = -1;
    int child_pos = -1;
    std::vector<control> children;
    std::vector<std::string> child_classes;
    std::vector<size_t> child_indices;
    std::vector<int> child_group_index;
    std::vector<control_layout> child_layout;
    std::vector<groupbox_node> nested_groups;
    std::vector<std::pair<int, int>> events;
    int extra_height_px = 0;
  };

  auto build_node = [&](auto&& self, groupbox_node& node) -> void
  {
    const control& gb = dd.controls[node.rc_index];
    for(size_t i = 0; i < dd.controls.size(); ++i)
    {
      if(parent_groupbox[i] != node.rc_index)
        continue;

      control relative = dd.controls[i];
      relative.x = relative.x - gb.x;
      relative.y = relative.y - gb.y + 4;

      node.children.push_back(relative);
      node.child_classes.push_back(qt_classes[i]);
      node.child_indices.push_back(i);

      if(qt_classes[i] == "QGroupBox")
      {
        groupbox_node sub;
        sub.rc_index = static_cast<int>(i);
        sub.child_pos = static_cast<int>(node.children.size()) - 1;
        node.child_group_index.push_back(static_cast<int>(node.nested_groups.size()));
        node.nested_groups.push_back(std::move(sub));
      }
      else
      {
        node.child_group_index.push_back(-1);
      }
    }

    for(auto& sub : node.nested_groups)
      self(self, sub);
  };

  /* Post-order: nested boxes are laid out before the box that contains them,
     so each box merges the expansion events and extra height of everything
     nested underneath it. */
  auto layout_node = [&](auto&& self, groupbox_node& node) -> void
  {
    for(auto& sub : node.nested_groups)
      self(self, sub);

    std::vector<int> extra_heights(node.children.size(), 0);
    std::vector<std::pair<int, int>> nested_events;
    for(const auto& sub : node.nested_groups)
    {
      int pos = sub.child_pos;
      extra_heights[pos] = sub.extra_height_px;

      int gb_rel_y_px = dlu_to_pixel_y(node.children[pos].y);
      for(const auto& ev : sub.events)
        nested_events.push_back({ ev.first + gb_rel_y_px, ev.second });
    }

    std::vector<std::pair<int, int>> events;
    layout_control_sizes(node.children, node.child_classes, node.child_layout,
                         &extra_heights, &nested_events, &events);
    node.events = std::move(events);

    if(!m_disable_geometry_adjustments)
    {
      int gb_h_px = dlu_to_pixel_y(dd.controls[node.rc_index].height);
      int max_bottom_rel = 0;
      for(size_t j = 0; j < node.children.size(); ++j)
      {
        int rel_bottom = dlu_to_pixel_y(node.children[j].y) +
                         node.child_layout[j].y_shift_px +
                         node.child_layout[j].height_px;
        if(rel_bottom > max_bottom_rel)
          max_bottom_rel = rel_bottom;
      }
      if(max_bottom_rel > gb_h_px)
        node.extra_height_px = max_bottom_rel - gb_h_px;
    }
  };

  std::vector<control> top_level;
  std::vector<std::string> top_classes;
  std::vector<int> top_extra;
  std::vector<size_t> top_indices;
  std::vector<groupbox_node> root_groups;
  std::vector<int> root_group_pos;

  std::vector<std::pair<int, int>> merged_events;

  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    if(parent_groupbox[i] != -1)
      continue;

    int extra = 0;
    int group_pos = -1;
    if(qt_classes[i] == "QGroupBox")
    {
      groupbox_node node;
      node.rc_index = static_cast<int>(i);
      build_node(build_node, node);
      layout_node(layout_node, node);

      extra = node.extra_height_px;

      int gb_origin_y_px = dlu_to_pixel_y(dd.controls[i].y);
      for(const auto& ev : node.events)
        merged_events.push_back({ ev.first + gb_origin_y_px, ev.second });

      group_pos = static_cast<int>(root_groups.size());
      root_groups.push_back(std::move(node));
    }

    top_level.push_back(dd.controls[i]);
    top_classes.push_back(qt_classes[i]);
    top_extra.push_back(extra);
    top_indices.push_back(i);
    root_group_pos.push_back(group_pos);
  }

  std::vector<control_layout> top_layout;
  layout_control_sizes(top_level, top_classes, top_layout, &top_extra,
                       &merged_events);

  int dialog_ph = dlu_to_pixel_y(dd.height);
  int growth = 0;
  if(!m_disable_geometry_adjustments)
  {
    for(size_t j = 0; j < top_level.size(); ++j)
    {
      int bottom = dlu_to_pixel_y(top_level[j].y) + top_layout[j].y_shift_px + top_layout[j].height_px;
      if(bottom - dialog_ph > growth)
        growth = bottom - dialog_ph;
    }
    if(growth < 0)
      growth = 0;
  }

  write_dialog_properties(widget, dd, growth);

  std::vector<int> top_shift(dd.controls.size(), 0);
  for(size_t j = 0; j < top_indices.size(); ++j)
    top_shift[top_indices[j]] = top_layout[j].y_shift_px;

  std::vector<bool> written(dd.controls.size(), false);

  auto write_node = [&](auto&& self, pugi::xml_node& parent, const groupbox_node& node, const control& ctrl, int y_shift_px) -> void
  {
    int gi = node.rc_index;
    write_control(parent, ctrl, dialog_name, y_shift_px, node.extra_height_px);
    written[gi] = true;

    pugi::xml_node gb_widget = parent.last_child();

    for(size_t j = 0; j < node.children.size(); ++j)
    {
      int sub_index = node.child_group_index[j];
      if(sub_index >= 0)
      {
        self(self, gb_widget, node.nested_groups[sub_index], node.children[j], node.child_layout[j].y_shift_px);
      }
      else
      {
        write_control(gb_widget, node.children[j], dialog_name, node.child_layout[j].y_shift_px);
        written[node.child_indices[j]] = true;
      }
    }
  };

  for(size_t t = 0; t < top_level.size(); ++t)
  {
    if(root_group_pos[t] >= 0)
      write_node(write_node, widget, root_groups[root_group_pos[t]], dd.controls[top_indices[t]], top_layout[t].y_shift_px);
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

std::vector<int> generator::compute_parent_groupbox(const std::vector<control>& controls,
                                                    const std::vector<std::string>& qt_classes) const
{
  std::vector<int> groupbox_indices;
  for(size_t i = 0; i < controls.size(); ++i)
  {
    if(qt_classes[i] == "QGroupBox")
      groupbox_indices.push_back(static_cast<int>(i));
  }

  std::vector<int> parent_groupbox(controls.size(), -1);
  for(size_t i = 0; i < controls.size(); ++i)
  {
    if(qt_classes[i] == "QGroupBox")
      continue;

    const auto& ctrl = controls[i];
    int disp_h_dlu = ctrl.height;
    if(qt_classes[i] == "QComboBox")
    {
      int closed_dlu = combo_closed_height_dlu(ctrl);
      if(closed_dlu > 0)
        disp_h_dlu = closed_dlu;
    }
    int16_t cx = ctrl.x + static_cast<int16_t>(ctrl.width / 2);
    int16_t cy = ctrl.y + static_cast<int16_t>(disp_h_dlu / 2);

    int best_gi = -1;
    long best_area = 0;
    for(int gi : groupbox_indices)
    {
      const auto& gb = controls[gi];
      int16_t gb_x = gb.x;
      int16_t gb_y = gb.y;
      uint16_t gb_w = gb.width;
      uint16_t gb_h = gb.height;

      if(cx >= gb_x && cx < gb_x + static_cast<int16_t>(gb_w) &&
         cy >= gb_y && cy < gb_y + static_cast<int16_t>(gb_h))
      {
        long area = static_cast<long>(gb_w) * gb_h;
        if(best_gi < 0 || area < best_area)
        {
          best_gi = gi;
          best_area = area;
        }
      }
    }
    parent_groupbox[i] = best_gi;
  }

  for(int gi : groupbox_indices)
  {
    const auto& gb = controls[gi];
    long gb_area = static_cast<long>(gb.width) * gb.height;

    int best_gi = -1;
    long best_area = 0;
    for(int gj : groupbox_indices)
    {
      if(gj == gi)
        continue;
      const auto& other = controls[gj];
      long other_area = static_cast<long>(other.width) * other.height;
      if(other_area <= gb_area)
        continue;

      /* RC authors often let a nested box poke a couple of DLUs past the right
         or bottom edge of its container; treat such a box as contained anyway. */
      constexpr int containment_slop_dlu = 2;
      if(gb.x >= other.x && gb.y >= other.y &&
         static_cast<int>(gb.x) + gb.width <= static_cast<int>(other.x) + other.width + containment_slop_dlu &&
         static_cast<int>(gb.y) + gb.height <= static_cast<int>(other.y) + other.height + containment_slop_dlu)
      {
        if(best_gi < 0 || other_area < best_area)
        {
          best_gi = gj;
          best_area = other_area;
        }
      }
    }
    parent_groupbox[gi] = best_gi;
  }

  return parent_groupbox;
}

void generator::control_layout_pixel_size(const control& ctrl, const std::string& qt_class,
                                          int& width_px, int& height_px)
{
  int width_dlu = ctrl.width;
  int height_dlu = ctrl.height;
  if(!m_disable_geometry_adjustments)
  {
    try
    {
      text_fit_info info = fit_text(ctrl.text, width_dlu, height_dlu, qt_class);
      width_dlu = info.width_dlu;
      height_dlu = info.height_dlu;
    }
    catch(const std::runtime_error&)
    {
      width_dlu = ctrl.width;
      height_dlu = ctrl.height;
    }
  }

  width_px = dlu_to_pixel_x(width_dlu);
  height_px = dlu_to_pixel_y(height_dlu);
  if(qt_class == "QComboBox")
  {
    int closed_dlu = combo_closed_height_dlu(ctrl);
    if(closed_dlu > 0)
      height_px = dlu_to_pixel_y(closed_dlu);
  }
  int min_w = min_width_px(qt_class);
  if(width_px < min_w)
    width_px = min_w;
  int min_h = min_height_px(qt_class);
  if(height_px < min_h)
    height_px = min_h;
}

int generator::multiline_edit_min_height(int height_dlu) const
{
  /* RC multiline edit controls are sized with the convention height = 8 DLU
     per text row plus a 10 DLU frame. Express the minimum height in terms of
     the rows the control should contain so it always shows that many lines,
     regardless of the runtime font. */
  int rows = std::max(1, static_cast<int>(std::lround((height_dlu - 10) / 8.0)));
  double line_height = m_font_height;
  double frame = dlu_to_pixel_y(10);
  int rows_height = static_cast<int>(std::lround(rows * line_height + frame));
  return std::max(rows_height, dlu_to_pixel_y(height_dlu));
}

void generator::write_dialog_layout(pugi::xml_node& parent, const resource& res)
{
  if(!std::holds_alternative<dialog_data>(res.data))
    return;
  const auto& dd = std::get<dialog_data>(res.data);
  std::string dialog_name = res.id;

  pugi::xml_node widget = parent.append_child("widget");
  set_attr(widget, "class", "QDialog");
  set_attr(widget, "name", dialog_name);

  setup_dialog_font(dd);

  std::vector<std::string> qt_classes(dd.controls.size());
  for(size_t i = 0; i < dd.controls.size(); ++i)
    qt_classes[i] = widget_class_for_control(dd.controls[i]);

  write_dialog_properties(widget, dd, 0);

  /* In layout mode the dialog keeps its RC size as a minimum so it opens at the
     original dimensions; fixed-size dialogs already got min==max above. */
  bool fixed_size = false;
  if(const dialog_stmt* stmt = find_statement(dd, "STYLE"))
    fixed_size = has_style(stmt->value, "DS_MODALFRAME");
  if(!fixed_size)
  {
    if(const dialog_stmt* stmt = find_statement(dd, "EXSTYLE"))
      fixed_size = has_style(stmt->value, "WS_EX_DLGMODALFRAME");
  }
  if(!fixed_size)
  {
    int pw = dlu_to_pixel_x(dd.width);
    int ph = dlu_to_pixel_y(dd.height);
    add_property_size(widget, "minimumSize", pw, ph);
  }

  std::vector<int> parent_groupbox = compute_parent_groupbox(dd.controls, qt_classes);

  /* Build the container tree (dialog plus every groupbox) with coordinates
     relative to each box, exactly as in the absolute path. Each container is
     then decomposed into box/grid layouts by emit_layout_container(). */
  layout_node root;
  auto build_tree = [&](auto&& self, layout_node& node, int parent_index) -> void
  {
    for(size_t i = 0; i < dd.controls.size(); ++i)
    {
      if(parent_groupbox[i] != parent_index)
        continue;

      control relative = dd.controls[i];
      if(parent_index >= 0)
      {
        const control& gb = dd.controls[parent_index];
        relative.x = relative.x - gb.x;
        relative.y = relative.y - gb.y + 4;
      }

      layout_child child;
      child.ctrl = relative;
      child.qt_class = qt_classes[i];
      if(qt_classes[i] == "QGroupBox")
      {
        child.nested_index = static_cast<int>(node.nested.size());
        node.nested.emplace_back();
        self(self, node.nested.back(), static_cast<int>(i));
      }
      node.children.push_back(std::move(child));
    }
  };

  build_tree(build_tree, root, -1);
  emit_layout_container(widget, root, dialog_name,
                        dlu_to_pixel_x(dd.width), dlu_to_pixel_y(dd.height));
}

void generator::emit_layout_container(pugi::xml_node& container_widget, const layout_node& node,
                                      const std::string& dialog_name,
                                      int container_w, int container_h)
{
  if(node.children.empty())
    return;

  /* Pixel rects feed the layout solver; they match the geometry write_control()
     would emit in absolute mode. */
  std::vector<rc::layout::child> items(node.children.size());
  for(size_t i = 0; i < node.children.size(); ++i)
  {
    const layout_child& child = node.children[i];
    items[i].control_index = static_cast<int>(i);
    items[i].qt_class = child.qt_class;
    items[i].bounds.x = dlu_to_pixel_x(child.ctrl.x);
    items[i].bounds.y = dlu_to_pixel_y(child.ctrl.y);
    control_layout_pixel_size(child.ctrl, child.qt_class, items[i].bounds.w, items[i].bounds.h);
  }

  /* Default: box decomposition + grid subgroups + per-item alignment. The
     stretch and trailing-spacer patterns measurably hurt fidelity (they push
     widgets off their RC positions), so they stay off by default. */
  layout::pattern_flag pattern_flags = rc::layout::pattern_box |
                                           rc::layout::pattern_grid |
                                           rc::layout::pattern_align;
  if(const char* env = std::getenv("RC2QT_PATTERNS"))
    pattern_flags = static_cast<layout::pattern_flag>(std::strtoul(env, nullptr, 0));

  layout::node root = layout::solve_container(items, pattern_flags,
                                              container_w, container_h);
  emit_layout_node(container_widget, root, node, dialog_name, items, pattern_flags,
                   container_widget.attribute("name").value());
}

/* Serialize one layout-tree node under parent (a container widget or an item).
   Leaves are written with write_control(); nested tree nodes recurse into a
   nested <layout> element inside the item. */
void generator::emit_layout_node(pugi::xml_node& parent, const rc::layout::node& ln,
                                 const layout_node& node, const std::string& dialog_name,
                                 const std::vector<rc::layout::child>& items,
                                 layout::pattern_flag pattern_flags,
                                 const std::string& container_name)
{
  const bool is_grid = ln.k == rc::layout::node::kind::grid;
  const bool is_box_x = ln.k == rc::layout::node::kind::box_x;

  pugi::xml_node layout = parent.append_child("layout");
  set_attr(layout, "class", is_grid ? "QGridLayout"
                                    : (is_box_x ? "QHBoxLayout" : "QVBoxLayout"));
  set_attr(layout, "name", unique_name("layout"));
  add_property_int(layout, "spacing", 6);
  add_property_int(layout, "margin", 0);

  if(is_grid && ln.label_column_minwidth > 0)
    set_attr(layout, "columnminimumwidth", ln.label_column_minwidth);

  if(is_grid && (pattern_flags & rc::layout::pattern_stretch) &&
     ln.equal_row_stretch > 0)
  {
    std::string row_stretch;
    for(int r = 0; r < ln.rows; ++r)
      row_stretch += (r ? "," : "") + std::to_string(ln.equal_row_stretch);
    std::string col_stretch;
    for(int c = 0; c < ln.columns; ++c)
      col_stretch += (c ? "," : "") + std::to_string(ln.equal_col_stretch);
    set_attr(layout, "rowstretch", row_stretch.c_str());
    set_attr(layout, "columnstretch", col_stretch.c_str());
  }

  /* Grid items are emitted in row-major order so the XML is stable; box items
     keep the solver's reading order. */
  std::vector<int> order(ln.children.size());
  for(size_t i = 0; i < order.size(); ++i)
    order[i] = static_cast<int>(i);
  if(is_grid)
  {
    std::stable_sort(order.begin(), order.end(), [&](int a, int b)
    {
      const rc::layout::node::cell& ca = ln.cells[a];
      const rc::layout::node::cell& cb = ln.cells[b];
      if(ca.row != cb.row)
        return ca.row < cb.row;
      if(ca.column != cb.column)
        return ca.column < cb.column;
      const rc::layout::rect& ra = items[ln.children[a].control_index].bounds;
      const rc::layout::rect& rb = items[ln.children[b].control_index].bounds;
      if(ra.y != rb.y)
        return ra.y < rb.y;
      return ra.x < rb.x;
    });
  }

  const auto alignment_attr = [&](const rc::layout::node& c) -> std::string
  {
    if(!(pattern_flags & rc::layout::pattern_align))
      return "";
    std::string h;
    std::string v;
    if(c.halign == rc::layout::align_h::left)
      h = "Qt::AlignLeft";
    else if(c.halign == rc::layout::align_h::right)
      h = "Qt::AlignRight";
    else if(c.halign == rc::layout::align_h::center)
      h = "Qt::AlignHCenter";
    if(c.valign == rc::layout::align_v::top)
      v = "Qt::AlignTop";
    else if(c.valign == rc::layout::align_v::bottom)
      v = "Qt::AlignBottom";
    else if(c.valign == rc::layout::align_v::center)
      v = "Qt::AlignVCenter";
    if(h.empty() && v.empty())
      return "";
    return h.empty() ? v : (v.empty() ? h : h + "|" + v);
  };

  for(int k : order)
  {
    const rc::layout::node& child = ln.children[k];
    pugi::xml_node item = layout.append_child("item");
    if(is_grid)
    {
      const rc::layout::node::cell& c = ln.cells[k];
      set_attr(item, "row", c.row);
      set_attr(item, "column", c.column);
      if(c.rowspan > 1)
        set_attr(item, "rowspan", c.rowspan);
      if(c.colspan > 1)
        set_attr(item, "colspan", c.colspan);
    }
    std::string align = alignment_attr(child);
    if(!align.empty())
      set_attr(item, "alignment", align.c_str());

    if(child.k == rc::layout::node::kind::item)
    {
      const int k_ctrl = child.control_index;
      const layout_child& lc = node.children[k_ctrl];
      write_control(item, lc.ctrl, dialog_name, 0, 0, false);

#ifdef HAVE_QT
      if(m_collect_verify)
      {
        render::target t;
        t.x = items[k_ctrl].bounds.x;
        t.y = items[k_ctrl].bounds.y;
        t.w = items[k_ctrl].bounds.w;
        t.h = items[k_ctrl].bounds.h;
        t.container = container_name;
        m_verify_targets[item.last_child().attribute("name").value()] = t;
      }
#endif

      if(lc.nested_index >= 0)
      {
        pugi::xml_node gb_widget = item.last_child();
        emit_layout_container(gb_widget, node.nested[lc.nested_index], dialog_name,
                              items[k_ctrl].bounds.w, items[k_ctrl].bounds.h);
      }
    }
    else
    {
      emit_layout_node(item, child, node, dialog_name, items, pattern_flags,
                       container_name);
    }
  }

  /* Trailing spacer absorbs the free space so the widgets keep their RC
     positions when the container grows. */
  if(ln.spacer_size > 0)
  {
    pugi::xml_node item = layout.append_child("item");
    pugi::xml_node spacer = item.append_child("spacer");
    set_attr(spacer, "name", unique_name("spacer"));
    set_attr(spacer, "orientation", is_box_x ? "Horizontal" : "Vertical");
    pugi::xml_node prop = spacer.append_child("property");
    set_attr(prop, "name", "sizeHint");
    pugi::xml_node size = prop.append_child("size");
    size.append_child("width").text() = is_box_x ? ln.spacer_size : 20;
    size.append_child("height").text() = is_box_x ? 20 : ln.spacer_size;
  }
}

void generator::setup_dialog_font(const dialog_data& dd)
{
  std::string original_font_name = "MS Sans Serif";
  if(const dialog_stmt* stmt = find_statement(dd, "FONT"))
    original_font_name = stmt->text_value;

  std::string font_name;
  if(m_prevent_font_substitution)
    font_name = original_font_name;
  else
    font_name = substitute_font(original_font_name);
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

void generator::write_control(pugi::xml_node& parent, const control& ctrl, const std::string& dialog_name, int y_shift_px, int extra_height_px, bool emit_geometry)
{
  std::string qt_class = widget_class_for_control(ctrl);

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
  if(getenv("RC2QT_DBG_WC") && (ctrl.text == "Completion Rule" || ctrl.text == "Current Goal Item (Must be same type)"))
    fprintf(stderr, "WC: '%s' ctrl.y=%d y_shift=%d py_final=%d extra=%d\n", ctrl.text.c_str(), ctrl.y, y_shift_px, py, extra_height_px);
  int min_w = min_width_px(qt_class);
  if(pw < min_w)
    pw = min_w;
  int min_h = min_height_px(qt_class);
  if(ph < min_h)
    ph = min_h;
  ph += extra_height_px;

  if(emit_geometry)
  {
    add_property_rect(widget, px, py, pw, ph);
  }
  else
  {
    /* In layout mode the geometry rect is replaced by a minimum size (from the
       RC size) plus a size policy, so widgets open at their RC size but reflow
       when the dialog is resized. Multiline edits derive their minimum height
       from the number of text rows they should contain. */
    int layout_ph = ph;
    if(qt_class == "QTextEdit")
      layout_ph = multiline_edit_min_height(ctrl_h_dlu);
    add_property_size(widget, "minimumSize", pw, layout_ph);
    if(layout_class_stretches(qt_class))
      add_property_sizepolicy(widget, "Expanding", "Expanding");
    else
      add_property_sizepolicy(widget, "Preferred", "Preferred");
  }

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

      if(emit_geometry)
      {
        pugi::xml_node tab_prop = tab_widget.append_child("property");
        set_attr(tab_prop, "name", "geometry");
        pugi::xml_node tab_rect = tab_prop.append_child("rect");
        tab_rect.append_child("x").text() = 0;
        tab_rect.append_child("y").text() = 0;
        tab_rect.append_child("width").text() = dlu_to_pixel_x(dd.width);
        tab_rect.append_child("height").text() = dlu_to_pixel_y(dd.height);
      }

      std::vector<std::string> child_classes(dd.controls.size());
      for(size_t i = 0; i < dd.controls.size(); ++i)
      {
        const auto& child_ctrl = dd.controls[i];
        std::string child_class = widget_class_for_control(child_ctrl);
        child_classes[i] = child_class;
      }

      if(!emit_geometry)
      {
        layout_node tab_node;
        for(size_t i = 0; i < dd.controls.size(); ++i)
        {
          layout_child child;
          child.ctrl = dd.controls[i];
          child.qt_class = child_classes[i];
          tab_node.children.push_back(std::move(child));
        }
        emit_layout_container(tab_widget, tab_node, dialog_name,
                              dlu_to_pixel_x(dd.width), dlu_to_pixel_y(dd.height));
      }
      else
      {
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

          int child_min_w = min_width_px(child_class);
          if(cw < child_min_w)
            cw = child_min_w;
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
  if(m_disable_geometry_adjustments)
    return;
  int closed_height_dlu = combo_closed_height_dlu(ctrl);
  bool is_dropdown = is_combo && closed_height_dlu > 0;
  if(is_dropdown)
  {
    // The COMBOBOX height parameter is the fully-expanded height (dropdown list
    // open). A drop-down combo box is collapsed to a one-line field; the list
    // extent is preserved through maxVisibleItems.
    int closed_height = dlu_to_pixel_y(closed_height_dlu);
    int item_height = dlu_to_pixel_y(8);
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

bool generator::load_uimetrics(const std::filesystem::path& filepath)
{
  std::ifstream file(filepath);
  if(!file.is_open())
    return false;

  m_uimetrics.clear();

  std::map<std::string, std::string>* current_section = nullptr;
  std::string line;
  while(std::getline(file, line))
  {
    std::string trimmed = line;
    while(!trimmed.empty() && (trimmed.front() == ' ' || trimmed.front() == '\t' || trimmed.front() == '\r'))
      trimmed.erase(trimmed.begin());
    while(!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t' || trimmed.back() == '\r'))
      trimmed.pop_back();

    if(trimmed.empty() || trimmed.front() == '#')
      continue;

    if(trimmed.front() == '[' && trimmed.back() == ']')
    {
      std::string name = trimmed.substr(1, trimmed.size() - 2);
      current_section = &m_uimetrics[name];
      continue;
    }

    if(current_section == nullptr)
      continue;

    size_t eq = trimmed.find('=');
    if(eq == std::string::npos)
      continue;

    std::string key = trimmed.substr(0, eq);
    std::string value = trimmed.substr(eq + 1);
    while(!key.empty() && (key.back() == ' ' || key.back() == '\t'))
      key.pop_back();
    while(!value.empty() && (value.front() == ' ' || value.front() == '\t'))
      value.erase(value.begin());

    (*current_section)[key] = value;
  }

  auto font_section = m_uimetrics.find("font");
  if(font_section != m_uimetrics.end())
  {
    auto dpi = font_section->second.find("dpi");
    if(dpi != font_section->second.end())
    {
      try
      {
        m_font_dpi = std::stod(dpi->second);
      }
      catch(const std::exception&)
      {
      }
    }
  }

  return true;
}

int generator::min_width_px(const std::string& qt_class) const
{
  if(m_disable_geometry_adjustments)
    return 0;

  auto it = m_uimetrics.find("widget:" + qt_class);
  if(it != m_uimetrics.end())
  {
    auto metric = it->second.find("minimumSizeHintW");
    if(metric != it->second.end())
    {
      try
      {
        // Minimum-size hints are text-area metrics stored in DLU and scaled by
        // the dialog font factor. Negative values (unlaid-out widgets) are
        // ignored and clamp to zero.
        double dlu = std::stod(metric->second);
        if(dlu > 0.0)
          return static_cast<int>(std::lround(dlu * m_dlu_x_factor));
      }
      catch(const std::exception&)
      {
      }
    }
  }
  return 0;
}

int generator::min_height_px(const std::string& qt_class) const
{
  if(m_disable_geometry_adjustments)
    return 0;

  auto it = m_uimetrics.find("widget:" + qt_class);
  if(it != m_uimetrics.end())
  {
    auto metric = it->second.find("minimumSizeHintH");
    if(metric != it->second.end())
    {
      try
      {
        // Minimum-size hints are text-area metrics stored in DLU and scaled by
        // the dialog font factor. Negative values (unlaid-out widgets) are
        // ignored and clamp to zero.
        double dlu = std::stod(metric->second);
        if(dlu > 0.0)
          return static_cast<int>(std::lround(dlu * m_dlu_y_factor));
      }
      catch(const std::exception&)
      {
      }
    }
  }
  return 0;
}

int generator::vertical_margin_px(const std::string& qt_class) const
{
  if(m_disable_geometry_adjustments)
    return 0;

  static const std::map<std::string, int> vertical_margin_map =
  {
    { "QCheckBox", 18 },
  };

  auto it = m_uimetrics.find("widget:" + qt_class);
  if(it != m_uimetrics.end())
  {
    auto metric = it->second.find("clickRectH");
    if(metric != it->second.end())
    {
      try
      {
        // Click-rect height is style-driven and stored as absolute pixels; it
        // does not scale with the dialog font.
        return std::stoi(metric->second);
      }
      catch(const std::exception&)
      {
      }
    }
  }

  auto fallback = vertical_margin_map.find(qt_class);
  if(fallback == vertical_margin_map.end())
    return 0;
  return fallback->second;
}

void generator::layout_control_sizes(const std::vector<control>& controls,
                                     const std::vector<std::string>& qt_classes,
                                     std::vector<control_layout>& layout,
                                     const std::vector<int>* extra_heights,
                                     const std::vector<std::pair<int, int>>* extra_events,
                                     std::vector<std::pair<int, int>>* out_events)
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
    if(!m_disable_geometry_adjustments)
    {
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
    }

    ph[i] = dlu_to_pixel_y(height_dlu);
    if(qt_class == "QComboBox")
    {
      int closed_dlu = combo_closed_height_dlu(ctrl);
      if(closed_dlu > 0)
        ph[i] = dlu_to_pixel_y(closed_dlu);
    }

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

    int expansion = std::max(0, min_h[i] - ph[i]);
    if(expansion > 0)
      events.push_back({ py[i] + ph[i], expansion });
  }
  if(extra_events != nullptr)
    events.insert(events.end(), extra_events->begin(), extra_events->end());
  std::sort(events.begin(), events.end());

  if(out_events != nullptr)
    *out_events = events;

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

  // Set font size. Qt computes the pixel size from the point size and the
  // screen's logical DPI with rounding: pixels = round(pointSize * dpi / 72).
  // The DPI is taken from the measurement font in the uimetrics file so this
  // path reproduces QFontMetrics on the same display.
  int pixel_height = static_cast<int>(std::round(font_size * m_font_dpi / 72.0));
  if(FT_Set_Pixel_Sizes(m_ft_face, 0, pixel_height))
  {
    FT_Done_Face(m_ft_face);
    m_ft_face = nullptr;
    throw std::runtime_error("Failed to set pixel size for FreeType face.");
  }

  // 3. Extract metrics equivalent to QFontMetrics.
  // Qt's QFontMetrics::height() equals ascent + descent (no line gap).
  // FreeType stores these as 26.6 fractional pixels; round each the way Qt
  // rounds its integer ascent/descent values.
  double ascent = std::round(static_cast<double>(m_ft_face->size->metrics.ascender) / 64.0);
  double descent = std::round(static_cast<double>(-m_ft_face->size->metrics.descender) / 64.0);
  double font_height = ascent + descent;
  m_font_height = font_height;
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

  double font_height = m_font_height;
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
  if(m_disable_geometry_adjustments || text.empty())
    return { width_dlu, height_dlu, false };

  const auto [mapped_w, mapped_h] = text_dimensions(text);
  if(mapped_w <= width_dlu && mapped_h <= height_dlu)
    return { width_dlu, height_dlu, false };

  set_current_font(m_mapped_font_name, m_font_size, m_font_weight, m_font_italic);

  if(supports_word_wrap(widget_class))
  {
    // A QLabel reflows onto multiple lines inside the given width instead of
    // growing beyond it, so a long label never exceeds the dialog width.
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
  if(m_disable_geometry_adjustments || text.empty())
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


