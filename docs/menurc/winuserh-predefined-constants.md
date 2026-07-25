# Predefined Constants (WinUser.h / CommCtrl.h)

Numeric constants defined in the Windows SDK headers `WinUser.h` and `CommCtrl.h` can be used interchangeably with their integer values anywhere in a resource script (.rc) file. For example, `VK_RETURN` and `13` are equivalent in an accelerator table, and `WS_CHILD` and `0x40000000` are equivalent in a STYLE statement.

The resource compiler resolves these symbols at compile time via `#include <windows.h>` (which includes both headers).

## Naming Conventions

Constants follow a prefix-based naming pattern organized by subsystem:

| Prefix | Subsystem | Purpose |
|--------|-----------|---------|
| CB\_ | Combo Box | Control messages |
| LB\_ | List Box | Control messages |
| EM\_ | Edit Control | Control messages |
| BM\_ | Button | Control messages |
| STM\_ | Static Control | Control messages |
| SBM\_ | Scroll Bar | Control messages |
| DM\_ | Dialog | Default button messages |
| CBN\_ | Combo Box | Notifications |
| LBN\_ | List Box | Notifications |
| EN\_ | Edit Control | Notifications |
| BN\_ | Button | Notifications |
| STN\_ | Static Control | Notifications |
| CBS\_ | Combo Box | Window styles |
| LBS\_ | List Box | Window styles |
| ES\_ | Edit Control | Window styles |
| BS\_ | Button | Window styles |
| SS\_ | Static Control | Window styles |
| SBS\_ | Scroll Bar | Window styles |
| DS\_ | Dialog | Window styles |
| WS\_ | Window | General window styles |
| WS\_EX\_ | Window | Extended window styles |
| WM\_ | Window | Messages |
| VK\_ | Keyboard | Virtual key codes |
| MB\_ | Message Box | Flags |
| ID | Dialog | Button identifiers |

## Control Messages

> **Header:** `WinUser.h`

These messages are sent to a control via `SendMessage` or `PostMessage`.

### CB\_ — Combo Box Messages

| Constant | Value | Description |
|----------|-------|-------------|
| CB\_ADDSTRING | 0x143 | Add a string to the list |
| CB\_DELETESTRING | 0x144 | Delete a string |
| CB\_DIR | 0x145 | Add directory listing |
| CB\_FINDSTRING | 0x14C | Find string prefix |
| CB\_FINDSTRINGEXACT | 0x158 | Find exact string |
| CB\_GETCOMBOBOXINFO | 0x164 | Get combo box info |
| CB\_GETCOUNT | 0x146 | Get item count |
| CB\_GETCURSEL | 0x147 | Get current selection |
| CB\_GETDROPPEDCONTROLRECT | 0x152 | Get drop-down rect |
| CB\_GETDROPPEDSTATE | 0x157 | Get drop-down state |
| CB\_GETDROPPEDWIDTH | 0x15F | Get drop-down width |
| CB\_GETEDITSEL | 0x140 | Get edit selection |
| CB\_GETEXTENDEDUI | 0x156 | Get extended UI flag |
| CB\_GETHORIZONTALEXTENT | 0x15D | Get horizontal extent |
| CB\_GETITEMDATA | 0x150 | Get item data |
| CB\_GETITEMHEIGHT | 0x154 | Get item height |
| CB\_GETLBTEXT | 0x148 | Get string |
| CB\_GETLBTEXTLEN | 0x149 | Get string length |
| CB\_GETLOCALE | 0x15A | Get locale |
| CB\_GETTOPINDEX | 0x15B | Get top visible index |
| CB\_INITSTORAGE | 0x161 | Initialize storage |
| CB\_INSERTSTRING | 0x14A | Insert a string |
| CB\_LIMITTEXT | 0x141 | Limit text length |
| CB\_RESETCONTENT | 0x14B | Remove all items |
| CB\_SELECTSTRING | 0x14D | Select string prefix |
| CB\_SETCURSEL | 0x14E | Set current selection |
| CB\_SETDROPPEDWIDTH | 0x160 | Set drop-down width |
| CB\_SETEDITSEL | 0x142 | Set edit selection |
| CB\_SETEXTENDEDUI | 0x155 | Set extended UI flag |
| CB\_SETHORIZONTALEXTENT | 0x15E | Set horizontal extent |
| CB\_SETITEMDATA | 0x151 | Set item data |
| CB\_SETITEMHEIGHT | 0x153 | Set item height |
| CB\_SETLOCALE | 0x159 | Set locale |
| CB\_SETTOPINDEX | 0x15C | Set top visible index |
| CB\_SHOWDROPDOWN | 0x14F | Show/hide drop-down |
| CB\_ERR | (-1) | Error return |
| CB\_ERRSPACE | (-2) | Memory error return |

### LB\_ — List Box Messages

| Constant | Value | Description |
|----------|-------|-------------|
| LB\_ADDFILE | 0x196 | Add file to directory list |
| LB\_ADDSTRING | 0x180 | Add a string |
| LB\_DELETESTRING | 0x182 | Delete a string |
| LB\_DIR | 0x18D | Add directory listing |
| LB\_FINDSTRING | 0x18F | Find string prefix |
| LB\_FINDSTRINGEXACT | 0x1A2 | Find exact string |
| LB\_GETANCHORINDEX | 0x19D | Get anchor index |
| LB\_GETCARETINDEX | 0x19F | Get caret index |
| LB\_GETCOUNT | 0x18B | Get item count |
| LB\_GETCURSEL | 0x188 | Get current selection |
| LB\_GETHORIZONTALEXTENT | 0x193 | Get horizontal extent |
| LB\_GETITEMDATA | 0x199 | Get item data |
| LB\_GETITEMHEIGHT | 0x1A1 | Get item height |
| LB\_GETITEMRECT | 0x198 | Get item rectangle |
| LB\_GETLISTBOXINFO | 0x1B2 | Get list box info |
| LB\_GETLOCALE | 0x1A6 | Get locale |
| LB\_GETSEL | 0x187 | Get selection state |
| LB\_GETSELCOUNT | 0x190 | Get selected count |
| LB\_GETSELITEMS | 0x191 | Get selected items |
| LB\_GETTEXT | 0x189 | Get string |
| LB\_GETTEXTLEN | 0x18A | Get string length |
| LB\_GETTOPINDEX | 0x18E | Get top visible index |
| LB\_INITSTORAGE | 0x1A8 | Initialize storage |
| LB\_INSERTSTRING | 0x181 | Insert a string |
| LB\_ITEMFROMPOINT | 0x1A9 | Get item from point |
| LB\_RESETCONTENT | 0x184 | Remove all items |
| LB\_SELECTSTRING | 0x18C | Select string prefix |
| LB\_SELITEMRANGE | 0x19B | Select item range |
| LB\_SELITEMRANGEEX | 0x183 | Select item range (extended) |
| LB\_SETANCHORINDEX | 0x19C | Set anchor index |
| LB\_SETCARETINDEX | 0x19E | Set caret index |
| LB\_SETCOLUMNWIDTH | 0x195 | Set column width |
| LB\_SETCOUNT | 0x1A7 | Set item count |
| LB\_SETCURSEL | 0x186 | Set current selection |
| LB\_SETHORIZONTALEXTENT | 0x194 | Set horizontal extent |
| LB\_SETITEMDATA | 0x19A | Set item data |
| LB\_SETITEMHEIGHT | 0x1A0 | Set item height |
| LB\_SETLOCALE | 0x1A5 | Set locale |
| LB\_SETSEL | 0x185 | Set selection |
| LB\_SETTABSTOPS | 0x192 | Set tab stops |
| LB\_SETTOPINDEX | 0x197 | Set top visible index |
| LB\_ERR | (-1) | Error return |
| LB\_ERRSPACE | (-2) | Memory error return |

