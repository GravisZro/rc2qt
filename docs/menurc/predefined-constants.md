# Predefined Constants (WinUser.h / CommCtrl.h)

Numeric constants defined in the Windows SDK headers `WinUser.h` and `CommCtrl.h` can be used interchangeably with their integer values anywhere in a resource script (.rc) file. For example, `VK_RETURN` and `13` are equivalent in an accelerator table, and `WS_CHILD` and `0x40000000` are equivalent in a STYLE statement.

The resource compiler resolves these symbols at compile time via `#include <windows.h>` (which includes both headers).

## Naming Conventions

Constants follow a prefix-based naming pattern organized by subsystem:

| Prefix | Subsystem | Purpose |
|--------|-----------|---------|
| CB\_ | Combo Box | Window styles |
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

## WS_* Window Styles

| Constant | Value | Description |
|----------|-------|-------------|
| WS_BORDER | 0x800000 | Creates a window that has a thin-line border. |
| WS_CAPTION | 0xc00000 | Creates a window that has a title bar (includes the WS_BORDER style). |
| WS_CHILD | 0x40000000 | Creates a child window. A window with this style cannot have a menu bar and cannot be used with the WS_POPUP style. |
| WS_CLIPCHILDREN | 0x2000000 | Excludes the area occupied by child windows when drawing occurs within the parent window. Used when creating the parent window. |
| WS_CLIPSIBLINGS | 0x4000000 | Clips child windows relative to each other; that is |
| WS_DISABLED | 0x8000000 | Creates a window that is initially disabled. A disabled window cannot receive input from the user. |
| WS_DLGFRAME | 0x400000 | Creates a window that has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar. |
| WS_GROUP | 0x20000 | Specifies the first control of a group of controls. The group consists of this first control and all controls defined after it |
| WS_HSCROLL | 0x100000 | Creates a window that has a horizontal scroll bar. |
| WS_MAXIMIZE | 0x1000000 | Creates a window that is initially maximized. |
| WS_MAXIMIZEBOX | 0x10000 | Creates a window that has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU. |
| WS_MINIMIZE | 0x20000000 | Creates a window that is initially minimized. |
| WS_MINIMIZEBOX | 0x20000 | Creates a window that has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU. |
| WS_OVERLAPPED | 0 | Creates an overlapped window. An overlapped window has a title bar and a border. |
| WS_POPUP | 0x80000000 | Creates a pop-up window. Cannot be used with the WS_CHILD style. |
| WS_SIZEBOX | 0x40000 | Creates a window that has a sizing border (same as WS_THICKFRAME). |
| WS_SYSMENU | 0x80000 | Creates a window that has a window menu on its title bar. Requires WS_CAPTION. |
| WS_TABSTOP | 0x10000 | Specifies a control that can receive the keyboard focus when the user presses the TAB key. Pressing TAB changes the focus to the next control with this style. |
| WS_VISIBLE | 0x10000000 | Creates a window that is initially visible. |
| WS_VSCROLL | 0x200000 | Creates a window that has a vertical scroll bar. |
| WS_THICKFRAME | 0x40000 | Creates a window that has a sizing border (same as WS_SIZEBOX). |
| WS_OVERLAPPEDWINDOW | 0xcf0000 | Creates an overlapped window with WS_OVERLAPPED |
| WS_POPUPWINDOW | 0x80880000 | Creates a pop-up window with WS_POPUP |

## WS_EX_* Extended Window Styles

| Constant | Value | Description |
|----------|-------|-------------|
| WS_EX_ACCEPTFILES | 0x10 | Specifies that a window created with this style accepts dropped files. |
| WS_EX_APPWINDOW | 0x40000 | Forces a top-level window onto the taskbar when the window is visible. |
| WS_EX_CLIENTEDGE | 0x200 | Specifies that a window has a border with a sunken edge. |
| WS_EX_COMPOSITED | 0x2000000 | Paints all descendants of a window in bottom-to-top painting order using double-buffering. |
| WS_EX_CONTEXTHELP | 0x400 | Includes a question mark in the title bar of the window. When clicked |
| WS_EX_CONTROLPARENT | 0x10000 | Allows the user to navigate among the child windows of the window by using the TAB key. |
| WS_EX_DLGMODALFRAME | 1 | Creates a window that has a double border; can optionally be created with a title bar by specifying WS_CAPTION. |
| WS_EX_LAYERED | 0x80000 | Creates a layered window. Note that this cannot be used if the window has a window class style of CS_OWNDC or CS_CLASSDC. |
| WS_EX_LAYOUTRTL | 0x400000 | Sets the horizontal origin to the right edge. Increasing horizontal values advance to the left. |
| WS_EX_LEFT | 0 | Gives the window left-aligned properties (default layout). |
| WS_EX_LEFTSCROLLBAR | 0x4000 | Places the vertical scroll bar (if present) to the left of the client area. |
| WS_EX_LTRREADING | 0 | Displays window text using left-to-right reading order properties (default). |
| WS_EX_MDICHILD | 0x40 | Creates a Multiple-Document Interface (MDI) child window. |
| WS_EX_NOACTIVATE | 0x8000000 | A top-level window created with this style does not become the active window when the user clicks it. |
| WS_EX_NOINHERITLAYOUT | 0x100000 | Prevents the window layout from being inherited by child windows. |
| WS_EX_NOPARENTNOTIFY | 4 | Specifies that a child window created with this style does not send the WM_PARENTNOTIFY message to its parent when created or destroyed. |
| WS_EX_PALETTEWINDOW | 0x188 | Combines WS_EX_WINDOWEDGE |
| WS_EX_RIGHT | 0x1000 | Gives the window right-aligned properties depending on window class. |
| WS_EX_RIGHTSCROLLBAR | 0 | Places the vertical scroll bar to the right of the client area (default). |
| WS_EX_RTLREADING | 0x2000 | Displays window text using Right-to-Left reading order properties. |
| WS_EX_STATICEDGE | 0x20000 | Gives a window a three-dimensional border style intended to be used for items that do not accept user input. |
| WS_EX_TOOLWINDOW | 0x80 | Creates a tool window intended as a floating toolbar; does not appear on the taskbar or in the ALT+TAB dialog. |
| WS_EX_TOPMOST | 8 | Specifies that a window created with this style should be placed above all non-topmost windows and remain above them |
| WS_EX_TRANSPARENT | 0x20 | Specifies that a window created with this style should not be painted until siblings beneath it have been painted. |
| WS_EX_WINDOWEDGE | 0x100 | Specifies that a window has a border with a raised edge. |

## DS_* Dialog Styles

