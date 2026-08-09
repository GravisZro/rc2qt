#ifndef RC_LAYOUT_H
#define RC_LAYOUT_H

#include <string>
#include <vector>

namespace rc
{
namespace layout
{

struct rect
{
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
};

/* One widget of a container. The layout engine is pure geometry: it only needs
   the pixel rect and the Qt widget class. control_index lets the caller map a
   plan entry back to its own container child (control + metadata). */
struct child
{
  int control_index = -1;
  std::string qt_class;
  rect bounds;
};

/* Placement of a widget inside a QGridLayout. */
struct cell_span
{
  int row = 0;
  int column = 0;
  int rowspan = 1;
  int colspan = 1;
};

/* Per-container layout strategy. M1 resolves every container to a single
   QGridLayout (kind == none). Pattern matchers (M2) may select a box/grid
   pattern instead. */
enum class pattern_kind
{
  none,
  button_row,
  label_table,
  stack_rows,
  keypad_grid
};

struct container_plan
{
  pattern_kind kind = pattern_kind::none;
  std::vector<cell_span> spans;
  int rows = 0;
  int columns = 0;
};

/* Resolve a container's children into a grid. Column boundaries are every
   distinct x-start/x-end and row boundaries every distinct y-start/y-end, so
   each child occupies exactly the row/column interval its rect spans. Two
   source-non-overlapping children therefore can never share a grid cell. */
container_plan solve_container(const std::vector<child>& children);

} // namespace layout
} // namespace rc

#endif
