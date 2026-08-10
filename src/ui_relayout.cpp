#include "rc_layout.h"
#include "xmlhelpers.h"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{

constexpr int k_spacing = 6;

struct ui_child
{
  pugi::xml_node widget;
  rc::layout::rect bounds;
  std::string qt_class;
};

int g_name_counts = 0;

std::string unique_name(const char* base)
{
  ++g_name_counts;
  if(g_name_counts == 1)
    return base;
  return std::format("{}_{}", base, g_name_counts);
}

bool is_container_class(const std::string& cls)
{
  return cls == "QDialog" || cls == "QWidget" || cls == "QGroupBox" ||
         cls == "QFrame";
}

bool get_geometry(const pugi::xml_node& widget, rc::layout::rect& out)
{
  bool ok = false;
  for(pugi::xml_node prop : widget.children("property"))
  {
    if(std::string_view(prop.attribute("name").value()) != "geometry")
      continue;
    pugi::xml_node rect = prop.child("rect");
    if(rect)
    {
      out.x = rect.child("x").text().as_int(0);
      out.y = rect.child("y").text().as_int(0);
      out.w = rect.child("width").text().as_int(0);
      out.h = rect.child("height").text().as_int(0);
      ok = true;
    }
    break;
  }
  return ok;
}

void add_property_int(pugi::xml_node node, const char* name, int value)
{
  pugi::xml_node prop = node.append_child("property");
  xml::set_attr(prop, "name", name);
  prop.append_child("number").text() = value;
}

void add_property_size(pugi::xml_node widget, const char* name, int width, int height)
{
  pugi::xml_node prop = widget.append_child("property");
  xml::set_attr(prop, "name", name);
  pugi::xml_node size = prop.append_child("size");
  size.append_child("width").text() = width;
  size.append_child("height").text() = height;
}

void add_property_sizepolicy(pugi::xml_node widget, const std::string& htype,
                             const std::string& vtype)
{
  pugi::xml_node prop = widget.append_child("property");
  xml::set_attr(prop, "name", "sizePolicy");
  pugi::xml_node policy = prop.append_child("sizepolicy");
  xml::set_attr(policy, "hsizetype", htype);
  xml::set_attr(policy, "vsizetype", vtype);
  policy.append_child("horstretch").text() = 0;
  policy.append_child("verstretch").text() = 0;
}

bool layout_class_stretches(const std::string& qt_class)
{
  static const std::vector<std::string> stretch_classes = {
    "QTextEdit", "QPlainTextEdit", "QListWidget", "QListView",
    "QTableWidget", "QTableView", "QTreeWidget", "QTreeView",
    "QScrollArea", "QWidget", "QOpenGLWidget", "QGLWidget",
  };
  return std::find(stretch_classes.begin(), stretch_classes.end(), qt_class) !=
         stretch_classes.end();
}

void remove_property(pugi::xml_node widget, const char* name)
{
  for(pugi::xml_node prop = widget.first_child(); prop;)
  {
    pugi::xml_node next = prop.next_sibling();
    if(std::string_view(prop.name()) == "property" &&
       std::string_view(prop.attribute("name").value()) == name)
    {
      widget.remove_child(prop);
    }
    prop = next;
  }
}

pugi::xml_node emit_layout_node(pugi::xml_node& parent, const rc::layout::node& ln,
                                const std::vector<ui_child>& items)
{
  const bool is_grid = ln.k == rc::layout::node::kind::grid;
  const bool is_box_x = ln.k == rc::layout::node::kind::box_x;

  pugi::xml_node layout = parent.append_child("layout");
  xml::set_attr(layout, "class",
                is_grid ? "QGridLayout"
                        : (is_box_x ? "QHBoxLayout" : "QVBoxLayout"));
  xml::set_attr(layout, "name", unique_name("layout").c_str());
  add_property_int(layout, "spacing", k_spacing);
  add_property_int(layout, "margin", 0);

  if(is_grid && ln.label_column_minwidth > 0)
    xml::set_attr(layout, "columnminimumwidth", ln.label_column_minwidth);

  if(is_grid && ln.equal_row_stretch > 0)
  {
    std::string row_stretch;
    for(int r = 0; r < ln.rows; ++r)
      row_stretch += (r ? "," : "") + std::to_string(ln.equal_row_stretch);
    std::string col_stretch;
    for(int c = 0; c < ln.columns; ++c)
      col_stretch += (c ? "," : "") + std::to_string(ln.equal_col_stretch);
    xml::set_attr(layout, "rowstretch", row_stretch.c_str());
    xml::set_attr(layout, "columnstretch", col_stretch.c_str());
  }

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
      xml::set_attr(item, "row", c.row);
      xml::set_attr(item, "column", c.column);
      if(c.rowspan > 1)
        xml::set_attr(item, "rowspan", c.rowspan);
      if(c.colspan > 1)
        xml::set_attr(item, "colspan", c.colspan);
    }
    std::string align = alignment_attr(child);
    if(!align.empty())
      xml::set_attr(item, "alignment", align.c_str());

    if(child.k == rc::layout::node::kind::item)
    {
      const int k_ctrl = child.control_index;
      const ui_child& uc = items[k_ctrl];
      item.append_copy(uc.widget);
      pugi::xml_node w = item.last_child();
      remove_property(w, "geometry");
      add_property_size(w, "minimumSize", uc.bounds.w, uc.bounds.h);
      if(layout_class_stretches(uc.qt_class))
        add_property_sizepolicy(w, "Expanding", "Expanding");
      else
        add_property_sizepolicy(w, "Preferred", "Preferred");
    }
    else
    {
      emit_layout_node(item, child, items);
    }
  }

  if(ln.spacer_size > 0)
  {
    pugi::xml_node item = layout.append_child("item");
    pugi::xml_node spacer = item.append_child("spacer");
    xml::set_attr(spacer, "name", unique_name("spacer").c_str());
    pugi::xml_node orient = spacer.append_child("property");
    xml::set_attr(orient, "name", "orientation");
    orient.append_child("enum").text() = is_box_x ? "Qt::Horizontal" : "Qt::Vertical";
    pugi::xml_node prop = spacer.append_child("property");
    xml::set_attr(prop, "name", "sizeHint");
    xml::set_attr(prop, "stdset", 0);
    pugi::xml_node size = prop.append_child("size");
    size.append_child("width").text() = is_box_x ? ln.spacer_size : 20;
    size.append_child("height").text() = is_box_x ? 20 : ln.spacer_size;
  }

  return layout;
}

