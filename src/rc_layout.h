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
  /* Extra margin of error (pixels) for this widget when deciding layout
     uniformity: row/column grouping, split compatibility, and alignment.
     0 = no override; the effective tolerance of a pair of widgets is the
     larger of the base tolerance and the two widgets' margins. */
  int tol = 0;
};

/* Alignment of a node on the cross axis of its parent box (or within a grid
   cell). fill stretches the node to the available extent; left/right/center
   and top/bottom/center pin it to a side. */
enum class align_h
{
  fill,
  left,
  right,
  center
};

enum class align_v
{
  fill,
  top,
  bottom,
  center
};

/* Recursive layout tree. A node is either a leaf widget (item), a horizontal
   box (box_x), a vertical box (box_y), or a grid. Box children stack in
   reading order with per-child alignment; grid children additionally carry a
   cell. The tree mirrors the RC geometry: boxes come from clean one-axis
   stacking, grids from 2D subsections whose rows and columns line up with the
   source boundaries. */
struct node
{
  enum class kind
  {
    item,
    box_x,
    box_y,
    grid
  };

  kind k = kind::item;
  /* item: index into solve_container()'s input children. */
  int control_index = -1;

  /* Placement of this node inside its parent box / grid cell. */
  align_h halign = align_h::fill;
  align_v valign = align_v::fill;

  /* Box children in stacking order (box_x: left to right; box_y: top to
     bottom). Grid children in the same order, one cell per child. */
  std::vector<node> children;

  struct cell
  {
    int row = 0;
    int column = 0;
    int rowspan = 1;
    int colspan = 1;
  };
  std::vector<cell> cells;

  /* Grid dimensions. These match the source boundary counts so the emitted
     row/column attributes mirror the absolute layout. */
  int rows = 0;
  int columns = 0;
  /* Grid: when > 0, every row/column gets this stretch factor so the cells
     stay equal on resize (keypad-style grids). */
  int equal_row_stretch = 0;
  int equal_col_stretch = 0;
  /* Grid: label/value column minimum width. When the first column holds only
     labels and the value column starts one or more empty (gap) columns later,
     extending the label column to the value column's left edge makes the
     values land at their RC x position. Emitted as columnminimumwidth. */
  int label_column_minwidth = 0;

  /* Box: trailing spacer that absorbs the free space below/right of the
     content so the widgets keep their RC positions when the container grows.
     size is the pixel gap at the RC size. */
  int spacer_size = 0;
};

/* A/B testing switch: the layout solver only applies the components whose bit
   is set, so they can be disabled to measure their individual impact. With a
   mask of 0 the solver falls back to the M1 edge-boundary grid. */
enum pattern_flag : uint8_t
{
  pattern_none    = 0x00,
  pattern_box     = 0x01, // box decomposition (VLayout/HLayout)
  pattern_grid    = 0x02, // grid subgroups inside boxes
  pattern_align   = 0x04, // per-item alignment
  pattern_stretch = 0x08, // grid equal row/column stretch
  pattern_spacer  = 0x10, // trailing spacer items for free space
  pattern_all = pattern_box | pattern_grid | pattern_align |
                pattern_stretch | pattern_spacer,
};

/* Resolve a container's children into a recursive layout tree. The edge-grid
   fallback (single QGridLayout, M1) is used when box decomposition is
   disabled or no clean stacking axis exists. container_w/container_h are the
   container's pixel size; they let boxes size their trailing spacer so the
   RC layout's free space is reproduced. */
node solve_container(const std::vector<child>& children,
                     pattern_flag enabled = pattern_all,
                     int container_w = 0,
                     int container_h = 0,
                     int tol = 2);

} // namespace layout
} // namespace rc

#endif
