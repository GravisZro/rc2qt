#include "rc_generator.h"
#include "rc_ast.h"
#include "rc_constants.h"
#include "rc_helpers.h"

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

bool generator::generate_all(const rc_file& file, const std::string& output_dir, const std::string& rc_basename)
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
      std::filesystem::create_directories(std::filesystem::path(output_dir) / rc_basename);
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
    {
      short_id = std::format("dialog_{}", dialog_index);
    }

    ++dialog_index;

    std::string unique_short_id = short_id;
    int suffix = 2;
    while(used_short_ids.count(unique_short_id))
      unique_short_id = std::format("{}_{}", short_id, suffix++);
    used_short_ids.insert(unique_short_id);

    std::filesystem::path filename = std::filesystem::path(output_dir) / rc_basename / (unique_short_id + ".ui");

    m_name_counts.clear();
    m_action_counter = 0;
    m_menubar_node = pugi::xml_node();


    pugi::xml_document doc;
    pugi::xml_node ui = doc.append_child("ui");
    ui.append_attribute("version") = "4.0";
    ui.append_child("class").text() = "Form";

    write_dialog(ui, res);

    pugi::xml_node root_widget = ui.child("widget");

    std::string menu_id = find_statement_id(dd, "MENU");

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

          std::string event_upper = a.event;
          std::transform(event_upper.begin(), event_upper.end(), event_upper.begin(), ::toupper);

          bool has_ctrl = false;
          bool has_shift = false;
          bool has_alt = false;

          for(const auto& mod : a.modifiers)
          {
            std::string mod_upper = mod;
            std::transform(mod_upper.begin(), mod_upper.end(), mod_upper.begin(), ::toupper);
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
                std::string f_upper = f;
                std::transform(f_upper.begin(), f_upper.end(), f_upper.begin(), ::toupper);
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
              std::string f_upper = f;
              std::transform(f_upper.begin(), f_upper.end(), f_upper.begin(), ::toupper);
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
        std::string kw_upper = stmt.keyword;
        std::transform(kw_upper.begin(), kw_upper.end(), kw_upper.begin(), ::toupper);
        if(kw_upper == "STYLE" && has_style(stmt.value, "DS_CONTROL"))
          is_ds_control = true;
      }
      if(is_ds_control)
        m_ds_control_dialogs[res.id] = &dd;
    }
  }
}

bool generator::generate_qrc(const rc_file& file, const std::string& output_path, const std::string& ui_path)
{
  std::vector<std::string> paths;
  if(!ui_path.empty())
    paths.push_back(ui_path);
  return generate_qrc(file, output_path, paths);
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
      file_node.append_attribute("alias") = res.id.c_str();
      std::string normalized = res.filename;
      std::replace(normalized.begin(), normalized.end(), '\\', '/');
      std::filesystem::path abs_path = std::filesystem::absolute(normalized);
      std::filesystem::path rel = abs_path.lexically_relative(qrc_dir_fs);
      std::string rel_str = rel.empty() ? normalized : rel.generic_string();
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
  widget.append_attribute("class") = "QDialog";
  widget.append_attribute("name") = dialog_name.c_str();

  write_dialog_properties(widget, dd);

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

  std::vector<bool> written(dd.controls.size(), false);

  for(int gi : groupbox_indices)
  {
    write_control(widget, dd.controls[gi], dialog_name);
    written[gi] = true;

    pugi::xml_node gb_widget = widget.last_child();

    for(size_t i = 0; i < dd.controls.size(); ++i)
    {
      if(parent_groupbox[i] != gi)
        continue;
      if(written[i])
        continue;

      const auto& gb = dd.controls[gi];
      const auto& ctrl = dd.controls[i];

      control relative = ctrl;
      relative.x = ctrl.x - gb.x;
      relative.y = ctrl.y - gb.y - 4;

      write_control(gb_widget, relative, dialog_name);
      written[i] = true;
    }
  }

  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    if(!written[i])
    {
      write_control(widget, dd.controls[i], dialog_name);
      written[i] = true;
    }
  }
}

