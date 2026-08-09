# Plan: Convert absolute RC coordinates into Qt layout managers

Status: in progress. Phase 1 (infrastructure) implemented and verified; Phase 2
(guillotine split) in progress.

## 1. Objective & scope

Add a new opt-in layout mode (`-l/--layout`) that emits Qt Designer layouts
(`QHBoxLayout`, `QVBoxLayout`, `QGridLayout`, spacers) instead of absolute
geometry, inferred from the RC's dialog-unit coordinates. Target: **resizable,
approximately-faithful** UI — widgets keep RC-derived minimum sizes so the dialog
opens near its original size but reflows cleanly. The current absolute-geometry
mode stays the default and its golden output must not change.

Scope: dialog body, nested groupboxes (existing hierarchy reused), and tab pages
(`DS_CONTROL` subdialogs). Not in scope: `QFormLayout` detection,
`QStackedLayout`, exact pixel fidelity.

## 2. Target .ui output shape

```xml
<widget class="QDialog" name="IDD_LEVELKEYPAD">
  <property name="geometry"><rect>0,0,274,1515</rect></property>  <!-- RC size -->
  <property name="minimumSize"><size>...</size></property>         <!-- layout min -->
  <layout class="QVBoxLayout" name="verticalLayout">
    <property name="spacing"><number>6</number></property>
    <item><widget class="QGroupBox" name="...">
      <property name="title"><string>Goal Information</string></property>
      <layout class="QGridLayout" name="...">
        <item row="0" column="0"><widget class="QPushButton" name="..."/></item>
        ...
      </layout>
    </widget></item>
    <item><spacer ...orientation Qt::Vertical.../></item>
  </layout>
</widget>
```

Key facts verified from `docs/qtschema.xsd`: a container widget holds one
`<layout>`; layout children are `<item row= column= rowspan= colspan=>` wrapping
`<widget>|<layout>|<spacer>`; layout attributes `stretch`, `rowstretch`,
`columnstretch`, `columnminimumwidth`, `rowminimumheight` exist;
`<layoutdefault>` sets default spacing/margin. Widgets inside a layout carry
**no** `geometry` property.

## 3. Core algorithm: layout inference

All coordinates converted with the existing `dlu_to_pixel_x/y`; text-fit height
adjustments (`fit_text`) and `min_width_px/min_height_px` feed widget minimum
sizes. The event/`y_shift_px` overlap system is **bypassed** in layout mode
(layouts manage spacing).

**Preprocessing** per container (dialog, each groupbox, each tab page): children
= the existing `groupbox_node.children` (relative rects). Each child gets
`(x,y) = dlu_to_pixel`, `(w,h) = max(dlu_to_pixel, min_size)`. Cut detection uses
geometric positions; min sizes only affect sizing.

**`decompose(children, rect)`** — recursive:

1. **Label-table specialization.** Cluster children into row bands by y-overlap.
   For a vertical cut at x=X, check consistency: in >= 2 bands, children exist on
   both sides of X and none straddles it. If a consistent X exists (use the
   leftmost), emit a `QGridLayout` whose rows are the bands and columns are
   `[label, value]`, with `columnminimumwidth` set from the label column width so
   **the split position is shared by all rows**. Recurse into each grid cell.
2. **Horizontal guillotine cut.** Find a y where every child is fully above or
   fully below (no straddle) and both halves are non-empty; prefer the largest
   gap, tie-break toward balance.
3. **Vertical guillotine cut.** Same over x (single-band case).
4. If a cut was found -> emit `QVBoxLayout` (horizontal cut) / `QHBoxLayout`
   (vertical cut) and recurse into each half.
5. **Grid leaf** — no clean cut exists: cluster rows by y-overlap and columns by
   x-overlap; compute `rowspan`/`colspan` for children that span multiple
   row/column bands; emit a `QGridLayout`.
6. **Base case:** 0 children -> nothing; 1 child -> single `<item>`.

