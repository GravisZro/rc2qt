#include "rc_layout.h"

#include <algorithm>
#include <cmath>

namespace rc
{
namespace layout
{

namespace
{

/* Tolerances (pixels) for grouping and matching. RC dialogs are grid-aligned;
   these absorb small rounding differences between the DLU->pixel conversion
   and the Qt style's own metrics. */
constexpr int k_band_tol = 2; /* y-center tolerance for grouping into rows */
constexpr int k_x_tol = 4;    /* x tolerance for a shared column split */
constexpr int k_h_tol = 4;    /* height tolerance for a button row */

bool is_button_class(const std::string& cls)
{
  return cls == "QPushButton" || cls == "QCheckBox" || cls == "QRadioButton";
}

int boundary_index(const std::vector<int>& bounds, int value)
{
  auto it = std::lower_bound(bounds.begin(), bounds.end(), value);
  return static_cast<int>(it - bounds.begin());
}

/* Edge-boundary grid fallback: every distinct x-start/x-end and y-start/y-end
   becomes a column/row boundary, and each child spans exactly the interval its
   rect covers. Guarantees source-non-overlapping children never share a cell.
   The boundary vectors are returned for band cell computation. */
container_plan edge_grid(const std::vector<child>& children,
                         std::vector<int>* xb_out = nullptr,
                         std::vector<int>* yb_out = nullptr)
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