### EM\_ — Edit Control Messages

| Constant | Value | Description |
|----------|-------|-------------|
| EM\_CANUNDO | 0x0C6 | Check if undo is possible |
| EM\_CHARFROMPOS | 0x0D7 | Get character from position |
| EM\_EMPTYUNDOBUFFER | 0xCD | Clear undo buffer |
| EM\_FMTLINES | 0x0C8 | Set line-break mode |
| EM\_GETIMESTATUS | 0x0D9 | Get IME status |
| EM\_GETFIRSTVISIBLELINE | 0xCE | Get first visible line |
| EM\_GETHANDLE | 0x0BD | Get text buffer handle |
| EM\_GETLIMITTEXT | 0x0D5 | Get text limit |
| EM\_GETLINE | 0x0C4 | Get a line of text |
| EM\_GETLINECOUNT | 0x0BA | Get line count |
| EM\_GETMARGINS | 0x0D4 | Get margins |
| EM\_GETMODIFY | 0x0B8 | Get modification flag |
| EM\_GETPASSWORDCHAR | 0x0D2 | Get password character |
| EM\_GETRECT | 0x0B2 | Get formatting rectangle |
| EM\_GETSEL | 0x0B0 | Get selection |
| EM\_GETTHUMB | 0x0BE | Get scroll position |
| EM\_GETWORDBREAKPROC | 0x0D1 | Get word-break procedure |
| EM\_LIMITTEXT | 0x0C5 | Set text limit |
| EM\_LINEFROMCHAR | 0x0C9 | Get line from char index |
| EM\_LINEINDEX | 0x0BB | Get line start index |
| EM\_LINELENGTH | 0x0C1 | Get line length |
| EM\_LINESCROLL | 0x0B6 | Scroll lines |
| EM\_POSFROMCHAR | 0x0D6 | Get position from char |
| EM\_REPLACESEL | 0x0C2 | Replace selection |
| EM\_SCROLL | 0x0B5 | Scroll edit control |
| EM\_SCROLLCARET | 0x0D0 | Scroll caret into view |
| EM\_SETHANDLE | 0x0BC | Set text buffer handle |
| EM\_SETIMESTATUS | 0x0D8 | Set IME status |
| EM\_SETLIMITTEXT | 0x0C5 | Set text limit |
| EM\_SETMARGINS | 0x0D3 | Set margins |
| EM\_SETMODIFY | 0x0B9 | Set modification flag |
| EM\_SETPASSWORDCHAR | 0x0CC | Set password character |
| EM\_SETREADONLY | 0x0CF | Set read-only mode |
| EM\_SETRECT | 0x0B3 | Set formatting rectangle |
| EM\_SETRECTNP | 0x0B4 | Set formatting rectangle (no paint) |
| EM\_SETSEL | 0x0B1 | Set selection |
| EM\_SETTABSTOPS | 0x0CB | Set tab stops |
| EM\_SETWORDBREAKPROC | 0x0D0 | Set word-break procedure |
| EM\_UNDO | 0x0C7 | Undo last edit |

### BM\_ — Button Messages

| Constant | Value | Description |
|----------|-------|-------------|
| BM\_CLICK | 0x0F5 | Simulate click |
| BM\_GETCHECK | 0x0F0 | Get check state |
| BM\_GETIMAGE | 0x0F6 | Get image handle |
| BM\_GETSTATE | 0x0F2 | Get button state |
| BM\_SETCHECK | 0x0F1 | Set check state |
| BM\_SETIMAGE | 0x0F7 | Set image handle |
| BM\_SETSTATE | 0x0F3 | Set button state |
| BM\_SETSTYLE | 0x0F4 | Set button style |

### STM\_ — Static Control Messages

| Constant | Value | Description |
|----------|-------|-------------|
| STM\_GETICON | 0x171 | Get icon handle |
| STM\_GETIMAGE | 0x173 | Get image handle |
| STM\_SETICON | 0x170 | Set icon handle |
| STM\_SETIMAGE | 0x172 | Set image handle |

### SBM\_ — Scroll Bar Messages

| Constant | Value | Description |
|----------|-------|-------------|
| SBM\_ENABLE\_ARROWS | 0x0E4 | Enable/disable arrows |
| SBM\_GETPOS | 0x0E1 | Get current position |
| SBM\_GETRANGE | 0x0E3 | Get scroll range |
| SBM\_SETPOS | 0x0E0 | Set current position |
| SBM\_SETRANGE | 0x0E2 | Set scroll range |
| SBM\_SETRANGEREDRAW | 0x0E6 | Set range and repaint |
| SBM\_GETSCROLLINFO | 0x0EA | Get scroll info |
| SBM\_SETSCROLLINFO | 0x0E9 | Set scroll info |
| SBM\_GETSCROLLBARINFO | 0x0EB | Get scroll bar info |

### DM\_ — Dialog Default Button Messages

| Constant | Value | Description |
|----------|-------|-------------|
| DM\_GETDEFID | 0x0400 | Get default push button ID |
| DM\_SETDEFID | 0x0401 | Set default push button ID |
| DM\_REPOSITION | 0x0402 | Reposition dialog |

## Notifications

> **Header:** `CommCtrl.h`

These are sent to a parent window via `WM_COMMAND` (low word of wParam = control ID, high word = notification code) or `WM_NOTIFY`.

### CBN\_ — Combo Box Notifications

| Constant | Value | Description |
|----------|-------|-------------|
| CBN\_CLOSEUP | 8 | Drop-down closed |
| CBN\_DBLCLK | 2 | Double-click on item |
| CBN\_DROPDOWN | 7 | Drop-down list about to open |
| CBN\_EDITCHANGE | 5 | Edit control text changed |
| CBN\_EDITUPDATE | 6 | Edit control about to display text |
| CBN\_ERRSPACE | (-1) | Insufficient memory |
| CBN\_KILLFOCUS | 4 | Input focus lost |
| CBN\_SELCHANGE | 1 | Selection changed |
| CBN\_SELENDCANCEL | 10 | Selection cancelled |
| CBN\_SELENDOK | 9 | Selection confirmed |
| CBN\_SETFOCUS | 3 | Input focus received |

### LBN\_ — List Box Notifications

| Constant | Value | Description |
|----------|-------|-------------|
| LBN\_DBLCLK | 2 | Double-click on item |
| LBN\_ERRSPACE | (-2) | Insufficient memory |
| LBN\_KILLFOCUS | 5 | Input focus lost |
| LBN\_SELCANCEL | 3 | Selection cancelled |
| LBN\_SELCHANGE | 1 | Selection changed |
| LBN\_SETFOCUS | 4 | Input focus received |

### EN\_ — Edit Control Notifications

