#!/bin/sh
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

usage()
{
  echo "Usage: $0 [options] [all|name...]" >&2
  echo "  options   -n  Disable all geometric adjustments" >&2
  echo "            -f  Prevent font substitutions (keep original font names)" >&2
  echo "  all       Generate all .rc files in $TESTDIR/ (default)" >&2
  echo "  name...   Basenames of .rc files in $TESTDIR/ (e.g. editor -> $TESTDIR/editor.rc)" >&2
}

run_rc2qt()
{
  rc_file=$1
  total=$((total + 1))
  rc_basename=$(basename "$rc_file" .rc)
  qrc_out="$OUTDIR/$rc_basename.qrc"

  # shellcheck disable=SC2086  # $FLAGS holds only -n/-f tokens
  output=$("$RC2QT" "$rc_file" -o "$OUTDIR/" -r "$rc_basename" -q "$qrc_out" $FLAGS 2>&1) && rc=0 || rc=$?
  if [ "$rc" -eq 0 ]; then
    success=$((success + 1))
  else
    echo "FAIL: $rc_file (exit $rc)"
    echo "$output" | sed 's/^/  /'
    fail=$((fail + 1))
  fi
}

FLAGS=""
saw_all=0
saw_name=0
for arg in "$@"; do
  case "$arg" in
    -n|-f)
      FLAGS="$FLAGS $arg"
      ;;
    all)
      saw_all=1
      ;;
    -*)
      echo "Warning: ignoring unknown option: $arg" >&2
      ;;
    *)
      saw_name=1
      ;;
  esac
done

total=0
success=0
fail=0

if [ $saw_all -eq 1 ] || [ $saw_name -eq 0 ]; then
  for rc_file in "$TESTDIR"/*.rc; do
    [ -f "$rc_file" ] || continue
    run_rc2qt "$rc_file"
  done
else
  for arg in "$@"; do
    case "$arg" in
      -n|-f)
        continue
        ;;
    esac
    rc_file="$TESTDIR/$arg.rc"
    if [ -f "$rc_file" ]; then
      run_rc2qt "$rc_file"
    else
      echo "Warning: skipping unknown name: $arg ($rc_file not found)" >&2
    fi
  done
  if [ $total -eq 0 ]; then
    usage
    exit 1
  fi
fi

echo "Done: $success/$total succeeded, $fail failed. Output in $OUTDIR/"