void generator::write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd)
{
  std::string font_family = find_statement_text(dd, "FONT");
  if(font_family.empty())
    font_family = "MS Sans Serif";

  font_family = map_ms_font(font_family);

  int font_size = find_statement_numeric(dd, "FONT", 8);
  int font_weight = find_statement_numeric2(dd, "FONT", 0);
  bool font_bold = (font_weight >= 700);
  bool font_italic = find_statement_italic(dd, "FONT");
  set_dlu_factors(font_family, font_size > 0 ? font_size : 8);

  int px = dlu_to_pixel_x(dd.x);
  int py = dlu_to_pixel_y(dd.y);
  int pw = dlu_to_pixel_x(dd.width);
  int ph = dlu_to_pixel_y(dd.height);
  add_property_rect(widget, px, py, pw, ph);

  std::string caption = find_statement_text(dd, "CAPTION");
  if(!caption.empty())
    add_property_string(widget, "windowTitle", caption);

  if(font_size > 0)
    add_property_font(widget, font_family, font_size, font_bold, font_italic);

  std::vector<std::string> flags;
  bool is_fixed_size = false;

  if(has_dialog_flag(dd, "STYLE", "DS_MODALFRAME") ||
     has_dialog_flag(dd, "EXSTYLE", "WS_EX_DLGMODALFRAME"))
  {
    flags.push_back("Qt::Dialog");
    flags.push_back("Qt::WindowCloseButtonHint");
    is_fixed_size = true;
  }

  if(has_dialog_flag(dd, "STYLE", "WS_MINIMIZEBOX"))
    flags.push_back("Qt::WindowMinimizeButtonHint");

  if(has_dialog_flag(dd, "STYLE", "WS_MAXIMIZEBOX"))
    flags.push_back("Qt::WindowMaximizeButtonHint");

  if(has_dialog_flag(dd, "EXSTYLE", "WS_EX_CONTEXTHELP"))
    flags.push_back("Qt::WindowContextHelpButtonHint");

  if(has_dialog_flag(dd, "EXSTYLE", "WS_EX_TOPMOST"))
    flags.push_back("Qt::WindowStaysOnTopHint");

  if(has_dialog_flag(dd, "EXSTYLE", "WS_EX_TOOLWINDOW"))
    flags.push_back("Qt::Tool");

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
    int ph = dlu_to_pixel_y(dd.height);
    add_property_size(widget, "minimumSize", pw, ph);
    add_property_size(widget, "maximumSize", pw, ph);
  }

  bool visible = has_dialog_flag(dd, "STYLE", "WS_VISIBLE");
  if(visible)
    add_property_bool(widget, "visible", true);

  bool enabled = !has_dialog_flag(dd, "STYLE", "WS_DISABLED");
  add_property_bool(widget, "enabled", enabled);
}

std::set<std::string> generator::id_words(const std::string& id) const
{
  std::set<std::string> words;
  std::string upper = id;
  std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

  for(const auto& prefix : {"IDD_", "DLG_", "IDC_", "IDM_"})
  {
    if(upper.size() > strlen(prefix) && upper.substr(0, strlen(prefix)) == prefix)
    {
      upper = upper.substr(strlen(prefix));
      break;
    }
  }

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

  std::string upper1 = id1;
  std::transform(upper1.begin(), upper1.end(), upper1.begin(), ::toupper);
  std::string upper2 = id2;
  std::transform(upper2.begin(), upper2.end(), upper2.begin(), ::toupper);

  for(const auto& prefix : {"IDD_", "DLG_", "IDC_", "IDM_"})
  {
    if(upper1.size() > strlen(prefix) && upper1.substr(0, strlen(prefix)) == prefix)
    {
      upper1 = upper1.substr(strlen(prefix));
      break;
    }
  }
  for(const auto& prefix : {"IDD_", "DLG_", "IDC_", "IDM_"})
  {
    if(upper2.size() > strlen(prefix) && upper2.substr(0, strlen(prefix)) == prefix)
    {
      upper2 = upper2.substr(strlen(prefix));
      break;
    }
  }

  for(const auto& w : words1)
  {
    if(upper2.find(w) != std::string::npos)
      return true;
  }
  for(const auto& w : words2)
  {
    if(upper1.find(w) != std::string::npos)
      return true;
  }

  return false;
}

