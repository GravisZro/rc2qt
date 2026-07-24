# RC Compiler (rc.exe) Quirks and Bugs

This document catalogs known quirks, bugs, and miscompilations in Microsoft's Windows
resource compiler (`rc.exe`). These were discovered primarily through fuzz testing with
`rc.exe` as the oracle, as documented in Ryan Liptak's article
"[Every bug/quirk of the Windows resource compiler (rc.exe), probably](https://www.ryanliptak.com/blog/every-rc-exe-bug-quirk-probably/)"
and the [`resinator`](https://github.com/squeek502/resinator) project.

Understanding these behaviors is critical for implementing a compatible parser, since
real-world `.rc` files may rely on (or accidentally trigger) these edge cases.

---

## Tokenization Quirks

### Special tokenization rules for resource IDs and types

Resource IDs and resource types use **non-standard tokenization rules**. They are
terminated only by whitespace (ASCII characters `0x05` through `0x20` inclusive), **not**
by punctuation, parentheses, braces, or double quotes.

```
1 "FOO" { "bar" }
```

Here `"FOO"` (including the quotes) becomes the resource type name, not `FOO`. The quotes
are part of the token because `"` (ASCII `0x22`) is not in the whitespace range
`0x05`-`0x20`.

Similarly, `//` within an ID/type **is** treated as a comment start (because `/` is
ASCII `0x2F`, within the whitespace range), but `;` within an ID/type is **not** treated
as a comment (because `;` is ASCII `0x3B`, outside the whitespace range). So `1 RC;DATA {}`
compiles as type `RC;DATA`.

Resource IDs do not have to be integers. They can be arbitrary strings: `123ABC`,
`ERROR{OUTOFMEMORY}!?U8`, etc. The ID is uppercased before being written to the `.res` file.

**Parser implication:** Our tokenizer should treat IDs and types as raw tokens delimited
by whitespace, not as standard C identifiers.

---

### Numbers and numeric expressions

#### Non-ASCII digits in number literals

The RC compiler uses `iswdigit` to determine if a character is a digit. This accepts
non-ASCII digit codepoints (e.g. `²`, `₃`, `伍`). The value is computed as
`codepoint - '0'`, which produces arbitrary results for non-ASCII digits:

```
1²3 → 1*100 + (0xB2 - 0x30)*10 + 3 = 1403
```

#### Unary `-` and `~` are part of the number literal

Unary `-` and `~` are **not** separate operator tokens. They are consumed as part of the
number literal itself. A standalone `-` is a valid number literal evaluating to `0`.
A standalone `~` evaluates to `0xFFFF` (or `0xFFFFFFFF` with `L` suffix).

```
1--     → 1 - 0 = 1       (the second - is a literal 0)
1+-1    → literal is -1
```

This also means expressions like `(-(123))` are **invalid** (mismatched parentheses),
because `-(123)` is parsed as one token followed by `)`.

#### Unary `+` is inconsistent

Unary `+` works in some contexts (e.g. `DIALOG` parameters: `+1, +2, +3, +4`) but not
others (e.g. raw data blocks: `{ +123 }` is an error).

#### All operators have equal precedence

Within number expressions, all operators have **equal precedence** and are evaluated
left-to-right. There is no C-style precedence. The preprocessor, however, does use C
precedence.

#### `L` suffix

An `L` suffix makes a number 32-bit instead of 16-bit. It is only meaningful in raw data
blocks. It is **disallowed** in:
- `LANGUAGE` statement parameters
- `FILEVERSION` statement parameters
- `PRODUCTVERSION` statement parameters

Using `L` in these contexts produces a compile error. However, numbers that overflow
`u16` without the `L` suffix are silently truncated.

---

## Resource Definition Quirks

### `BEGIN` or `{` as filename

For resource types that require a file (like `ICON`, `CURSOR`, `BITMAP`), the compiler
may misinterpret `BEGIN` or `{` as a filename:

```
1 ICON BEGIN "foo" END    → error: file not found: BEGIN
1 ICON { "foo" }          → error: file not found: ICON (confused)
```

### Number expressions as filenames

A resource filename can be specified as an arbitrarily complex number expression. The
compiler takes the **last number literal** in the expression and tries to read from a file
with that name:

```
1 FOO (1 | 2)+(2-1 & 0xFF)   → tries to read from file "0xFF"
```

### Incomplete resource at EOF

An incomplete resource definition like `2 FOO` at end-of-file causes the compiler to
treat `FOO` as both the type **and** the filename. If a file named `FOO` exists, it
compiles successfully.

### Dangling literal at EOF

A single dangling literal at end-of-file is silently accepted and treated as if it does
not exist (no corresponding resource in the `.res` file). This is relied upon by several
real-world `.rc` files.

---

## MENU Quirks

### ID type inference in DIALOG `MENU` statement

The `MENU` statement in `DIALOG`/`DIALOGEX` resources uses different ID type inference
rules than normal resource IDs. If the first character is a digit, the **entire token** is
interpreted as a number, regardless of what follows:

```
MENU 1ABC    → interpreted as ordinal 2899, not string "1ABC"
```

The value is computed using the same `codepoint - '0'` algorithm, which can produce `u16`
overflow for characters with ASCII value below `'0'`.

**This is always a miscompilation** — it leads to the wrong menu being loaded.

The same quirk affects the `CLASS` statement.

### Once-a-number, always-a-number

If **any** duplicate `MENU` or `CLASS` statement is interpreted as a number, then the
**last** statement (even if it's a string) is also forced to be interpreted as a number:

```
MENU 123                → ordinal (ignored)
MENU IM_A_STRING        → misinterpreted as ordinal 8360
```

---

## VERSIONINFO Quirks

### Comma between key and first string value affects padding

In `VALUE` statements, the comma between the key and the first value is significant.
When the first value is a quoted string and the comma is omitted, alignment padding
bytes are also omitted, corrupting the binary structure:

```
VALUE "key", "value"    → correct (padding inserted)
VALUE "key" "value"     → miscompilation (no padding)
```

### String NUL termination depends on commas

Adjacent string values without a comma between them are concatenated with only one NUL
terminator at the end:

```
VALUE "foo", "bar"    → NUL after both "foo" and "bar"
VALUE "foo" "bar"     → NUL only after the concatenated "foobar"
```

### Mixed string/number length units

When strings and numbers are mixed in a single `VALUE`, the type is reported as "binary"
(byte count) but the length calculation mixes byte counts (for numbers) and UTF-16 code
unit counts (for strings), producing an incorrect length.

---

## `NOT` Operator Quirks

`NOT` is used to turn off default-enabled flags. It is evaluated left-to-right against
the current accumulated value.

Beyond simple `NOT <number>`, the behavior is **incomprehensible**:

- `NOT (1 | 2)` evaluates to `2` (not `~3` as intuition suggests)
- `NOT () 2` evaluates to `2` (no syntax error)
- `7 NOT NOT 4 NOT 2 NOT NOT 1` evaluates to `2`

`NOT` is accepted in many nonsensical contexts (e.g. `DIALOGEX` x/y/width/height
parameters).

---

## String Quirks

### Quote escaping

The RC parser uses `""` (doubled quotes) for escaping, not `\"`. However, the
preprocessor processes `\"` using C rules, creating a disagreement between preprocessing
and parsing.

### Multiline strings

Strings can span lines without `\` continuation (undocumented). Whitespace in multiline
strings is collapsed to a single space + newline.

### Tab expansion

Tab characters within string literals are expanded to 1-8 spaces depending on the column
position in the file. This happens during preprocessing.

### Escape sequence quirks

- `\a` maps to `0x08` (backspace), not `0x07` (bell) as in C
- `\t` and `\a` are case-insensitive; `\n` and `\r` are case-sensitive
- Invalid escape sequences in `L"..."` strings cause the escaped character to disappear
  entirely

---

## File Encoding Quirks

### The preprocessor always outputs UTF-16

Regardless of input encoding, the preprocessor always outputs UTF-16. The parser/compiler
always ingests UTF-16.

### `#pragma code_page` desync

When `#pragma code_page` is the first thing in the file, it only sets the **input** code
page, not the output code page, causing an input/output code page desync.

### Special codepoints

- `U+FFFE` / `U+FFFF`: Causes all non-ASCII codepoints to be byte-swapped
- `U+0000` (NUL): Error in odd-length files, succeeds silently in even-length files
- `U+0004` (EOT): Acts as "skip next character" outside strings
- `U+001A` (SUB): Treated as end-of-file marker
- `U+0A0D`, `U+0D00`, `U+2000`: Confused for whitespace due to endianness heuristic

---

## Known Miscompilations

| Behavior | Description |
|----------|-------------|
| MENU id miscompilation | `MENU 1ABC` in DIALOG is always miscompiled |
| Once-a-number-always-a-number | Duplicate MENU/CLASS with a numeric one forces the last (even string) to be interpreted as a number |
| VERSIONINFO missing padding | Omitting comma between key and first quoted string value omits alignment padding |
| VERSIONINFO mixed length units | Mixing strings and numbers in one VALUE produces wrong byte count |
| CONTROL padding miscompilation | Odd-length control data causes 2 extra padding bytes |
| FONT resource FONTDIRENTRY | Offsets to `dfDevice`/`dfFace` were never updated for v3 format |
| `\a` escape | Maps to `0x08` (backspace) instead of `0x07` (bell) |

---

## Reference

- Ryan Liptak, "[Every bug/quirk of the Windows resource compiler (rc.exe), probably](https://www.ryanliptak.com/blog/every-rc-exe-bug-quirk-probably/)" (2024-10-11)
- [`resinator`](https://github.com/squeek502/resinator) — clean-room RC compiler with bug-for-bug compatibility
- [win32-samples-rc-tests](https://github.com/squeek502/win32-samples-rc-tests/) — compatibility test corpus