| Constant | Value | Description |
|----------|-------|-------------|
| DS_3DLOOK | 4 | Gives dialog box controls a 3D look (historical compatibility style). |
| DS_ABSALIGN | 1 | Indicates that the coordinates of the dialog box are screen coordinates (otherwise client coordinates relative to owner). |
| DS_CENTER | 0x800 | Centers the dialog box on the display screen or monitor. |
| DS_CENTERMOUSE | 0x1000 | Centers the dialog box under the mouse cursor. |
| DS_CONTEXTHELP | 0x2000 | Includes a Help button in the title bar of the dialog box. |
| DS_CONTROL | 0x400 | Creates a dialog box that can work as a control child inside another dialog box (like a tab control page). |
| DS_FIXEDSYS | 8 | Causes the dialog box to use the system font with fixed spacing instead of proportional spacing. |
| DS_LOCALEDIT | 0x20 | Applies to edit controls in the dialog box |
| DS_MODALFRAME | 0x80 | Creates a dialog box with a modal dialog frame border. |
| DS_NOFAILCREATE | 0x10 | Causes CreateDialog or DialogBox to succeed even if some child controls cannot be created. |
| DS_NOIDLEMSG | 0x100 | Suppresses WM_ENTERIDLE messages that the system otherwise sends to the owner while the dialog box is displayed. |
| DS_SETFONT | 0x40 | Indicates that the dialog template specifies a custom font for rendering text in the dialog box. |
| DS_SETFOREGROUND | 0x200 | Brings the dialog box to the foreground upon creation. |
| DS_SYSMODAL | 2 | Creates a system-modal dialog box (historical style |
| DS_SHELLFONT | 0x48 | Uses the system shell font for rendering text inside the dialog box (requires DS_SETFONT). |

## BS_* Button Styles

| Constant | Value | Description |
|----------|-------|-------------|
| BS_3STATE | 5 | Creates a button that functions like a checkbox but can be dimmed (indeterminate state) as well as checked or unchecked. |
| BS_AUTO3STATE | 6 | Same as BS_3STATE |
| BS_AUTOCHECKBOX | 3 | Same as BS_CHECKBOX |
| BS_AUTORADIOBUTTON | 9 | Same as BS_RADIOBUTTON |
| BS_BITMAP | 0x80 | Specifies that the button displays a bitmap instead of text. |
| BS_BOTTOM | 0x800 | Places text at the bottom of the button rectangle. |
| BS_CENTER | 0x300 | Horizontally centers text in the button rectangle. |
| BS_CHECKBOX | 2 | Creates a small square box with adjacent text. Typically used to allow multi-select options. |
| BS_DEFPUSHBUTTON | 1 | Creates a push button with a heavy black border representing the default action when ENTER is pressed. |
| BS_GROUPBOX | 7 | Creates a rectangle in which other controls can be grouped |
| BS_ICON | 0x40 | Specifies that the button displays an icon instead of text. |
| BS_LEFT | 0x100 | Left-aligns the text in the button rectangle. |
| BS_LEFTTEXT | 0x20 | Places the radio button circle or checkbox square on the right side of the control text. |
| BS_MULTILINE | 0x2000 | Wraps the button text onto multiple lines if it is too long to fit on a single line. |
| BS_NOTIFY | 0x4000 | Enables a button to send BN_KILLFOCUS |
| BS_OWNERDRAW | 11 | Creates an owner-drawn button where the parent window is responsible for painting the button's appearance. |
| BS_PUSHBUTTON | 0 | Creates a standard push button that posts a WM_COMMAND message to its owner window when clicked. |
| BS_PUSHLIKE | 0x1000 | Makes a checkbox or radio button look and act like a standard push button. |
| BS_RADIOBUTTON | 4 | Creates a small circle with adjacent text |
| BS_RIGHT | 0x200 | Right-aligns the text in the button rectangle. |
| BS_RIGHTBUTTON | 0x20 | Positions the button box on the right side of the text (same as BS_LEFTTEXT). |
| BS_TEXT | 0 | Specifies that the button displays text (default behavior). |
| BS_TOP | 0x400 | Places text at the top of the button rectangle. |
| BS_VCENTER | 0xc00 | Vertically centers text in the button rectangle. |
| BS_FLAT | 0x8000 | Specifies that the button is drawn flat |

## ES_* Edit Control Styles

| Constant | Value | Description |
|----------|-------|-------------|
| ES_AUTOHSCROLL | 0x80 | Automatically scrolls edit control text to the right when typing reaches the right border. |
| ES_AUTOVSCROLL | 0x40 | Automatically scrolls edit control text up when pressing ENTER on the last visible line. |
| ES_CENTER | 1 | Centers text in a single-line or multiline edit control. |
| ES_LEFT | 0 | Left-aligns text in an edit control. |
| ES_LOWERCASE | 0x10 | Converts all characters entered into the edit control to lowercase. |
| ES_MULTILINE | 4 | Designates a multiline edit control allowing text entry on multiple lines. |
| ES_NOHIDESEL | 0x100 | Prevents the edit control from hiding the text selection when it loses input focus. |
| ES_NUMBER | 0x2000 | Restricts the edit control input to digits only. |
| ES_OEMCONVERT | 0x400 | Converts text typed in the edit control from ANSI to OEM character set and back. |
| ES_PASSWORD | 0x20 | Displays an asterisk or custom mask character for every character typed into the edit control. |
| ES_READONLY | 0x800 | Prevents the user from typing or editing text in the edit control. |
| ES_RIGHT | 2 | Right-aligns text in a single-line or multiline edit control. |
| ES_UPPERCASE | 8 | Converts all characters entered into the edit control to uppercase. |
| ES_WANTRETURN | 0x1000 | Inserts a carriage return when the user presses ENTER in a multiline edit control instead of submitting the dialog. |

## SS_* Static Control Styles

| Constant | Value | Description |
|----------|-------|-------------|
| SS_BITMAP | 14 | Specifies that the static control displays a bitmap resource. |
| SS_BLACKFRAME | 7 | Displays a box drawn with a frame of the same color as window frames (default black). |
| SS_BLACKRECT | 4 | Displays a solid rectangle filled with the color used to draw window frames. |
| SS_CENTER | 1 | Designates a simple control displaying centered text wrapped automatically across lines. |
| SS_CENTERIMAGE | 0x200 | Centers the image or text vertically and horizontally inside the static control bounding area. |
| SS_EDITCONTROL | 0x2000 | Mimics the text-wrapping and rendering behavior of a standard edit control in a static control. |
| SS_ENHMETAFILE | 15 | Specifies that the static control displays an enhanced metafile. |
| SS_ETCHEDFRAME | 0x12 | Draws the frame of the static control using the EDGE_ETCHED border style. |
| SS_ETCHEDHORZ | 0x10 | Draws a horizontal line using the EDGE_ETCHED style. |
| SS_ETCHEDVERT | 0x11 | Draws a vertical line using the EDGE_ETCHED style. |
| SS_GRAYFRAME | 8 | Displays a box drawn with a frame of the current desktop background color. |
| SS_GRAYRECT | 5 | Displays a solid rectangle filled with the current desktop background color. |
| SS_ICON | 3 | Displays an icon in the static control. |
| SS_LEFT | 0 | Left-aligns text in a static control with word wrapping. |
| SS_LEFTNOWORDWRAP | 12 | Left-aligns text without wrapping long lines; excess text is clipped. |
| SS_NOPREFIX | 0x80 | Prevents interpreting ampersands (&) as accelerator prefix characters in static control text. |
| SS_NOTIFY | 0x100 | Sends mouse click notification messages (STN_CLICKED |
| SS_OWNERDRAW | 13 | Delegates control painting responsibility to the parent window via WM_DRAWITEM. |
| SS_REALSIZECONTROL | 0x40 | Prevents resizing bitmap or icon images inside the static control |
| SS_REALSIZEIMAGE | 0x800 | Clips the image if it is larger than the static control bounds rather than scaling it. |
| SS_RIGHT | 2 | Right-aligns text in a static control with word wrapping. |
| SS_RIGHTJUST | 0x400 | Specifies that the upper-right corner of the static image/icon remains fixed when resized. |
| SS_SIMPLE | 11 | Displays a single line of left-aligned static text that cannot be wrapped or modified dynamically. |
| SS_SUNKEN | 0x1000 | Draws a sunken 3D border around the static control. |
| SS_WHITEFRAME | 9 | Displays a box drawn with a frame matching the window background color (default white). |
| SS_WHITERECT | 6 | Displays a solid rectangle filled with the window background color. |
| SS_TYPEMASK | 0x1f | Bitmask used to isolate static control style types. |
| SS_ENDELLIPSIS | 0x4000 | Replaces characters at the end of static text with an ellipsis if they exceed control bounds. |
| SS_PATHELLIPSIS | 0x8000 | Replaces characters in the middle of a file path with an ellipsis to fit inside the static control. |
| SS_WORDELLIPSIS | 0xc000 | Truncates text that extends beyond the control end and inserts an ellipsis after the last visible word. |

## LBS_* List Box Styles

| Constant | Value | Description |
|----------|-------|-------------|
| LBS_DISABLENOSCROLL | 0x1000 | Shows a disabled vertical scroll bar in the list box when it contains too few items to scroll. |
| LBS_EXTENDEDSEL | 0x800 | Allows multiple selection using SHIFT and CTRL key combinations or mouse dragging. |
| LBS_HASSTRINGS | 0x40 | Specifies that an owner-drawn list box contains items consisting of strings. |
| LBS_MULTICOLUMN | 0x200 | Specifies a multi-column list box that scrolls horizontally. |
| LBS_MULTIPLESEL | 8 | Toggles item selection on each click without requiring SHIFT or CTRL modifier keys. |
| LBS_NODATA | 0x2000 | Specifies a virtual list box |
| LBS_NOINTEGRALHEIGHT | 0x100 | Prevents the list box from automatically resizing to avoid showing partial items. |
| LBS_NOREDRAW | 4 | Prevents updating the list box visual representation when changes are made. |
| LBS_NOSEL | 0x4000 | Specifies that items in the list box can be viewed but not selected. |
| LBS_NOTIFY | 1 | Sends notification messages to the parent window whenever the user clicks or double-clicks an item. |
| LBS_OWNERDRAWFIXED | 0x10 | Specifies an owner-drawn list box where all items have the same uniform height. |
| LBS_OWNERDRAWVARIABLE | 0x20 | Specifies an owner-drawn list box where individual items can vary in height. |
| LBS_SORT | 2 | Alphabetically sorts items added to the list box. |
| LBS_STANDARD | 0xa00003 | Combines LBS_NOTIFY |
| LBS_USETABSTOPS | 0x80 | Enables the list box to recognize and expand tab characters within item strings. |
| LBS_WANTKEYBOARDINPUT | 0x400 | Sends WM_VKEYTOITEM messages to the parent window whenever a key is pressed while the list box has focus. |

## CBS_* Combo Box Styles

| Constant | Value | Description |
|----------|-------|-------------|
| CBS_AUTOHSCROLL | 0x40 | Automatically scrolls text in the combo box edit control horizontally when typed past the right edge. |
| CBS_DISABLENOSCROLL | 0x800 | Shows a disabled scroll bar in the drop-down list when it does not contain enough items to scroll. |
| CBS_DROPDOWN | 2 | Displays a drop-down list with an editable text field. |
| CBS_DROPDOWNLIST | 3 | Displays a drop-down list where the user can only select existing items (edit box is read-only). |
| CBS_HASSTRINGS | 0x200 | Specifies that an owner-drawn combo box contains items consisting of strings. |
| CBS_LOWERCASE | 0x4000 | Converts all entered text in the combo box edit field to lowercase. |
| CBS_NOINTEGRALHEIGHT | 0x400 | Prevents the drop-down list from resizing automatically to show only complete items. |
| CBS_OEMCONVERT | 0x80 | Converts character sets from ANSI to OEM for text entered into the combo box edit field. |
| CBS_OWNERDRAWFIXED | 0x10 | Delegates drop-down item drawing to the parent window |
| CBS_OWNERDRAWVARIABLE | 0x20 | Delegates item drawing to the parent window |
| CBS_SIMPLE | 1 | Displays the list box at all times underneath the edit control. |
| CBS_SORT | 0x100 | Alphabetically sorts items added to the combo box list. |
| CBS_UPPERCASE | 0x2000 | Converts all entered text in the combo box edit field to uppercase. |

## SBS_* Scroll Bar Styles

| Constant | Value | Description |
|----------|-------|-------------|
| SBS_BOTTOMALIGN | 4 | Aligns the scroll bar along the bottom edge of the defined rectangle. |
| SBS_HORZ | 0 | Specifies a horizontal scroll bar. |
| SBS_LEFTALIGN | 2 | Aligns the scroll bar along the left edge of the defined rectangle. |
| SBS_RIGHTALIGN | 4 | Aligns the scroll bar along the right edge of the defined rectangle. |
| SBS_SIZEBOX | 8 | Specifies a sizing box control instead of a scroll bar. |
| SBS_SIZEBOXBOTTOMRIGHTALIGN | 4 | Aligns the size box with the bottom-right corner of the window. |
| SBS_SIZEBOXTOPLEFTALIGN | 2 | Aligns the size box with the top-left corner of the window. |
| SBS_SIZEGRIP | 0x10 | Displays a sizing box featuring a visible grip pattern. |
| SBS_TOPALIGN | 2 | Aligns the scroll bar along the top edge of the defined rectangle. |
| SBS_VERT | 1 | Specifies a vertical scroll bar. |

## WM_* Window Messages

| Constant | Value | Description |
|----------|-------|-------------|
| WM_NULL | 0 | Performs no operation. Used as a placeholder or to test message routing. |
| WM_CREATE | 1 | Sent when an application requests that a window be created by calling CreateWindowEx. |
| WM_DESTROY | 2 | Sent when a window is being destroyed |
| WM_MOVE | 3 | Sent after a window has been moved. |
| WM_SIZE | 5 | Sent to a window after its size has changed. |
| WM_ACTIVATE | 6 | Sent to both the window being activated and the window being deactivated. |
| WM_SETFOCUS | 7 | Sent to a window after it has gained keyboard focus. |
| WM_KILLFOCUS | 8 | Sent to a window immediately before it loses keyboard focus. |
| WM_ENABLE | 10 | Sent when an application changes the enabled state of a window. |
| WM_SETREDRAW | 11 | Sent to a window to allow or prevent changes in that window from being redrawn. |
| WM_SETTEXT | 12 | Sets the text of a window (title bar or control text). |
| WM_GETTEXT | 13 | Copies the text corresponding to a window into a caller-supplied buffer. |
| WM_GETTEXTLENGTH | 14 | Determines the length |
| WM_PAINT | 15 | Sent when the system or another application makes a request to paint a portion of an application's window. |
| WM_CLOSE | 0x10 | Sent as a signal that a window or an application should terminate. |
| WM_QUERYENDSESSION | 0x11 | Sent when the user chooses to end the session or when an application calls one of the shutdown functions. |
| WM_QUIT | 0x12 | Indicates a request to terminate an application and is generated when PostQuitMessage is called. |
| WM_ERASEBKGND | 0x14 | Sent when the window background must be erased (for example |
| WM_SHOWWINDOW | 0x18 | Sent to a window when it is about to be hidden or shown. |
| WM_ACTIVATEAPP | 0x1c | Sent when a window belonging to a different application than the active window is about to be activated. |
| WM_CANCELMODE | 0x1f | Sent to cancel certain modes |
| WM_SETCURSOR | 0x20 | Sent to a window if the mouse cursor moves within a window and mouse input is not captured. |
| WM_MOUSEACTIVATE | 0x21 | Sent when the cursor is in an inactive window and the user presses a mouse button. |
| WM_CHILDACTIVATE | 0x22 | Sent to a child window when the user clicks its title bar or when the window is created |
| WM_GETMINMAXINFO | 0x24 | Sent to a window when the size or position is about to change |
| WM_SETFONT | 0x30 | Sets the font that a control is to use when drawing text. |
| WM_GETFONT | 0x31 | Retrieves the font with which a control is currently drawing its text. |
| WM_SETHOTKEY | 0x32 | Associates a hot key with a window. |
| WM_GETHOTKEY | 0x33 | Determines the hot key associated with a window. |
| WM_QUERYDRAGICON | 0x37 | Sent to a minimized window that does not have an icon defined for its class so the system can display a cursor while dragging. |
| WM_COMPAREITEM | 0x39 | Sent to determine the relative position of a new item in a sorted owner-drawn list box or combo box. |
| WM_WINDOWPOSCHANGING | 0x46 | Sent to a window whose size |
| WM_WINDOWPOSCHANGED | 0x47 | Sent to a window whose size |
| WM_COPYDATA | 0x4a | Sends data from one application to another using inter-process communication. |
| WM_NOTIFY | 0x4e | Sent by a common control to its parent window when an event has occurred or the control requires information. |
| WM_CONTEXTMENU | 0x7b | Notifies a window that the user clicked the right mouse button (context menu request) in the window. |
| WM_STYLECHANGING | 0x7c | Sent to a window when the SetWindowLong function is about to change one or more of the window's styles. |
| WM_STYLECHANGED | 0x7d | Sent to a window after SetWindowLong has changed one or more of the window's styles. |
| WM_DISPLAYCHANGE | 0x7e | Sent to all windows when the display resolution has changed. |
| WM_GETICON | 0x7f | Sent to a window to retrieve a handle to the large or small icon associated with the window. |
| WM_SETICON | 0x80 | Associates a new large or small icon with a window. |
| WM_NCCREATE | 0x81 | Sent prior to WM_CREATE when a window is first created to initialize non-client area elements. |
| WM_NCDESTROY | 0x82 | Notifies a window that its non-client area is being destroyed; this is the final message received by a window. |
| WM_NCCALCSIZE | 0x83 | Sent when the size and position of a window's client area must be calculated. |
| WM_NCHITTEST | 0x84 | Sent to a window in order to determine what part of the window corresponds to a particular screen coordinate. |
| WM_NCPAINT | 0x85 | Sent to a window when its frame (non-client area) must be painted. |
| WM_NCACTIVATE | 0x86 | Sent to a window when its non-client area needs to be changed to indicate an active or inactive state. |
| WM_GETDLGCODE | 0x87 | Sent to the window procedure associated with a control to allow custom processing of direction and TAB keys. |
| WM_KEYDOWN | 0x100 | Posted to the window with the keyboard focus when a non-system key is pressed. |
| WM_KEYUP | 0x101 | Posted to the window with the keyboard focus when a non-system key is released. |
| WM_CHAR | 0x102 | Posted to the window with the keyboard focus when a WM_KEYDOWN message is translated by TranslateMessage. |
| WM_DEADCHAR | 0x103 | Posted when a WM_KEYUP message is translated by TranslateMessage for dead keys (such as accent marks). |
| WM_SYSKEYDOWN | 0x104 | Posted when the user presses the ALT key alongside another key or types when no window has focus. |
| WM_SYSKEYUP | 0x105 | Posted when the user releases a key that was pressed while the ALT key was held down. |
| WM_SYSCHAR | 0x106 | Posted when a WM_SYSKEYDOWN message is translated by the TranslateMessage function. |
| WM_SYSDEADCHAR | 0x107 | Sent when a WM_SYSKEYDOWN message is translated for a character that represents a dead key. |
| WM_INITDIALOG | 0x110 | Sent to the dialog box procedure immediately before a dialog box is displayed. |
| WM_COMMAND | 0x111 | Sent when the user selects a command item from a menu |
| WM_SYSCOMMAND | 0x112 | Sent when the user selects a command from the Window menu or clicks the maximize |
| WM_TIMER | 0x113 | Posted to the installing thread's message queue when a timer set by SetTimer expires. |
| WM_HSCROLL | 0x114 | Sent to a window when an event occurs in the window's standard horizontal scroll bar or horizontal scroll control. |
| WM_VSCROLL | 0x115 | Sent to a window when an event occurs in the window's standard vertical scroll bar or vertical scroll control. |
| WM_INITMENU | 0x116 | Sent when a menu is about to become active |
| WM_INITMENUPOPUP | 0x117 | Sent when a pop-up menu or submenu is about to become active |
| WM_MENUSELECT | 0x11f | Sent to a menu's owner window when the user selects a menu item. |
| WM_MENUCHAR | 0x120 | Sent when a menu is active and the user presses a key that does not match any predefined mnemonic or accelerator. |
| WM_ENTERIDLE | 0x121 | Sent to an owner window when a modal dialog box or menu enters an idle state. |
| WM_MENURBUTTONUP | 0x122 | Sent when the user releases the right mouse button while the cursor is on a menu item. |
| WM_MENUDRAG | 0x123 | Sent to the owner of a drag-and-drop menu when the user drags a menu item. |
| WM_MENUGETOBJECT | 0x124 | Sent to the owner of a drag-and-drop menu when the mouse cursor enters a menu item or registers a drop target. |
| WM_UNINITMENUPOPUP | 0x125 | Sent when a drop-down menu or submenu has been destroyed. |
| WM_MENUCOMMAND | 0x126 | Sent when the user makes a selection from a menu configured with the MNS_NOTIFYBYPOS flag. |
| WM_CTLCOLORMSGBOX | 0x132 | Sent to the owner window of a message box before drawing to set color characteristics. |
| WM_CTLCOLOREDIT | 0x133 | Sent to the parent window of an edit control when the control is about to be drawn |
| WM_CTLCOLORLISTBOX | 0x134 | Sent to the parent window of a list box before drawing to customize text and background colors. |
| WM_CTLCOLORBTN | 0x135 | Sent to the parent window of a button control before drawing to customize button colors. |
| WM_CTLCOLORDLG | 0x136 | Sent to a dialog box procedure before drawing the dialog background to customize dialog colors. |
| WM_CTLCOLORSCROLLBAR | 0x137 | Sent to the parent window of a scroll bar control before drawing to customize control colors. |
| WM_CTLCOLORSTATIC | 0x138 | Sent to the parent window of a static control when the control is about to be drawn. |
| WM_MOUSEMOVE | 0x200 | Posted to a window when the cursor moves across its client area. |
| WM_LBUTTONDOWN | 0x201 | Posted when the user presses the left mouse button while the cursor is in the client area of a window. |
| WM_LBUTTONUP | 0x202 | Posted when the user releases the left mouse button while the cursor is in the client area of a window. |
| WM_LBUTTONDBLCLK | 0x203 | Posted when the user double-clicks the left mouse button while the cursor is in the client area of a window. |
| WM_RBUTTONDOWN | 0x204 | Posted when the user presses the right mouse button while the cursor is in the client area of a window. |
| WM_RBUTTONUP | 0x205 | Posted when the user releases the right mouse button while the cursor is in the client area of a window. |
| WM_RBUTTONDBLCLK | 0x206 | Posted when the user double-clicks the right mouse button while the cursor is in the client area of a window. |
| WM_MBUTTONDOWN | 0x207 | Posted when the user presses the middle mouse button while the cursor is in the client area of a window. |
| WM_MBUTTONUP | 0x208 | Posted when the user releases the middle mouse button while the cursor is in the client area of a window. |
| WM_MBUTTONDBLCLK | 0x209 | Posted when the user double-clicks the middle mouse button while the cursor is in the client area of a window. |
| WM_MOUSEWHEEL | 0x20a | Sent to the active focus window when the mouse wheel is rotated. |
| WM_PARENTNOTIFY | 0x210 | Sent to the parent window when a child window is created or destroyed |
| WM_ENTERMENULOOP | 0x211 | Notifies an application's main window procedure that a menu modal loop has been entered. |
| WM_EXITMENULOOP | 0x212 | Notifies an application's main window procedure that a menu modal loop has been exited. |
| WM_NEXTMENU | 0x213 | Sent to an application when the user switches between the menu bar and the system menu using arrow keys. |
| WM_SIZING | 0x214 | Sent to a window that the user is currently resizing |
| WM_PRINT | 0x317 | Sent to a window to request that it draw itself in the specified device context (DC) |
| WM_PRINTCLIENT | 0x318 | Sent to a window to request that it draw its client area in the specified device context. |
| WM_USER | 0x400 | Defines the starting threshold boundary for private message IDs reserved for custom window classes. |
| WM_APP | 0x8000 | Defines the starting threshold boundary for message IDs available for application-wide private messaging. |

## VK_* Virtual Key Codes

| Constant | Value | Description |
|----------|-------|-------------|
| VK_LBUTTON | 1 | Virtual key code representing the left mouse button. |
| VK_RBUTTON | 2 | Virtual key code representing the right mouse button. |
| VK_CANCEL | 3 | Virtual key code representing Control-break processing. |
| VK_MBUTTON | 4 | Virtual key code representing the middle mouse button. |
| VK_XBUTTON1 | 5 | Virtual key code representing the first X mouse button (back button on multi-button mice). |
| VK_XBUTTON2 | 6 | Virtual key code representing the second X mouse button (forward button on multi-button mice). |
| VK_BACK | 8 | Virtual key code representing the BACKSPACE key. |
| VK_TAB | 9 | Virtual key code representing the TAB key. |
| VK_CLEAR | 12 | Virtual key code representing the CLEAR key (typically Numpad 5 with Num Lock off). |
| VK_RETURN | 13 | Virtual key code representing the ENTER key. |
| VK_SHIFT | 0x10 | Virtual key code representing the SHIFT key. |
| VK_CONTROL | 0x11 | Virtual key code representing the CTRL key. |
| VK_MENU | 0x12 | Virtual key code representing the ALT key. |
| VK_PAUSE | 0x13 | Virtual key code representing the PAUSE key. |
| VK_CAPITAL | 0x14 | Virtual key code representing the CAPS LOCK key. |
| VK_ESCAPE | 0x1b | Virtual key code representing the ESC key. |
| VK_SPACE | 0x20 | Virtual key code representing the SPACEBAR key. |
| VK_PRIOR | 0x21 | Virtual key code representing the PAGE UP key. |
| VK_NEXT | 0x22 | Virtual key code representing the PAGE DOWN key. |
| VK_END | 0x23 | Virtual key code representing the END key. |
| VK_HOME | 0x24 | Virtual key code representing the HOME key. |
| VK_LEFT | 0x25 | Virtual key code representing the LEFT ARROW key. |
| VK_UP | 0x26 | Virtual key code representing the UP ARROW key. |
| VK_RIGHT | 0x27 | Virtual key code representing the RIGHT ARROW key. |
| VK_DOWN | 0x28 | Virtual key code representing the DOWN ARROW key. |
| VK_SELECT | 0x29 | Virtual key code representing the SELECT key. |
| VK_PRINT | 0x2a | Virtual key code representing the PRINT key. |
| VK_EXECUTE | 0x2b | Virtual key code representing the EXECUTE key. |
| VK_SNAPSHOT | 0x2c | Virtual key code representing the PRINT SCREEN key. |
| VK_INSERT | 0x2d | Virtual key code representing the INS key. |
| VK_DELETE | 0x2e | Virtual key code representing the DEL key. |
| VK_HELP | 0x2f | Virtual key code representing the HELP key. |
| VK_0 | 0x30 | Virtual key code representing the '0' key. |
| VK_1 | 0x31 | Virtual key code representing the '1' key. |
| VK_2 | 0x32 | Virtual key code representing the '2' key. |
| VK_3 | 0x33 | Virtual key code representing the '3' key. |
| VK_4 | 0x34 | Virtual key code representing the '4' key. |
| VK_5 | 0x35 | Virtual key code representing the '5' key. |
| VK_6 | 0x36 | Virtual key code representing the '6' key. |
| VK_7 | 0x37 | Virtual key code representing the '7' key. |
| VK_8 | 0x38 | Virtual key code representing the '8' key. |
| VK_9 | 0x39 | Virtual key code representing the '9' key. |
| VK_A | 0x41 | Virtual key code representing the 'A' key. |
| VK_B | 0x42 | Virtual key code representing the 'B' key. |
| VK_C | 0x43 | Virtual key code representing the 'C' key. |
| VK_D | 0x44 | Virtual key code representing the 'D' key. |
| VK_E | 0x45 | Virtual key code representing the 'E' key. |
| VK_F | 0x46 | Virtual key code representing the 'F' key. |
| VK_G | 0x47 | Virtual key code representing the 'G' key. |
| VK_H | 0x48 | Virtual key code representing the 'H' key. |
| VK_I | 0x49 | Virtual key code representing the 'I' key. |
| VK_J | 0x4a | Virtual key code representing the 'J' key. |
| VK_K | 0x4b | Virtual key code representing the 'K' key. |
| VK_L | 0x4c | Virtual key code representing the 'L' key. |
| VK_M | 0x4d | Virtual key code representing the 'M' key. |
| VK_N | 0x4e | Virtual key code representing the 'N' key. |
| VK_O | 0x4f | Virtual key code representing the 'O' key. |
| VK_P | 0x50 | Virtual key code representing the 'P' key. |
| VK_Q | 0x51 | Virtual key code representing the 'Q' key. |
| VK_R | 0x52 | Virtual key code representing the 'R' key. |
| VK_S | 0x53 | Virtual key code representing the 'S' key. |
| VK_T | 0x54 | Virtual key code representing the 'T' key. |
| VK_U | 0x55 | Virtual key code representing the 'U' key. |
| VK_V | 0x56 | Virtual key code representing the 'V' key. |
| VK_W | 0x57 | Virtual key code representing the 'W' key. |
| VK_X | 0x58 | Virtual key code representing the 'X' key. |
| VK_Y | 0x59 | Virtual key code representing the 'Y' key. |
| VK_Z | 0x5a | Virtual key code representing the 'Z' key. |
| VK_LWIN | 0x5b | Virtual key code representing the Left Windows key. |
| VK_RWIN | 0x5c | Virtual key code representing the Right Windows key. |
| VK_APPS | 0x5d | Virtual key code representing the Applications key (context menu key). |
| VK_SLEEP | 0x5f | Virtual key code representing the Computer Sleep key. |
| VK_NUMPAD0 | 0x60 | Virtual key code representing the '0' key on the numeric keypad. |
| VK_NUMPAD1 | 0x61 | Virtual key code representing the '1' key on the numeric keypad. |
| VK_NUMPAD2 | 0x62 | Virtual key code representing the '2' key on the numeric keypad. |
| VK_NUMPAD3 | 0x63 | Virtual key code representing the '3' key on the numeric keypad. |
| VK_NUMPAD4 | 0x64 | Virtual key code representing the '4' key on the numeric keypad. |
| VK_NUMPAD5 | 0x65 | Virtual key code representing the '5' key on the numeric keypad. |
| VK_NUMPAD6 | 0x66 | Virtual key code representing the '6' key on the numeric keypad. |
| VK_NUMPAD7 | 0x67 | Virtual key code representing the '7' key on the numeric keypad. |
| VK_NUMPAD8 | 0x68 | Virtual key code representing the '8' key on the numeric keypad. |
| VK_NUMPAD9 | 0x69 | Virtual key code representing the '9' key on the numeric keypad. |
| VK_MULTIPLY | 0x6a | Virtual key code representing the Multiply key (*) on the numeric keypad. |
| VK_ADD | 0x6b | Virtual key code representing the Add key (+) on the numeric keypad. |
| VK_SEPARATOR | 0x6c | Virtual key code representing the Separator key. |
| VK_SUBTRACT | 0x6d | Virtual key code representing the Subtract key (-) on the numeric keypad. |
| VK_DECIMAL | 0x6e | Virtual key code representing the Decimal key (.) on the numeric keypad. |
| VK_DIVIDE | 0x6f | Virtual key code representing the Divide key (/) on the numeric keypad. |
| VK_F1 | 0x70 | Virtual key code representing the F1 function key. |
| VK_F2 | 0x71 | Virtual key code representing the F2 function key. |
| VK_F3 | 0x72 | Virtual key code representing the F3 function key. |
| VK_F4 | 0x73 | Virtual key code representing the F4 function key. |
| VK_F5 | 0x74 | Virtual key code representing the F5 function key. |
| VK_F6 | 0x75 | Virtual key code representing the F6 function key. |
| VK_F7 | 0x76 | Virtual key code representing the F7 function key. |
| VK_F8 | 0x77 | Virtual key code representing the F8 function key. |
| VK_F9 | 0x78 | Virtual key code representing the F9 function key. |
| VK_F10 | 0x79 | Virtual key code representing the F10 function key. |
| VK_F11 | 0x7a | Virtual key code representing the F11 function key. |
| VK_F12 | 0x7b | Virtual key code representing the F12 function key. |
| VK_NUMLOCK | 0x90 | Virtual key code representing the NUM LOCK key. |
| VK_SCROLL | 0x91 | Virtual key code representing the SCROLL LOCK key. |
| VK_LSHIFT | 0xa0 | Virtual key code representing the Left SHIFT key. |
| VK_RSHIFT | 0xa1 | Virtual key code representing the Right SHIFT key. |
| VK_LCONTROL | 0xa2 | Virtual key code representing the Left CTRL key. |
| VK_RCONTROL | 0xa3 | Virtual key code representing the Right CTRL key. |
| VK_LMENU | 0xa4 | Virtual key code representing the Left ALT key. |
| VK_RMENU | 0xa5 | Virtual key code representing the Right ALT key. |

## MB_* Message Box Flags

| Constant | Value | Description |
|----------|-------|-------------|
| MB_OK | 0 | Message box style containing one push button: OK. |
| MB_OKCANCEL | 1 | Message box style containing two push buttons: OK and Cancel. |
| MB_ABORTRETRYIGNORE | 2 | Message box style containing three push buttons: Abort |
| MB_YESNOCANCEL | 3 | Message box style containing three push buttons: Yes |
| MB_YESNO | 4 | Message box style containing two push buttons: Yes and No. |
| MB_RETRYCANCEL | 5 | Message box style containing two push buttons: Retry and Cancel. |
| MB_CANCELTRYCONTINUE | 6 | Message box style containing three push buttons: Cancel |
| MB_ICONHAND | 0x10 | Message box flag displaying a stop-sign icon. |
| MB_ICONERROR | 0x10 | Message box flag displaying an error icon (same as MB_ICONHAND). |
| MB_ICONQUESTION | 0x20 | Message box flag displaying a question mark icon. |
| MB_ICONEXCLAMATION | 0x30 | Message box flag displaying an exclamation point icon. |
| MB_ICONWARNING | 0x30 | Message box flag displaying a warning icon (same as MB_ICONEXCLAMATION). |
| MB_ICONASTERISK | 0x40 | Message box flag displaying an information icon consisting of a lowercase 'i' inside a circle. |
| MB_ICONINFORMATION | 0x40 | Message box flag displaying an information icon (same as MB_ICONASTERISK). |
| MB_DEFBUTTON1 | 0 | Sets the first button in the message box as the default focused button. |
| MB_DEFBUTTON2 | 0x100 | Sets the second button in the message box as the default focused button. |
| MB_DEFBUTTON3 | 0x200 | Sets the third button in the message box as the default focused button. |
| MB_DEFBUTTON4 | 0x300 | Sets the fourth button in the message box as the default focused button. |
| MB_APPLMODAL | 0 | Makes the message box application-modal; user must respond before continuing work in the current application window. |
| MB_SYSTEMMODAL | 0x1000 | Makes the message box system-modal; forces the message box to remain top-most on the system screen. |
| MB_TASKMODAL | 0x2000 | Makes the message box task-modal; acts like application modal but targets thread-specific top-level windows. |
| MB_HELP | 0x4000 | Adds a Help button to the message box |
| MB_SETFOREGROUND | 0x10000 | Forces the message box window to become the foreground window upon display. |
| MB_DEFAULT_DESKTOP_ONLY | 0x20000 | Constrains message box creation strictly to the default active desktop layout. |
| MB_TOPMOST | 0x40000 | Specifies that the message box window is created with the WS_EX_TOPMOST extended style. |
| MB_RIGHT | 0x80000 | Right-aligns the message box text. |
| MB_RTLREADING | 0x100000 | Displays message box text in Right-to-Left reading order for RTL language systems. |

## MF_*/MFT_*/MFS_* Menu Flags

| Constant | Value | Description |
|----------|-------|-------------|
| MF_STRING | 0 | Specifies that a menu item uses a text string as its content. |
| MF_GRAYED | 1 | Disables and grays out a menu item so it cannot be selected. |
| MF_DISABLED | 2 | Disables a menu item so that it cannot be selected |
| MF_CHECKED | 8 | Places a check mark next to a menu item. |
| MF_POPUP | 0x10 | Specifies that a menu item opens a drop-down menu or submenu. |
| MF_MENUBARBREAK | 0x20 | Places the item on a new line (for menu bars) or in a new column (for pop-up menus) with a vertical dividing line. |
| MF_MENUBREAK | 0x40 | Places the item on a new line (for menu bars) or in a new column (for pop-up menus) without a dividing line. |
| MF_END | 0x80 | Indicates the final menu item in a menu bar or pop-up menu resource template (historical flag). |
| MF_OWNERDRAW | 0x100 | Specifies that the item is an owner-drawn menu item. |
| MF_SEPARATOR | 0x800 | Draws a horizontal dividing line in a pop-up menu or submenu. |
| MF_BYCOMMAND | 0 | Indicates that menu manipulation functions identify menu items by command ID (default behavior). |
| MF_BYPOSITION | 0x400 | Indicates that menu manipulation functions identify menu items by zero-based relative position. |
| MF_DEFAULT | 0x1000 | Highlights the menu item as the default selection (typically displayed in bold text). |
| MF_SYSMENU | 0x2000 | Indicates that the specified menu is the window system menu. |
| MF_HELP | 0x4000 | Aligns a menu item to the right side of the menu bar (historically reserved for Help items). |
| MF_RIGHTJUSTIFY | 0x4000 | Right-justifies a menu item or submenu on the main menu bar. |
| MF_MOUSESELECT | 0x8000 | Flag indicating that a menu item was selected using mouse interaction. |
| MFT_STRING | 0 | Modern menu flag specifying that the menu item displays a text string (replaces MF_STRING). |
| MFT_BITMAP | 4 | Modern menu flag specifying that the menu item displays a bitmap (replaces MF_BITMAP). |
| MFT_MENUBARBREAK | 0x20 | Modern menu flag placing the item in a new column separated by a line (replaces MF_MENUBARBREAK). |
| MFT_MENUBREAK | 0x40 | Modern menu flag placing the item in a new column without a line (replaces MF_MENUBREAK). |
| MFT_OWNERDRAW | 0x100 | Modern menu flag assigning owner-drawn responsibilities to the item (replaces MF_OWNERDRAW). |
| MFT_RADIOCHECK | 0x200 | Displays a radio-button bullet next to the checked menu item instead of a standard check mark. |
| MFT_SEPARATOR | 0x800 | Modern menu flag creating a horizontal line separator inside a pop-up menu. |
| MFT_RIGHTORDER | 0x2000 | Specifies that menu items are rendered in Right-to-Left order for cascading menus. |
| MFT_RIGHTJUSTIFY | 0x4000 | Right-justifies the menu item and all subsequent items on the menu bar. |
| MFS_ENABLED | 0 | Menu state flag indicating the menu item is enabled and selectable. |
| MFS_DISABLED | 3 | Menu state flag indicating the menu item is disabled. |
| MFS_GRAYED | 3 | Menu state flag indicating the menu item is disabled and visually grayed out. |
| MFS_CHECKED | 8 | Menu state flag indicating the menu item has a check mark or radio mark. |
| MFS_UNCHECKED | 0 | Menu state flag indicating the menu item is not checked. |
| MFS_HILITE | 0x80 | Menu state flag indicating the menu item is currently highlighted/selected. |
| MFS_UNHILITE | 0 | Menu state flag indicating the menu item highlight state is removed. |
| MFS_DEFAULT | 0x1000 | Menu state flag setting the menu item as the default bold action item. |

## F_* Accelerator Key Flags

| Constant | Value | Description |
|----------|-------|-------------|
| FVIRTKEY | 1 | Indicates that the accelerator key structure code is a virtual key code rather than an ASCII character. |
| FNOINVERT | 2 | Prevents top-level menu item highlighting when an accelerator key combination is triggered. |
| FSHIFT | 4 | Specifies that the SHIFT key must be held down to activate the menu accelerator entry. |
| FCONTROL | 8 | Specifies that the CTRL key must be held down to activate the menu accelerator entry. |
| FALT | 0x10 | Specifies that the ALT key must be held down to activate the menu accelerator entry. |
| NOINVERT | 2 | Flag preventing menu item highlight state flipping during shortcut processing (same as FNOINVERT). |

## RT_* Resource Types

| Constant | Value | Description |
|----------|-------|-------------|
| RT_CURSOR | 1 | Predefined resource type representing a hardware mouse cursor resource. |
| RT_BITMAP | 2 | Predefined resource type representing a bitmap graphics resource. |
| RT_ICON | 3 | Predefined resource type representing an application icon resource. |
| RT_MENU | 4 |  |
| RT_DIALOG | 5 |  |
| RT_STRING | 6 |  |
| RT_FONTDIR | 7 |  |
| RT_FONT | 8 |  |
| RT_ACCELERATOR | 9 |  |
| RT_RCDATA | 10 |  |
| RT_MESSAGETABLE | 11 |  |
| RT_GROUP_CURSOR | 12 |  |
| RT_GROUP_ICON | 14 |  |
| RT_VERSION | 0x10 |  |
| RT_DLGINCLUDE | 0x11 |  |
| RT_PLUGPLAY | 0x13 |  |
| RT_VXD | 0x14 |  |
| RT_ANICURSOR | 0x15 |  |
| RT_ANIICON | 0x16 |  |
| RT_HTML | 0x17 |  |
| RT_MANIFEST | 0x18 |  |

## Dialog Button Identifiers

| Constant | Value | Description |
|----------|-------|-------------|
| IDOK | 1 |  |
| IDCANCEL | 2 |  |
| IDABORT | 3 |  |
| IDRETRY | 4 |  |
| IDIGNORE | 5 |  |
| IDYES | 6 |  |
| IDNO | 7 |  |
| IDCLOSE | 8 |  |
| IDHELP | 9 |  |
| IDTRYAGAIN | 10 |  |
| IDCONTINUE | 11 |  |
| IDTIMEOUT | 0x7d00 |  |

## System Control Identifiers

| Constant | Value | Description |
|----------|-------|-------------|
| IDC_STATIC | (-1) |  |

## AFX_IDC_* Control Identifiers

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDC_BROWSE | 0x4b3 | Control ID for file browse button |
| AFX_IDC_CHANGE | 0x65 | Control ID for Change button |
| AFX_IDC_CLEAR | 0x4b4 | Control ID for Clear button |
| AFX_IDC_COLORPROP | 0x45c | Control ID for Color property page control |
| AFX_IDC_COLOR_BLACK | 0x44c | Control ID for Black color selection box |
| AFX_IDC_COLOR_BLUE | 0x450 | Control ID for Blue color selection box |
| AFX_IDC_COLOR_CYAN | 0x453 | Control ID for Cyan color selection box |
| AFX_IDC_COLOR_DARKBLUE | 0x458 | Control ID for Dark Blue color selection box |
| AFX_IDC_COLOR_DARKCYAN | 0x45b | Control ID for Dark Cyan color selection box |
| AFX_IDC_COLOR_DARKGREEN | 0x457 | Control ID for Dark Green color selection box |
| AFX_IDC_COLOR_DARKMAGENTA | 0x45a | Control ID for Dark Magenta color selection box |
| AFX_IDC_COLOR_DARKRED | 0x456 | Control ID for Dark Red color selection box |
| AFX_IDC_COLOR_GRAY | 0x454 | Control ID for Gray color selection box |
| AFX_IDC_COLOR_GREEN | 0x44f | Control ID for Green color selection box |
| AFX_IDC_COLOR_LIGHTBROWN | 0x459 | Control ID for Light Brown color selection box |
| AFX_IDC_COLOR_LIGHTGRAY | 0x455 | Control ID for Light Gray color selection box |
| AFX_IDC_COLOR_MAGENTA | 0x452 | Control ID for Magenta color selection box |
| AFX_IDC_COLOR_RED | 0x44e | Control ID for Red color selection box |
| AFX_IDC_COLOR_WHITE | 0x44d | Control ID for White color selection box |
| AFX_IDC_COLOR_YELLOW | 0x451 | Control ID for Yellow color selection box |
| AFX_IDC_CONTEXTHELP | 0x7901 | Cursor ID for Context Help cursor |
| AFX_IDC_FONTNAMES | 0x3e9 | Control ID for Font Name list |
| AFX_IDC_FONTPROP | 0x3e8 | Control ID for Font property page control |
| AFX_IDC_FONTSIZES | 0x3eb | Control ID for Font Size list |
| AFX_IDC_FONTSTYLES | 0x3ea | Control ID for Font Style list |
| AFX_IDC_HSPLITBAR | 0x7904 | Cursor ID for Horizontal Splitter bar |
| AFX_IDC_LISTBOX | 0x64 | Control ID for standard List Box control |
| AFX_IDC_MAGNIFY | 0x7902 | Cursor ID for Magnifier tool cursor |
| AFX_IDC_MOVE4WAY | 0x790c | Cursor ID for 4-way move operation |
| AFX_IDC_NODROPCRSR | 0x7906 | Cursor ID for No Drop drop-target indicator |
| AFX_IDC_PICTURE | 0x4b2 | Control ID for Picture property control |
| AFX_IDC_PRINT_DOCNAME | 0xc9 | Control ID for Document Name static text in Print dialog |
| AFX_IDC_PRINT_PAGENUM | 0xcc | Control ID for Page Number static text in Print dialog |
| AFX_IDC_PRINT_PORTNAME | 0xcb | Control ID for Port Name static text in Print dialog |
| AFX_IDC_PRINT_PRINTERNAME | 0xca | Control ID for Printer Name static text in Print dialog |
| AFX_IDC_PROPNAME | 0x4b1 | Control ID for Property Name static text |
| AFX_IDC_SAMPLEBOX | 0x3ee | Control ID for Font Sample display box |
| AFX_IDC_SMALLARROWS | 0x7903 | Cursor ID for small directional adjustment arrows |
| AFX_IDC_STRIKEOUT | 0x3ec | Control ID for Strikeout checkbox |
| AFX_IDC_SYSTEMCOLORS | 0x45d | Control ID for System Colors list |
| AFX_IDC_TAB_CONTROL | 0x3020 | Control ID for Tab Control in property sheets |
| AFX_IDC_TRACK4WAY | 0x790b | Cursor ID for 4-way object tracking |
| AFX_IDC_TRACKNESW | 0x7908 | Cursor ID for Northeast-Southwest tracking |
| AFX_IDC_TRACKNS | 0x7909 | Cursor ID for North-South tracking |
| AFX_IDC_TRACKNWSE | 0x7907 | Cursor ID for Northwest-Southeast tracking |
| AFX_IDC_TRACKWE | 0x790a | Cursor ID for West-East tracking |
| AFX_IDC_UNDERLINE | 0x3ed | Control ID for Underline checkbox |
| AFX_IDC_VSPLITBAR | 0x7905 | Cursor ID for Vertical Splitter bar |

## AFX_IDD_* Dialog Template IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDD_BUSY | 0x780a | Dialog Template ID for OLE Busy dialog |
| AFX_IDD_CHANGEICON | 0x7805 | Dialog Template ID for OLE Change Icon dialog |
| AFX_IDD_CHANGESOURCE | 0x780d | Dialog Template ID for OLE Change Source dialog |
| AFX_IDD_CONVERT | 0x7806 | Dialog Template ID for OLE Convert Object dialog |
| AFX_IDD_EDITLINKS | 0x7808 | Dialog Template ID for OLE Edit Links dialog |
| AFX_IDD_FILEBROWSE | 0x7809 | Dialog Template ID for File Browse dialog |
| AFX_IDD_INSERTOBJECT | 0x7804 | Dialog Template ID for OLE Insert Object dialog |
| AFX_IDD_NEWTYPEDLG | 0x7801 | Dialog Template ID for New File Type Selection dialog |
| AFX_IDD_OBJECTPROPERTIES | 0x780c | Dialog Template ID for OLE Object Properties dialog |
| AFX_IDD_PASTESPECIAL | 0x7807 | Dialog Template ID for OLE Paste Special dialog |
| AFX_IDD_PREVIEW_TOOLBAR | 0x7803 | Dialog Template ID for Print Preview toolbar |
| AFX_IDD_PRINTDLG | 0x7802 | Dialog Template ID for Print Status dialog |
| IDD_ABOUTBOX | 0x64 | Default resource ID assigned to an application About dialog template |

## AFX_IDS_* String IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDS_ACTIVATE_VERB | 0xf082 | String ID for OLE Activate verb menu label |
| AFX_IDS_ALLFILTER | 0xf002 | String ID for generic wildcard filter string |
| AFX_IDS_ALL_FILES | 0xf087 | String ID for All Files filter string |
| AFX_IDS_AUTO | 0xf084 | String ID for Auto label |
| AFX_IDS_BITMAP_FORMAT | 0xf090 | String ID for Bitmap Clipboard format name |
| AFX_IDS_CHANGE_LINK | 0xf083 | String ID for Change Link action label |
| AFX_IDS_EDIT_VERB | 0xf081 | String ID for OLE Edit verb label |
| AFX_IDS_EMBED_FORMAT | 0xf092 | String ID for Embedded Object format display |
| AFX_IDS_EXIT_MENU | 0xf08c | String ID for Exit command menu label |
| AFX_IDS_FROZEN | 0xf086 | String ID for Frozen state indicator label |
| AFX_IDS_HIDE | 0xf011 | String ID for Hide command label |
| AFX_IDS_LINKSOURCE_FORMAT | 0xf091 | String ID for Link Source clipboard format display |
| AFX_IDS_MANUAL | 0xf085 | String ID for Manual label |
| AFX_IDS_METAFILE_FORMAT | 0xf08e | String ID for Metafile Clipboard format display |
| AFX_IDS_OBJECT_MENUITEM | 0xf080 | String ID for Dynamic OLE Object menu item label |
| AFX_IDS_ONEPAGE | 0xf041 | String ID for One Page status text in Print Preview |
| AFX_IDS_OPENFILE | 0xf000 | String ID for Open File dialog title |
| AFX_IDS_PASTELINKEDTYPE | 0xf094 | String ID for Paste Linked Object type description |
| AFX_IDS_PREVIEW_CLOSE | 0xf005 | String ID for Close Print Preview tooltip label |
| AFX_IDS_PRINTCAPTION | 0xf048 | String ID for Print job progress window caption |
| AFX_IDS_PRINTDEFAULT | 0xf046 | String ID for Default Printer description string |
| AFX_IDS_PRINTDEFAULTEXT | 0xf045 | String ID for Default Print File extension |
| AFX_IDS_PRINTFILTER | 0xf047 | String ID for Print File filter string |
| AFX_IDS_RTF_FORMAT | 0xf096 | Display string describing Rich Text Format (RTF) data |
| AFX_IDS_SAVEFILE | 0xf001 | Prompt or title displayed in dialogs when saving an existing file |
| AFX_IDS_SAVEFILECOPY | 0xf004 | Prompt or title displayed in dialogs when saving a copy of a file |
| AFX_IDS_SAVE_AS_MENU | 0xf08a | Dynamic menu text used for the Save As command |
| AFX_IDS_SAVE_COPY_AS_MENU | 0xf08b | Dynamic menu text used for the Save Copy As command |
| AFX_IDS_SAVE_MENU | 0xf088 | Dynamic menu text used for saving the active document |
| AFX_IDS_TEXT_FORMAT | 0xf097 | Display string describing plain unformatted text clipboard data |
| AFX_IDS_TWOPAGE | 0xf042 | Button or UI string for toggling two-page view in Print Preview |
| AFX_IDS_UNKNOWNTYPE | 0xf095 | Display string indicating an unknown or unrecognized data object |
| AFX_IDS_UNTITLED | 0xf003 | Default Untitled string prefix for new unsaved documents |
| AFX_IDS_UPDATE_MENU | 0xf089 | Dynamic menu text for updating an embedded OLE object |
| AFX_IDS_UPDATING_ITEMS | 0xf08d | Status message displayed while links/embedded objects update |

## AFX_IDB_*/IDB_* Bitmap IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDB_CHECKBOX | 0x7801 | Standard MFC bitmap strip providing checkbox glyph states |
| AFX_IDB_CHECKLISTBOX_95 | 0x7914 | Bitmap ID for CCheckListBox check images (Windows 95) |
| AFX_IDB_CHECKLISTBOX_NT | 0x7913 | Bitmap ID for CCheckListBox check images (Windows NT) |
| AFX_IDB_COLOR_PALETTE | 0x7807 | Stock color palette button icons for MFC color controls |
| AFX_IDB_COLOR_TOOL | 0x7812 | Toolbar bitmap icon displaying color picker/fill tools |
| AFX_IDB_HMENU_ARROW | 0x7f02 | Internal MFC macro for submenu right-pointing expansion arrows |
| AFX_IDB_HMENU_DOT | 0x7f01 | Internal MFC macro for radio item bullet checkmark glyphs in menus |
| AFX_IDB_MENU_IMAGES | 0x780f | Visual glyph strip containing caption-bar icons |
| AFX_IDB_MINIFRAME_MENU | 0x7912 | Bitmap ID for mini-frame window system menu button |
| AFX_IDB_MINI_BTNS | 0x7802 | MFC resource strip containing small window control glyphs for mini frame windows |
| AFX_IDB_MINI_CALENDAR | 0x7803 | MFC navigation arrow and date glyph strip used in date pickers |
| AFX_IDB_POPUP_SUBMENU | 0x7806 | Submenu expansion arrow glyphs used across MFC popup and context menus |
| AFX_IDB_PROPERTY_GRID_BTNS | 0x7804 | Expand/collapse and drop-down button glyphs for CMFCPropertyGridCtrl |
| AFX_IDB_RIBBON_BAR_BTN_DEFAULT | 0x7808 | Generic placeholder image used by CMFCRibbonBar when item image is missing |
| AFX_IDB_RIBBON_PRINT_LARGE | 0x7809 | Print/print preview icon for Ribbon Application Menu |
| AFX_IDB_RIBBON_PRINT_SMALL | 0x780a | Print command icon for Ribbon menus and Quick Access Toolbars |
| AFX_IDB_RIBBON_SLIDER_BTN_MINUS | 0x7814 | Zoom Out button graphic used on Ribbon status bar zoom controls |
| AFX_IDB_RIBBON_SLIDER_BTN_PLUS | 0x7815 | Zoom In button graphic used on Ribbon status bar zoom controls |
| AFX_IDB_SCROLL_DOWN | 0x780e | Downward scroll arrow icon for vertically oriented toolbars |
| AFX_IDB_SCROLL_LEFT | 0x780b | Left scroll arrow icon for tabbed control bars |
| AFX_IDB_SCROLL_RIGHT | 0x780c | Right scroll arrow icon for tabbed control bars |
| AFX_IDB_SCROLL_UP | 0x780d | Upward scroll arrow icon for vertically oriented toolbars |
| AFX_IDB_TASKPANE_CLOSE | 0x7810 | Close button graphic used in task pane headers |
| AFX_IDB_TASKPANE_EXPAND | 0x7811 | Expand/collapse indicator arrows used within CMFCTasksPane |
| AFX_IDB_TASKPANE_SCROLL_BTNS | 0x7805 | Up and down scroll arrow buttons for scrollable task pane containers |
| AFX_IDB_TEAR | 0x7813 | Visual indicator graphic marking detachable tear-off menus and toolbars |
| IDB_HIST_DISABLED | 14 | Explorer travel buttons and favorites bitmaps in disabled state |
| IDB_HIST_HOT | 13 | Explorer travel buttons and favorites bitmaps in hot-tracked state |
| IDB_HIST_LARGE_COLOR | 9 | Explorer-style navigation bitmap with 24x24 color icons |
| IDB_HIST_NORMAL | 12 | Explorer travel buttons and favorites bitmaps in normal state |
| IDB_HIST_PRESSED | 15 | Explorer travel buttons and favorites bitmaps in pressed state |
| IDB_HIST_SMALL_COLOR | 8 | Explorer-style navigation bitmap with 16x16 color icons |
| IDB_STD_LARGE_COLOR | 1 | Standard Common Controls toolbar bitmap with 24x24 color icons |
| IDB_STD_LARGE_MONO | 3 | Standard Common Controls bitmap with 24x24 monochrome icons |
| IDB_STD_SMALL_COLOR | 0 | Standard Common Controls toolbar bitmap with 16x16 color icons |
| IDB_STD_SMALL_MONO | 2 | Standard Common Controls bitmap with 16x16 monochrome icons |
| IDB_VIEW_LARGE_COLOR | 5 | Standard toolbar bitmap with 24x24 color icons for list-view styles |
| IDB_VIEW_LARGE_MONO | 7 | Standard toolbar bitmap with 24x24 monochrome icons for view styles |
| IDB_VIEW_SMALL_COLOR | 4 | Standard toolbar bitmap with 16x16 color icons for list-view styles |
| IDB_VIEW_SMALL_MONO | 6 | Standard toolbar bitmap with 16x16 monochrome icons for view styles |

## AFX_IDI_*/IDI_* Icon IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDI_STD_MDIFRAME | 0x7a01 | Icon ID for standard MDI application frame window |
| AFX_IDI_STD_FRAME | 0x7a02 | Icon ID for standard application frame window |

## AFX_IDP_* Prompt IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDP_ASK_TO_SAVE | 0xf103 | Prompt ID for asking user to save modified document |
| AFX_IDP_COMMAND_FAILURE | 0xf109 | Prompt ID for Command execution failure message |
| AFX_IDP_DLL_BAD_VERSION | 0xf10e | Prompt ID for Incompatible DLL version error |
| AFX_IDP_DLL_LOAD_FAILED | 0xf10d | Prompt ID for DLL loading failure message |
| AFX_IDP_FAILED_MEMORY_ALLOC | 0xf10a | Prompt ID for Memory allocation failure prompt |
| AFX_IDP_FAILED_TO_CREATE_DOC | 0xf104 | Prompt ID for Document creation failure prompt |
| AFX_IDP_FAILED_TO_LAUNCH_HELP | 0xf107 | Prompt ID for Windows Help launch failure prompt |
| AFX_IDP_FAILED_TO_OPEN_DOC | 0xf101 | Prompt ID for Document open failure prompt |
| AFX_IDP_FAILED_TO_SAVE_DOC | 0xf102 | Prompt ID for Document save failure prompt |
| AFX_IDP_FAILED_TO_START_PRINT | 0xf106 | Prompt ID for Print job initialization failure prompt |
| AFX_IDP_FILE_TOO_LARGE | 0xf105 | Prompt ID for File size exceeds maximum limit |
| AFX_IDP_INTERNAL_FAILURE | 0xf108 | Prompt ID for Internal application failure prompt |
| AFX_IDP_INVALID_FILENAME | 0xf100 | Prompt ID for Invalid file name error prompt |
| AFX_IDP_NO_ERROR_AVAILABLE | 0xf020 | Prompt ID for No error description available message |
| AFX_IDP_PARSE_BYTE | 0xf116 | Prompt ID for Data Validation: Byte parse error |
| AFX_IDP_PARSE_CURRENCY | 0xf119 | Prompt ID for Data Validation: Currency parse error |
| AFX_IDP_PARSE_DATETIME | 0xf118 | Prompt ID for Data Validation: Date/Time parse error |
| AFX_IDP_PARSE_INT | 0xf110 | Prompt ID for Data Validation: Integer parse error |
| AFX_IDP_PARSE_INT_RANGE | 0xf112 | Prompt ID for Data Validation: Integer out of range error |
| AFX_IDP_PARSE_RADIO_BUTTON | 0xf115 | Prompt ID for Data Validation: Radio button selection error |
| AFX_IDP_PARSE_REAL | 0xf111 | Prompt ID for Data Validation: Floating point parse error |
| AFX_IDP_PARSE_REAL_RANGE | 0xf113 | Prompt ID for Data Validation: Floating point out of range error |
| AFX_IDP_PARSE_STRING_SIZE | 0xf114 | Prompt ID for Data Validation: String length out of range error |
| AFX_IDP_PARSE_UINT | 0xf117 | Prompt ID for Data Validation: Unsigned integer parse error |
| AFX_IDP_UNREG_DONE | 0xf10b | Prompt ID for Unregistration completed message |
| AFX_IDP_UNREG_FAILURE | 0xf10c | Prompt ID for Unregistration failed message |
| AFX_IDS_RESOURCE_EXCEPTION | 0xf022 | Error message string displayed when a general resource exception occurs |
| AFX_IDS_USER_EXCEPTION | 0xf024 | Standard string prompt for generic user-initiated exceptions |
| AFX_IDP_ARCH_BADCLASS | 0xf1b6 | Archive Error: Class bad or unexpected |
| AFX_IDP_ARCH_BADINDEX | 0xf1b5 | Archive Error: Bad file index |
| AFX_IDP_ARCH_BADSCHEMA | 0xf1b7 | Archive Error: Bad schema version |
| AFX_IDP_ARCH_ENDOFFILE | 0xf1b3 | Archive Error: Unexpected end of file |
| AFX_IDP_ARCH_GENERIC | 0xf1b1 | Generic archive operation error |
| AFX_IDP_ARCH_NONE | 0xf1b0 | Archive Error: No error |
| AFX_IDP_ARCH_READONLY | 0xf1b2 | Archive Error: Attempted write to read-only archive |
| AFX_IDP_ARCH_WRITEONLY | 0xf1b4 | Archive Error: Attempted read from write-only archive |
| AFX_IDP_FILE_ACCESS_DENIED | 0xf1a5 | CFileException: Access denied |
| AFX_IDP_FILE_BAD_PATH | 0xf1a3 | CFileException: Bad path |
| AFX_IDP_FILE_BAD_SEEK | 0xf1a9 | CFileException: Bad seek operation |
| AFX_IDP_FILE_DIR_FULL | 0xf1a8 | CFileException: Directory full |
| AFX_IDP_FILE_DISKFULL | 0xf1ad | CFileException: Disk full |
| AFX_IDP_FILE_EOF | 0xf1ae | CFileException: Unexpected end of file |
| AFX_IDP_FILE_GENERIC | 0xf1a1 | CFileException: Generic file error |
| AFX_IDP_FILE_HARD_IO | 0xf1aa | CFileException: Hardware I/O error |
| AFX_IDP_FILE_INVALID_FILE | 0xf1a6 | CFileException: Invalid file handle |
| AFX_IDP_FILE_LOCKING | 0xf1ac | CFileException: File locking violation |
| AFX_IDP_FILE_NONE | 0xf1a0 | CFileException: No error |
| AFX_IDP_FILE_NOT_FOUND | 0xf1a2 | CFileException: File not found |
| AFX_IDP_FILE_REMOVE_CURRENT | 0xf1a7 | CFileException: Cannot remove current directory |
| AFX_IDP_FILE_SHARING | 0xf1ab | CFileException: Sharing violation |
| AFX_IDP_FILE_TOO_MANY_OPEN | 0xf1a4 | CFileException: Too many open files |
| AFX_IDP_SQL_API_CONFORMANCE | 0xf28e | ODBC driver is incompatible with MFC database classes |
| AFX_IDP_SQL_BOOKMARKS_NOT_ENABLED | 0xf29c | Bookmarks are not enabled on the recordset |
| AFX_IDP_SQL_BOOKMARKS_NOT_SUPPORTED | 0xf29b | Bookmarks are not supported by the underlying ODBC driver |
| AFX_IDP_SQL_CONNECT_FAIL | 0xf281 | Attempt to connect to the SQL data source failed |
| AFX_IDP_SQL_DATA_TRUNCATED | 0xf295 | Data returned from the SQL operation was truncated |
| AFX_IDP_SQL_DYNAMIC_CURSOR_NOT_SUPPORTED | 0xf299 | Dynamic cursors are not supported by the ODBC driver |
| AFX_IDP_SQL_DYNASET_NOT_SUPPORTED | 0xf28c | Dynasets are not supported by the ODBC driver |
| AFX_IDP_SQL_EMPTY_COLUMN_LIST | 0xf283 | Attempted to open a table but no columns were specified |
| AFX_IDP_SQL_FIELD_NOT_FOUND | 0xf29a | The specified field or column name was not found |
| AFX_IDP_SQL_FIELD_SCHEMA_MISMATCH | 0xf284 | Query returned an unexpected column data type |
| AFX_IDP_SQL_ILLEGAL_MODE | 0xf285 | Attempted update or delete in an illegal recordset mode |
| AFX_IDP_SQL_INCORRECT_ODBC | 0xf297 | Incompatible or incorrect ODBC driver installed |
| AFX_IDP_SQL_LOCK_MODE_NOT_SUPPORTED | 0xf294 | The requested locking mode is not supported |
| AFX_IDP_SQL_MULTIPLE_ROWS_AFFECTED | 0xf286 | Multiple rows were unexpectedly updated or deleted |
| AFX_IDP_SQL_NO_CURRENT_RECORD | 0xf287 | Operation failed because there is no current record |
| AFX_IDP_SQL_NO_DATA_FOUND | 0xf290 | No data was returned or found for the operation |
| AFX_IDP_SQL_NO_POSITIONED_UPDATES | 0xf293 | Positioned updates or deletes are not supported |
| AFX_IDP_SQL_NO_ROWS_AFFECTED | 0xf288 | No rows were affected by the update or delete operation |
| AFX_IDP_SQL_ODBC_LOAD_FAILED | 0xf28b | Failed to load the required ODBC driver library |
| AFX_IDP_SQL_ODBC_V2_REQUIRED | 0xf292 | An ODBC 2.0 or higher compliant driver is required |
| AFX_IDP_SQL_RECORDSET_FORWARD_ONLY | 0xf282 | The recordset only supports forward scrolling |
| AFX_IDP_SQL_RECORDSET_READONLY | 0xf289 | The recordset is read-only and cannot be modified |
| AFX_IDP_SQL_ROW_FETCH | 0xf296 | Error encountered while fetching rows from the database |
| AFX_IDP_SQL_ROW_UPDATE_NOT_SUPPORTED | 0xf291 | Row update operations are not supported by the driver |
| AFX_IDP_SQL_SNAPSHOT_NOT_SUPPORTED | 0xf28d | Snapshots require static cursor support from the ODBC driver |
| AFX_IDP_SQL_SQL_CONFORMANCE | 0xf28f | ODBC driver does not meet required SQL conformance levels |
| AFX_IDP_SQL_SQL_NO_TOTAL | 0xf28a | ODBC driver does not support the MFC CLongBinary data model |
| AFX_IDP_SQL_UPDATE_DELETE_FAILED | 0xf298 | The update or delete operation failed |

## OBM_* OEM Bitmap IDs

| Constant | Value | Description |
|----------|-------|-------------|
| OBM_BTNCORNERS | 0x7f1f | Legacy OEM bitmap for rounded push-button corners |
| OBM_BTSIZE | 0x7f1d | OEM window resizing grip bitmap |
| OBM_CHECK | 0x7f20 | OEM checkmark bitmap drawn next to selected menu items |
| OBM_CHECKBOXES | 0x7f1e | OEM system bitmap strip containing default checkbox state graphics |
| OBM_CLOSE | 0x7f2a | OEM title bar close button bitmap |
| OBM_COMBO | 0x7fe2 | OEM bitmap providing drop-down arrow graphic for combo box controls |
| OBM_DNARROW | 0x7f28 | OEM scrollbar down-arrow button bitmap |
| OBM_DNARROWD | 0x7fe1 | OEM bitmap providing pressed state for down-arrows |
| OBM_DNARROWI | 0x7fe8 | Inactive/disabled state down scrollbar arrow glyph |
| OBM_LFARROW | 0x7f26 | OEM scrollbar left-arrow button bitmap |
| OBM_LFARROWD | 0x7fec | Pressed state left scrollbar arrow button |
| OBM_LFARROWI | 0x7fe6 | Inactive/disabled state left scrollbar arrow glyph |
| OBM_MNARROW | 0x7fe3 | OEM bitmap providing right-pointing submenu indicator arrow |
| OBM_OLD_CLOSE | 0x7f18 | Classic OEM close button bitmap for legacy title bars |
| OBM_OLD_DNARROW | 0x7f17 | Classic OEM down-arrow bitmap used in legacy scrollbars |
| OBM_OLD_LFARROW | 0x7f14 | Classic OEM left-arrow bitmap used in 16-bit scrollbars |
| OBM_OLD_REDUCE | 0x7f1a | Classic OEM minimize button bitmap |
| OBM_OLD_RESTORE | 0x7f1b | Classic OEM restore button bitmap |
| OBM_OLD_RGARROW | 0x7f15 | Classic OEM right-arrow bitmap used in legacy scrollbars |
| OBM_OLD_UPARROW | 0x7f16 | Classic OEM up-arrow bitmap used in legacy scrollbars |
| OBM_OLD_ZOOM | 0x7f19 | Classic OEM maximize button bitmap |
| OBM_REDUCE | 0x7f25 | System OEM minimize button bitmap |
| OBM_REDUCED | 0x7ff2 | Pressed state of standard Minimize button |
| OBM_RESTORE | 0x7f23 | System OEM restore button bitmap |
| OBM_RESTORED | 0x7ffa | Pressed state of standard Restore button |
| OBM_RGARROW | 0x7f27 | OEM scrollbar right-arrow button bitmap |
| OBM_RGARROWD | 0x7fed | Pressed state right scrollbar arrow button |
| OBM_RGARROWI | 0x7fe7 | Inactive/disabled state right scrollbar arrow glyph |
| OBM_SIZE | 0x7f1c | OEM sizing frame bitmap asset |
| OBM_UPARROW | 0x7f29 | OEM scrollbar up-arrow button bitmap |
| OBM_UPARROWD | 0x7fe0 | OEM bitmap providing pressed state for up-arrows |
| OBM_UPARROWI | 0x7fe9 | Inactive/disabled state up scrollbar arrow glyph |
| OBM_ZOOM | 0x7f24 | System OEM maximize button bitmap |
| OBM_ZOOMD | 0x7ff1 | Pressed state of standard Maximize button |

## ID_* System Resource IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_ID_PREVIEW_CLOSE | 0xe300 | Command ID for the Print Preview Close button |
| AFX_ID_PREVIEW_NEXT | 0xe302 | Command ID for the Print Preview Next Page button |
| AFX_ID_PREVIEW_NUMPAGE | 0xe301 | Command ID for toggling one/two page view in Print Preview |
| AFX_ID_PREVIEW_PREV | 0xe303 | Command ID for the Print Preview Previous Page button |
| AFX_ID_PREVIEW_PRINT | 0xe304 | Command ID for the Print Preview Print button |
| AFX_ID_PREVIEW_ZOOMIN | 0xe305 | Command ID for zooming in during Print Preview |
| AFX_ID_PREVIEW_ZOOMOUT | 0xe306 | Command ID for zooming out during Print Preview |
| IDP_OLE_INIT_FAILED | 0xe000 | String resource ID for OLE initialization failure prompt message |
| ID_APPLY_NOW | 0x3021 | Applies changes in a property sheet without closing it |
| ID_APP_ABOUT | 0xe140 | Displays the About dialog box for the application |
| ID_APP_EXIT | 0xe141 | Prompts to save documents and exits the application |
| ID_CONTEXT_HELP | 0xe144 | Enters context-sensitive Help mode (Shift+F1) |
| ID_DEFAULT_HELP | 0xe147 | Runs default help topic for the active window or control |
| ID_EDIT_CLEAR | 0xe120 | Erases the selected content |
| ID_EDIT_CLEAR_ALL | 0xe121 | Erases all content in the document or window |
| ID_EDIT_COPY | 0xe122 | Copies the selection and places it on the Clipboard |
| ID_EDIT_CUT | 0xe123 | Cuts the selection and places it on the Clipboard |
| ID_EDIT_FIND | 0xe124 | Opens the Find dialog to search for specified text |
| ID_EDIT_PASTE | 0xe125 | Inserts Clipboard contents at the insertion point |
| ID_EDIT_PASTE_LINK | 0xe126 | Inserts Clipboard contents and creates a link to the source |
| ID_EDIT_PASTE_SPECIAL | 0xe127 | Inserts Clipboard contents with formatting options |
| ID_EDIT_REDO | 0xe12c | Redoes the previously undone action |
| ID_EDIT_REPEAT | 0xe128 | Repeats the last action performed |
| ID_EDIT_REPLACE | 0xe129 | Replaces specific text with different text |
| ID_EDIT_SELECT_ALL | 0xe12a | Selects the entire document or contents |
| ID_EDIT_UNDO | 0xe12b | Undoes the last action performed |
| ID_FILE_CLOSE | 0xe102 | Closes the active document |
| ID_FILE_NEW | 0xe100 | Creates a new document |
| ID_FILE_NEW_FRAME | 0xe10d | Opens a new frame window for an existing document |
| ID_FILE_OPEN | 0xe101 | Opens an existing document |
| ID_FILE_PAGE_SETUP | 0xe105 | Opens the Page Setup dialog box |
| ID_FILE_PRINT | 0xe107 | Prints the active document |
| ID_FILE_PRINT_DIRECT | 0xe108 | Prints the document directly using current printer settings |
| ID_FILE_PRINT_PREVIEW | 0xe109 | Displays full pages on-screen to preview output |
| ID_FILE_PRINT_SETUP | 0xe106 | Opens the Print Setup dialog to choose a printer |
| ID_FILE_SAVE | 0xe103 | Saves the active document under its current name |
| ID_FILE_SAVE_AS | 0xe104 | Saves the active document under a new name |
| ID_FILE_SAVE_COPY_AS | 0xe10b | Saves a copy of the active document under a new name |
| ID_FILE_SEND_MAIL | 0xe10c | Sends the active document via electronic mail |
| ID_FILE_UPDATE | 0xe10a | Updates an embedded container document with object changes |
| ID_FORMAT_FONT | 0xe160 | Selects fonts for current selection or document |
| ID_HELP | 0xe146 | Displays help for the current task or command (F1) |
| ID_HELP_FINDER | 0xe143 | Displays Help contents/index or search prompt |
| ID_HELP_INDEX | 0xe142 | Displays the main index for Help topics |
| ID_HELP_USING | 0xe144 | Displays instructions on how to use Help |
| ID_INDICATOR_CAPS | 0xe701 | Status bar indicator for Caps Lock state |
| ID_INDICATOR_EXT | 0xe700 | Status bar indicator for extended selection mode |
| ID_INDICATOR_KANA | 0xe706 | Status bar indicator for Kana input mode |
| ID_INDICATOR_NUM | 0xe702 | Status bar indicator for Num Lock state |
| ID_INDICATOR_OVR | 0xe704 | Status bar indicator for Overtype mode |
| ID_INDICATOR_REC | 0xe705 | Status bar indicator for macro recording mode |
| ID_INDICATOR_SCRL | 0xe703 | Status bar indicator for Scroll Lock state |
| ID_NEXT_PANE | 0xe150 | Switches focus to the next window pane |
| ID_OLE_EDIT_CHANGE_ICON | 0xe203 | Changes the icon displaying an OLE embedded item |
| ID_OLE_EDIT_CONVERT | 0xe202 | Converts an embedded OLE object to a different type |
| ID_OLE_EDIT_LINKS | 0xe201 | Displays options to edit or update linked OLE objects |
| ID_OLE_EDIT_PROPERTIES | 0xe204 | Displays OLE item properties dialog |
| ID_OLE_INSERT_NEW | 0xe200 | Inserts a new OLE embedded object into the document |
| ID_OLE_VERB_FIRST | 0xe210 | First ID for dynamically populated OLE object verbs |
| ID_PREV_PANE | 0xe151 | Switches focus back to the previous window pane |
| ID_RECORD_LAST | 0xe243 | Moves focus/selection to the last record |
| ID_RECORD_NEXT | 0xe242 | Moves focus/selection to the next record |
| ID_RECORD_PREV | 0xe241 | Moves focus/selection to the previous record |
| ID_REC_FIRST | 0xe240 | Alias for ID_RECORD_FIRST; moves to the first record |
| ID_SEPARATOR | 0 | Used as a visual separator line in menus and toolbars |
| ID_VIEW_AUTOARRANGE | 0xe816 | Automatically arranges icons in a list/grid view |
| ID_VIEW_BYNAME | 0xe815 | Sorts list view items alphabetically by name |
| ID_VIEW_DETAILS | 0xe813 | Displays detailed information about each item |
| ID_VIEW_LARGEICON | 0xe810 | Displays items using large icons |
| ID_VIEW_LINEUP | 0xe814 | Aligns icons to an invisible grid |
| ID_VIEW_LIST | 0xe812 | Displays items as a simple list |
| ID_VIEW_REBAR | 0xe802 | Toggles display of the rebar control bar |
| ID_VIEW_SMALLICON | 0xe811 | Displays items using small icons |
| ID_VIEW_STATUS_BAR | 0xe801 | Toggles display of the application status bar |
| ID_VIEW_TOOLBAR | 0xe800 | Toggles display of the application main toolbar |
| ID_WINDOW_ARRANGE | 0xe131 | Arranges icons at the bottom of an MDI workspace |
| ID_WINDOW_CASCADE | 0xe132 | Arranges open MDI windows in an overlapping cascade |
| ID_WINDOW_NEW | 0xe130 | Opens another window for the active document |
| ID_WINDOW_SPLIT | 0xe135 | Splits the active window into multiple panes |
| ID_WINDOW_TILE_HORIZ | 0xe133 | Tiles open MDI windows horizontally |
| ID_WINDOW_TILE_VERT | 0xe134 | Tiles open MDI windows vertically |
| ID_WIZBACK | 0x3023 | Wizard control button ID for navigating to the previous page |
| ID_WIZFINISH | 0x3025 | Wizard control button ID for completing and closing the wizard |
| ID_WIZNEXT | 0x3024 | Wizard control button ID for navigating to the next page |
| ID_FILE_MRU_FILE1 | 0xe110 | Opens most recently used file #1 |
| ID_FILE_MRU_FILE2 | 0xe111 | Opens most recently used file #2 |
| ID_FILE_MRU_FILE3 | 0xe112 | Opens most recently used file #3 |
| ID_FILE_MRU_FILE4 | 0xe113 | Opens most recently used file #4 |
| ID_FILE_MRU_FILE5 | 0xe114 | Opens most recently used file #5 |
| ID_FILE_MRU_FILE6 | 0xe115 | Opens most recently used file #6 |
| ID_FILE_MRU_FILE7 | 0xe116 | Opens most recently used file #7 |
| ID_FILE_MRU_FILE8 | 0xe117 | Opens most recently used file #8 |
| ID_FILE_MRU_FILE9 | 0xe118 | Opens most recently used file #9 |
| ID_FILE_MRU_FILE10 | 0xe119 | Opens most recently used file #10 |
| ID_FILE_MRU_FILE11 | 0xe11a | Opens most recently used file #11 |
| ID_FILE_MRU_FILE12 | 0xe11b | Opens most recently used file #12 |
| ID_FILE_MRU_FILE13 | 0xe11c | Opens most recently used file #13 |
| ID_FILE_MRU_FILE14 | 0xe11d | Opens most recently used file #14 |
| ID_FILE_MRU_FILE15 | 0xe11e | Opens most recently used file #15 |
| ID_FILE_MRU_FILE16 | 0xe11f | Opens most recently used file #16 |
| ID_FILE_MRU_FIRST | 0xe110 | First ID reserved for the Most Recently Used file list |
| ID_FILE_MRU_LAST | 0xe11f | Last ID reserved for the Most Recently Used file list |

## AFX_IDR_* Accelerator Table IDs

| Constant | Value | Description |
|----------|-------|-------------|
| AFX_IDR_PREVIEW_ACCEL | 0x7915 | Accelerator Table ID for Print Preview mode |

