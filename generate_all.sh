#!/bin/bash
set -e

RC2QT="./build/rc2qt"
OUTDIR="temporary_files"
TESTDIR="test"
ALL_DIALOGS=false

if [ "$1" = "-a" ]; then
  ALL_DIALOGS=true
fi

if [ ! -x "$RC2QT" ]; then
  echo "Error: $RC2QT not found. Run 'cmake --build build' first." >&2
  exit 1
fi

mkdir -p "$OUTDIR"

total=0
success=0
fail=0

for rc_file in "$TESTDIR"/*.rc; do
  [ -f "$rc_file" ] || continue
  total=$((total + 1))
  name=$(basename "$rc_file" .rc)
  ui_out="$OUTDIR/${name}.ui"
  qrc_out="$OUTDIR/${name}.qrc"

  if $ALL_DIALOGS; then
    if "$RC2QT" "$rc_file" -a -o "$ui_out" -q "$qrc_out" >/dev/null 2>&1; then
      success=$((success + 1))
    else
      echo "FAIL: $rc_file"
      fail=$((fail + 1))
    fi
  else
    if "$RC2QT" "$rc_file" -o "$ui_out" -q "$qrc_out" >/dev/null 2>&1; then
      success=$((success + 1))
    else
      echo "FAIL: $rc_file"
      fail=$((fail + 1))
    fi
  fi
done

echo "Done: $success/$total succeeded, $fail failed. Output in $OUTDIR/"