void generator::write_control(pugi::xml_node& parent, const control& ctrl, const std::string& dialog_name)
{
  std::string qt_class = map_keyword_to_widget(ctrl.keyword);

  if(qt_class.empty() && ctrl.keyword == "CONTROL")
    qt_class = map_class_to_widget(ctrl.class_name, ctrl.style);

  if(qt_class.empty())
    qt_class = "QWidget";

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

  int px = dlu_to_pixel_x(ctrl.x);
  int py = dlu_to_pixel_y(ctrl.y);
  int pw = dlu_to_pixel_x(ctrl.width);
  int ph = dlu_to_pixel_y(ctrl.height);
  add_property_rect(widget, px, py, pw, ph);

  if(!ctrl.text.empty())
    add_property_string(widget, "text", ctrl.text);

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
      std::string dlg_id_upper = dlg_id;
      std::transform(dlg_id_upper.begin(), dlg_id_upper.end(), dlg_id_upper.begin(), ::toupper);

      for(const auto& stmt : dd.statements)
      {
        std::string kw_upper = stmt.keyword;
        std::transform(kw_upper.begin(), kw_upper.end(), kw_upper.begin(), ::toupper);
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
      tab_attr.append_attribute("name") = "title";
      tab_attr.append_child("string").text() = tab_title.c_str();

      pugi::xml_node tab_prop = tab_widget.append_child("property");
      tab_prop.append_attribute("name") = "geometry";
      pugi::xml_node tab_rect = tab_prop.append_child("rect");
      tab_rect.append_child("x").text() = 0;
      tab_rect.append_child("y").text() = 0;
      tab_rect.append_child("width").text() = dlu_to_pixel_x(dd.width);
      tab_rect.append_child("height").text() = dlu_to_pixel_y(dd.height);

      for(const auto& child_ctrl : dd.controls)
      {
        int cx = dlu_to_pixel_x(child_ctrl.x);
        int cy = dlu_to_pixel_y(child_ctrl.y);
        int cw = dlu_to_pixel_x(child_ctrl.width);
        int ch = dlu_to_pixel_y(child_ctrl.height);

        std::string child_class = map_keyword_to_widget(child_ctrl.keyword);
        if(child_class.empty() && child_ctrl.keyword == "CONTROL")
          child_class = map_class_to_widget(child_ctrl.class_name, child_ctrl.style);
        if(child_class.empty())
          child_class = "QWidget";

        std::string child_name = unique_name(child_ctrl.id);
        pugi::xml_node child_widget = add_widget(tab_widget, child_class, child_name);
        add_property_rect(child_widget, cx, cy, cw, ch);

        if(!child_ctrl.text.empty())
          add_property_string(child_widget, "text", child_ctrl.text);
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

pugi::xml_node generator::add_widget(pugi::xml_node& parent, const std::string& qt_class, const std::string& name)
{
  pugi::xml_node widget = parent.append_child("widget");
  widget.append_attribute("class") = qt_class.c_str();
  widget.append_attribute("name") = name.c_str();
  return widget;
}

void generator::add_property_rect(pugi::xml_node& widget, int x, int y, int width, int height)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = "geometry";
  pugi::xml_node rect = prop.append_child("rect");
  rect.append_child("x").text() = x;
  rect.append_child("y").text() = y;
  rect.append_child("width").text() = width;
  rect.append_child("height").text() = height;
}

void generator::add_property_string(pugi::xml_node& widget, const std::string& name, const std::string& value)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = name.c_str();
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

void generator::add_property_bool(pugi::xml_node& widget, const std::string& name, bool value)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = name.c_str();
  prop.append_child("bool").text() = value ? "true" : "false";
}

void generator::add_property_int(pugi::xml_node& widget, const std::string& name, int value)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = name.c_str();
  prop.append_child("number").text() = value;
}

void generator::add_property_size(pugi::xml_node& widget, const std::string& name, int width, int height)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = name.c_str();
  pugi::xml_node size = prop.append_child("size");
  size.append_child("width").text() = width;
  size.append_child("height").text() = height;
}

