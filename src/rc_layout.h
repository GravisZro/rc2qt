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

/* Per-container layout strategy. none is the edge-boundary grid fallback (M1);
   pattern matchers (M2) may select a box/grid pattern instead. */
enum class pattern_kind
{
  none,
  button_row,
  label_table,
  stack_rows,
  keypad_grid
};

/* A band split: a subset of a container's children placed into a nested
   pattern layout at one cell of the container's top-level grid. children lists
   the child indices (into solve_container's input); spans gives each child's
   placement inside the nested layout (band-relative row/column). */
struct band
{
  pattern_kind kind = pattern_kind::none;
  std::vector<int> children;
  std::vector<cell_span> spans;
  int row = 0;
  int column = 0;
  int rowspan = 1;
  int colspan = 1;
  int rows = 0;
  int columns = 0;
  int label_column_minwidth = 0;
};

struct container_plan
{
  pattern_kind kind = pattern_kind::none;
  std::vector<cell_span> spans;
  int rows = 0;
  int columns = 0;
  /* label_table: minimum width (px) for the shared label column (column 0),
     emitted as the QGridLayout columnminimumwidth attribute. */
  int label_column_minwidth = 0;
  /* Band splits. When non-empty the container is a top-level QGridLayout (kind
     stays none) whose cells hold either a direct child or a nested pattern
     layout from bands[]. plan.spans for band children is the band's cell. */
  std::vector<band> bands;
};

/* A/B testing switch: solve_container only applies the matchers whose bit is
   set, so patterns can be disabled to measure their individual impact. */
enum pattern_flag : unsigned
{
  pattern_none = 0,
  pattern_button_row = 1u << 0,
  pattern_label_table = 1u << 1,
  pattern_stack_rows = 1u << 2,
  pattern_keypad_grid = 1u << 3,
  pattern_all = pattern_button_row | pattern_label_table |
                pattern_stack_rows | pattern_keypad_grid,
};

/* Resolve a container's children into a layout plan. The edge-boundary grid
   (kind == none) is the fallback: column boundaries are every distinct
   x-start/x-end and row boundaries every distinct y-start/y-end, so each child
   occupies exactly the row/column interval its rect spans and two
   source-non-overlapping children can never share a grid cell. Pattern
   matchers (M2) run first; the best-scoring match wins. */
container_plan solve_container(const std::vector<child>& children,
                               unsigned enabled = pattern_all);

} // namespace layout
} // namespace rc

#endif