  if(xb_out)
    *xb_out = std::move(xb);
  if(yb_out)
    *yb_out = std::move(yb);
  return plan;
}

/* Group child indices into horizontal bands by row center (tolerance
   k_band_tol). Bands are returned in top-to-bottom, left-to-right order. */
std::vector<std::vector<int>> y_bands(const std::vector<child>& children)
{
  std::vector<int> order(children.size());
  for(size_t i = 0; i < order.size(); ++i)
    order[i] = static_cast<int>(i);
  std::stable_sort(order.begin(), order.end(), [&](int a, int b)
  {
    if(children[a].bounds.y != children[b].bounds.y)
      return children[a].bounds.y < children[b].bounds.y;
    return children[a].bounds.x < children[b].bounds.x;
  });

  std::vector<std::vector<int>> bands;
  for(int idx : order)
  {
    const rect& b = children[idx].bounds;
    int center = b.y + b.h / 2;
    bool placed = false;
    for(auto& band : bands)
    {
      const rect& ref = children[band.front()].bounds;
      if(std::abs(center - (ref.y + ref.h / 2)) <= k_band_tol)
      {
        band.push_back(idx);
        placed = true;
        break;
      }
    }
    if(!placed)
      bands.push_back({ idx });
  }
  for(auto& band : bands)
  {
    std::stable_sort(band.begin(), band.end(), [&](int a, int b)
    {
      return children[a].bounds.x < children[b].bounds.x;
    });
  }
  return bands;
}

void assign_row_major(container_plan& plan,
                      const std::vector<std::vector<int>>& bands)
{
  for(size_t r = 0; r < bands.size(); ++r)
  {
    for(size_t c = 0; c < bands[r].size(); ++c)
    {
      cell_span& span = plan.spans[bands[r][c]];
      span.row = static_cast<int>(r);
      span.column = static_cast<int>(c);
    }
  }
}

/* -- whole-container matchers ------------------------------------------- */

/* ButtonRow: every child is a button-class widget, all share one y band and
   roughly equal heights, and none overlap horizontally. Emitted as a single
   QHBoxLayout with buttons in left-to-right order. */
bool match_button_row(const std::vector<child>& children, container_plan& plan)
{
  if(children.size() < 2)
    return false;
  int y0 = children.front().bounds.y;
  int h0 = children.front().bounds.h;
  for(const child& c : children)
  {
    if(!is_button_class(c.qt_class))
      return false;
    if(std::abs(c.bounds.y - y0) > k_band_tol)
      return false;
    if(std::abs(c.bounds.h - h0) > k_h_tol)
      return false;
  }
  for(size_t i = 0; i < children.size(); ++i)
  {
    for(size_t j = i + 1; j < children.size(); ++j)
    {
      const rect& a = children[i].bounds;
      const rect& b = children[j].bounds;
      if(a.x < b.x + b.w && b.x < a.x + a.w)
        return false;
    }
  }

  std::vector<int> order(children.size());
  for(size_t i = 0; i < order.size(); ++i)
    order[i] = static_cast<int>(i);
  std::stable_sort(order.begin(), order.end(), [&](int a, int b)
  {
    return children[a].bounds.x < children[b].bounds.x;
  });
  for(size_t c = 0; c < order.size(); ++c)
  {
    cell_span& span = plan.spans[order[c]];
    span.row = 0;
    span.column = static_cast<int>(c);
  }
  plan.kind = pattern_kind::button_row;
  plan.rows = 1;
  plan.columns = static_cast<int>(children.size());
  return true;
}

/* LabelTable: children form rows (y bands) of label + value pairs. Every row
   has the same column count, its first widget is a QLabel, and the label and
   value columns share a common x split across rows. Emitted as a QGridLayout
   with the shared label column enforced via columnminimumwidth. */
bool match_label_table(const std::vector<child>& children, container_plan& plan)
{
  if(children.size() < 4)
    return false;
  std::vector<std::vector<int>> bands = y_bands(children);
  if(bands.size() < 2)
    return false;
  size_t cols = bands.front().size();
  if(cols < 2)
    return false;
  for(const auto& band : bands)
  {
    if(band.size() != cols)
      return false;
    if(children[band.front()].qt_class != "QLabel")
      return false;
  }

  int label_x = children[bands.front().front()].bounds.x;
  int value_x = children[bands.front()[1]].bounds.x;
  int label_w = 0;
  for(const auto& band : bands)
  {
    if(std::abs(children[band.front()].bounds.x - label_x) > k_x_tol)
      return false;
    if(std::abs(children[band[1]].bounds.x - value_x) > k_x_tol)
      return false;
    label_w = std::max(label_w, children[band.front()].bounds.w);
  }

  assign_row_major(plan, bands);
  plan.kind = pattern_kind::label_table;
  plan.rows = static_cast<int>(bands.size());
  plan.columns = static_cast<int>(cols);
  plan.label_column_minwidth = label_w;
  return true;
}

/* StackRows: non-overlapping horizontal bands each holding exactly one widget,
   so children stack vertically. Emitted as a single QVBoxLayout. */
bool match_stack_rows(const std::vector<child>& children, container_plan& plan)
{
  if(children.size() < 2)
    return false;
  std::vector<std::vector<int>> bands = y_bands(children);
  if(bands.size() != children.size())
    return false;

  for(size_t r = 0; r < bands.size(); ++r)
  {
    cell_span& span = plan.spans[bands[r].front()];
    span.row = static_cast<int>(r);
    span.column = 0;
  }
  plan.kind = pattern_kind::stack_rows;
  plan.rows = static_cast<int>(children.size());
  plan.columns = 1;
  return true;
}

/* KeypadGrid: many equal-sized, evenly spaced children forming a dense grid.
   Emitted as a QGridLayout with equal row/column stretch so cells stay equal
   on resize. */
bool match_keypad_grid(const std::vector<child>& children, container_plan& plan)
{
  if(children.size() < 6)
    return false;
  const rect& r0 = children.front().bounds;
  for(const child& c : children)
  {
    if(std::abs(c.bounds.w - r0.w) > 1 || std::abs(c.bounds.h - r0.h) > 1)
      return false;
  }

  /* Uniform spacing: the horizontal and vertical pitch must each be constant
     across the whole grid. */
  int pitch_x = 0;
  int pitch_y = 0;
  for(const child& c : children)
  {
    bool found_x = false;
    bool found_y = false;
    for(const child& d : children)
    {
      if(&c == &d)
        continue;
      if(d.bounds.x > c.bounds.x && d.bounds.y == c.bounds.y &&
         std::abs(d.bounds.h - c.bounds.h) <= 1 && !found_x)
      {
        found_x = true;
        int px = d.bounds.x - c.bounds.x;
        if(pitch_x == 0)
          pitch_x = px;
        else if(px != pitch_x)
          return false;
      }
      if(d.bounds.y > c.bounds.y && d.bounds.x == c.bounds.x &&
         std::abs(d.bounds.w - c.bounds.w) <= 1 && !found_y)
      {
        found_y = true;
        int py = d.bounds.y - c.bounds.y;
        if(pitch_y == 0)
          pitch_y = py;
        else if(py != pitch_y)
          return false;
      }
    }
  }
  if(pitch_x <= 0 || pitch_y <= 0)
    return false;

  /* Children must lie exactly on the grid. */
  int x0 = r0.x;
  int y0 = r0.y;
  for(const child& c : children)
  {
    if((c.bounds.x - x0) % pitch_x != 0)
      return false;
    if((c.bounds.y - y0) % pitch_y != 0)
      return false;
  }

  std::vector<std::vector<int>> bands = y_bands(children);
  size_t rows = bands.size();
  if(rows < 2)
    return false;
  size_t cols = bands.front().size();
  if(cols < 2 || cols * rows != children.size())
    return false;

  assign_row_major(plan, bands);
  plan.kind = pattern_kind::keypad_grid;
  plan.rows = static_cast<int>(rows);
  plan.columns = static_cast<int>(cols);
  return true;
}

int pattern_score(pattern_kind kind, size_t n)
{
  switch(kind)
  {
    case pattern_kind::button_row:
      return static_cast<int>(n) * 4;
    case pattern_kind::label_table:
      return static_cast<int>(n) * 3;
    case pattern_kind::keypad_grid:
      return static_cast<int>(n) * 2;
    case pattern_kind::stack_rows:
      return static_cast<int>(n) * 1;
    case pattern_kind::none:
      return 0;
  }
  return 0;
}

/* -- band splits --------------------------------------------------------- */

struct band_candidate
{
  pattern_kind kind = pattern_kind::none;
  std::vector<int> children;
  std::vector<cell_span> spans;
  int label_column_minwidth = 0;
  int score = 0;
};

bool rects_overlap_x(const rect& a, const rect& b)
{
  return a.x < b.x + b.w && b.x < a.x + a.w;
}

/* Best button_row band: a y-band whose children are all buttons of roughly
   equal height with no horizontal overlap. */
band_candidate find_button_row_band(const std::vector<child>& children,
                                    const std::vector<std::vector<int>>& bands)
{
  band_candidate best;
  for(const auto& band : bands)
  {
    if(band.size() < 2)
      continue;
    bool ok = true;
    int h0 = children[band.front()].bounds.h;
    for(int idx : band)
    {
      if(!is_button_class(children[idx].qt_class) ||
         std::abs(children[idx].bounds.h - h0) > k_h_tol)
      {
        ok = false;
        break;
      }
    }
    if(!ok)
      continue;
    for(size_t i = 0; ok && i < band.size(); ++i)
    {
      for(size_t j = i + 1; j < band.size(); ++j)
      {
        if(rects_overlap_x(children[band[i]].bounds, children[band[j]].bounds))
        {
          ok = false;
          break;
        }
      }
    }
    if(!ok)
      continue;

    std::vector<int> order = band;
    std::stable_sort(order.begin(), order.end(), [&](int a, int b)
    {
      return children[a].bounds.x < children[b].bounds.x;
    });
    band_candidate cand;
    cand.kind = pattern_kind::button_row;
    cand.children = order;
    cand.spans.assign(order.size(), cell_span{});
    for(size_t c = 0; c < order.size(); ++c)
      cand.spans[c].column = static_cast<int>(c);
    cand.score = static_cast<int>(order.size()) * 4;
    if(cand.score > best.score)
      best = std::move(cand);
  }
  return best;
}

/* Best label_table band: a contiguous run (in y order) of label+value rows
   sharing a common x split. The run must have at least two rows and four
   widgets. */
band_candidate find_label_table_band(const std::vector<child>& children,
                                     const std::vector<std::vector<int>>& bands)
{
  auto is_form_row = [&](const std::vector<int>& band, int& label_x, int& value_x)
  {
    if(band.size() < 2)
      return false;
    if(children[band.front()].qt_class != "QLabel")
      return false;
    label_x = children[band.front()].bounds.x;
    value_x = children[band[1]].bounds.x;
    return true;
  };

  band_candidate best;
  size_t r = 0;
  while(r < bands.size())
  {
    int label_x = 0;
    int value_x = 0;
    if(!is_form_row(bands[r], label_x, value_x))
    {
      ++r;
      continue;
    }
    size_t run_start = r;
    std::vector<int> run_children = bands[r];
    int max_label_w = children[bands[r].front()].bounds.w;
    size_t end = r;
    while(end + 1 < bands.size())
    {
      int lx = 0;
      int vx = 0;
      if(!is_form_row(bands[end + 1], lx, vx))
        break;
      if(std::abs(lx - label_x) > k_x_tol || std::abs(vx - value_x) > k_x_tol)
        break;
      ++end;
      run_children.insert(run_children.end(), bands[end].begin(), bands[end].end());
      max_label_w = std::max(max_label_w, children[bands[end].front()].bounds.w);
    }
    (void)run_start;
    if(run_children.size() >= 4)
    {
      band_candidate cand;
      cand.kind = pattern_kind::label_table;
      cand.children = run_children;
      cand.spans.assign(run_children.size(), cell_span{});
      for(size_t br = r; br <= end; ++br)
      {
        for(size_t bc = 0; bc < bands[br].size(); ++bc)
        {
          size_t offset = 0;
          for(size_t pr = r; pr < br; ++pr)
            offset += bands[pr].size();
          offset += bc;
          cell_span& span = cand.spans[offset];
          span.row = static_cast<int>(br - r);
          span.column = static_cast<int>(bc);
        }
      }
      /* Encode the RC alignment directly: column 0 width is chosen so the
         value column starts exactly at the shared value x. */
      cand.label_column_minwidth = value_x - label_x - 6;
      cand.score = static_cast<int>(run_children.size()) * 3;
      if(cand.score > best.score)
        best = std::move(cand);
    }
    r = end + 1;
  }
  return best;
}

/* Convert a band candidate into the container plan's band record, computing
   its cell in the top-level edge grid from its bounding box. */
band finish_band(const band_candidate& cand,
                             const std::vector<child>& children,
                             const std::vector<int>& xb, const std::vector<int>& yb)
{
  rc::layout::band b;
  b.kind = cand.kind;
  b.children = cand.children;
  b.spans = cand.spans;
  b.label_column_minwidth = cand.label_column_minwidth;

  int min_x = children[cand.children.front()].bounds.x;
  int max_x = min_x + children[cand.children.front()].bounds.w;
  int min_y = children[cand.children.front()].bounds.y;
  int max_y = min_y + children[cand.children.front()].bounds.h;
  for(int idx : cand.children)
  {
    const rect& r = children[idx].bounds;
    min_x = std::min(min_x, r.x);
    min_y = std::min(min_y, r.y);
    max_x = std::max(max_x, r.x + r.w);
    max_y = std::max(max_y, r.y + r.h);
  }
  b.column = boundary_index(xb, min_x);
  int right = boundary_index(xb, max_x);
  b.colspan = std::max(1, right - b.column);
  b.row = boundary_index(yb, min_y);
  int bottom = boundary_index(yb, max_y);
  b.rowspan = std::max(1, bottom - b.row);

  int max_row = 0;
  int max_col = 0;
  for(const cell_span& s : cand.spans)
  {
    max_row = std::max(max_row, s.row + s.rowspan);
    max_col = std::max(max_col, s.column + s.colspan);
  }
  b.rows = max_row;
  b.columns = max_col;
  return b;
}

} // namespace

container_plan solve_container(const std::vector<child>& children, unsigned enabled)
{
  container_plan best = edge_grid(children);
  int best_score = 0;

  auto try_match = [&](pattern_kind kind, unsigned flag, bool (*match)(const std::vector<child>&, container_plan&))
  {
    if((enabled & flag) == 0)
      return;
    container_plan plan;
    plan.spans.assign(children.size(), cell_span{});
    if(!match(children, plan))
      return;
    int score = pattern_score(kind, children.size());
    if(score > best_score)
    {
      best = plan;
      best_score = score;
    }
  };

  try_match(pattern_kind::label_table, pattern_label_table, match_label_table);
  try_match(pattern_kind::button_row, pattern_button_row, match_button_row);
  try_match(pattern_kind::keypad_grid, pattern_keypad_grid, match_keypad_grid);
  try_match(pattern_kind::stack_rows, pattern_stack_rows, match_stack_rows);

  /* Band splits: keep the top-level edge grid and move a detected pattern into
     a nested layout cell. Only applied when no whole-container pattern won. */
  if(best.kind == pattern_kind::none)
  {
    std::vector<int> xb;
    std::vector<int> yb;
    edge_grid(children, &xb, &yb);

    std::vector<std::vector<int>> bands = y_bands(children);
    band_candidate buttons = find_button_row_band(children, bands);
    band_candidate table = find_label_table_band(children, bands);

    /* Apply every detected band split; the two detectors look at disjoint
       y-regions (a button row band vs. contiguous form rows), so their cells
       in the top-level grid never collide. */
    std::vector<band_candidate*> splits;
    if((enabled & pattern_button_row) && buttons.score > 0)
      splits.push_back(&buttons);
    if((enabled & pattern_label_table) && table.score > 0)
      splits.push_back(&table);

    for(band_candidate* split : splits)
    {
      best.bands.push_back(finish_band(*split, children, xb, yb));
      /* Band children share the band's grid cell; direct children keep their
         edge-grid spans. */
      for(size_t k = 0; k < children.size(); ++k)
      {
        bool in_band = false;
        for(int ci : best.bands.back().children)
        {
          if(ci == static_cast<int>(k))
          {
            in_band = true;
            break;
          }
        }
        if(in_band)
          best.spans[k] = cell_span{ best.bands.back().row,
                                     best.bands.back().column,
                                     best.bands.back().rowspan,
                                     best.bands.back().colspan };
      }
    }
  }

  return best;
}

} // namespace layout
} // namespace rc
