#!/bin/bash
set -e

RC2QT="./build/rc2qt"
OUTDIR="temporary_files"
TESTDIR="test"

if [ ! -x "$RC2QT" ]; then
  echo "Error: $RC2QT not found. Run 'cmake --build build' first." >&2
  exit 1
fi

mkdir -p "$OUTDIR/ui" "$OUTDIR/qrc"

total=0
success=0
fail=0

for rc_file in "$TESTDIR"/*.rc; do
  [ -f "$rc_file" ] || continue
  total=$((total + 1))
  name=$(basename "$rc_file" .rc)
  ui_out="$OUTDIR/ui/${name}.ui"
  qrc_out="$OUTDIR/qrc/${name}.qrc"

  if "$RC2QT" "$rc_file" -o "$ui_out" -q "$qrc_out" >/dev/null 2>&1; then
    success=$((success + 1))
  else
    echo "FAIL: $rc_file"
    fail=$((fail + 1))
  fi
done

echo "Done: $success/$total succeeded, $fail failed. Output in $OUTDIR/"
