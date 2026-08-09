# Plan: Convert absolute RC coordinates into Qt layout managers

Status: in progress. M1 (edge-boundary grid solver, one QGridLayout per
container, layout feature gated under HAVE_QT) is implemented and verified
(zero grid-induced stacking, uic 0 failures, byte-determinism, absolute-mode
byte-identical); M3 (offscreen verifier) next.

## 1. Objective & scope

Add an opt-in layout mode (`-l/--layout`) that emits Qt Designer layouts
(`QGridLayout`, later `QHBoxLayout`/`QVBoxLayout` and pattern layouts) instead
of absolute geometry, inferred from the RC's dialog-unit coordinates. Target:
**resizable, approximately-faithful** UI — widgets keep RC-derived minimum sizes
so the dialog opens near its original size but reflows cleanly. The current
absolute-geometry mode stays the default and its golden output must not change.

The **entire layout feature is gated under `HAVE_QT`** (`BUILD_WITH_QT=ON`):
- non-Qt builds have no `-l` flag, no layout code, no verifier;
- the FreeType (default) build is untouched and its absolute output stays
  byte-identical;
- layout-mode `.ui` output and the render verifier are Qt-build features only.

Scope: dialog body, nested groupboxes, and tab pages (`DS_CONTROL`
subdialogs). Not in scope: `QFormLayout` detection, `QStackedLayout`, exact
pixel fidelity.

## 2. File layout

The layout engine and the render verifier live in their own files; nothing is
crammed into `rc_generator.*`.

- **`src/rc_layout.h` / `src/rc_layout.cpp`** — layout-inference engine. Pure
  geometry: **no Qt, no fonts, no pugi XML**. Compiled under `HAVE_QT`.
  Input: per-container children (control index, qt class, pixel rect). Output:
  a `container_plan` (grid cells / pattern) plus the target rects. M1 = edge
  solver; M2 = patterns.
- **`src/rc_render.h` / `src/rc_render.cpp`** — Qt-only (M3). Builds the widget
  tree directly with the Qt Widget API (no `.ui` file is written or loaded, no
  `QUiLoader`, no QtUiTools), renders offscreen, and reports per-widget
  geometry deviation + overlap violations against the RC-derived target rects.
- **`src/rc_generator.*`** — keeps font/metrics, rect computation
  (`control_layout_pixel_size`/`fit_text`), the container tree, and the XML
  (`.ui`) emission driven by `rc_layout` plans. The guillotine recursion
  (`emit_layout_region`/`emit_layout_item`) is deleted.
- **`src/main.cpp`** — `-l` gated under `HAVE_QT`; `--verify-layout` mode
  (M3) invoking `rc_render`.
- **`CMakeLists.txt`** — `BUILD_WITH_QT` links `Qt5::Widgets` only; conditionally
  compiles `src/rc_layout.cpp` and `src/rc_render.cpp`.

## 3. Layout inference (M1: edge-boundary grid)

Each container (dialog, each groupbox, each tab page) is resolved into a
**single `QGridLayout`**:

1. Column boundaries = every distinct child x-start and x-end; row boundaries =
   every distinct child y-start and y-end (all in the pixel rects the generator
   computes, identical to absolute-mode geometry).
2. A child occupies exactly the row/column interval spanned by its rect, so two
   source-non-overlapping children can never share a grid cell — **zero induced
   stacking by construction**.
3. Emit one `QGridLayout` per container with `spacing = 6`, `margin = 0`;
   items in row/column order (tie-break by y then x). Nested groupboxes recurse
   as their own container grid. Layout count ≈ dialogs + groupboxes + tab
   pages (no box-layout recursion), nesting depth ≤ 2.

Widgets keep the layout-mode properties already emitted by `write_control`:
`minimumSize` (RC-derived), `sizePolicy` (`Expanding/Expanding` for
list/edit/view/draw-area classes, else `Preferred/Preferred`).

## 4. Patterns (M2)

Per-container matchers applied before the edge-grid fallback, best score wins,
≤ 1 top-level band split per dialog, depth ≤ 3:

- **ButtonRow** — a row of button-class children sharing a y band and roughly
  equal heights → one `QHBoxLayout`.
- **LabelTable** — vertical bands of `label + value` pairs sharing a common x
  split → one `QGridLayout` with a shared label column (`columnminimumwidth`).
- **StackRows** — non-overlapping horizontal bands each one widget tall →
  one `QVBoxLayout`.
- **KeypadGrid** — many equal-sized, closely-spaced children in a dense grid →
  one `QGridLayout` of equal cells.
- **Edge-grid fallback** — M1 solver for unmatched children.

Each pattern is kept only if it improves the M4 aggregate closeness score vs
the edge-grid baseline.

## 5. Offscreen verification (M3)

Built into `rc2qt` under `HAVE_QT` as `--verify-layout [--render-dir DIR]`.
No `.ui` files are written; the widget tree is built from the `container_plan`
via the Qt Widget API and rendered offscreen (`QT_QPA_PLATFORM=offscreen`,
fixed style):

- build layout-mode widget tree, resize dialog to the RC size, show,
  `processEvents`;
- read each widget's geometry (relative to the dialog) and compare against the
  **RC-derived target rects** carried in the `container_plan`;
- report per-widget deviation, overlap violations between
  source-non-overlapping pairs, and an aggregate closeness score;
- `--render-dir` dumps PNGs to `temporary_files/` for visual inspection.

This is a dev/tuning instrument (Qt-style/font-dependent, so not a ctest
regression oracle); deterministic gates stay script checks.

## 6. Milestones

- **M1 — correctness floor**: `rc_layout` edge solver; one grid per container;
  explicit `margin=0`; delete guillotine code; `-l` gated under `HAVE_QT`;
  verify stacked cells → 0 induced, layout count/depth targets, uic 0
  failures, byte-determinism, absolute mode unchanged.
- **M3 — verifier**: `rc_render` + `--verify-layout`; establish corpus score
  baseline.
- **M2 — patterns**: the four matchers above + edge fallback; keep only
  score-improving ones.
- **M4 — tuning**: centralized per-pattern constants (margins, spacing,
  sizePolicy, stretch, alignment, `columnminimumwidth`) driven by the score.
- **M5 — gates & commit**: uic 0 failures, byte-determinism, absolute-mode
  byte-identical, stacked-cells → 0 induced, layout count/depth targets;
  update `generate.sh`; commit per milestone.

## 7. Verification tooling

1. **uic compile**: `uic file.ui -o /dev/null` over all layout-mode `.ui`
   (Qt build corpus).
2. **`--verify-layout`**: offscreen widget-geometry score (M3+).
3. **Determinism**: run corpus twice, byte-compare.
4. **Absolute-mode no-regression**: FreeType build output byte-identical to
   committed snapshots.
5. **Stacked-cell metric**: source-non-overlapping pairs sharing a grid cell.

## 8. Risks / open items

- Exact look depends on Qt style at runtime; verifier uses a fixed style with
  tolerance.
- Very irregular dialogs won't match pixel-for-pixel (intended for resizable
  mode).
- `-n` (geometry adjustments) is ignored in layout mode.

## History

- **P1 (commit 8331b1c)**: `-l` flag, geometry suppression, single grid leaf
  per container; verified corpus + determinism + absolute no-regression.
- **P2 (commit 95cf349)**: recursive guillotine decomposition; rejected as too
  layout-heavy (4122 layouts corpus-wide, depth 13, compound default margins,
  redundant single-child/same-orientation chains).
- **Superseded plan (commit 400b17b)**: replaced by the pattern + edge-grid +
  offscreen-verify strategy in this document.
