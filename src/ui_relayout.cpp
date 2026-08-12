#include "rc_layout.h"
#include "utils.h"
#include "xml.h"
#ifdef HAVE_QT
# include "rc_render.h"
# include <QApplication>
#endif

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <utility>
#include <vector>

namespace
{

constexpr int k_spacing = 6;

struct ui_child
{
  xml::node widget;
  rc::layout::rect bounds;
  std::string qt_class;
  std::string name;
};

int g_name_counts = 0;

int g_default_tol = 2;
std::map<std::string, int> g_widget_tol;
std::map<std::string, int> g_class_tol;

/* Qt widget classes that rc2qt can emit as child widgets; tolerances can be
   given per class as well as per widget name. */
const std::vector<std::string> k_supported_widgets = {
  "QCalendarWidget", "QCheckBox", "QComboBox", "QDateTimeEdit",
  "QGroupBox", "QHeaderView", "QLabel", "QLineEdit", "QListWidget",
  "QProgressBar", "QPushButton", "QRadioButton", "QScrollBar",
  "QSlider", "QSpinBox", "QStackedWidget", "QStatusBar", "QTableWidget",
  "QTabWidget", "QTextEdit", "QToolBar", "QTreeWidget", "QWidget",
};

bool is_supported_widget(const std::string& key)
{
  return std::find(k_supported_widgets.begin(), k_supported_widgets.end(),
                   key) != k_supported_widgets.end();
}

#ifdef HAVE_QT
std::map<std::string, rc::render::target> g_verify_targets;
#endif

void usage()
{
  std::cerr << "usage: ui_relayout -o <out.ui> [-t <tolerances.txt>] [-v [-d <dir>]] <in.ui>\n";
  std::cerr << "  -o <file>      Write the relaid-out .ui to <file>\n";
  std::cerr << "  -t <file>      Read per-widget and per-class layout tolerances from <file>\n";
  std::cerr << "  -T             Explain how to write the -t tolerances file\n";
#ifdef HAVE_QT
  std::cerr << "  -v             Render the relaid-out dialog offscreen and verify geometry\n";
  std::cerr << "  -d <dir>       Render dump directory for -v (default: none)\n";
#endif
  std::cerr << "  -h             Show this help\n";
}

void print_tolerances_help()
{
  std::cerr << "Tolerances file for -t <file>:\n";
  std::cerr << "\n";
  std::cerr << "The -t option reads a text file that sets a margin of error, in pixels,\n";
  std::cerr << "used by the layout solver. Two widgets whose edges are within that many\n";
  std::cerr << "pixels of each other are treated as sharing a grid row/column, so the\n";
  std::cerr << "solver snaps them together instead of splitting into separate bands.\n";
  std::cerr << "The built-in default is 2 pixels.\n";
  std::cerr << "\n";
  std::cerr << "Format: one rule per line; '#' starts a comment.\n";
  std::cerr << "\n";
  std::cerr << "  default <n>            Tolerance for every widget (>= 0)\n";
  std::cerr << "  <widget-name> <n>      Override for one widget, matched against the .ui\n";
  std::cerr << "                         <widget name=\"...\"> attribute\n";
  std::cerr << "  <widget-class> <n>     Override for every widget of that Qt class; the\n";
  std::cerr << "                         supported classes are listed below\n";
  std::cerr << "\n";
  std::cerr << "The effective tolerance for a pair of widgets is the larger of the\n";
  std::cerr << "default, either widget's per-name override, and either widget's per-class\n";
  std::cerr << "override. A per-name override wins over a per-class one.\n";
  std::cerr << "\n";
  std::cerr << "Supported widget classes:\n";
  for(size_t i = 0; i < k_supported_widgets.size(); ++i)
  {
    if(i > 0 && i % 5 == 0)
      std::cerr << "\n";
    std::cerr << "  " << k_supported_widgets[i];
  }
  std::cerr << "\n";
  std::cerr << "\n";
  std::cerr << "Example:\n";
  std::cerr << "  # loosen everything\n";
  std::cerr << "  default 5\n";
  std::cerr << "  # keep every button on its own row\n";
  std::cerr << "  QPushButton 0\n";
  std::cerr << "  # but let this specific control be loose\n";
  std::cerr << "  IDC_LIST1 8\n";
}

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

bool get_geometry(const xml::node& widget, rc::layout::rect& out)
{
  bool ok = false;
  for(xml::node prop : widget.children("property"))
  {
    if(std::string_view(prop.attribute("name").value()) != "geometry")
      continue;
    xml::node rect = prop.child("rect");
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

void add_property_int(xml::node node, const char* name, int value)
{
  xml::node prop = node.append_child("property");
  prop.add_attr("name", name);
  prop.append_child("number").text() = value;
}

void add_property_size(xml::node widget, const char* name, int width, int height)
{
  xml::node prop = widget.append_child("property");
  prop.add_attr("name", name);
  xml::node size = prop.append_child("size");
  size.append_child("width").text() = width;
  size.append_child("height").text() = height;
}

void add_property_sizepolicy(xml::node widget, const std::string& htype,
                             const std::string& vtype)
{
  xml::node prop = widget.append_child("property");
  prop.add_attr("name", "sizePolicy");
  xml::node policy = prop.append_child("sizepolicy");
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

bool load_tolerances_file(const std::string& path, int& default_tol,
                          std::map<std::string, int>& widget_tol,
                          std::map<std::string, int>& class_tol)
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
    else if(is_supported_widget(key))
      class_tol[key] = value;
    else
      widget_tol[key] = value;
  }
  return true;
}

void remove_property(xml::node widget, const char* name)
{
  for(xml::node prop = widget.first_child(); prop;)
  {
    xml::node next = prop.next_sibling();
    if(std::string_view(prop.name()) == "property" &&
       std::string_view(prop.attribute("name").value()) == name)
    {
      widget.remove_child(prop);
    }
    prop = next;
  }
}

xml::node emit_layout_node(xml::node& parent, const rc::layout::node& ln,
                                const std::vector<ui_child>& items)
{
  const bool is_grid = ln.k == rc::layout::node::kind::grid;
  const bool is_box_x = ln.k == rc::layout::node::kind::box_x;

  xml::node layout = parent.append_child("layout");
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
    xml::node item = layout.append_child("item");
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
      xml::node w = item.last_child();
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
    xml::node item = layout.append_child("item");
    xml::node spacer = item.append_child("spacer");
    spacer.add_attr("name", unique_name("spacer").c_str());
    xml::node orient = spacer.append_child("property");
    orient.add_attr("name", "orientation");
    orient.append_child("enum").text() = is_box_x ? "Qt::Horizontal" : "Qt::Vertical";
    xml::node prop = spacer.append_child("property");
    prop.add_attr("name", "sizeHint");
    prop.add_attr("stdset", 0);
    xml::node size = prop.append_child("size");
    size.append_child("width").text() = is_box_x ? ln.spacer_size : 20;
    size.append_child("height").text() = is_box_x ? 20 : ln.spacer_size;
  }

  return layout;
}

void process(xml::node widget, const std::string& cls,
             rc::layout::rect bounds, const std::string& container_name)
{
#ifndef HAVE_QT
  (void)container_name;
#endif
  if(cls == "QTabWidget")
  {
    for(xml::node page : widget.children("widget"))
    {
      rc::layout::rect page_bounds;
      if(get_geometry(page, page_bounds))
        remove_property(page, "geometry");
      process(page, page.attribute("class").as_string(), page_bounds,
              page.attribute("name").as_string());
    }
  }
  else if(is_container_class(cls))
  {
    std::vector<ui_child> positioned;
    for(xml::node child : widget.children("widget"))
    {
      ui_child c;
      c.widget = child;
      c.qt_class = child.attribute("class").as_string();
      c.name = child.attribute("name").as_string();
      if(get_geometry(child, c.bounds))
        positioned.push_back(std::move(c));
    }

    for(const auto& c : positioned)
      process(c.widget, c.qt_class, c.bounds, c.name);

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
        else
        {
          auto cit = g_class_tol.find(positioned[i].qt_class);
          if(cit != g_class_tol.end())
            ch.tol = cit->second;
        }
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

      xml::node layout = emit_layout_node(widget, plan, positioned);

#ifdef HAVE_QT
      for(const auto& c : positioned)
      {
        rc::render::target t;
        t.x = c.bounds.x;
        t.y = c.bounds.y;
        t.w = c.bounds.w;
        t.h = c.bounds.h;
        t.container = container_name;
        g_verify_targets[c.name] = t;
      }
#endif

      for(const auto& c : positioned)
        widget.remove_child(c.widget);

      for(xml::node l = widget.child("layout"); l;)
      {
        xml::node next = l.next_sibling("layout");
        if(l != layout)
          widget.remove_child(l);
        l = next;
      }
    }
  }
}

