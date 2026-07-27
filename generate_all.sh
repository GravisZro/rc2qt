#!/bin/bash
set -e

RC2QT="./build/rc2qt"
OUTDIR="temporary_files"
TESTDIR="test"

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
  qrc_out="$OUTDIR/$(basename "$rc_file" .rc).qrc"

  output=$("$RC2QT" "$rc_file" -o "$OUTDIR/" -q "$qrc_out" 2>&1) && rc=0 || rc=$?
  if [ $rc -eq 0 ]; then
    success=$((success + 1))
  else
    echo "FAIL: $rc_file (exit $rc)"
    echo "$output" | sed 's/^/  /'
    fail=$((fail + 1))
  fi
done

echo "Done: $success/$total succeeded, $fail failed. Output in $OUTDIR/"