void generator::add_property_set(pugi::xml_node& widget, const std::string& name, const std::string& value)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = name.c_str();
  prop.append_child("set").text() = value.c_str();
}

void generator::add_property_enum(pugi::xml_node& widget, const std::string& name, const std::string& value)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = name.c_str();
  prop.append_child("enum").text() = value.c_str();
}

void generator::add_property_font(pugi::xml_node& widget, const std::string& family, int pointsize, bool bold, bool italic)
{
  pugi::xml_node prop = widget.append_child("property");
  prop.append_attribute("name") = "font";
  pugi::xml_node font = prop.append_child("font");
  font.append_child("family").text() = family.c_str();
  font.append_child("pointsize").text() = pointsize;
  font.append_child("bold").text() = bold ? "true" : "false";
  font.append_child("italic").text() = italic ? "true" : "false";
}

std::string generator::map_keyword_to_widget(const std::string& keyword)
{
  if(keyword == "PUSHBUTTON" || keyword == "DEFPUSHBUTTON")
    return "QPushButton";
  if(keyword == "CHECKBOX" || keyword == "AUTOCHECKBOX" || keyword == "AUTO3STATE" || keyword == "STATE3")
    return "QCheckBox";
  if(keyword == "RADIOBUTTON" || keyword == "AUTORADIOBUTTON")
    return "QRadioButton";
  if(keyword == "GROUPBOX")
    return "QGroupBox";
  if(keyword == "LTEXT" || keyword == "CTEXT" || keyword == "RTEXT" || keyword == "ICON")
    return "QLabel";
  if(keyword == "EDITTEXT")
    return "QLineEdit";
  if(keyword == "LISTBOX")
    return "QListWidget";
  if(keyword == "COMBOBOX")
    return "QComboBox";
  if(keyword == "SCROLLBAR")
    return "QScrollBar";
  if(keyword == "PUSHBOX")
    return "QPushButton";
  return "";
}