| Constant | Value | Description |
|----------|-------|-------------|
| EN\_CHANGE | 0x0300 | Text content changed |
| EN\_ERRSPACE | 0x0500 | Insufficient memory |
| EN\_HSCROLL | 0x0601 | Horizontal scroll |
| EN\_KILLFOCUS | 0x0200 | Input focus lost |
| EN\_MAXTEXT | 0x0501 | Text limit reached |
| EN\_SETFOCUS | 0x0100 | Input focus received |
| EN\_UPDATE | 0x0400 | Text about to change |
| EN\_VSCROLL | 0x0602 | Vertical scroll |

### BN\_ — Button Notifications

| Constant | Value | Description |
|----------|-------|-------------|
| BN\_CLICKED | 0 | Button clicked |
| BN\_DBLCLK | 5 | Double-clicked |
| BN\_DISABLE | 4 | Enabled state changed |
| BN\_DOUBLECLICKED | 5 | Double-clicked (alias) |
| BN\_HILITE | 2 | Highlighted |
| BN\_KILLFOCUS | 7 | Input focus lost |
| BN\_PAINT | 1 | Button should paint |
| BN\_PUSHED | 2 | Highlighted (alias) |
| BN\_SETFOCUS | 6 | Input focus received |
| BN\_UNHILITE | 3 | Unhighlighted |
| BN\_UNPUSHED | 3 | Unhighlighted (alias) |

### STN\_ — Static Control Notifications

| Constant | Value | Description |
|----------|-------|-------------|
| STN\_CLICKED | 0 | Control clicked |
| STN\_DBLCLK | 1 | Control double-clicked |
| STN\_DISABLE | 3 | Control disabled |
| STN\_ENABLE | 2 | Control enabled |

## Window Styles

> **Header:** `WinUser.h` (some control-specific styles also defined in `CommCtrl.h`)

These constants are combined with the bitwise OR operator (`|`) in STYLE statements and control definitions to set the appearance and behavior of windows and controls.

### WS\_ — General Window Styles

| Constant | Value | Description |
|----------|-------|-------------|
| WS\_BORDER | 0x00800000 | Window with thin-line border |
| WS\_CAPTION | 0x00C00000 | Window with title bar |
| WS\_CHILD | 0x40000000 | Child window |
| WS\_CLIPCHILDREN | 0x02000000 | Exclude child areas when painting |
| WS\_CLIPSIBLINGS | 0x04000000 | Clip child windows |
| WS\_DISABLED | 0x08000000 | Initial state: disabled |
| WS\_DLGFRAME | 0x00400000 | Window with dialog border |
| WS\_GROUP | 0x00020000 | First control of a group |
| WS\_HSCROLL | 0x00100000 | Window with horizontal scroll bar |
| WS\_MAXIMIZE | 0x01000000 | Initial state: maximized |
| WS\_MAXIMIZEBOX | 0x00010000 | Window with maximize button |
| WS\_MINIMIZE | 0x20000000 | Initial state: minimized |
| WS\_MINIMIZEBOX | 0x00020000 | Window with minimize button |
| WS\_OVERLAPPED | 0x00000000 | Overlapped window |
| WS\_POPUP | 0x80000000 | Popup window |
| WS\_SIZEBOX | 0x00040000 | Window with sizing border |
| WS\_SYSMENU | 0x00080000 | Window with System menu |
| WS\_TABSTOP | 0x00010000 | Control receives TAB focus |
| WS\_VISIBLE | 0x10000000 | Initial state: visible |
| WS\_VSCROLL | 0x00200000 | Window with vertical scroll bar |

Composite styles (for reference):

| Constant | Value | Equivalent |
|----------|-------|------------|
| WS\_OVERLAPPEDWINDOW | 0x00CF0000 | WS\_OVERLAPPED \| WS\_CAPTION \| WS\_SYSMENU \| WS\_THICKFRAME \| WS\_MINIMIZEBOX \| WS\_MAXIMIZEBOX |
| WS\_POPUPWINDOW | 0x80880000 | WS\_POPUP \| WS\_BORDER \| WS\_SYSMENU |

### WS\_EX\_ — Extended Window Styles

| Constant | Value | Description |
|----------|-------|-------------|
| WS\_EX\_ACCEPTFILES | 0x00000010 | Accept drag-and-drop files |
| WS\_EX\_APPWINDOW | 0x00040000 | Top-level window on taskbar |
| WS\_EX\_CLIENTEDGE | 0x00000200 | Sunken border |
| WS\_EX\_COMPOSITED | 0x02000000 | Double-buffered painting |
| WS\_EX\_CONTEXTHELP | 0x00000400 | Context help border |
| WS\_EX\_CONTROLPARENT | 0x00010000 | Tab-stop navigation |
| WS\_EX\_DLGMODALFRAME | 0x00000001 | Double border style |
| WS\_EX\_LAYERED | 0x00080000 | Layered window |
| WS\_EX\_LAYOUTRTL | 0x00400000 | Right-to-left layout |
| WS\_EX\_LEFT | 0x00000000 | Left-aligned text (default) |
| WS\_EX\_LEFTSCROLLBAR | 0x00004000 | Left scroll bar |
| WS\_EX\_LTRREADING | 0x00000000 | Left-to-right reading (default) |
| WS\_EX\_MDICHILD | 0x00000040 | MDI child window |
| WS\_EX\_NOACTIVATE | 0x08000000 | No activation on click |
| WS\_EX\_NOINHERITLAYOUT | 0x00100000 | Don't inherit layout |
| WS\_EX\_NOPARENTNOTIFY | 0x00000004 | No WM\_PARENTNOTIFY |
| WS\_EX\_PALETTEWINDOW | 0x00000188 | Palette window |
| WS\_EX\_RIGHT | 0x00001000 | Right-aligned text |
| WS\_EX\_RIGHTSCROLLBAR | 0x00000000 | Right scroll bar (default) |
| WS\_EX\_RTLREADING | 0x00002000 | Right-to-left reading |
| WS\_EX\_STATICEDGE | 0x00020000 | 3D border style |
| WS\_EX\_TOOLWINDOW | 0x00000080 | Tool window |
| WS\_EX\_TOPMOST | 0x00000008 | Always on top |
| WS\_EX\_TRANSPARENT | 0x00000020 | Transparent to input |
| WS\_EX\_WINDOWEDGE | 0x00000100 | Raised border |

### DS\_ — Dialog Styles

| Constant | Value | Description |
|----------|-------|-------------|
| DS\_3DLOOK | 0x0004 | Thicker frame for 3D appearance |
| DS\_ABSALIGN | 0x0001 | Dialog coordinates are screen-relative |
| DS\_CENTER | 0x0800 | Center dialog on screen |
| DS\_CENTERMOUSE | 0x1000 | Center dialog on mouse cursor |
| DS\_CONTEXTHELP | 0x2000 | Context help button |
| DS\_CONTROL | 0x0400 | Dialog is a child of another dialog |
| DS\_FIXEDSYS | 0x0008 | Use SYSTEM\_FIXED\_FONT |
| DS\_LOCALEDIT | 0x0020 | Edit controls use resource language |
| DS\_MODALFRAME | 0x0080 | Double border, no caption |
| DS\_NOFAILCREATE | 0x0010 | Ignore dialog creation failures |
| DS\_NOIDLEMSG | 0x0100 | No WM\_ENTERIDLE message |
| DS\_SETFONT | 0x0040 | Use font specified in FONT statement |
| DS\_SETFOREGROUND | 0x0200 | Dialog is foreground |
| DS\_SYSMODAL | 0x0002 | System modal dialog |
| DS\_SHELLFONT | (DS\_SETFONT \| DS\_FIXEDSYS) | Shell font combination |

