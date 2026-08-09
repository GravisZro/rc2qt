#include "rc_layout.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <numeric>

namespace rc
{
namespace layout
{

namespace
{

/* Pixel tolerance for grouping and alignment. RC dialogs are grid-aligned;
   these absorb small rounding differences between the DLU->pixel conversion
   and the Qt style's own metrics. */
constexpr int k_tol = 2;
constexpr int k_margin = 24;

/* Group indices into components of the interval-overlap graph on one axis.
   Two children belong to the same group when their intervals on that axis
   overlap (within k_tol), transitively. vertical selects the y axis. Groups
   come back ordered (top to bottom / left to right), members in reading
   order. */
std::vector<std::vector<int>> group_by_interval(const std::vector<child>& children,
                                                const std::vector<int>& indices,
                                                bool vertical)
{
  const int n = static_cast<int>(indices.size());
  std::vector<int> parent(n);
  std::iota(parent.begin(), parent.end(), 0);

  const auto find_root = [&](auto&& self, int i) -> int
  {
    if(parent[i] != i)
      parent[i] = self(self, parent[i]);
    return parent[i];
  };

  const auto overlap = [&](int a, int b)
  {
    const rect& ra = children[a].bounds;
    const rect& rb = children[b].bounds;
    if(vertical)
      return ra.y < rb.y + rb.h + k_tol && rb.y < ra.y + ra.h + k_tol;
    return ra.x < rb.x + rb.w + k_tol && rb.x < ra.x + ra.w + k_tol;
  };

  for(int i = 0; i < n; ++i)
  {
    for(int j = i + 1; j < n; ++j)
    {
      int ri = find_root(find_root, i);
      int rj = find_root(find_root, j);
      if(ri != rj && overlap(indices[i], indices[j]))
        parent[ri] = rj;
    }
  }

  std::vector<int> root_of(n);
  std::vector<std::vector<int>> local_groups;
  for(int i = 0; i < n; ++i)
  {
    int root = find_root(find_root, i);
    root_of[i] = root;
    bool found = false;
    for(size_t gi = 0; gi < local_groups.size(); ++gi)
    {
      if(root_of[local_groups[gi].front()] == root)
      {
        local_groups[gi].push_back(i);
        found = true;
        break;
      }
    }
    if(!found)
      local_groups.push_back({ i });
  }

  std::vector<std::vector<int>> groups;
  groups.reserve(local_groups.size());
  for(const auto& lg : local_groups)
  {
    std::vector<int> g;
    g.reserve(lg.size());
    for(int li : lg)
      g.push_back(indices[li]);
    groups.push_back(std::move(g));
  }

  for(auto& group : groups)
  {
    std::stable_sort(group.begin(), group.end(), [&](int a, int b)
    {
      const rect& ra = children[a].bounds;
      const rect& rb = children[b].bounds;
      if(vertical)
        return ra.x < rb.x || (ra.x == rb.x && ra.y < rb.y);
      return ra.y < rb.y || (ra.y == rb.y && ra.x < rb.x);
    });
  }

  std::sort(groups.begin(), groups.end(), [&](const std::vector<int>& a,
                                              const std::vector<int>& b)
  {
    const rect& ra = children[a.front()].bounds;
    const rect& rb = children[b.front()].bounds;
    if(vertical)
      return ra.y < rb.y || (ra.y == rb.y && ra.x < rb.x);
    return ra.x < rb.x || (ra.x == rb.x && ra.y < rb.y);
  });

  return groups;
}

/* Bounding box of a node's leaves. */
void node_bounds(const std::vector<child>& children, const node& n,
                 int& min_x, int& min_y, int& max_x, int& max_y)
{
  if(n.k == node::kind::item)
  {
    const rect& b = children[n.control_index].bounds;
    min_x = std::min(min_x, b.x);
    min_y = std::min(min_y, b.y);
    max_x = std::max(max_x, b.x + b.w);
    max_y = std::max(max_y, b.y + b.h);
    return;
  }
  for(const node& c : n.children)
    node_bounds(children, c, min_x, min_y, max_x, max_y);
}

/* Edge-boundary grid over the given children: every distinct start/end on
   each axis becomes a row/column boundary and each child spans exactly the
   interval its rect covers. */
node build_grid(const std::vector<child>& children, const std::vector<int>& indices)
{
  node g;
  g.k = node::kind::grid;
  if(indices.empty())
    return g;

  std::vector<int> xb;
  std::vector<int> yb;
  xb.reserve(indices.size() * 2);
  yb.reserve(indices.size() * 2);
  for(int idx : indices)
  {
    const rect& b = children[idx].bounds;
    xb.push_back(b.x);
    xb.push_back(b.x + b.w);
    yb.push_back(b.y);
    yb.push_back(b.y + b.h);
  }
  std::sort(xb.begin(), xb.end());
  xb.erase(std::unique(xb.begin(), xb.end()), xb.end());
  std::sort(yb.begin(), yb.end());
  yb.erase(std::unique(yb.begin(), yb.end()), yb.end());

  g.columns = static_cast<int>(xb.size()) - 1;
  g.rows = static_cast<int>(yb.size()) - 1;

  const auto boundary_index = [](const std::vector<int>& bounds, int value)
  {
    return static_cast<int>(
      std::lower_bound(bounds.begin(), bounds.end(), value) - bounds.begin());
  };

  for(int idx : indices)
  {
    const rect& b = children[idx].bounds;
    node leaf;
    leaf.control_index = idx;
    node::cell c;
    c.column = boundary_index(xb, b.x);
    c.colspan = std::max(1, boundary_index(xb, b.x + b.w) - c.column);
    c.row = boundary_index(yb, b.y);
    c.rowspan = std::max(1, boundary_index(yb, b.y + b.h) - c.row);
    g.cells.push_back(c);
    g.children.push_back(std::move(leaf));
  }

  /* Label/value tables: if column 0 holds only labels and the value column
     starts one or more empty columns later, extend the label column to the
     value column's left edge so values land on their RC x position. */
  if(g.rows >= 2)
  {
    bool labels_only = true;
    for(size_t i = 0; i < g.cells.size(); ++i)
    {
      if(g.cells[i].column == 0 && g.children[i].k == node::kind::item)
      {
        const std::string& cls = children[g.children[i].control_index].qt_class;
        if(cls != "QLabel" || g.cells[i].colspan != 1)
        {
          labels_only = false;
          break;
        }
      }
    }
    if(labels_only)
    {
      int value_col = INT_MAX;
      for(size_t i = 0; i < g.cells.size(); ++i)
      {
        const node::cell& c = g.cells[i];
        const std::string& cls = children[g.children[i].control_index].qt_class;
        if(c.column > 0 && cls != "QLabel")
          value_col = std::min(value_col, c.column);
      }
      if(value_col != INT_MAX && value_col < static_cast<int>(xb.size()))
        g.label_column_minwidth = xb[value_col] - xb[0];
    }
  }

  /* Keypad-style grids: many equal-sized cells forming a dense rectangle. Give
     every row/column equal stretch so the cells stay equal on resize. */
  if(indices.size() >= 6 && g.rows >= 2 && g.columns >= 2 &&
     g.rows * g.columns == static_cast<int>(indices.size()))
  {
    const rect& r0 = children[indices.front()].bounds;
    bool uniform = true;
    for(int idx : indices)
    {
      const rect& b = children[idx].bounds;
      if(std::abs(b.w - r0.w) > 1 || std::abs(b.h - r0.h) > 1)
      {
        uniform = false;
        break;
      }
    }
    for(const node::cell& c : g.cells)
    {
      if(c.rowspan != 1 || c.colspan != 1)
      {
        uniform = false;
        break;
      }
    }
    if(uniform)
    {
      g.equal_row_stretch = 1;
      g.equal_col_stretch = 1;
    }
  }

  return g;
}

/* Cross-axis boundaries strictly inside a group's extent. For a row group
   (vertical stacking) these are the x coordinates that split the row into
   columns; for a column group they are the y coordinates that split it into
   rows. */
std::vector<int> group_splits(const std::vector<child>& children,
                              const std::vector<int>& group, bool vertical)
{
  int lo = INT_MAX;
  int hi = INT_MIN;
  for(int idx : group)
  {
    const rect& b = children[idx].bounds;
    int s = vertical ? b.x : b.y;
    int e = vertical ? b.x + b.w : b.y + b.h;
    lo = std::min(lo, s);
    hi = std::max(hi, e);
  }
  std::vector<int> splits;
  for(int idx : group)
  {
    const rect& b = children[idx].bounds;
    int s = vertical ? b.x : b.y;
    int e = vertical ? b.x + b.w : b.y + b.h;
    if(s > lo && s < hi)
      splits.push_back(s);
    if(e > lo && e < hi)
      splits.push_back(e);
  }
  std::sort(splits.begin(), splits.end());
  splits.erase(std::unique(splits.begin(), splits.end()), splits.end());
  return splits;
}

bool splits_compatible(const std::vector<int>& a, const std::vector<int>& b)
{
  if(a.size() != b.size())
    return false;
  for(size_t i = 0; i < a.size(); ++i)
  {
    if(std::abs(a[i] - b[i]) > k_tol)
      return false;
  }
  return true;
}

node solve_rec(const std::vector<child>& children, const std::vector<int>& indices,
               unsigned enabled, int depth, int container_w, int container_h);

/* Build a box from a group decomposition of its children. vertical selects
   box_y (groups are horizontal bands) vs box_x (groups are vertical columns).
   Consecutive groups that share a cross-axis structure become a nested grid
   subgroup. container_extent is the container size along the box axis
   (meaningful for the root container only) and drives the trailing spacer. */
node build_box(const std::vector<child>& children, bool vertical,
               const std::vector<std::vector<int>>& groups,
               unsigned enabled, int depth, int container_extent,
               int container_w, int container_h)
{
  node box;
  box.k = vertical ? node::kind::box_y : node::kind::box_x;

  struct slot
  {
    bool is_grid = false;
    std::vector<int> members;
  };

  std::vector<slot> slots;
  size_t i = 0;
  while(i < groups.size())
  {
    if(groups[i].size() < 2 || !(enabled & pattern_grid))
    {
      slots.push_back({ false, groups[i] });
      ++i;
      continue;
    }
    std::vector<int> splits = group_splits(children, groups[i], vertical);
    size_t j = i + 1;
    while(j < groups.size() && groups[j].size() >= 2)
    {
      if(!splits_compatible(splits, group_splits(children, groups[j], vertical)))
        break;
      ++j;
    }
    if(j - i >= 2)
    {
      std::vector<int> members;
      for(size_t g = i; g < j; ++g)
        members.insert(members.end(), groups[g].begin(), groups[g].end());
      slots.push_back({ true, std::move(members) });
    }
    else
    {
      slots.push_back({ false, groups[i] });
    }
    i = j;
  }

  for(const slot& s : slots)
  {
    if(s.is_grid)
    {
      box.children.push_back(build_grid(children, s.members));
    }
    else
    {
      box.children.push_back(solve_rec(children, s.members, enabled, depth + 1,
                                       container_w, container_h));
    }
  }

  /* Cross-axis alignment of each child against the box's overall extent. */
  int min_a = INT_MAX;
  int max_a = INT_MIN;
  {
    int mnx = INT_MAX, mny = INT_MAX, mxx = INT_MIN, mxy = INT_MIN;
    node_bounds(children, box, mnx, mny, mxx, mxy);
    min_a = vertical ? mnx : mny;
    max_a = vertical ? mxx : mxy;
  }
  const int extent = max_a - min_a;
  for(node& c : box.children)
  {
    int mnx = INT_MAX, mny = INT_MAX, mxx = INT_MIN, mxy = INT_MIN;
    node_bounds(children, c, mnx, mny, mxx, mxy);
    const int c_lo = vertical ? mnx : mny;
    const int c_hi = vertical ? mxx : mxy;
    if(vertical)
    {
      if((mxx - mnx) >= extent - k_tol)
        c.halign = align_h::fill;
      else if(c.k == node::kind::grid && mnx <= min_a + k_margin &&
              mxx >= max_a - k_margin)
      {
        /* A grid whose content spans nearly the whole band is a full-width row
           with wide internal gaps: fill it so the empty gap columns expand into
           real space instead of collapsing under a left alignment. */
        c.halign = align_h::fill;
      }
      else if(c_lo <= min_a + k_margin)
        c.halign = align_h::left;
      else if(c_hi >= max_a - k_margin)
        c.halign = align_h::right;
      else
        c.halign = align_h::center;
    }
    else
    {
      if((mxy - mny) >= extent - k_tol)
        c.valign = align_v::fill;
      else if(c.k == node::kind::grid && mny <= min_a + k_margin &&
              mxy >= max_a - k_margin)
        c.valign = align_v::fill;
      else if(c_lo <= min_a + k_margin)
        c.valign = align_v::top;
      else if(c_hi >= max_a - k_margin)
        c.valign = align_v::bottom;
      else
        c.valign = align_v::center;
    }
  }

  /* Trailing spacer for the free space below the content, so widgets keep
     their RC positions when the container grows. */
  if(depth == 0 && (enabled & pattern_spacer) && container_extent > 0)
  {
    int mnx = INT_MAX, mny = INT_MAX, mxx = INT_MIN, mxy = INT_MIN;
    node_bounds(children, box, mnx, mny, mxx, mxy);
    const int used = vertical ? mxy : mxx;
    const int leftover = container_extent - used;
    if(leftover > 0)
      box.spacer_size = leftover;
  }

  if(box.children.size() == 1 && box.spacer_size == 0)
    return std::move(box.children.front());

  return box;
}

node solve_rec(const std::vector<child>& children, const std::vector<int>& indices,
               unsigned enabled, int depth, int container_w, int container_h)
{
  if(indices.empty())
    return {};

  if(indices.size() == 1)
  {
    node leaf;
    leaf.control_index = indices.front();
    return leaf;
  }

  std::vector<std::vector<int>> bands = group_by_interval(children, indices, true);
  std::vector<std::vector<int>> cols = group_by_interval(children, indices, false);

  if(bands.size() > 1)
  {
    return build_box(children, true, bands, enabled, depth, container_h,
                     container_w, container_h);
  }

  /* One horizontal band: several widgets side by side. QGridLayout sizes its
     columns from the widgets' minimum widths, which reproduces the RC x
     positions far better than a QHBoxLayout (whose sizeHint-based
     distribution shifts trailing widgets). Emit a single-row grid. */
  if(cols.size() > 1)
  {
    return build_grid(children, indices);
  }

  /* One strip on both axes with several children: a genuinely overlapping
     group, fall back to the edge-boundary grid. */
  return build_grid(children, indices);
}

} // namespace

node solve_container(const std::vector<child>& children, unsigned enabled,
                     int container_w, int container_h)
{
  if(children.empty())
    return {};

  std::vector<int> indices(children.size());
  for(size_t i = 0; i < indices.size(); ++i)
    indices[i] = static_cast<int>(i);

  if(!(enabled & pattern_box))
    return build_grid(children, indices);

  node root = solve_rec(children, indices, enabled, 0, container_w, container_h);

  if(root.k == node::kind::item)
    return build_grid(children, indices);

  return root;
}

} // namespace layout
} // namespace rc