std::string generator::map_class_to_widget(const std::string& class_name, const style_expr& style)
{
  std::string lower = class_name;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  if(lower == "button")
  {
    if(has_style(style, "BS_GROUPBOX"))
      return "QGroupBox";
    if(has_style(style, "BS_CHECKBOX") || has_style(style, "BS_AUTOCHECKBOX") ||
       has_style(style, "BS_AUTO3STATE") || has_style(style, "BS_3STATE"))
      return "QCheckBox";
    if(has_style(style, "BS_RADIOBUTTON") || has_style(style, "BS_AUTORADIOBUTTON"))
      return "QRadioButton";
    return "QPushButton";
  }

  if(lower == "static")
  {
    return "QLabel";
  }

  if(lower == "edit")
  {
    if(has_style(style, "ES_MULTILINE"))
      return "QTextEdit";
    return "QLineEdit";
  }

  if(lower == "listbox")
    return "QListWidget";
  if(lower == "combobox")
    return "QComboBox";
  if(lower == "comboboxex32")
    return "QComboBox";
  if(lower == "scrollbar")
    return "QScrollBar";

  if(lower == "systabcontrol32")
    return "QTabWidget";
  if(lower == "systreeview32")
    return "QTreeWidget";
  if(lower == "syslistview32")
    return "QTableWidget";
  if(lower == "msctls_progress32")
    return "QProgressBar";
  if(lower == "msctls_trackbar32")
    return "QSlider";
  if(lower == "msctls_updown32")
    return "QSpinBox";
  if(lower == "sysdatetimepick32")
    return "QDateTimeEdit";
  if(lower == "sysmonthcal32")
    return "QCalendarWidget";
  if(lower == "syslink")
    return "QLabel";
  if(lower == "toolbarwindow32")
    return "QToolBar";
  if(lower == "rebarwindow32")
    return "QToolBar";
  if(lower == "tooltips_class32")
    return "QWidget";
  if(lower == "#128")
    return map_class_to_widget("BUTTON", style);
  if(lower == "#129")
    return map_class_to_widget("EDIT", style);
  if(lower == "#130")
    return "QLabel";
  if(lower == "#131")
    return "QListWidget";
  if(lower == "#132")
    return "QScrollBar";
  if(lower == "#133")
    return "QComboBox";
  if(lower == "#32774")
    return "QPushButton";
  if(lower == "#32768")
    return "QWidget";
  if(lower == "sysanimate32")
    return "QLabel";
  if(lower == "syspager")
    return "QStackedWidget";
  if(lower == "richedit" || lower == "richedit20a" || lower == "richedit20w" || lower == "richedit50w")
    return "QTextEdit";
  if(lower == "msctls_statusbar32")
    return "QStatusBar";
  if(lower == "sysheader32")
    return "QHeaderView";
  if(lower == "msctls_hotkey32")
    return "QWidget";
  if(lower == "nativefontctl")
    return "QWidget";

  return "QWidget";
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

int generator::dlu_to_pixel_x(int dlu) const
{
  return static_cast<int>(dlu * m_dlu_x_factor);
}

int generator::dlu_to_pixel_y(int dlu) const
{
  return static_cast<int>(dlu * m_dlu_y_factor);
}

void generator::set_dlu_factors(const std::string& font_family, int font_size)
{
  struct font_metrics
  {
    double avg_width;
    double avg_height;
  };

  static const std::map<std::string, font_metrics> known_fonts = {
    {"MS Sans Serif", {6.5, 13.0}},
    {"Liberation Sans", {6.5, 13.0}},
    {"Arial", {6.5, 13.0}},
    {"Tahoma", {6.0, 15.0}},
    {"Segoe UI", {7.0, 15.0}},
    {"MS Shell Dlg", {6.5, 13.0}},
    {"MS Shell Dlg 2", {6.5, 13.0}},
    {"Liberation Mono", {6.0, 13.0}},
    {"Courier New", {6.0, 13.0}},
    {"Liberation Serif", {6.5, 13.0}},
    {"Times New Roman", {6.5, 13.0}},
    {"Verdana", {6.5, 13.0}},
    {"Carlito", {6.5, 13.0}},
    {"Calibri", {7.0, 15.0}},
    {"Georgia", {7.0, 13.0}},
  };

  auto it = known_fonts.find(font_family);
  if(it != known_fonts.end())
  {
    double scale = static_cast<double>(font_size) / 8.0;
    m_dlu_x_factor = (it->second.avg_width * scale) / 4.0;
    m_dlu_y_factor = (it->second.avg_height * scale) / 8.0;
  }
  else
  {
    m_dlu_x_factor = 1.75;
    m_dlu_y_factor = 1.75;
  }
}

bool generator::has_style(const style_expr& style, const std::string& flag) const
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

std::string generator::find_statement_text(const dialog_data& dd, const std::string& keyword) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
      return s.text_value;
  }
  return "";
}

std::string generator::find_statement_id(const dialog_data& dd, const std::string& keyword) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
      return s.id_value;
  }
  return "";
}

int generator::find_statement_numeric(const dialog_data& dd, const std::string& keyword, int default_value) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
    {
      if(s.value.resolved_value >= 0)
        return static_cast<int>(s.value.resolved_value);
      if(s.numeric_value > 0)
        return static_cast<int>(s.numeric_value);
      return default_value;
    }
  }
  return default_value;
}

int generator::find_statement_numeric2(const dialog_data& dd, const std::string& keyword, int default_value) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
    {
      if(s.numeric_value2 > 0)
        return static_cast<int>(s.numeric_value2);
      return default_value;
    }
  }
  return default_value;
}

bool generator::find_statement_italic(const dialog_data& dd, const std::string& keyword) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
      return s.italic;
  }
  return false;
}

const style_expr* generator::find_statement_style(const dialog_data& dd, const std::string& keyword) const
{
  for(const auto& s : dd.statements)
  {
    if(s.keyword == keyword)
      return &s.value;
  }
  return nullptr;
}

