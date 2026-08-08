#!/bin/bash
set -e
set -x
RC2QT="./build/rc2qt"
OUTDIR="temporary_files"
TESTDIR="test"

if [ ! -x "$RC2QT" ]; then
  echo "Error: $RC2QT not found. Run 'cmake --build build' first." >&2
  exit 1
fi

mkdir -p "$OUTDIR"

if [ $# -eq 0 ] || [ "$1" = "all" ]; then
  files=("$TESTDIR"/*.rc)
else
  files=()
  for name in "$@"; do
    rc_file="$TESTDIR/$name.rc"
    if [ -f "$rc_file" ]; then
      files+=("$rc_file")
    else
      echo "Warning: skipping unknown name: $name ($rc_file not found)" >&2
    fi
  done
  if [ ${#files[@]} -eq 0 ]; then
    echo "Usage: $0 [all|name...]" >&2
    echo "  all       Generate all .rc files in $TESTDIR/ (default)" >&2
    echo "  name...   Basenames of .rc files in $TESTDIR/ (e.g. editor -> $TESTDIR/editor.rc)" >&2
    exit 1
  fi
fi

total=0
success=0
fail=0

for rc_file in "${files[@]}"; do
  [ -f "$rc_file" ] || continue
  total=$((total + 1))
  rc_basename=$(basename "$rc_file" .rc)
  qrc_out="$OUTDIR/$rc_basename.qrc"

  output=$("$RC2QT" "$rc_file" -o "$OUTDIR/" -r "$rc_basename" -q "$qrc_out" 2>&1) && rc=0 || rc=$?
  if [ $rc -eq 0 ]; then
    success=$((success + 1))
  else
    echo "FAIL: $rc_file (exit $rc)"
    echo "$output" | sed 's/^/  /'
    fail=$((fail + 1))
  fi
done

echo "Done: $success/$total succeeded, $fail failed. Output in $OUTDIR/"
