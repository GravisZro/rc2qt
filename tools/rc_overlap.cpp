#include "rc_ast.h"
#include "rc_parser.h"
#include "rc_tokenizer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
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

bool overlap_extents(const rect& a, const rect& b, int& out_w, int& out_h)
{
  int w = std::min(a.x + a.w, b.x + b.w) - std::max(a.x, b.x);
  int h = std::min(a.y + a.h, b.y + b.h) - std::max(a.y, b.y);
  out_w = std::max(w, 0);
  out_h = std::max(h, 0);
  return w > 0 && h > 0;
}

bool is_groupbox(const rc::control& ctrl)
{
  if(ctrl.keyword == "GROUPBOX")
    return true;
  if(ctrl.keyword != "CONTROL")
    return false;
  if(ctrl.class_name != "Button")
    return false;
  for(const auto& op : ctrl.style.ops)
  {
    if(op.first == "BS_GROUPBOX" || op.second == "BS_GROUPBOX")
      return true;
  }
  return ctrl.style.first == "BS_GROUPBOX";
}

// Mirrors the generator: a non-groupbox control whose centre point lies inside
// a groupbox rect is treated as that groupbox's child, so the control sitting
// on its own groupbox frame is overlap-by-design. Combo boxes use their closed
// height, since the drop-down list is only shown while open.
bool centre_inside(const rc::control& ctrl, const rc::control& gb)
{
  int disp_h = static_cast<int>(ctrl.height);
  if(ctrl.keyword == "COMBOBOX")
    disp_h = 14;
  int cx = ctrl.x + static_cast<int>(ctrl.width) / 2;
  int cy = ctrl.y + disp_h / 2;
  int gb_x1 = gb.x;
  int gb_y1 = gb.y;
  int gb_x2 = gb.x + static_cast<int>(gb.width);
  int gb_y2 = gb.y + static_cast<int>(gb.height);
  return cx >= gb_x1 && cx < gb_x2 && cy >= gb_y1 && cy < gb_y2;
}

void label(const rc::control& c, std::string& out)
{
  out = c.id;
  if(!c.text.empty())
    out += " ['" + c.text + "']";
}

void check_dialog(const rc::resource& res)
{
  const auto& dd = std::get<rc::dialog_data>(res.data);
  std::vector<rc::control> controls = dd.controls;
  std::vector<int> groupbox_flags(controls.size(), 0);
  for(size_t i = 0; i < controls.size(); ++i)
  {
    if(is_groupbox(controls[i]))
      groupbox_flags[i] = 1;
  }

  std::vector<int> is_child(controls.size(), -1);
  for(size_t g = 0; g < controls.size(); ++g)
  {
    if(!groupbox_flags[g])
      continue;
    for(size_t i = 0; i < controls.size(); ++i)
    {
      if(static_cast<int>(i) == static_cast<int>(g))
        continue;
      if(is_child[i] >= 0)
        continue;
      if(groupbox_flags[i])
        continue;
      if(centre_inside(controls[i], controls[g]))
        is_child[i] = static_cast<int>(g);
    }
  }

  int genuine = 0;
  std::cout << res.id << "\n";
  for(size_t i = 0; i < controls.size(); ++i)
  {
    rect a = { controls[i].x, controls[i].y,
               static_cast<int>(controls[i].width), static_cast<int>(controls[i].height) };
    for(size_t j = i + 1; j < controls.size(); ++j)
    {
      rect b = { controls[j].x, controls[j].y,
                 static_cast<int>(controls[j].width), static_cast<int>(controls[j].height) };
      int ow;
      int oh;
      if(!overlap_extents(a, b, ow, oh))
        continue;

      if(is_child[static_cast<int>(i)] == static_cast<int>(j) ||
         is_child[static_cast<int>(j)] == static_cast<int>(i))
        continue;

      std::string na;
      std::string nb;
      label(controls[i], na);
      label(controls[j], nb);
      int rx1 = std::max(a.x, b.x);
      int ry1 = std::max(a.y, b.y);
      int rx2 = rx1 + ow;
      int ry2 = ry1 + oh;
      std::cout << "OVERLAP:\n"
                << "  " << na << " (" << a.x << ", " << a.y << ", " << a.x + a.w << ", " << a.y + a.h << ")\n"
                << "  " << nb << " (" << b.x << ", " << b.y << ", " << b.x + b.w << ", " << b.y + b.h << ")\n"
                << "  Overlap Region: (" << rx1 << ", " << ry1 << ", " << rx2 << ", " << ry2 << ")\n";
      ++genuine;
    }
  }
  std::cout << "  -- " << genuine << " overlap(s)\n\n";
}
}  // namespace

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cerr << "usage: rc_overlap <file.rc>\n";
    return 1;
  }

  std::ifstream ifs(argv[1]);
  if(!ifs)
  {
    std::cerr << argv[1] << ": cannot open\n";
    return 1;
  }
  std::string input((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  auto tokens = rc::tokenize(input);
  rc::parser p(tokens);
  auto file = p.parse();

  for(const auto& res : file.resources)
  {
    if(std::holds_alternative<rc::dialog_data>(res.data))
      check_dialog(res);
  }
  return 0;
}
