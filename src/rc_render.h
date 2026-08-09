#ifndef RC_RENDER_H
#define RC_RENDER_H

#include <map>
#include <string>
#include <vector>

#include <pugixml.hpp>

namespace rc
{
namespace render
{

/* One widget's RC-derived geometry, in the coordinate space of its direct
   container widget (the widget whose layout contains it: dialog, groupbox,
   or tab page). Used as the comparison target for the rendered widget. */
struct target
{
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  std::string container;
};

/* Everything the verifier needs for one dialog: the layout-mode .ui document
   (built in memory by the generator, never written to disk) and the target
   rects keyed by widget objectName. */
struct verify_input
{
  std::string name;
  pugi::xml_document doc;
  std::map<std::string, target> targets;
  int dialog_width = 0;
  int dialog_height = 0;
};

struct result
{
  int target_widgets = 0;
  int rendered_widgets = 0;
  int missing_widgets = 0;
  double mean_iou = 0.0;
  double mean_dx = 0.0;
  double mean_dy = 0.0;
  double mean_dw = 0.0;
  double mean_dh = 0.0;
  double max_dx = 0.0;
  double max_dy = 0.0;
  int overlap_violations = 0;
};

/* Build the widget tree described by input.doc with Qt's Widget API, render
   it offscreen, compare each widget's geometry against input.targets, and
   report deviation/overlap metrics. dump_dir (if non-empty) receives a PNG
   of each rendered dialog. */
result verify_layout(const verify_input& input, const std::string& dump_dir = "");

} // namespace render
} // namespace rc

#endif