bool generator::has_dialog_flag(const dialog_data& dd, const std::string& keyword, const std::string& flag) const
{
  const style_expr* expr = find_statement_style(dd, keyword);
  if(expr)
    return has_style(*expr, flag);
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
    m_menubar_node.append_attribute("class") = "QMenuBar";
    m_menubar_node.append_attribute("name") = "menubar";

    pugi::xml_node geom = m_menubar_node.append_child("property");
    geom.append_attribute("name") = "geometry";
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
      menu.append_attribute("class") = "QMenu";
      menu.append_attribute("name") = menu_name.c_str();

      pugi::xml_node title = menu.append_child("property");
      title.append_attribute("name") = "title";
      title.append_child("string").text() = popup_ptr->text.c_str();

      write_menu_entries(menu, popup_ptr->entries);

      pugi::xml_node addaction = m_menubar_node.append_child("addaction");
      addaction.append_attribute("name") = menu_name.c_str();
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
      {
        pugi::xml_node addaction = menu_node.append_child("addaction");
        addaction.append_attribute("name") = "separator";
        continue;
      }

      std::string action_name = mi.id.empty()
        ? std::format("action{}", m_action_counter++)
        : mi.id;

      pugi::xml_node addaction = menu_node.append_child("addaction");
      addaction.append_attribute("name") = action_name.c_str();
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
      sub_menu.append_attribute("class") = "QMenu";
      sub_menu.append_attribute("name") = sub_name.c_str();

      pugi::xml_node title = sub_menu.append_child("property");
      title.append_attribute("name") = "title";
      title.append_child("string").text() = sub->text.c_str();

      write_menu_entries(sub_menu, sub->entries);

      pugi::xml_node addaction = menu_node.append_child("addaction");
      addaction.append_attribute("name") = sub_name.c_str();
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
  toolbar.append_attribute("class") = "QToolBar";
  toolbar.append_attribute("name") = tb_name.c_str();

  for(const auto& entry : td.entries)
  {
    if(entry.is_separator)
    {
      pugi::xml_node addaction = toolbar.append_child("addaction");
      addaction.append_attribute("name") = "separator";
    }
    else if(!entry.id.empty())
    {
      pugi::xml_node addaction = toolbar.append_child("addaction");
      addaction.append_attribute("name") = entry.id.c_str();
    }
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
        {
          if(std::holds_alternative<menu_item>(entry.item))
          {
            const auto& mi = std::get<menu_item>(entry.item);
            if(!mi.id.empty() && mi.text != "-")
              actions_defined[mi.id] = true;
          }
          else if(std::holds_alternative<std::shared_ptr<popup>>(entry.item))
          {
            auto sub = std::get<std::shared_ptr<popup>>(entry.item);
            collect(sub->entries);
          }
        }
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
    pugi::xml_node action = parent.append_child("action");
    action.append_attribute("name") = id.c_str();

    std::string display_text = id;
    auto text_it = m_menu_text_map.find(id);
    if(text_it != m_menu_text_map.end())
      display_text = text_it->second;

    pugi::xml_node text = action.append_child("property");
    text.append_attribute("name") = "text";
    text.append_child("string").text() = display_text.c_str();

    auto acc_it = m_accelerator_map.find(id);
    if(acc_it != m_accelerator_map.end() && !acc_it->second.empty())
    {
      pugi::xml_node shortcut = action.append_child("property");
      shortcut.append_attribute("name") = "shortcut";
      shortcut.append_child("string").text() = acc_it->second.c_str();
    }

    auto str_it = m_string_table_map.find(id);
    if(str_it != m_string_table_map.end() && !str_it->second.empty())
    {
      pugi::xml_node tooltip = action.append_child("property");
      tooltip.append_attribute("name") = "toolTip";
      tooltip.append_child("string").text() = str_it->second.c_str();
    }

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
  std::string key = vk_code;
  std::transform(key.begin(), key.end(), key.begin(), ::toupper);

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

std::string generator::map_ms_font(const std::string& ms_font)
{
  static const std::map<std::string, std::string> font_map = {
    {"Arial", "Liberation Sans"},
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
    {"Courier 10,12,15", "Liberation Mono"},
  };

  auto it = font_map.find(ms_font);
  if(it != font_map.end())
    return it->second;
  return ms_font;
}

}


