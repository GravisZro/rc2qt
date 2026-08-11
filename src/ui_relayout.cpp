#include "rc_layout.h"
#include "utils.h"
#include "xml.h"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
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
  rc::xml::node widget;
  rc::layout::rect bounds;
  std::string qt_class;
  std::string name;
};

int g_name_counts = 0;

int g_default_tol = 2;
std::map<std::string, int> g_widget_tol;

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

bool get_geometry(const rc::xml::node& widget, rc::layout::rect& out)
{
  bool ok = false;
  for(rc::xml::node prop : widget.children("property"))
  {
    if(std::string_view(prop.attribute("name").value()) != "geometry")
      continue;
    rc::xml::node rect = prop.child("rect");
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

void add_property_int(rc::xml::node node, const char* name, int value)
{
  rc::xml::node prop = node.append_child("property");
  prop.add_attr("name", name);
  prop.append_child("number").text() = value;
}

void add_property_size(rc::xml::node widget, const char* name, int width, int height)
{
  rc::xml::node prop = widget.append_child("property");
  prop.add_attr("name", name);
  rc::xml::node size = prop.append_child("size");
  size.append_child("width").text() = width;
  size.append_child("height").text() = height;
}

void add_property_sizepolicy(rc::xml::node widget, const std::string& htype,
                             const std::string& vtype)
{
  rc::xml::node prop = widget.append_child("property");
  prop.add_attr("name", "sizePolicy");
  rc::xml::node policy = prop.append_child("sizepolicy");
  policy.add_attr("hsizetype", htype);
  policy.add_attr("vsizetype", vtype);
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

bool load_margins_file(const std::string& path, int& default_tol,
                       std::map<std::string, int>& widget_tol)
{
  std::ifstream in(path);
  if(!in)
    return false;
  std::string line;
  while(std::getline(in, line))
  {
    size_t hash = line.find('#');
    if(hash != std::string::npos)
      line = line.substr(0, hash);
    std::istringstream iss(line);
    std::string key;
    int value;
    if(!(iss >> key) || !(iss >> value) || value < 0)
      continue;
    if(key == "default")
      default_tol = value;
    else
      widget_tol[key] = value;
  }
  return true;
}

void remove_property(rc::xml::node widget, const char* name)
{
  for(rc::xml::node prop = widget.first_child(); prop;)
  {
    rc::xml::node next = prop.next_sibling();
    if(std::string_view(prop.name()) == "property" &&
       std::string_view(prop.attribute("name").value()) == name)
    {
      widget.remove_child(prop);
    }
    prop = next;
  }
}

rc::xml::node emit_layout_node(rc::xml::node& parent, const rc::layout::node& ln,
                                const std::vector<ui_child>& items)
{
  const bool is_grid = ln.k == rc::layout::node::kind::grid;
  const bool is_box_x = ln.k == rc::layout::node::kind::box_x;

  rc::xml::node layout = parent.append_child("layout");
  layout.add_attr("class",
                is_grid ? "QGridLayout"
                        : (is_box_x ? "QHBoxLayout" : "QVBoxLayout"));
  layout.add_attr("name", unique_name("layout").c_str());
  add_property_int(layout, "spacing", k_spacing);
  add_property_int(layout, "margin", 0);

  if(is_grid && ln.label_column_minwidth > 0)
    layout.add_attr("columnminimumwidth", ln.label_column_minwidth);

  if(is_grid && ln.equal_row_stretch > 0)
  {
    std::string row_stretch;
    for(int r = 0; r < ln.rows; ++r)
      row_stretch += (r ? "," : "") + std::to_string(ln.equal_row_stretch);
    std::string col_stretch;
    for(int c = 0; c < ln.columns; ++c)
      col_stretch += (c ? "," : "") + std::to_string(ln.equal_col_stretch);
    layout.add_attr("rowstretch", row_stretch.c_str());
    layout.add_attr("columnstretch", col_stretch.c_str());
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
    rc::xml::node item = layout.append_child("item");
    if(is_grid)
    {
      const rc::layout::node::cell& c = ln.cells[k];
      item.add_attr("row", c.row);
      item.add_attr("column", c.column);
      if(c.rowspan > 1)
        item.add_attr("rowspan", c.rowspan);
      if(c.colspan > 1)
        item.add_attr("colspan", c.colspan);
    }
    std::string align = alignment_attr(child);
    if(!align.empty())
      item.add_attr("alignment", align.c_str());

    if(child.k == rc::layout::node::kind::item)
    {
      const int k_ctrl = child.control_index;
      const ui_child& uc = items[k_ctrl];
      item.append_copy(uc.widget);
      rc::xml::node w = item.last_child();
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
    rc::xml::node item = layout.append_child("item");
    rc::xml::node spacer = item.append_child("spacer");
    spacer.add_attr("name", unique_name("spacer").c_str());
    rc::xml::node orient = spacer.append_child("property");
    orient.add_attr("name", "orientation");
    orient.append_child("enum").text() = is_box_x ? "Qt::Horizontal" : "Qt::Vertical";
    rc::xml::node prop = spacer.append_child("property");
    prop.add_attr("name", "sizeHint");
    prop.add_attr("stdset", 0);
    rc::xml::node size = prop.append_child("size");
    size.append_child("width").text() = is_box_x ? ln.spacer_size : 20;
    size.append_child("height").text() = is_box_x ? 20 : ln.spacer_size;
  }

  return layout;
}

void process(rc::xml::node widget, const std::string& cls,
             rc::layout::rect bounds)
{
  if(cls == "QTabWidget")
  {
    for(rc::xml::node page : widget.children("widget"))
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
    for(rc::xml::node child : widget.children("widget"))
    {
      ui_child c;
      c.widget = child;
      c.qt_class = child.attribute("class").as_string();
      c.name = child.attribute("name").as_string();
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
        auto it = g_widget_tol.find(positioned[i].name);
        if(it != g_widget_tol.end())
          ch.tol = it->second;
        lchildren.push_back(std::move(ch));
      }

      rc::layout::pattern_flag pattern_flags = rc::layout::pattern_box |
                                                   rc::layout::pattern_grid |
                                                   rc::layout::pattern_align;
      if(const char* env = std::getenv("RC2QT_PATTERNS"))
        pattern_flags = static_cast<rc::layout::pattern_flag>(
            std::strtoul(env, nullptr, 0));

      rc::layout::node plan = rc::layout::solve_container(
          lchildren, pattern_flags, bounds.w, bounds.h, g_default_tol);

      rc::xml::node layout = emit_layout_node(widget, plan, positioned);

      for(const auto& c : positioned)
        widget.remove_child(c.widget);

      for(rc::xml::node l = widget.child("layout"); l;)
      {
        rc::xml::node next = l.next_sibling("layout");
        if(l != layout)
          widget.remove_child(l);
        l = next;
      }
    }
  }
}

void comment_out_duplicate_cells(rc::xml::node node)
{
  for(rc::xml::node child : node.children())
  {
    if(std::string_view(child.name()) == "layout" &&
       std::string_view(child.attribute("class").value()) == "QGridLayout")
    {
      std::vector<rc::xml::node> items;
      for(rc::xml::node item : child.children("item"))
      {
        if(item.attribute("row") && item.attribute("column"))
          items.push_back(item);
      }

      std::set<std::pair<int, int>> seen;
      for(rc::xml::node item : items)
      {
        const int row = item.attribute("row").as_int();
        const int col = item.attribute("column").as_int();
        if(!seen.emplace(row, col).second)
        {
          std::ostringstream out;
          item.print(out, "  ");
          std::string content = "\n" + out.str();
          rc::xml::node comment = child.insert_child_before(rc::xml::node_type::comment, item);
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
  std::string margins_path;

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
    else if(arg == "-t" || arg == "--margins-file")
    {
      if(i + 1 >= argc)
      {
        std::cerr << "error: -t requires a file path\n";
        return 1;
      }
      margins_path = argv[++i];
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
    std::cerr << "usage: ui_relayout -o <out.ui> [-t <margins.txt>] <in.ui>\n";
    return 1;
  }

  if(!margins_path.empty() && !load_margins_file(margins_path, g_default_tol,
                                                 g_widget_tol))
  {
    std::cerr << "error: could not read margins file \"" << margins_path << "\"\n";
    return 1;
  }

  rc::xml::document doc;
  rc::xml::parse_result result = doc.load_file(
      in_path.c_str(), rc::xml::parse_default | rc::xml::parse_flags::declaration |
                           rc::xml::parse_flags::doctype | rc::xml::parse_flags::pi);
  if(!result)
  {
    std::cerr << "error: " << in_path << ": " << result.description() << "\n";
    return 1;
  }

  rc::xml::node ui = doc.child("ui");
  rc::xml::node root = ui.child("widget");
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