void comment_out_duplicate_cells(xml::node node)
{
  for(xml::node child : node.children())
  {
    if(std::string_view(child.name()) == "layout" &&
       std::string_view(child.attribute("class").value()) == "QGridLayout")
    {
      std::vector<xml::node> items;
      for(xml::node item : child.children("item"))
      {
        if(item.attribute("row") && item.attribute("column"))
          items.push_back(item);
      }

      std::set<std::pair<int, int>> seen;
      for(xml::node item : items)
      {
        const int row = item.attribute("row").as_int();
        const int col = item.attribute("column").as_int();
        if(!seen.emplace(row, col).second)
        {
          std::ostringstream out;
          item.print(out, "  ");
          std::string content = "\n" + out.str();
          xml::node comment = child.insert_child_before(xml::node_type::comment, item);
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
  std::string tolerances_path;
#ifdef HAVE_QT
  bool verify = false;
  std::string dump_dir;
#endif

  int opt;
#ifdef HAVE_QT
  while((opt = getopt(argc, argv, "o:t:vTd:h")) != -1)
#else
  while((opt = getopt(argc, argv, "o:t:Th")) != -1)
#endif
  {
    switch(opt)
    {
      case 'o':
        out_path = optarg;
        break;
      case 't':
        tolerances_path = optarg;
        break;
      case 'T':
        print_tolerances_help();
        return 0;
#ifdef HAVE_QT
      case 'v':
        verify = true;
        break;
      case 'd':
        dump_dir = optarg;
        break;
#endif
      case 'h':
        usage();
        return 0;
      default:
        usage();
        return 1;
    }
  }

  if(optind < argc)
  {
    in_path = argv[optind++];
    if(optind < argc)
    {
      std::cerr << "error: unexpected extra argument \"" << argv[optind] << "\"\n";
      return 1;
    }
  }

  if(in_path.empty() || out_path.empty())
  {
    usage();
    return 1;
  }

  if(!tolerances_path.empty() && !load_tolerances_file(tolerances_path, g_default_tol,
                                                 g_widget_tol, g_class_tol))
  {
    std::cerr << "error: could not read margins file \"" << tolerances_path << "\"\n";
    return 1;
  }

  xml::document doc;
  xml::parse_result result = doc.load_file(
      in_path.c_str(), xml::parse_default | xml::parse_flags::declaration |
                           xml::parse_flags::doctype | xml::parse_flags::pi);
  if(!result)
  {
    std::cerr << "error: " << in_path << ": " << result.description() << "\n";
    return 1;
  }

  xml::node ui = doc.child("ui");
  xml::node root = ui.child("widget");
  if(!root)
  {
    std::cerr << "error: " << in_path << ": no <widget> under <ui>\n";
    return 1;
  }

  rc::layout::rect root_bounds;
  get_geometry(root, root_bounds);
  process(root, root.attribute("class").as_string(), root_bounds,
          root.attribute("name").as_string());

  comment_out_duplicate_cells(ui);

  if(!doc.save_file(out_path.c_str(), "  "))
  {
    std::cerr << "error: could not write \"" << out_path << "\"\n";
    return 1;
  }

  std::cout << "wrote " << out_path << "\n";

#ifdef HAVE_QT
  if(verify)
  {
    /* Render offscreen so the verify works without a display; pin the DPI to
       96 to match the generator's pixel conversion. The QApplication must
       exist before any widget is constructed. */
    qputenv("QT_QPA_PLATFORM", "offscreen");
    qputenv("QT_FONT_DPI", "96");
    int qargc = 1;
    QApplication app(qargc, nullptr);

    /* Render the relaid-out document offscreen and compare every widget that
       went into a layout against its original absolute geometry. */
    rc::render::verify_input input;
    input.name = std::filesystem::path(in_path).stem().string();
    input.doc = std::move(doc);
    input.targets = std::move(g_verify_targets);
    input.dialog_width = root_bounds.w;
    input.dialog_height = root_bounds.h;

    rc::render::result r = rc::render::verify_layout(input, dump_dir);
    std::cout << "Verify: " << input.name << ": targets=" << r.target_widgets
              << " rendered=" << r.rendered_widgets
              << " missing=" << r.missing_widgets
              << " iou=" << r.mean_iou
              << " dx=" << r.mean_dx << " dy=" << r.mean_dy
              << " dw=" << r.mean_dw << " dh=" << r.mean_dh
              << " max_dx=" << r.max_dx << " max_dy=" << r.max_dy
              << " overlaps=" << r.overlap_violations << std::endl;
  }
#endif

  return 0;
}