**Gap/alignment handling (resizable mode):** per-layout `spacing` = DLU-derived
average gap in that region (fallback ~6px); container margins small/uniform; a
trailing `<spacer>` (Expanding) in the dialog's top-level layout absorbs leftover
vertical space so short dialogs don't stretch widgets. Widget size policies:
entry/label/button widgets `Preferred` (or `Fixed`), multi-line/list/edit/draw-area
widgets `Expanding` so they grow on resize. Dialog `geometry` = RC size;
`minimumSize` = layout minimum.

**Overlapping siblings** (~1000 pairs exist corpus-wide) cannot be represented in
a box/grid layout; they reflow by construction. Layout mode logs a warning
(`stderr`) when detected. Documented limitation.

## 4. Integration points

- **`src/rc_generator.h`**: add `bool m_use_layouts` + `set_use_layouts(bool)`
  (mirrors existing `-n`/`-f` setters); new private methods:
  `write_dialog_layout(...)`, `emit_layout(container, children, classes, ...)`,
  region-decomposition helpers (`find_horizontal_cut`, `find_vertical_cut`,
  `find_shared_label_cut`, `cluster_grid`), and a small `layout_region` struct.
- **`src/rc_generator.cpp`**:
  - `write_dialog`: branch to a layout path when `m_use_layouts` (reuse
    `setup_dialog_font`, containment/`groupbox_node` hierarchy building,
    `unique_name`).
  - `write_control`: suppress `add_property_rect` in layout mode (geometry comes
    from layout items); still emit all text/style/min-size properties.
  - QTabWidget page path: emit a layout inside each `tabPageN` instead of
    per-child rects.
- **`src/main.cpp`**: add `-l` case -> `set_use_layouts(true)`; update usage text.
- **`generate.sh`**: pass `-l` through (extend the existing `-n/-f/-h`
  passthrough).

## 5. Verification (read-only tooling already available)

1. **uic compile**: `uic file.ui -o /dev/null` over all ~465 generated
   layout-mode `.ui` files (validates layout XML against Qt's own parser;
   `/usr/bin/uic` present).
2. **Qt render harness** (new small tool, built with `BUILD_WITH_QT`):
   `QUiLoader` loads each `.ui`, `show()` offscreen, dumps resulting widget
   geometries; assert no sibling overlaps, all widgets have positive size, dialog
   min-size sane; report corpus stats.
3. **Determinism**: run corpus twice, byte-compare.
4. **Screenshots**: grab a handful of complex dialogs (levelkeypad,
   robot_weapon_dialog, roomkeypad, disdraw) and inspect.
5. **No regression to absolute mode**: existing corpus/golden checks and
   `tools/rc_overlap.cpp` must remain green.
6. **ctest**: add a layout-mode smoke test (gated on `uic` availability).

## 6. Phased milestones

- **P1 Infrastructure** (DONE): `-l` flag, geometry suppression, `emit_layout`
  emitting a single grid leaf per container; uic-validated the whole corpus.
  Verified absolute mode is byte-identical to pre-change output across all 522
  `.rc` files, and layout output is deterministic. Known limitation: a single
  grid leaf per container stacks widgets whose x- or y-intervals are captured by
  a wide/tall spanning sibling (825 of 946 stacked cells are grid-induced, i.e.
  source-non-overlapping).
- **P2 Guillotine**: H/V cuts + grid leaves, no label-table logic; full-corpus
  uic pass.
- **P3 Label-table specialization**: shared vertical cuts across row bands with
  `columnminimumwidth`; spacing/margins/size-policies.
- **P4 Tab pages, spacers/alignment, overlap warnings.**
- **P5 Verification harness, layout-mode corpus, commit.**

## 7. Risks / open items

- Exact look depends on Qt style at runtime; harness uses the default style with
  tolerance.
- Very irregular dialogs won't match pixel-for-pixel (intended for resizable
  mode).
- `-n` (geometry adjustments) is meaningless in layout mode — document that it is
  ignored there.
