#include "rc_generator.h"
#include "rc_ast.h"
#include "rc_constants.h"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>

namespace rc
{

bool generator::generate(const rc_file& file, const std::string& output_path)
{
  collect_global_data(file);

  pugi::xml_document doc;

  pugi::xml_node ui = doc.append_child("ui");
  ui.append_attribute("version") = "4.0";
  ui.append_child("class").text() = "Form";

  pugi::xml_node root_widget;
  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<dialog_data>(res.data))
    {
      if(!root_widget)
      {
        write_dialog(ui, res);
        root_widget = ui.child("widget");
      }
    }
  }

  if(!root_widget)
  {
    root_widget = ui.append_child("widget");
    root_widget.append_attribute("class") = "QWidget";
    root_widget.append_attribute("name") = "Form";
  }

  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<menu_data>(res.data))
      write_menu(root_widget, res);
    if(std::holds_alternative<toolbar_data>(res.data))
      write_toolbar(root_widget, res);
  }

  write_actions(root_widget, file);

  return doc.save_file(output_path.c_str(), "  ");
}

bool generator::generate_all(const rc_file& file, const std::string& output_dir, const std::string& rc_basename)
{
  collect_global_data(file);

  std::vector<std::string> generated_files;

  for(const auto& res : file.resources)
  {
    if(!std::holds_alternative<dialog_data>(res.data))
      continue;

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
      continue;

    std::string dialog_name = res.id;
    std::string sanitized = dialog_name;
    std::replace(sanitized.begin(), sanitized.end(), ' ', '_');

    std::string filename = output_dir + "/" + rc_basename + " - " + sanitized + ".ui";

    name_counts_.clear();
    action_counter_ = 0;


    pugi::xml_document doc;
    pugi::xml_node ui = doc.append_child("ui");
    ui.append_attribute("version") = "4.0";
    ui.append_child("class").text() = "Form";

    write_dialog(ui, res);

    pugi::xml_node root_widget = ui.child("widget");

    for(const auto& menu_res : file.resources)
    {
      if(std::holds_alternative<menu_data>(menu_res.data))
      {
        const auto& md = std::get<menu_data>(menu_res.data);
        bool found_menu_stmt = false;
        for(const auto& stmt : dd.statements)
        {
          if(stmt.keyword == "MENU")
            found_menu_stmt = true;
        }
        if(found_menu_stmt)
          write_menu(root_widget, menu_res);
      }
    }

    write_actions(root_widget, file);

    if(doc.save_file(filename.c_str(), "  "))
      generated_files.push_back(filename);
  }

  return !generated_files.empty();
}

bool generator::generate_single_dialog(const rc_file& file, const resource& res, const std::string& output_path)
{
  name_counts_.clear();
    action_counter_ = 0;


  pugi::xml_document doc;
  pugi::xml_node ui = doc.append_child("ui");
  ui.append_attribute("version") = "4.0";
  ui.append_child("class").text() = "Form";

  write_dialog(ui, res);

  pugi::xml_node root_widget = ui.child("widget");

  const auto& dd = std::get<dialog_data>(res.data);
  bool has_menu = false;
  for(const auto& stmt : dd.statements)
  {
    if(stmt.keyword == "MENU")
      has_menu = true;
  }
  if(has_menu)
  {
    for(const auto& menu_res : file.resources)
    {
      if(std::holds_alternative<menu_data>(menu_res.data))
        write_menu(root_widget, menu_res);
    }
  }

  write_actions(root_widget, file);

  return doc.save_file(output_path.c_str(), "  ");
}