### CBS\_ — Combo Box Styles

| Constant | Value | Description |
|----------|-------|-------------|
| CBS\_AUTOHSCROLL | 0x0040 | Auto-scroll in edit field |
| CBS\_DISABLENOSCROLL | 0x0800 | Disabled scroll bar when not needed |
| CBS\_DROPDOWN | 0x0002 | Edit + drop-down list |
| CBS\_DROPDOWNLIST | 0x0003 | Drop-down list only (no edit) |
| CBS\_HASSTRINGS | 0x0200 | Owner-draw with strings |
| CBS\_LOWERCASE | 0x4000 | Convert text to lowercase |
| CBS\_NOINTEGRALHEIGHT | 0x0400 | Exact height specified |
| CBS\_OEMCONVERT | 0x0080 | Convert text to OEM charset |
| CBS\_OWNERDRAWFIXED | 0x0010 | Owner-draw, fixed height |
| CBS\_OWNERDRAWVARIABLE | 0x0020 | Owner-draw, variable height |
| CBS\_SIMPLE | 0x0001 | Always visible list |
| CBS\_SORT | 0x0100 | Auto-sort strings |
| CBS\_UPPERCASE | 0x2000 | Convert text to uppercase |

### LBS\_ — List Box Styles

| Constant | Value | Description |
|----------|-------|-------------|
| LBS\_DISABLENOSCROLL | 0x1000 | Disabled scroll bar when not needed |
| LBS\_EXTENDEDSEL | 0x0800 | Extended selection with Shift/Ctrl |
| LBS\_HASSTRINGS | 0x0040 | Owner-draw with strings |
| LBS\_MULTICOLUMN | 0x0200 | Multi-column list |
| LBS\_MULTIPLESEL | 0x0008 | Multiple selection |
| LBS\_NODATA | 0x2000 | No data, virtual list box |
| LBS\_NOINTEGRALHEIGHT | 0x0100 | Exact height specified |
| LBS\_NOREDRAW | 0x0004 | Don't repaint on change |
| LBS\_NOSEL | 0x4000 | Items cannot be selected |
| LBS\_NOTIFY | 0x0001 | Send parent notifications |
| LBS\_OWNERDRAWFIXED | 0x0010 | Owner-draw, fixed height |
| LBS\_OWNERDRAWVARIABLE | 0x0020 | Owner-draw, variable height |
| LBS\_SORT | 0x0002 | Auto-sort strings |
| LBS\_STANDARD | 0xA00003 | LBS\_SORT \| LBS\_NOTIFY \| WS\_BORDER \| WS\_VSCROLL |
| LBS\_USETABSTOPS | 0x0080 | Recognize tab characters |
| LBS\_WANTKEYBOARDINPUT | 0x0400 | Send WM\_KEYDOWN to owner |

### ES\_ — Edit Control Styles

| Constant | Value | Description |
|----------|-------|-------------|
| ES\_AUTOHSCROLL | 0x0080 | Auto-scroll horizontally |
| ES\_AUTOVSCROLL | 0x0040 | Auto-scroll vertically |
| ES\_CENTER | 0x0001 | Center text |
| ES\_LEFT | 0x0000 | Left-aligned text (default) |
| ES\_LOWERCASE | 0x0010 | Convert to lowercase |
| ES\_MULTILINE | 0x0004 | Multi-line edit |
| ES\_NOHIDESEL | 0x0100 | Keep selection when focus lost |
| ES\_NUMBER | 0x2000 | Allow only digits |
| ES\_OEMCONVERT | 0x0400 | Convert to OEM charset |
| ES\_PASSWORD | 0x0020 | Display asterisks |
| ES\_READONLY | 0x0800 | Read-only |
| ES\_RIGHT | 0x0002 | Right-aligned text |
| ES\_UPPERCASE | 0x0008 | Convert to uppercase |
| ES\_WANTRETURN | 0x1000 | Accept Enter key |

### BS\_ — Button Styles

| Constant | Value | Description |
|----------|-------|-------------|
| BS\_3STATE | 0x0005 | Three-state checkbox |
| BS\_AUTO3STATE | 0x0006 | Auto three-state checkbox |
| BS\_AUTOCHECKBOX | 0x0003 | Auto checkbox |
| BS\_AUTORADIOBUTTON | 0x0009 | Auto radio button |
| BS\_BITMAP | 0x0080 | Bitmap display |
| BS\_BOTTOM | 0x0800 | Text at bottom |
| BS\_CENTER | 0x0300 | Text centered horizontally |
| BS\_CHECKBOX | 0x0002 | Checkbox |
| BS\_DEFPUSHBUTTON | 0x0001 | Default push button |
| BS\_GROUPBOX | 0x0007 | Group box |
| BS\_ICON | 0x0040 | Icon display |
| BS\_LEFT | 0x0100 | Text left-aligned |
| BS\_LEFTTEXT | 0x0020 | Text on left of checkbox/radio |
| BS\_MULTILINE | 0x2000 | Multi-line text |
| BS\_NOTIFY | 0x4000 | Send notifications |
| BS\_OWNERDRAW | 0x000B | Owner-drawn button |
| BS\_PUSHBUTTON | 0x0000 | Push button |
| BS\_PUSHLIKE | 0x1000 | Checkbox/radio with push appearance |
| BS\_RADIOBUTTON | 0x0004 | Radio button |
| BS\_RIGHT | 0x0200 | Text right-aligned |
| BS\_RIGHTBUTTON | 0x0020 | Checkbox/radio on right |
| BS\_TEXT | 0x0000 | Text display (default) |
| BS\_TOP | 0x0400 | Text at top |
| BS\_VCENTER | 0x0C00 | Text vertically centered |
| BS\_FLAT | 0x8000 | Flat button |

### SS\_ — Static Control Styles

| Constant | Value | Description |
|----------|-------|-------------|
| SS\_BITMAP | 0x000E | Bitmap display |
| SS\_BLACKFRAME | 0x0007 | Black frame |
| SS\_BLACKRECT | 0x0004 | Black rectangle |
| SS\_CENTER | 0x0001 | Centered text |
| SS\_CENTERIMAGE | 0x0200 | Center image vertically |
| SS\_EDITCONTROL | 0x2000 | Multi-line, no word wrap |
| SS\_ENHMETAFILE | 0x000F | Enhanced metafile display |
| SS\_ETCHEDFRAME | 0x0012 | Etched frame |
| SS\_ETCHEDHORZ | 0x0010 | Etched horizontal line |
| SS\_ETCHEDVERT | 0x0011 | Etched vertical line |
| SS\_GRAYFRAME | 0x0008 | Gray frame |
| SS\_GRAYRECT | 0x0005 | Gray rectangle |
| SS\_ICON | 0x0003 | Icon display |
| SS\_LEFT | 0x0000 | Left-aligned text (default) |
| SS\_LEFTNOWORDWRAP | 0x000C | Left-aligned, no word wrap |
| SS\_NOPREFIX | 0x0080 | No "&" prefix processing |
| SS\_NOTIFY | 0x0100 | Send click notifications |
| SS\_OWNERDRAW | 0x000D | Owner-drawn |
| SS\_REALSIZECONTROL | 0x0040 | Size to fit control |
| SS\_REALSIZEIMAGE | 0x0800 | Don't stretch image |
| SS\_RIGHT | 0x0002 | Right-aligned text |
| SS\_RIGHTJUST | 0x0400 | Right-justify on resize |
| SS\_SIMPLE | 0x000B | Single-line, no word wrap |
| SS\_SUNKEN | 0x1000 | Sunken border |
| SS\_WHITEFRAME | 0x0009 | White frame |
| SS\_WHITERECT | 0x0006 | White rectangle |
| SS\_TYPEMASK | 0x0000001F | Mask for type bits |
| SS\_ENDELLIPSIS | 0x00004000 | Ellipsis at end |
| SS\_PATHELLIPSIS | 0x00008000 | Ellipsis in path |
| SS\_WORDELLIPSIS | 0x0000C000 | Ellipsis at word boundary |

