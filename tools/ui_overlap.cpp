#include "xml.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace
{
struct rect
{
  int x;
  int y;
  int w;
  int h;
};

bool read_rect(rc::xml::node widget, rect& out)
{
  for(rc::xml::node prop : widget.children("property"))
  {
    if(std::string(prop.attribute("name").value()) != "geometry")
      continue;
    rc::xml::node r = prop.child("rect");
    if(!r)
      return false;
    out.x = r.child("x").text().as_int();
    out.y = r.child("y").text().as_int();
    out.w = r.child("width").text().as_int();
    out.h = r.child("height").text().as_int();
    return true;
  }
  return false;
}

bool overlap_extents(const rect& a, const rect& b, int& out_w, int& out_h)
{
  int w = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
  int h = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);
  out_w = std::max(w, 0);
  out_h = std::max(h, 0);
  return w > 0 && h > 0;
}

void report(const rect& a, const rect& b, const std::string& name_a,
            const std::string& name_b, const std::string& path)
{
  int ow;
  int oh;
  overlap_extents(a, b, ow, oh);
  int rx1 = std::max(a.x, b.x);
  int ry1 = std::max(a.y, b.y);
  int rx2 = rx1 + ow;
  int ry2 = ry1 + oh;
  std::cout << path << "\n"
            << "OVERLAP:\n"
            << "  " << name_a << " (" << a.x << ", " << a.y << ", " << a.x + a.w << ", " << a.y + a.h << ")\n"
            << "  " << name_b << " (" << b.x << ", " << b.y << ", " << b.x + b.w << ", " << b.y + b.h << ")\n"
            << "  Overlap Region: (" << rx1 << ", " << ry1 << ", " << rx2 << ", " << ry2 << ")\n";
}

int scan(rc::xml::node container, const std::string& path)
{
  int count = 0;
  std::string class_name = container.attribute("class").value();
  bool is_tab = (class_name == "QTabWidget");

  std::vector<rc::xml::node> children;
  for(rc::xml::node child : container.children("widget"))
    children.push_back(child);

  // Compare siblings within the same coordinate space. Tab pages are stacked
  // by design at an identical geometry, so a QTabWidget's direct children
  // (the pages) are not compared against each other.
  if(!is_tab)
  {
    for(size_t i = 0; i < children.size(); ++i)
    {
      rect a;
      if(!read_rect(children[i], a))
        continue;
      std::string name_a = children[i].attribute("name").value();
      for(size_t j = i + 1; j < children.size(); ++j)
      {
        rect b;
        if(!read_rect(children[j], b))
          continue;
        int ow;
        int oh;
        if(overlap_extents(a, b, ow, oh))
        {
          std::string name_b = children[j].attribute("name").value();
          report(a, b, name_a, name_b, path);
          ++count;
        }
      }
    }
  }

  for(rc::xml::node child : children)
  {
    std::string child_path = path + "/" + child.attribute("name").value();
    count += scan(child, child_path);
  }
  return count;
}

void check_file(const std::string& file)
{
  rc::xml::document doc;
  rc::xml::parse_result result = doc.load_file(file.c_str());
  if(!result)
  {
    std::cerr << file << ": parse error: " << result.description() << "\n";
    return;
  }

  rc::xml::node ui = doc.child("ui");
  int count = 0;
  for(rc::xml::node widget : ui.children("widget"))
    count += scan(widget, widget.attribute("name").value());
  std::cout << file << ": " << count << " overlapping sibling pair(s)\n\n";
}
}  // namespace

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cerr << "usage: ui_overlap <file.ui> [<file.ui> ... | <dir>]\n";
    return 1;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::filesystem::path p(argv[i]);
    if(std::filesystem::is_directory(p))
    {
      std::vector<std::string> files;
      for(auto& entry : std::filesystem::directory_iterator(p))
      {
        if(entry.path().extension() == ".ui")
          files.push_back(entry.path().string());
      }
      std::sort(files.begin(), files.end());
      for(const auto& f : files)
        check_file(f);
    }
    else
    {
      check_file(argv[i]);
    }
  }
  return 0;
}
