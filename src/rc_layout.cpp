#include "rc_layout.h"

#include <algorithm>

namespace rc
{
namespace layout
{

namespace
{

int boundary_index(const std::vector<int>& bounds, int value)
{
  auto it = std::lower_bound(bounds.begin(), bounds.end(), value);
  return static_cast<int>(it - bounds.begin());
}

} // namespace

container_plan solve_container(const std::vector<child>& children)
{
  container_plan plan;
  plan.spans.assign(children.size(), cell_span{});
  if(children.empty())
    return plan;

  std::vector<int> xb;
  std::vector<int> yb;
  xb.reserve(children.size() * 2);
  yb.reserve(children.size() * 2);
  for(const child& c : children)
  {
    xb.push_back(c.bounds.x);
    xb.push_back(c.bounds.x + c.bounds.w);
    yb.push_back(c.bounds.y);
    yb.push_back(c.bounds.y + c.bounds.h);
  }
  std::sort(xb.begin(), xb.end());
  xb.erase(std::unique(xb.begin(), xb.end()), xb.end());
  std::sort(yb.begin(), yb.end());
  yb.erase(std::unique(yb.begin(), yb.end()), yb.end());

  plan.columns = static_cast<int>(xb.size()) - 1;
  plan.rows = static_cast<int>(yb.size()) - 1;

  for(size_t i = 0; i < children.size(); ++i)
  {
    const rect& b = children[i].bounds;
    int left = boundary_index(xb, b.x);
    int right = boundary_index(xb, b.x + b.w);
    int top = boundary_index(yb, b.y);
    int bottom = boundary_index(yb, b.y + b.h);
    cell_span& span = plan.spans[i];
    span.column = left;
    span.colspan = std::max(1, right - left);
    span.row = top;
    span.rowspan = std::max(1, bottom - top);
  }

  return plan;
}

} // namespace layout
} // namespace rc