### SBS\_ — Scroll Bar Styles

| Constant | Value | Description |
|----------|-------|-------------|
| SBS\_BOTTOMALIGN | 0x0004 | Bottom-aligned |
| SBS\_HORZ | 0x0000 | Horizontal scroll bar |
| SBS\_LEFTALIGN | 0x0002 | Left-aligned |
| SBS\_RIGHTALIGN | 0x0004 | Right-aligned |
| SBS\_SIZEBOX | 0x0008 | Size box |
| SBS\_SIZEBOXBOTTOMRIGHTALIGN | 0x0004 | Size box, bottom-right |
| SBS\_SIZEBOXTOPLEFTALIGN | 0x0002 | Size box, top-left |
| SBS\_SIZEGRIP | 0x0010 | Size grip |
| SBS\_TOPALIGN | 0x0002 | Top-aligned |
| SBS\_VERT | 0x0001 | Vertical scroll bar |

## Window Messages (WM\_)

> **Header:** `WinUser.h`

Sent to a window's `WndProc` function. These are listed here for reference when interpreting `WM_COMMAND` notification codes in resource scripts.

| Constant | Value | Description |
|----------|-------|-------------|
| WM\_NULL | 0x0000 | No operation |
| WM\_CREATE | 0x0001 | Window being created |
| WM\_DESTROY | 0x0002 | Window being destroyed |
| WM\_MOVE | 0x0003 | Window moved |
| WM\_SIZE | 0x0005 | Window resized |
| WM\_ACTIVATE | 0x0006 | Window activated/deactivated |
| WM\_SETFOCUS | 0x0007 | Window received input focus |
| WM\_KILLFOCUS | 0x0008 | Window lost input focus |
| WM\_ENABLE | 0x000A | Window enabled/disabled |
| WM\_SETREDRAW | 0x000B | Enable/disable repainting |
| WM\_SETTEXT | 0x000C | Set window text |
| WM\_GETTEXT | 0x000D | Get window text |
| WM\_GETTEXTLENGTH | 0x000E | Get text length |
| WM\_PAINT | 0x000F | Window needs painting |
| WM\_CLOSE | 0x0010 | Close request |
| WM\_QUERYENDSESSION | 0x0011 | End session query |
| WM\_QUIT | 0x0012 | Quit request |
| WM\_ERASEBKGND | 0x0014 | Erase background |
| WM\_SHOWWINDOW | 0x0018 | Show/hide window |
| WM\_ACTIVATEAPP | 0x001C | App activation |
| WM\_CANCELMODE | 0x001F | Cancel mode |
| WM\_SETCURSOR | 0x0020 | Set cursor |
| WM\_MOUSEACTIVATE | 0x0021 | Mouse click activation |
| WM\_CHILDACTIVATE | 0x0022 | Child window activation |
| WM\_GETMINMAXINFO | 0x0024 | Min/max window info |
| WM\_PAINTICON | 0x0026 | Paint icon |
| WM\_ICONERASEBKGND | 0x0027 | Erase icon background |
| WM\_NEXTDLGCTL | 0x0028 | Set focus to next control |
| WM\_DRAWITEM | 0x002B | Owner-draw item |
| WM\_MEASUREITEM | 0x002C | Measure owner-draw item |
| WM\_DELETEITEM | 0x002D | Delete owner-draw item |
| WM\_SETFONT | 0x0030 | Set font |
| WM\_GETFONT | 0x0031 | Get font |
| WM\_SETHOTKEY | 0x0032 | Assign hot key |
| WM\_GETHOTKEY | 0x0033 | Get hot key |
| WM\_QUERYDRAGICON | 0x0037 | Query drag icon |
| WM\_COMPAREITEM | 0x0039 | Compare owner-draw items |
| WM\_COMPACTING | 0x0041 | System memory low |
| WM\_WINDOWPOSCHANGING | 0x0046 | Window position changing |
| WM\_WINDOWPOSCHANGED | 0x0047 | Window position changed |
| WM\_COPYDATA | 0x004A | Copy data between processes |
| WM\_NOTIFY | 0x004E | Control notification |
| WM\_INPUTLANGCHANGEREQUEST | 0x0050 | Language change request |
| WM\_INPUTLANGCHANGE | 0x0051 | Language changed |
| WM\_HELP | 0x0053 | Help request |
| WM\_CONTEXTMENU | 0x007B | Right-click context menu |
| WM\_STYLECHANGING | 0x007C | Style changing |
| WM\_STYLECHANGED | 0x007D | Style changed |
| WM\_DISPLAYCHANGE | 0x007E | Display resolution changed |
| WM\_GETICON | 0x007F | Get icon |
| WM\_SETICON | 0x0080 | Set icon |
| WM\_NCCREATE | 0x0081 | Non-client area created |
| WM\_NCDESTROY | 0x0082 | Non-client area destroyed |
| WM\_NCCALCSIZE | 0x0083 | Calculate non-client size |
| WM\_NCHITTEST | 0x0084 | Non-client hit test |
| WM\_NCPAINT | 0x0085 | Paint non-client area |
| WM\_NCACTIVATE | 0x0086 | Activate/deactivate non-client |
| WM\_GETDLGCODE | 0x0087 | Get dialog code |
| WM\_NCMOUSEMOVE | 0x00A0 | Non-client mouse move |
| WM\_NCLBUTTONDOWN | 0x00A1 | Non-client left button down |
| WM\_NCLBUTTONUP | 0x00A2 | Non-client left button up |
| WM\_NCLBUTTONDBLCLK | 0x00A3 | Non-client left double-click |
| WM\_NCRBUTTONDOWN | 0x00A4 | Non-client right button down |
| WM\_NCRBUTTONUP | 0x00A5 | Non-client right button up |
| WM\_NCRBUTTONDBLCLK | 0x00A6 | Non-client right double-click |
| WM\_NCMBUTTONDOWN | 0x00A7 | Non-client middle button down |
| WM\_NCMBUTTONUP | 0x00A8 | Non-client middle button up |
| WM\_NCMBUTTONDBLCLK | 0x00A9 | Non-client middle double-click |
| WM\_KEYDOWN | 0x0100 | Key pressed |
| WM\_KEYUP | 0x0101 | Key released |
| WM\_CHAR | 0x0102 | Character input |
| WM\_DEADCHAR | 0x0103 | Dead character |
| WM\_SYSKEYDOWN | 0x0104 | System key pressed |
| WM\_SYSKEYUP | 0x0105 | System key released |
| WM\_SYSCHAR | 0x0106 | System character input |
| WM\_SYSDEADCHAR | 0x0107 | System dead character |
| WM\_INITDIALOG | 0x0110 | Dialog initialization |
| WM\_COMMAND | 0x0111 | Command notification |
| WM\_SYSCOMMAND | 0x0112 | System command |
| WM\_TIMER | 0x0113 | Timer expired |
| WM\_HSCROLL | 0x0114 | Horizontal scroll |
| WM\_VSCROLL | 0x0115 | Vertical scroll |
| WM\_INITMENU | 0x0116 | Menu activation |
| WM\_INITMENUPOPUP | 0x0117 | Submenu activation |
| WM\_MENUSELECT | 0x011F | Menu item selected |
| WM\_MENUCHAR | 0x0120 | Menu character |
| WM\_ENTERIDLE | 0x0121 | Idle processing |
| WM\_CTLCOLORMSGBOX | 0x0132 | Message box color |
| WM\_CTLCOLOREDIT | 0x0133 | Edit control color |
| WM\_CTLCOLORLISTBOX | 0x0134 | List box color |
| WM\_CTLCOLORBTN | 0x0135 | Button color |
| WM\_CTLCOLORDLG | 0x0136 | Dialog color |
| WM\_CTLCOLORSCROLLBAR | 0x0137 | Scroll bar color |
| WM\_CTLCOLORSTATIC | 0x0138 | Static control color |
| WM\_MOUSEMOVE | 0x0200 | Mouse moved |
| WM\_LBUTTONDOWN | 0x0201 | Left button pressed |
| WM\_LBUTTONUP | 0x0202 | Left button released |
| WM\_LBUTTONDBLCLK | 0x0203 | Left button double-clicked |
| WM\_RBUTTONDOWN | 0x0204 | Right button pressed |
| WM\_RBUTTONUP | 0x0205 | Right button released |
| WM\_RBUTTONDBLCLK | 0x0206 | Right button double-clicked |
| WM\_MBUTTONDOWN | 0x0207 | Middle button pressed |
| WM\_MBUTTONUP | 0x0208 | Middle button released |
| WM\_MBUTTONDBLCLK | 0x0209 | Middle button double-clicked |
| WM\_MOUSEWHEEL | 0x020A | Mouse wheel |
| WM\_PARENTNOTIFY | 0x0210 | Parent notified |
| WM\_ENTERMENULOOP | 0x0211 | Menu loop entered |
| WM\_EXITMENULOOP | 0x0212 | Menu loop exited |
| WM\_NEXTMENU | 0x0213 | Next/previous menu |
| WM\_SIZING | 0x0214 | Window being resized |
| WM\_CAPTURECHANGED | 0x0215 | Mouse capture changed |
| WM\_MOVING | 0x0216 | Window being moved |
| WM\_POWERBROADCAST | 0x0218 | Power event |
| WM\_DEVICECHANGE | 0x0219 | Device configuration changed |
| WM\_MDICREATE | 0x0220 | MDI child create |
| WM\_MDIDESTROY | 0x0221 | MDI child destroy |
| WM\_MDIACTIVATE | 0x0222 | MDI child activate |
| WM\_MDIRESTORE | 0x0223 | MDI child restore |
| WM\_MDINEXT | 0x0224 | MDI next child |
| WM\_MDIMAXIMIZE | 0x0225 | MDI maximize child |
| WM\_MDITILE | 0x0226 | MDI tile |
| WM\_MDICASCADE | 0x0227 | MDI cascade |
| WM\_MDIICONARRANGE | 0x0228 | MDI arrange icons |
| WM\_MDIGETACTIVE | 0x0229 | Get active MDI child |
| WM\_MDISETMENU | 0x0230 | Set MDI menus |
| WM\_PRINT | 0x0317 | Print window |
| WM\_PRINTCLIENT | 0x0318 | Print client area |
| WM\_HANDHELDFIRST | 0x0358 | Handheld first |
| WM\_HANDHELDLAST | 0x035F | Handheld last |
| WM\_AFXFIRST | 0x0360 | AFX first |
| WM\_AFXLAST | 0x037F | AFX last |
| WM\_PENWINFIRST | 0x0380 | Pen first |
| WM\_PENWINLAST | 0x038F | Pen last |
| WM\_USER | 0x0400 | User-defined range start |
| WM\_APP | 0x8000 | Application-defined range start |

