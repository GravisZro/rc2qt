#include "rc_generator.h"
#include "rc_ast.h"
#include "rc_constants.h"

#include <algorithm>
#include <iostream>

namespace rc
{

bool generator::generate(const rc_file& file, const std::string& output_path)
{
  pugi::xml_document doc;

  pugi::xml_node ui = doc.append_child("ui");
  ui.append_attribute("version") = "4.0";

  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<dialog_data>(res.data))
      write_dialog(ui, res);
  }

  return doc.save_file(output_path.c_str(), "  ");
}

bool generator::generate_qrc(const rc_file& file, const std::string& output_path)
{
  pugi::xml_document doc;

  pugi::xml_node qresource = doc.append_child("RCC");
  pugi::xml_node res_node = qresource.append_child("qresource");

  bool has_resources = false;

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
      file_node.text() = res.filename.c_str();
      has_resources = true;
    }
  }

  if(!has_resources)
  {
    pugi::xml_node file_node = res_node.append_child("file");
    file_node.text() = ".";
  }

  return doc.save_file(output_path.c_str(), "  ");
}

void generator::write_dialog(pugi::xml_node& parent, const resource& res)
{
  const auto& dd = std::get<dialog_data>(res.data);
  std::string dialog_name = res.id;

  pugi::xml_node widget = parent.append_child("widget");
  widget.append_attribute("class") = "QDialog";
  widget.append_attribute("name") = dialog_name.c_str();

  write_dialog_properties(widget, dd);

  for(const auto& ctrl : dd.controls)
    write_control(widget, ctrl);
}

void generator::write_dialog_properties(pugi::xml_node& widget, const dialog_data& dd)
{
  int px = dlu_to_pixel_x(dd.x);
  int py = dlu_to_pixel_y(dd.y);
  int pw = dlu_to_pixel_x(dd.width);
  int ph = dlu_to_pixel_y(dd.height);
  add_property_rect(widget, px, py, pw, ph);

  std::string caption = find_statement_text(dd, "CAPTION");
  if(!caption.empty())
    add_property_string(widget, "windowTitle", caption);

  int font_size = find_statement_numeric(dd, "FONT", 8);
  if(font_size > 0)
    add_property_font(widget, "MS Sans Serif", font_size, false, false);

  add_property_bool(widget, "enabled", true);
}

void generator::write_control(pugi::xml_node& parent, const control& ctrl)
{
  std::string qt_class = map_keyword_to_widget(ctrl.keyword);

  if(qt_class.empty() && ctrl.keyword == "CONTROL")
    qt_class = map_class_to_widget(ctrl.class_name, ctrl.style);

  if(qt_class.empty())
    qt_class = "QWidget";

  std::string name = unique_name(ctrl.id);
  pugi::xml_node widget = add_widget(parent, qt_class, name);

  int px = dlu_to_pixel_x(ctrl.x);
  int py = dlu_to_pixel_y(ctrl.y);
  int pw = dlu_to_pixel_x(ctrl.width);
  int ph = dlu_to_pixel_y(ctrl.height);
  add_property_rect(widget, px, py, pw, ph);

  if(!ctrl.text.empty())
    add_property_string(widget, "text", ctrl.text);

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
    if(has_style(ctrl.style, "ES_MULTILINE"))
    {
      widget.attribute("class") = "QTextEdit";
      qt_class = "QTextEdit";
    }
    else
    {
      if(has_style(ctrl.style, "ES_READONLY"))
        add_property_bool(widget, "readOnly", true);
      if(has_style(ctrl.style, "ES_PASSWORD"))
        add_property_enum(widget, "echoMode", "QLineEdit::Password");
    }
  }

  if(qt_class == "QTextEdit")
  {
    if(has_style(ctrl.style, "ES_READONLY"))
      add_property_bool(widget, "readOnly", true);
  }

  if(qt_class == "QGroupBox")
    add_property_bool(widget, "flat", false);

  if(qt_class == "QComboBox")
  {
    if(has_style(ctrl.style, "CBS_DROPDOWNLIST"))
      add_property_bool(widget, "editable", false);
    else if(has_style(ctrl.style, "CBS_DROPDOWN"))
      add_property_bool(widget, "editable", true);
  }

  bool enabled = !has_style(ctrl.style, "WS_DISABLED");
  add_property_bool(widget, "enabled", enabled);
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
  prop.append_child("string").text() = value.c_str();
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
    if(has_style(style, "SS_ICON") || has_style(style, "SS_BITMAP") ||
       has_style(style, "SS_ENHMETAFILE"))
      return "QLabel";
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
  if(lower == "tooltips_class32" || lower == "#32774")
    return "QToolTip";
  if(lower == "#32768")
    return "QMenu";
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
  return static_cast<int>(dlu * 1.75);
}

int generator::dlu_to_pixel_y(int dlu) const
{
  return static_cast<int>(dlu * 1.75);
}

bool generator::has_style(const style_expr& style, const std::string& flag) const
{
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
      return default_value;
    }
  }
  return default_value;
}

}