void generator::collect_global_data(const rc_file& file)
{
  accelerator_map_.clear();
  string_table_map_.clear();
  menu_text_map_.clear();
  menu_disabled_map_.clear();
  menu_checked_map_.clear();
  dlginit_map_.clear();
  ds_control_dialogs_.clear();
  name_counts_.clear();
    action_counter_ = 0;

  menubar_node_ = pugi::xml_node();

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

          accelerator_map_[a.id] = qt_key;
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
          string_table_map_[s.id] = s.value;
      }
    }

    if(res.type == "DLGINIT" &&
       std::holds_alternative<std::vector<dlginit_entry>>(res.data))
    {
      const auto& items = std::get<std::vector<dlginit_entry>>(res.data);
      for(const auto& item : items)
      {
        if(item.message == 0x0401 || item.message == 0x0402 ||
           item.message == 0x0403 || item.message == 0x0404)
          dlginit_map_[item.control_id].push_back(item.text);
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
                menu_text_map_[mi.id] = display_text;

              for(const auto& f : mi.flags)
              {
                std::string f_upper = f;
                std::transform(f_upper.begin(), f_upper.end(), f_upper.begin(), ::toupper);
                if(f_upper == "GRAYED" || f_upper == "INACTIVE" || f_upper == "MFS_GRAYED" || f_upper == "MFS_UNHILITE")
                  menu_disabled_map_[mi.id] = true;
                if(f_upper == "CHECKED" || f_upper == "MFS_CHECKED")
                  menu_checked_map_[mi.id] = true;
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
                menu_disabled_map_[sub->text] = true;
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
        ds_control_dialogs_[res.id] = &dd;
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

  bool has_resources = false;
  std::string qrc_dir = std::filesystem::path(output_path).parent_path().generic_string();
  if(qrc_dir.empty())
    qrc_dir = ".";

  for(const auto& up : ui_paths)
  {
    if(!up.empty())
    {
      pugi::xml_node file_node = res_node.append_child("file");
      std::string normalized = up;
      std::replace(normalized.begin(), normalized.end(), '\\', '/');
      std::string abs_path = std::filesystem::absolute(normalized).generic_string();
      std::string rel = std::filesystem::path(abs_path).lexically_relative(qrc_dir).generic_string();
      file_node.text() = rel.c_str();
      has_resources = true;
    }
  }

  for(const auto& res : file.resources)
  {
    if(res.filename.empty())
      continue;

    if(res.type == "BITMAP" || res.type == "ICON" || res.type == "CURSOR" ||
       res.type == "RT_MANIFEST" || res.type == "REGISTRY" ||
       res.type == "TYPELIB" || res.type == "DATA")
    {
      pugi::xml_node file_node = res_node.append_child("file");
      file_node.append_attribute("alias") = res.id.c_str();
      std::string normalized = res.filename;
      std::replace(normalized.begin(), normalized.end(), '\\', '/');
      std::string abs_path = std::filesystem::absolute(normalized).generic_string();
      std::string rel = std::filesystem::path(abs_path).lexically_relative(qrc_dir).generic_string();
      file_node.text() = rel.c_str();
      has_resources = true;
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
    write_control(widget, dd.controls[gi]);
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

      write_control(gb_widget, relative);
      written[i] = true;
    }
  }

  for(size_t i = 0; i < dd.controls.size(); ++i)
  {
    if(!written[i])
    {
      write_control(widget, dd.controls[i]);
      written[i] = true;
    }
  }
}

void generator::write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd)
{
  std::string font_family = find_statement_text(dd, "FONT");
  if(font_family.empty())
    font_family = "MS Sans Serif";
  else
    font_family = map_ms_font(font_family);

  int font_size = find_statement_numeric(dd, "FONT", 8);
  int font_weight = find_statement_numeric2(dd, "FONT", 0);
  bool font_bold = (font_weight >= 700);
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
    add_property_font(widget, font_family, font_size, font_bold, false);

  std::vector<std::string> flags;

  if(has_dialog_flag(dd, "STYLE", "DS_MODALFRAME") ||
     has_dialog_flag(dd, "EXSTYLE", "WS_EX_DLGMODALFRAME"))
    flags.push_back("Qt::MSWindowsFixedSizeDialogHint");

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

  bool visible = has_dialog_flag(dd, "STYLE", "WS_VISIBLE");
  if(visible)
    add_property_bool(widget, "visible", true);

  bool enabled = !has_dialog_flag(dd, "STYLE", "WS_DISABLED");
  add_property_bool(widget, "enabled", enabled);
}

void generator::write_control(pugi::xml_node& parent, const control& ctrl)
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

    auto it = dlginit_map_.find(ctrl.id);
    if(it != dlginit_map_.end())
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
    for(const auto& [dlg_id, dlg_ptr] : ds_control_dialogs_)
    {
      if(!dlg_ptr)
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
        tab_title = dlg_id;

      std::string tab_name = "tabPage";
      tab_name += std::to_string(tab_idx);

      pugi::xml_node tab_widget = add_widget(widget, "QWidget", tab_name);

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

    auto it = dlginit_map_.find(ctrl.id);
    if(it != dlginit_map_.end())
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
  int& count = name_counts_[base];
  if(count == 0)
  {
    count = 1;
    return base;
  }
  count++;
  return base + "_" + std::to_string(count);
}

int generator::dlu_to_pixel_x(int dlu) const
{
  return static_cast<int>(dlu * dlu_x_factor_);
}

int generator::dlu_to_pixel_y(int dlu) const
{
  return static_cast<int>(dlu * dlu_y_factor_);
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
    dlu_x_factor_ = (it->second.avg_width * scale) / 4.0;
    dlu_y_factor_ = (it->second.avg_height * scale) / 8.0;
  }
  else
  {
    dlu_x_factor_ = 1.75;
    dlu_y_factor_ = 1.75;
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

  if(!menubar_node_)
  {
    menubar_node_ = parent.append_child("widget");
    menubar_node_.append_attribute("class") = "QMenuBar";
    menubar_node_.append_attribute("name") = "menubar";

    pugi::xml_node geom = menubar_node_.append_child("property");
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

      pugi::xml_node menu = menubar_node_.append_child("widget");
      menu.append_attribute("class") = "QMenu";
      menu.append_attribute("name") = menu_name.c_str();

      pugi::xml_node title = menu.append_child("property");
      title.append_attribute("name") = "title";
      title.append_child("string").text() = popup_ptr->text.c_str();

      write_menu_entries(menu, popup_ptr->entries);

      pugi::xml_node addaction = menubar_node_.append_child("addaction");
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
        ? "action" + std::to_string(action_counter_++)
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
    auto text_it = menu_text_map_.find(id);
    if(text_it != menu_text_map_.end())
      display_text = text_it->second;

    pugi::xml_node text = action.append_child("property");
    text.append_attribute("name") = "text";
    text.append_child("string").text() = display_text.c_str();

    auto acc_it = accelerator_map_.find(id);
    if(acc_it != accelerator_map_.end() && !acc_it->second.empty())
    {
      pugi::xml_node shortcut = action.append_child("property");
      shortcut.append_attribute("name") = "shortcut";
      shortcut.append_child("string").text() = acc_it->second.c_str();
    }

    auto str_it = string_table_map_.find(id);
    if(str_it != string_table_map_.end() && !str_it->second.empty())
    {
      pugi::xml_node tooltip = action.append_child("property");
      tooltip.append_attribute("name") = "toolTip";
      tooltip.append_child("string").text() = str_it->second.c_str();
    }

    auto dis_it = menu_disabled_map_.find(id);
    if(dis_it != menu_disabled_map_.end() && dis_it->second)
      add_property_bool(action, "enabled", false);

    auto chk_it = menu_checked_map_.find(id);
    if(chk_it != menu_checked_map_.end() && chk_it->second)
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

  if(key == "VK_CONTROL" || key == "VK_LCONTROL" || key == "VK_RCONTROL")
    return "";
  if(key == "VK_SHIFT" || key == "VK_LSHIFT" || key == "VK_RSHIFT")
    return "";
  if(key == "VK_MENU" || key == "VK_LMENU" || key == "VK_RMENU")
    return "";

  if(key == "VK_RETURN")    return "Return";
  if(key == "VK_ESCAPE")    return "Escape";
  if(key == "VK_TAB")       return "Tab";
  if(key == "VK_BACK")      return "Backspace";
  if(key == "VK_DELETE")    return "Delete";
  if(key == "VK_INSERT")    return "Insert";
  if(key == "VK_HOME")      return "Home";
  if(key == "VK_END")       return "End";
  if(key == "VK_PRIOR")     return "PgUp";
  if(key == "VK_NEXT")      return "PgDown";
  if(key == "VK_LEFT")      return "Left";
  if(key == "VK_RIGHT")     return "Right";
  if(key == "VK_UP")        return "Up";
  if(key == "VK_DOWN")      return "Down";
  if(key == "VK_F1")        return "F1";
  if(key == "VK_F2")        return "F2";
  if(key == "VK_F3")        return "F3";
  if(key == "VK_F4")        return "F4";
  if(key == "VK_F5")        return "F5";
  if(key == "VK_F6")        return "F6";
  if(key == "VK_F7")        return "F7";
  if(key == "VK_F8")        return "F8";
  if(key == "VK_F9")        return "F9";
  if(key == "VK_F10")       return "F10";
  if(key == "VK_F11")       return "F11";
  if(key == "VK_F12")       return "F12";
  if(key == "VK_SPACE")     return "Space";

  if(key.size() == 1 && key[0] >= 'A' && key[0] <= 'Z')
    return key;

  if(key.size() == 1 && key[0] >= '0' && key[0] <= '9')
    return key;

  return "";
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
    {"Tahoma", "Liberation Sans"},
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