## Virtual Key Codes (VK\_)

> **Header:** `WinUser.h`

Used in accelerator tables and key event processing. These can be used in RC files as event specifiers.

### Mouse and Button Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_LBUTTON | 0x01 | Left mouse button |
| VK\_RBUTTON | 0x02 | Right mouse button |
| VK\_CANCEL | 0x03 | Cancel (Ctrl+Break) |
| VK\_MBUTTON | 0x04 | Middle mouse button |
| VK\_XBUTTON1 | 0x05 | X1 mouse button |
| VK\_XBUTTON2 | 0x06 | X2 mouse button |

### Common Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_BACK | 0x08 | Backspace |
| VK\_TAB | 0x09 | Tab |
| VK\_CLEAR | 0x0C | Clear |
| VK\_RETURN | 0x0D | Enter |
| VK\_SHIFT | 0x10 | Shift |
| VK\_CONTROL | 0x11 | Ctrl |
| VK\_MENU | 0x12 | Alt |
| VK\_PAUSE | 0x13 | Pause |
| VK\_CAPITAL | 0x14 | Caps Lock |
| VK\_ESCAPE | 0x1B | Escape |
| VK\_SPACE | 0x20 | Spacebar |
| VK\_PRIOR | 0x21 | Page Up |
| VK\_NEXT | 0x22 | Page Down |
| VK\_END | 0x23 | End |
| VK\_HOME | 0x24 | Home |
| VK\_SELECT | 0x29 | Select |
| VK\_PRINT | 0x2A | Print |
| VK\_EXECUTE | 0x2B | Execute |
| VK\_SNAPSHOT | 0x2C | Print Screen |
| VK\_INSERT | 0x2D | Insert |
| VK\_DELETE | 0x2E | Delete |
| VK\_HELP | 0x2F | Help |

### Arrow Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_LEFT | 0x25 | Left arrow |
| VK\_UP | 0x26 | Up arrow |
| VK\_RIGHT | 0x27 | Right arrow |
| VK\_DOWN | 0x28 | Down arrow |

### Number Keys (Top Row)

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_0 | 0x30 | 0 key |
| VK\_1 | 0x31 | 1 key |
| VK\_2 | 0x32 | 2 key |
| VK\_3 | 0x33 | 3 key |
| VK\_4 | 0x34 | 4 key |
| VK\_5 | 0x35 | 5 key |
| VK\_6 | 0x36 | 6 key |
| VK\_7 | 0x37 | 7 key |
| VK\_8 | 0x38 | 8 key |
| VK\_9 | 0x39 | 9 key |