void process(pugi::xml_node widget, const std::string& cls,
             rc::layout::rect bounds)
{
  if(cls == "QTabWidget")
  {
    for(pugi::xml_node page : widget.children("widget"))
    {
      rc::layout::rect page_bounds;
      if(get_geometry(page, page_bounds))
        remove_property(page, "geometry");
      process(page, page.attribute("class").as_string(), page_bounds);
    }
  }
  else if(is_container_class(cls))
  {
    std::vector<ui_child> positioned;
    for(pugi::xml_node child : widget.children("widget"))
    {
      ui_child c;
      c.widget = child;
      c.qt_class = child.attribute("class").as_string();
      if(get_geometry(child, c.bounds))
        positioned.push_back(std::move(c));
    }

    for(const auto& c : positioned)
      process(c.widget, c.qt_class, c.bounds);

    if(positioned.size() >= 2)
    {
      std::vector<rc::layout::child> lchildren;
      lchildren.reserve(positioned.size());
      for(size_t i = 0; i < positioned.size(); ++i)
      {
        rc::layout::child ch;
        ch.control_index = static_cast<int>(i);
        ch.qt_class = positioned[i].qt_class;
        ch.bounds = positioned[i].bounds;
        lchildren.push_back(std::move(ch));
      }

      rc::layout::pattern_flag pattern_flags = rc::layout::pattern_box |
                                                   rc::layout::pattern_grid |
                                                   rc::layout::pattern_align;
      if(const char* env = std::getenv("RC2QT_PATTERNS"))
        pattern_flags = static_cast<rc::layout::pattern_flag>(
            std::strtoul(env, nullptr, 0));

      rc::layout::node plan = rc::layout::solve_container(
          lchildren, pattern_flags, bounds.w, bounds.h);

      pugi::xml_node layout = emit_layout_node(widget, plan, positioned);

      for(const auto& c : positioned)
        widget.remove_child(c.widget);

      for(pugi::xml_node l = widget.child("layout"); l;)
      {
        pugi::xml_node next = l.next_sibling("layout");
        if(l != layout)
          widget.remove_child(l);
        l = next;
      }
    }
  }
}

void comment_out_duplicate_cells(pugi::xml_node node)
{
  for(pugi::xml_node child : node.children())
  {
    if(std::string_view(child.name()) == "layout" &&
       std::string_view(child.attribute("class").value()) == "QGridLayout")
    {
      std::vector<pugi::xml_node> items;
      for(pugi::xml_node item : child.children("item"))
      {
        if(item.attribute("row") && item.attribute("column"))
          items.push_back(item);
      }

      std::set<std::pair<int, int>> seen;
      for(pugi::xml_node item : items)
      {
        const int row = item.attribute("row").as_int();
        const int col = item.attribute("column").as_int();
        if(!seen.emplace(row, col).second)
        {
          std::ostringstream out;
          item.print(out, "  ");
          std::string content = out.str();
          pugi::xml_node comment = child.insert_child_before(pugi::node_comment, item);
          comment.set_value(content.c_str());
          child.remove_child(item);
        }
      }
    }
    comment_out_duplicate_cells(child);
  }
}

}  // namespace

int main(int argc, char** argv)
{
  std::string in_path;
  std::string out_path;

  for(int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if(arg == "-o" || arg == "--output")
    {
      if(i + 1 >= argc)
      {
        std::cerr << "error: -o requires a file path\n";
        return 1;
      }
      out_path = argv[++i];
    }
    else if(!in_path.empty())
    {
      std::cerr << "error: unexpected extra argument \"" << arg << "\"\n";
      return 1;
    }
    else
    {
      in_path = arg;
    }
  }

  if(in_path.empty() || out_path.empty())
  {
    std::cerr << "usage: ui_relayout -o <out.ui> <in.ui>\n";
    return 1;
  }

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(
      in_path.c_str(), pugi::parse_default | pugi::parse_declaration |
                           pugi::parse_doctype | pugi::parse_pi);
  if(!result)
  {
    std::cerr << "error: " << in_path << ": " << result.description() << "\n";
    return 1;
  }

  pugi::xml_node ui = doc.child("ui");
  pugi::xml_node root = ui.child("widget");
  if(!root)
  {
    std::cerr << "error: " << in_path << ": no <widget> under <ui>\n";
    return 1;
  }

  rc::layout::rect root_bounds;
  get_geometry(root, root_bounds);
  process(root, root.attribute("class").as_string(), root_bounds);

  comment_out_duplicate_cells(ui);

  if(!doc.save_file(out_path.c_str(), "  "))
  {
    std::cerr << "error: could not write \"" << out_path << "\"\n";
    return 1;
  }

  std::cout << "wrote " << out_path << "\n";
  return 0;
}