### Letter Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_A | 0x41 | A key |
| VK\_B | 0x42 | B key |
| VK\_C | 0x43 | C key |
| VK\_D | 0x44 | D key |
| VK\_E | 0x45 | E key |
| VK\_F | 0x46 | F key |
| VK\_G | 0x47 | G key |
| VK\_H | 0x48 | H key |
| VK\_I | 0x49 | I key |
| VK\_J | 0x4A | J key |
| VK\_K | 0x4B | K key |
| VK\_L | 0x4C | L key |
| VK\_M | 0x4D | M key |
| VK\_N | 0x4E | N key |
| VK\_O | 0x4F | O key |
| VK\_P | 0x50 | P key |
| VK\_Q | 0x51 | Q key |
| VK\_R | 0x52 | R key |
| VK\_S | 0x53 | S key |
| VK\_T | 0x54 | T key |
| VK\_U | 0x55 | U key |
| VK\_V | 0x56 | V key |
| VK\_W | 0x57 | W key |
| VK\_X | 0x58 | X key |
| VK\_Y | 0x59 | Y key |
| VK\_Z | 0x5A | Z key |

### Windows and Application Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_LWIN | 0x5B | Left Windows key |
| VK\_RWIN | 0x5C | Right Windows key |
| VK\_APPS | 0x5D | Applications key |

### Numpad Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_NUMPAD0 | 0x60 | Numpad 0 |
| VK\_NUMPAD1 | 0x61 | Numpad 1 |
| VK\_NUMPAD2 | 0x62 | Numpad 2 |
| VK\_NUMPAD3 | 0x63 | Numpad 3 |
| VK\_NUMPAD4 | 0x64 | Numpad 4 |
| VK\_NUMPAD5 | 0x65 | Numpad 5 |
| VK\_NUMPAD6 | 0x66 | Numpad 6 |
| VK\_NUMPAD7 | 0x67 | Numpad 7 |
| VK\_NUMPAD8 | 0x68 | Numpad 8 |
| VK\_NUMPAD9 | 0x69 | Numpad 9 |
| VK\_MULTIPLY | 0x6A | Numpad \* |
| VK\_ADD | 0x6B | Numpad + |
| VK\_SEPARATOR | 0x6C | Numpad separator |
| VK\_SUBTRACT | 0x6D | Numpad - |
| VK\_DECIMAL | 0x6E | Numpad . |
| VK\_DIVIDE | 0x6F | Numpad / |

### Function Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_F1 | 0x70 | F1 |
| VK\_F2 | 0x71 | F2 |
| VK\_F3 | 0x72 | F3 |
| VK\_F4 | 0x73 | F4 |
| VK\_F5 | 0x74 | F5 |
| VK\_F6 | 0x75 | F6 |
| VK\_F7 | 0x76 | F7 |
| VK\_F8 | 0x77 | F8 |
| VK\_F9 | 0x78 | F9 |
| VK\_F10 | 0x79 | F10 |
| VK\_F11 | 0x7A | F11 |
| VK\_F12 | 0x7B | F12 |

### Lock and Modifier Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_NUMLOCK | 0x90 | Num Lock |
| VK\_SCROLL | 0x91 | Scroll Lock |
| VK\_LSHIFT | 0xA0 | Left Shift |
| VK\_RSHIFT | 0xA1 | Right Shift |
| VK\_LCONTROL | 0xA2 | Left Ctrl |
| VK\_RCONTROL | 0xA3 | Right Ctrl |
| VK\_LMENU | 0xA4 | Left Alt |
| VK\_RMENU | 0xA5 | Right Alt |

### Multimedia Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_BROWSER\_BACK | 0xA6 | Browser Back |
| VK\_BROWSER\_FORWARD | 0xA7 | Browser Forward |
| VK\_BROWSER\_REFRESH | 0xA8 | Browser Refresh |
| VK\_BROWSER\_STOP | 0xA9 | Browser Stop |
| VK\_BROWSER\_SEARCH | 0xAA | Browser Search |
| VK\_BROWSER\_FAVORITES | 0xAB | Browser Favorites |
| VK\_BROWSER\_HOME | 0xAC | Browser Home |
| VK\_VOLUME\_MUTE | 0xAD | Volume Mute |
| VK\_VOLUME\_DOWN | 0xAE | Volume Down |
| VK\_VOLUME\_UP | 0xAF | Volume Up |
| VK\_MEDIA\_NEXT\_TRACK | 0xB0 | Next Track |
| VK\_MEDIA\_PREV\_TRACK | 0xB1 | Previous Track |
| VK\_MEDIA\_STOP | 0xB2 | Stop |
| VK\_MEDIA\_PLAY\_PAUSE | 0xB3 | Play/Pause |
| VK\_LAUNCH\_MAIL | 0xB4 | Launch Mail |
| VK\_LAUNCH\_MEDIA\_SELECT | 0xB5 | Launch Media Select |
| VK\_LAUNCH\_APP1 | 0xB6 | Launch App 1 |
| VK\_LAUNCH\_APP2 | 0xB7 | Launch App 2 |

### OEM Keys

| Constant | Value | Description |
|----------|-------|-------------|
| VK\_OEM\_1 | 0xBA | ;: key (US) |
| VK\_OEM\_PLUS | 0xBB | += key |
| VK\_OEM\_COMMA | 0xBC | ,< key |
| VK\_OEM\_MINUS | 0xBD | -\_ key |
| VK\_OEM\_PERIOD | 0xBE | .> key |
| VK\_OEM\_2 | 0xBF | /? key (US) |
| VK\_OEM\_3 | 0xC0 | \~ key (US) |
| VK\_OEM\_4 | 0xDB | {\[ key (US) |
| VK\_OEM\_5 | 0xDC | \\| key (US) |
| VK\_OEM\_6 | 0xDD | }] key (US) |
| VK\_OEM\_7 | 0xDE | '" key (US) |
| VK\_OEM\_102 | 0xE2 | Angle bracket or backslash |
| VK\_SLEEP | 0x5F | Computer Sleep key |

## Message Box Flags (MB\_)

> **Header:** `WinUser.h`

Used with the `MessageBox` function. Values can be combined with `|`.

| Constant | Value | Description |
|----------|-------|-------------|
| MB\_OK | 0x00000000 | OK button only |
| MB\_OKCANCEL | 0x00000001 | OK and Cancel buttons |
| MB\_ABORTRETRYIGNORE | 0x00000002 | Abort, Retry, Ignore |
| MB\_YESNOCANCEL | 0x00000003 | Yes, No, Cancel |
| MB\_YESNO | 0x00000004 | Yes and No buttons |
| MB\_RETRYCANCEL | 0x00000005 | Retry and Cancel |
| MB\_CANCELTRYCONTINUE | 0x00000006 | Cancel, Try Again, Continue |
| MB\_ICONHAND | 0x00000010 | Error icon |
| MB\_ICONERROR | 0x00000010 | Error icon (alias) |
| MB\_ICONQUESTION | 0x00000020 | Question icon |
| MB\_ICONEXCLAMATION | 0x00000030 | Warning icon |
| MB\_ICONWARNING | 0x00000030 | Warning icon (alias) |
| MB\_ICONASTERISK | 0x00000040 | Information icon |
| MB\_ICONINFORMATION | 0x00000040 | Information icon (alias) |
| MB\_DEFBUTTON1 | 0x00000000 | First button is default |
| MB\_DEFBUTTON2 | 0x00000100 | Second button is default |
| MB\_DEFBUTTON3 | 0x00000200 | Third button is default |
| MB\_DEFBUTTON4 | 0x00000300 | Fourth button is default |
| MB\_APPLMODAL | 0x00000000 | Application modal |
| MB\_SYSTEMMODAL | 0x00001000 | System modal |
| MB\_TASKMODAL | 0x00002000 | Task modal |
| MB\_HELP | 0x00004000 | Help button |
| MB\_SETFOREGROUND | 0x00010000 | Foreground window |
| MB\_DEFAULT\_DESKTOP\_ONLY | 0x00020000 | Desktop only |
| MB\_TOPMOST | 0x00040000 | Always on top |
| MB\_RIGHT | 0x00080000 | Right-aligned text |
| MB\_RTLREADING | 0x00100000 | Right-to-left text |

## Dialog Button Identifiers

> **Header:** `WinUser.h`

Standard return values from dialog box buttons.

| Constant | Value | Description |
|----------|-------|-------------|
| IDOK | 1 | OK button |
| IDCANCEL | 2 | Cancel button |
| IDABORT | 3 | Abort button |
| IDRETRY | 4 | Retry button |
| IDIGNORE | 5 | Ignore button |
| IDYES | 6 | Yes button |
| IDNO | 7 | No button |
| IDCLOSE | 8 | Close button |
| IDHELP | 9 | Help button |
| IDTRYAGAIN | 10 | Try Again button |
| IDCONTINUE | 11 | Continue button |

### System Control Identifiers

| Constant | Value | Description |
|----------|-------|-------------|
| IDC\_STATIC | (-1) | Non-interactive static control (label, image) |

## Menu Flags (MF\_\*, MFT\_\*, MFS\_\*)

> **Header:** `WinUser.h`

Used in MENU resource statements to configure menu item appearance and behavior.

### MF\_ — Classic Menu Flags

| Constant | Value | Description |
|----------|-------|-------------|
| MF\_STRING | 0x0000 | Display text string (default) |
| MF\_GRAYED | 0x0001 | Grayed, non-selectable |
| MF\_DISABLED | 0x0002 | Disabled, non-selectable |
| MF\_CHECKED | 0x0008 | Check mark next to item |
| MF\_UNCHECKED | 0x0000 | No check mark (default) |
| MF\_POPUP | 0x0010 | Item opens a submenu |
| MF\_MENUBARBREAK | 0x0020 | New column with separator |
| MF\_MENUBREAK | 0x0040 | New column, no separator |
| MF\_SEPARATOR | 0x0800 | Horizontal separator line |
| MF\_OWNERDRAW | 0x0100 | Owner-drawn item |
| MF\_DEFAULT | 0x1000 | Default menu item (bold) |
| MF\_SYSMENU | 0x2000 | System menu item |
| MF\_HELP | 0x4000 | Help item |
| MF\_END | 0x0080 | Last item in menu |
| MF\_RIGHTJUSTIFY | 0x4000 | Right-justify group |
| MF\_MOUSESELECT | 0x8000 | Selected by mouse |
| MF\_BYCOMMAND | 0x0000 | Identify by command ID (default) |
| MF\_BYPOSITION | 0x0400 | Identify by position |

### MFT\_ — Extended Menu Item Types

Used with `MENUEX` resources. These correspond to the `fType` member of `MENUITEMINFO`.

| Constant | Value | Description |
|----------|-------|-------------|
| MFT\_STRING | 0x0000 | Text string |
| MFT\_BITMAP | 0x0004 | Bitmap |
| MFT\_MENUBARBREAK | 0x0020 | New column with separator |
| MFT\_MENUBREAK | 0x0040 | New column, no separator |
| MFT\_OWNERDRAW | 0x0100 | Owner-drawn |
| MFT\_RADIOCHECK | 0x0200 | Radio-style check mark |
| MFT\_SEPARATOR | 0x0800 | Horizontal separator |
| MFT\_RIGHTORDER | 0x2000 | Right-to-left ordering |
| MFT\_RIGHTJUSTIFY | 0x4000 | Right-justify group |

### MFS\_ — Extended Menu Item States

Used with `MENUEX` resources. These correspond to the `fState` member of `MENUITEMINFO`.

| Constant | Value | Description |
|----------|-------|-------------|
| MFS\_ENABLED | 0x0000 | Enabled (default) |
| MFS\_DISABLED | 0x0003 | Disabled |
| MFS\_GRAYED | 0x0003 | Grayed |
| MFS\_CHECKED | 0x0008 | Checked |
| MFS\_UNCHECKED | 0x0000 | Unchecked (default) |
| MFS\_HILITE | 0x0080 | Highlighted |
| MFS\_UNHILITE | 0x0000 | Not highlighted (default) |
| MFS\_DEFAULT | 0x1000 | Default item (bold) |

## Accelerator Key Flags (F\_\*, NOINVERT)

> **Header:** `WinUser.h`

Used inside ACCELERATORS resource statements to define modifier keys for accelerator entries.

| Constant | Value | Description |
|----------|-------|-------------|
| FVIRTKEY | 0x01 | Virtual key code (VK\_\*) follows |
| FNOINVERT | 0x02 | No uppercase conversion on Alt |
| FSHIFT | 0x04 | Shift key must be pressed |
| FCONTROL | 0x08 | Ctrl key must be pressed |
| FALT | 0x10 | Alt key must be pressed |
| NOINVERT | 0x02 | Alias for FNOINVERT (RC keyword) |

## Predefined Resource Types (RT\_\*)

> **Header:** `WinUser.h`

Internal resource type ordinals used by the resource compiler. In RC files, you use the resource keyword (ICON, BITMAP, DIALOG, etc.) instead of these directly. They are defined here for completeness and programmatic use.

| Constant | Value | RC Keyword Equivalent |
|----------|-------|-----------------------|
| RT\_CURSOR | 1 | CURSOR |
| RT\_BITMAP | 2 | BITMAP |
| RT\_ICON | 3 | ICON |
| RT\_MENU | 4 | MENU |
| RT\_DIALOG | 5 | DIALOG |
| RT\_STRING | 6 | STRINGTABLE |
| RT\_FONTDIR | 7 | (internal) |
| RT\_FONT | 8 | FONT |
| RT\_ACCELERATOR | 9 | ACCELERATORS |
| RT\_RCDATA | 10 | RCDATA |
| RT\_MESSAGETABLE | 11 | MESSAGETABLE |
| RT\_GROUP\_CURSOR | 22 | CURSOR (group) |
| RT\_GROUP\_ICON | 23 | ICON (group) |
| RT\_VERSION | 16 | VERSIONINFO |
| RT\_DLGINCLUDE | 17 | DLGINCLUDE |
| RT\_PLUGPLAY | 19 | PLUGPLAY (obsolete) |
| RT\_VXD | 20 | VXD (obsolete) |
| RT\_ANICURSOR | 21 | (animated cursor) |
| RT\_ANIICON | 22 | (animated icon) |
| RT\_HTML | 23 | HTML |
| RT\_MANIFEST | 24 | (manifest resource) |

## See also

<dl> <dt>

[**DIALOG Resource**](dialog-resource.md)
</dt> <dt>

[**CONTROL Control**](control-control.md)
</dt> <dt>

[**PUSHBUTTON Control**](pushbutton-control.md)
</dt> <dt>

[**STYLE Statement**](style-statement.md)
</dt> <dt>

[**MENU Resource**](menu-resource.md)
</dt> <dt>

[**ACCELERATORS Resource**](accelerators-resource.md)
</dt> </dl>
