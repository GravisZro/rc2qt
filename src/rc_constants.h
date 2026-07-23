#ifndef RC_CONSTANTS_H
#define RC_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace rc
{

/* TODO: integrate the information and definitons below into the existing code

== table of more control identifiers ==
AFX_IDC_LISTBOX	100	Property sheet internal list box
AFX_IDC_CHANGE	101	Font/Color dialog change button
AFX_IDC_BROWSE	102	File browse button
AFX_IDC_PRINT_DOCNAME	201	Print preview document title
AFX_IDC_PRINT_PRINTERNAME	202	Print preview printer name
AFX_IDC_PRINT_PORTNAME	203	Print preview port name
AFX_IDC_PRINT_PAGENUM	204	Print preview current page display

== CSV table of descriptions to be added to identifiers ==
Macro Name,Description
WS_BORDER,Creates a window that has a thin-line border.
WS_CAPTION,Creates a window that has a title bar (includes the WS_BORDER style).
WS_CHILD,Creates a child window. A window with this style cannot have a menu bar and cannot be used with the WS_POPUP style.
WS_CLIPCHILDREN,Excludes the area occupied by child windows when drawing occurs within the parent window. Used when creating the parent window.
WS_CLIPSIBLINGS,"Clips child windows relative to each other; that is, when a particular child window receives a WM_PAINT message, this style clips all other overlapping child windows out of the region of the child window to be updated."
WS_DISABLED,Creates a window that is initially disabled. A disabled window cannot receive input from the user.
WS_DLGFRAME,Creates a window that has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar.
WS_GROUP,"Specifies the first control of a group of controls. The group consists of this first control and all controls defined after it, up to the next control with the WS_GROUP style."
WS_HSCROLL,Creates a window that has a horizontal scroll bar.
WS_MAXIMIZE,Creates a window that is initially maximized.
WS_MAXIMIZEBOX,Creates a window that has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU.
WS_MINIMIZE,Creates a window that is initially minimized.
WS_MINIMIZEBOX,Creates a window that has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU.
WS_OVERLAPPED,Creates an overlapped window. An overlapped window has a title bar and a border.
WS_POPUP,Creates a pop-up window. Cannot be used with the WS_CHILD style.
WS_SIZEBOX,Creates a window that has a sizing border (same as WS_THICKFRAME).
WS_SYSMENU,Creates a window that has a window menu on its title bar. Requires WS_CAPTION.
WS_TABSTOP,Specifies a control that can receive the keyboard focus when the user presses the TAB key. Pressing TAB changes the focus to the next control with this style.
WS_VISIBLE,Creates a window that is initially visible.
WS_VSCROLL,Creates a window that has a vertical scroll bar.
WS_THICKFRAME,Creates a window that has a sizing border (same as WS_SIZEBOX).
WS_OVERLAPPEDWINDOW,"Creates an overlapped window with WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, and WS_MAXIMIZEBOX styles."
WS_POPUPWINDOW,"Creates a pop-up window with WS_POPUP, WS_BORDER, and WS_SYSMENU styles. Requires WS_CAPTION to make the window menu visible."
WS_EX_ACCEPTFILES,Specifies that a window created with this style accepts dropped files.
WS_EX_APPWINDOW,Forces a top-level window onto the taskbar when the window is visible.
WS_EX_CLIENTEDGE,Specifies that a window has a border with a sunken edge.
WS_EX_COMPOSITED,Paints all descendants of a window in bottom-to-top painting order using double-buffering.
WS_EX_CONTEXTHELP,"Includes a question mark in the title bar of the window. When clicked, the cursor changes to a question mark with a pointer for context help."
WS_EX_CONTROLPARENT,Allows the user to navigate among the child windows of the window by using the TAB key.
WS_EX_DLGMODALFRAME,Creates a window that has a double border; can optionally be created with a title bar by specifying WS_CAPTION.
WS_EX_LAYERED,Creates a layered window. Note that this cannot be used if the window has a window class style of CS_OWNDC or CS_CLASSDC.
WS_EX_LAYOUTRTL,Sets the horizontal origin to the right edge. Increasing horizontal values advance to the left.
WS_EX_LEFT,Gives the window left-aligned properties (default layout).
WS_EX_LEFTSCROLLBAR,Places the vertical scroll bar (if present) to the left of the client area.
WS_EX_LTRREADING,Displays window text using left-to-right reading order properties (default).
WS_EX_MDICHILD,Creates a Multiple-Document Interface (MDI) child window.
WS_EX_NOACTIVATE,A top-level window created with this style does not become the active window when the user clicks it.
WS_EX_NOINHERITLAYOUT,Prevents the window layout from being inherited by child windows.
WS_EX_NOPARENTNOTIFY,Specifies that a child window created with this style does not send the WM_PARENTNOTIFY message to its parent when created or destroyed.
WS_EX_PALETTEWINDOW,"Combines WS_EX_WINDOWEDGE, WS_EX_TOOLWINDOW, and WS_EX_TOPMOST styles."
WS_EX_RIGHT,Gives the window right-aligned properties depending on window class.
WS_EX_RIGHTSCROLLBAR,Places the vertical scroll bar to the right of the client area (default).
WS_EX_RTLREADING,Displays window text using Right-to-Left reading order properties.
WS_EX_STATICEDGE,Gives a window a three-dimensional border style intended to be used for items that do not accept user input.
WS_EX_TOOLWINDOW,Creates a tool window intended as a floating toolbar; does not appear on the taskbar or in the ALT+TAB dialog.
WS_EX_TOPMOST,"Specifies that a window created with this style should be placed above all non-topmost windows and remain above them, even when deactivated."
WS_EX_TRANSPARENT,Specifies that a window created with this style should not be painted until siblings beneath it have been painted.
WS_EX_WINDOWEDGE,Specifies that a window has a border with a raised edge.
DS_3DLOOK,Gives dialog box controls a 3D look (historical compatibility style).
DS_ABSALIGN,Indicates that the coordinates of the dialog box are screen coordinates (otherwise client coordinates relative to owner).
DS_CENTER,Centers the dialog box on the display screen or monitor.
DS_CENTERMOUSE,Centers the dialog box under the mouse cursor.
DS_CONTEXTHELP,Includes a Help button in the title bar of the dialog box.
DS_CONTROL,Creates a dialog box that can work as a control child inside another dialog box (like a tab control page).
DS_FIXEDSYS,Causes the dialog box to use the system font with fixed spacing instead of proportional spacing.
DS_LOCALEDIT,"Applies to edit controls in the dialog box, allocating memory from the application's local heap."
DS_MODALFRAME,Creates a dialog box with a modal dialog frame border.
DS_NOFAILCREATE,Causes CreateDialog or DialogBox to succeed even if some child controls cannot be created.
DS_NOIDLEMSG,Suppresses WM_ENTERIDLE messages that the system otherwise sends to the owner while the dialog box is displayed.
DS_SETFONT,Indicates that the dialog template specifies a custom font for rendering text in the dialog box.
DS_SETFOREGROUND,Brings the dialog box to the foreground upon creation.
DS_SYSMODAL,"Creates a system-modal dialog box (historical style, behaves similarly to modal dialogs on modern OS)."
DS_SHELLFONT,Uses the system shell font for rendering text inside the dialog box (requires DS_SETFONT).
BS_3STATE,Creates a button that functions like a checkbox but can be dimmed (indeterminate state) as well as checked or unchecked.
BS_AUTO3STATE,"Same as BS_3STATE, but automatically cycles through three states when selected by the user."
BS_AUTOCHECKBOX,"Same as BS_CHECKBOX, but automatically toggles its check mark state when clicked."
BS_AUTORADIOBUTTON,"Same as BS_RADIOBUTTON, but automatically checks itself and unchecks other radio buttons in the group when clicked."
BS_BITMAP,Specifies that the button displays a bitmap instead of text.
BS_BOTTOM,Places text at the bottom of the button rectangle.
BS_CENTER,Horizontally centers text in the button rectangle.
BS_CHECKBOX,Creates a small square box with adjacent text. Typically used to allow multi-select options.
BS_DEFPUSHBUTTON,Creates a push button with a heavy black border representing the default action when ENTER is pressed.
BS_GROUPBOX,"Creates a rectangle in which other controls can be grouped, with a caption in the upper-left corner."
BS_ICON,Specifies that the button displays an icon instead of text.
BS_LEFT,Left-aligns the text in the button rectangle.
BS_LEFTTEXT,Places the radio button circle or checkbox square on the right side of the control text.
BS_MULTILINE,Wraps the button text onto multiple lines if it is too long to fit on a single line.
BS_NOTIFY,"Enables a button to send BN_KILLFOCUS, BN_SETFOCUS, BN_DBLCLK, and BN_DISABLE notification messages to its parent window."
BS_OWNERDRAW,Creates an owner-drawn button where the parent window is responsible for painting the button's appearance.
BS_PUSHBUTTON,Creates a standard push button that posts a WM_COMMAND message to its owner window when clicked.
BS_PUSHLIKE,Makes a checkbox or radio button look and act like a standard push button.
BS_RADIOBUTTON,"Creates a small circle with adjacent text, used for mutually exclusive options in a group."
BS_RIGHT,Right-aligns the text in the button rectangle.
BS_RIGHTBUTTON,Positions the button box on the right side of the text (same as BS_LEFTTEXT).
BS_TEXT,Specifies that the button displays text (default behavior).
BS_TOP,Places text at the top of the button rectangle.
BS_VCENTER,Vertically centers text in the button rectangle.
BS_FLAT,"Specifies that the button is drawn flat, without a 3D shadow frame."
ES_AUTOHSCROLL,Automatically scrolls edit control text to the right when typing reaches the right border.
ES_AUTOVSCROLL,Automatically scrolls edit control text up when pressing ENTER on the last visible line.
ES_CENTER,Centers text in a single-line or multiline edit control.
ES_LEFT,Left-aligns text in an edit control.
ES_LOWERCASE,Converts all characters entered into the edit control to lowercase.
ES_MULTILINE,Designates a multiline edit control allowing text entry on multiple lines.
ES_NOHIDESEL,Prevents the edit control from hiding the text selection when it loses input focus.
ES_NUMBER,Restricts the edit control input to digits only.
ES_OEMCONVERT,Converts text typed in the edit control from ANSI to OEM character set and back.
ES_PASSWORD,Displays an asterisk or custom mask character for every character typed into the edit control.
ES_READONLY,Prevents the user from typing or editing text in the edit control.
ES_RIGHT,Right-aligns text in a single-line or multiline edit control.
ES_UPPERCASE,Converts all characters entered into the edit control to uppercase.
ES_WANTRETURN,Inserts a carriage return when the user presses ENTER in a multiline edit control instead of submitting the dialog.
SS_BITMAP,Specifies that the static control displays a bitmap resource.
SS_BLACKFRAME,Displays a box drawn with a frame of the same color as window frames (default black).
SS_BLACKRECT,Displays a solid rectangle filled with the color used to draw window frames.
SS_CENTER,Designates a simple control displaying centered text wrapped automatically across lines.
SS_CENTERIMAGE,Centers the image or text vertically and horizontally inside the static control bounding area.
SS_EDITCONTROL,Mimics the text-wrapping and rendering behavior of a standard edit control in a static control.
SS_ENHMETAFILE,Specifies that the static control displays an enhanced metafile.
SS_ETCHEDFRAME,Draws the frame of the static control using the EDGE_ETCHED border style.
SS_ETCHEDHORZ,Draws a horizontal line using the EDGE_ETCHED style.
SS_ETCHEDVERT,Draws a vertical line using the EDGE_ETCHED style.
SS_GRAYFRAME,Displays a box drawn with a frame of the current desktop background color.
SS_GRAYRECT,Displays a solid rectangle filled with the current desktop background color.
SS_ICON,Displays an icon in the static control.
SS_LEFT,Left-aligns text in a static control with word wrapping.
SS_LEFTNOWORDWRAP,Left-aligns text without wrapping long lines; excess text is clipped.
SS_NOPREFIX,Prevents interpreting ampersands (&) as accelerator prefix characters in static control text.
SS_NOTIFY,"Sends mouse click notification messages (STN_CLICKED, etc.) to the parent window when interacted with."
SS_OWNERDRAW,Delegates control painting responsibility to the parent window via WM_DRAWITEM.
SS_REALSIZECONTROL,"Prevents resizing bitmap or icon images inside the static control, preserving original image dimensions."
SS_REALSIZEIMAGE,Clips the image if it is larger than the static control bounds rather than scaling it.
SS_RIGHT,Right-aligns text in a static control with word wrapping.
SS_RIGHTJUST,Specifies that the upper-right corner of the static image/icon remains fixed when resized.
SS_SIMPLE,Displays a single line of left-aligned static text that cannot be wrapped or modified dynamically.
SS_SUNKEN,Draws a sunken 3D border around the static control.
SS_WHITEFRAME,Displays a box drawn with a frame matching the window background color (default white).
SS_WHITERECT,Displays a solid rectangle filled with the window background color.
SS_TYPEMASK,Bitmask used to isolate static control style types.
SS_ENDELLIPSIS,Replaces characters at the end of static text with an ellipsis if they exceed control bounds.
SS_PATHELLIPSIS,Replaces characters in the middle of a file path with an ellipsis to fit inside the static control.
SS_WORDELLIPSIS,Truncates text that extends beyond the control end and inserts an ellipsis after the last visible word.
LBS_DISABLENOSCROLL,Shows a disabled vertical scroll bar in the list box when it contains too few items to scroll.
LBS_EXTENDEDSEL,Allows multiple selection using SHIFT and CTRL key combinations or mouse dragging.
LBS_HASSTRINGS,Specifies that an owner-drawn list box contains items consisting of strings.
LBS_MULTICOLUMN,Specifies a multi-column list box that scrolls horizontally.
LBS_MULTIPLESEL,Toggles item selection on each click without requiring SHIFT or CTRL modifier keys.
LBS_NODATA,"Specifies a virtual list box, omitting memory allocation for item string storage internally."
LBS_NOINTEGRALHEIGHT,Prevents the list box from automatically resizing to avoid showing partial items.
LBS_NOREDRAW,Prevents updating the list box visual representation when changes are made.
LBS_NOSEL,Specifies that items in the list box can be viewed but not selected.
LBS_NOTIFY,Sends notification messages to the parent window whenever the user clicks or double-clicks an item.
LBS_OWNERDRAWFIXED,Specifies an owner-drawn list box where all items have the same uniform height.
LBS_OWNERDRAWVARIABLE,Specifies an owner-drawn list box where individual items can vary in height.
LBS_SORT,Alphabetically sorts items added to the list box.
LBS_STANDARD,"Combines LBS_NOTIFY, LBS_SORT, and WS_VSCROLL for standard list box behavior."
LBS_USETABSTOPS,Enables the list box to recognize and expand tab characters within item strings.
LBS_WANTKEYBOARDINPUT,Sends WM_VKEYTOITEM messages to the parent window whenever a key is pressed while the list box has focus.
CBS_AUTOHSCROLL,Automatically scrolls text in the combo box edit control horizontally when typed past the right edge.
CBS_DISABLENOSCROLL,Shows a disabled scroll bar in the drop-down list when it does not contain enough items to scroll.
CBS_DROPDOWN,Displays a drop-down list with an editable text field.
CBS_DROPDOWNLIST,Displays a drop-down list where the user can only select existing items (edit box is read-only).
CBS_HASSTRINGS,Specifies that an owner-drawn combo box contains items consisting of strings.
CBS_LOWERCASE,Converts all entered text in the combo box edit field to lowercase.
CBS_NOINTEGRALHEIGHT,Prevents the drop-down list from resizing automatically to show only complete items.
CBS_OEMCONVERT,Converts character sets from ANSI to OEM for text entered into the combo box edit field.
CBS_OWNERDRAWFIXED,"Delegates drop-down item drawing to the parent window, with every item fixed to the same height."
CBS_OWNERDRAWVARIABLE,"Delegates item drawing to the parent window, allowing variable height for items in the drop-down list."
CBS_SIMPLE,Displays the list box at all times underneath the edit control.
CBS_SORT,Alphabetically sorts items added to the combo box list.
CBS_UPPERCASE,Converts all entered text in the combo box edit field to uppercase.
SBS_BOTTOMALIGN,Aligns the scroll bar along the bottom edge of the defined rectangle.
SBS_HORZ,Specifies a horizontal scroll bar.
SBS_LEFTALIGN,Aligns the scroll bar along the left edge of the defined rectangle.
SBS_RIGHTALIGN,Aligns the scroll bar along the right edge of the defined rectangle.
SBS_SIZEBOX,Specifies a sizing box control instead of a scroll bar.
SBS_SIZEBOXBOTTOMRIGHTALIGN,Aligns the size box with the bottom-right corner of the window.
SBS_SIZEBOXTOPLEFTALIGN,Aligns the size box with the top-left corner of the window.
SBS_SIZEGRIP,Displays a sizing box featuring a visible grip pattern.
SBS_TOPALIGN,Aligns the scroll bar along the top edge of the defined rectangle.
SBS_VERT,Specifies a vertical scroll bar.
VK_LBUTTON,Virtual key code representing the left mouse button.
VK_RBUTTON,Virtual key code representing the right mouse button.
VK_CANCEL,Virtual key code representing Control-break processing.
VK_MBUTTON,Virtual key code representing the middle mouse button.
VK_XBUTTON1,Virtual key code representing the first X mouse button (back button on multi-button mice).
VK_XBUTTON2,Virtual key code representing the second X mouse button (forward button on multi-button mice).
VK_BACK,Virtual key code representing the BACKSPACE key.
VK_TAB,Virtual key code representing the TAB key.
VK_CLEAR,Virtual key code representing the CLEAR key (typically Numpad 5 with Num Lock off).
VK_RETURN,Virtual key code representing the ENTER key.
VK_SHIFT,Virtual key code representing the SHIFT key.
VK_CONTROL,Virtual key code representing the CTRL key.
VK_MENU,Virtual key code representing the ALT key.
VK_PAUSE,Virtual key code representing the PAUSE key.
VK_CAPITAL,Virtual key code representing the CAPS LOCK key.
VK_ESCAPE,Virtual key code representing the ESC key.
VK_SPACE,Virtual key code representing the SPACEBAR key.
VK_PRIOR,Virtual key code representing the PAGE UP key.
VK_NEXT,Virtual key code representing the PAGE DOWN key.
VK_END,Virtual key code representing the END key.
VK_HOME,Virtual key code representing the HOME key.
VK_LEFT,Virtual key code representing the LEFT ARROW key.
VK_UP,Virtual key code representing the UP ARROW key.
VK_RIGHT,Virtual key code representing the RIGHT ARROW key.
VK_DOWN,Virtual key code representing the DOWN ARROW key.
VK_SELECT,Virtual key code representing the SELECT key.
VK_PRINT,Virtual key code representing the PRINT key.
VK_EXECUTE,Virtual key code representing the EXECUTE key.
VK_SNAPSHOT,Virtual key code representing the PRINT SCREEN key.
VK_INSERT,Virtual key code representing the INS key.
VK_DELETE,Virtual key code representing the DEL key.
VK_HELP,Virtual key code representing the HELP key.
VK_0,Virtual key code representing the '0' key.
VK_1,Virtual key code representing the '1' key.
VK_2,Virtual key code representing the '2' key.
VK_3,Virtual key code representing the '3' key.
VK_4,Virtual key code representing the '4' key.
VK_5,Virtual key code representing the '5' key.
VK_6,Virtual key code representing the '6' key.
VK_7,Virtual key code representing the '7' key.
VK_8,Virtual key code representing the '8' key.
VK_9,Virtual key code representing the '9' key.
VK_A,Virtual key code representing the 'A' key.
VK_B,Virtual key code representing the 'B' key.
VK_C,Virtual key code representing the 'C' key.
VK_D,Virtual key code representing the 'D' key.
VK_E,Virtual key code representing the 'E' key.
VK_F,Virtual key code representing the 'F' key.
VK_G,Virtual key code representing the 'G' key.
VK_H,Virtual key code representing the 'H' key.
VK_I,Virtual key code representing the 'I' key.
VK_J,Virtual key code representing the 'J' key.
VK_K,Virtual key code representing the 'K' key.
VK_L,Virtual key code representing the 'L' key.
VK_M,Virtual key code representing the 'M' key.
VK_N,Virtual key code representing the 'N' key.
VK_O,Virtual key code representing the 'O' key.
VK_P,Virtual key code representing the 'P' key.
VK_Q,Virtual key code representing the 'Q' key.
VK_R,Virtual key code representing the 'R' key.
VK_S,Virtual key code representing the 'S' key.
VK_T,Virtual key code representing the 'T' key.
VK_U,Virtual key code representing the 'U' key.
VK_V,Virtual key code representing the 'V' key.
VK_W,Virtual key code representing the 'W' key.
VK_X,Virtual key code representing the 'X' key.
VK_Y,Virtual key code representing the 'Y' key.
VK_Z,Virtual key code representing the 'Z' key.
VK_LWIN,Virtual key code representing the Left Windows key.
VK_RWIN,Virtual key code representing the Right Windows key.
VK_APPS,Virtual key code representing the Applications key (context menu key).
VK_SLEEP,Virtual key code representing the Computer Sleep key.
VK_NUMPAD0,Virtual key code representing the '0' key on the numeric keypad.
VK_NUMPAD1,Virtual key code representing the '1' key on the numeric keypad.
VK_NUMPAD2,Virtual key code representing the '2' key on the numeric keypad.
VK_NUMPAD3,Virtual key code representing the '3' key on the numeric keypad.
VK_NUMPAD4,Virtual key code representing the '4' key on the numeric keypad.
VK_NUMPAD5,Virtual key code representing the '5' key on the numeric keypad.
VK_NUMPAD6,Virtual key code representing the '6' key on the numeric keypad.
VK_NUMPAD7,Virtual key code representing the '7' key on the numeric keypad.
VK_NUMPAD8,Virtual key code representing the '8' key on the numeric keypad.
VK_NUMPAD9,Virtual key code representing the '9' key on the numeric keypad.
VK_MULTIPLY,Virtual key code representing the Multiply key (*) on the numeric keypad.
VK_ADD,Virtual key code representing the Add key (+) on the numeric keypad.
VK_SEPARATOR,Virtual key code representing the Separator key.
VK_SUBTRACT,Virtual key code representing the Subtract key (-) on the numeric keypad.
VK_DECIMAL,Virtual key code representing the Decimal key (.) on the numeric keypad.
VK_DIVIDE,Virtual key code representing the Divide key (/) on the numeric keypad.
VK_F1,Virtual key code representing the F1 function key.
VK_F2,Virtual key code representing the F2 function key.
VK_F3,Virtual key code representing the F3 function key.
VK_F4,Virtual key code representing the F4 function key.
VK_F5,Virtual key code representing the F5 function key.
VK_F6,Virtual key code representing the F6 function key.
VK_F7,Virtual key code representing the F7 function key.
VK_F8,Virtual key code representing the F8 function key.
VK_F9,Virtual key code representing the F9 function key.
VK_F10,Virtual key code representing the F10 function key.
VK_F11,Virtual key code representing the F11 function key.
VK_F12,Virtual key code representing the F12 function key.
VK_NUMLOCK,Virtual key code representing the NUM LOCK key.
VK_SCROLL,Virtual key code representing the SCROLL LOCK key.
VK_LSHIFT,Virtual key code representing the Left SHIFT key.
VK_RSHIFT,Virtual key code representing the Right SHIFT key.
VK_LCONTROL,Virtual key code representing the Left CTRL key.
VK_RCONTROL,Virtual key code representing the Right CTRL key.
VK_LMENU,Virtual key code representing the Left ALT key.
VK_RMENU,Virtual key code representing the Right ALT key.
WM_NULL,Performs no operation. Used as a placeholder or to test message routing.
WM_CREATE,Sent when an application requests that a window be created by calling CreateWindowEx.
WM_DESTROY,"Sent when a window is being destroyed, signaling that the window is being removed from the screen."
WM_MOVE,Sent after a window has been moved.
WM_SIZE,Sent to a window after its size has changed.
WM_ACTIVATE,Sent to both the window being activated and the window being deactivated.
WM_SETFOCUS,Sent to a window after it has gained keyboard focus.
WM_KILLFOCUS,Sent to a window immediately before it loses keyboard focus.
WM_ENABLE,Sent when an application changes the enabled state of a window.
WM_SETREDRAW,Sent to a window to allow or prevent changes in that window from being redrawn.
WM_SETTEXT,Sets the text of a window (title bar or control text).
WM_GETTEXT,Copies the text corresponding to a window into a caller-supplied buffer.
WM_GETTEXTLENGTH,"Determines the length, in characters, of the text associated with a window."
WM_PAINT,Sent when the system or another application makes a request to paint a portion of an application's window.
WM_CLOSE,Sent as a signal that a window or an application should terminate.
WM_QUERYENDSESSION,Sent when the user chooses to end the session or when an application calls one of the shutdown functions.
WM_QUIT,Indicates a request to terminate an application and is generated when PostQuitMessage is called.
WM_ERASEBKGND,"Sent when the window background must be erased (for example, when a window is resized)."
WM_SHOWWINDOW,Sent to a window when it is about to be hidden or shown.
WM_ACTIVATEAPP,Sent when a window belonging to a different application than the active window is about to be activated.
WM_CANCELMODE,"Sent to cancel certain modes, such as mouse capture or tracking modal loops."
WM_SETCURSOR,Sent to a window if the mouse cursor moves within a window and mouse input is not captured.
WM_MOUSEACTIVATE,Sent when the cursor is in an inactive window and the user presses a mouse button.
WM_CHILDACTIVATE,"Sent to a child window when the user clicks its title bar or when the window is created, moved, or resized."
WM_GETMINMAXINFO,"Sent to a window when the size or position is about to change, allowing the window to override default minimum/maximum sizes."
WM_SETFONT,Sets the font that a control is to use when drawing text.
WM_GETFONT,Retrieves the font with which a control is currently drawing its text.
WM_SETHOTKEY,Associates a hot key with a window.
WM_GETHOTKEY,Determines the hot key associated with a window.
WM_QUERYDRAGICON,Sent to a minimized window that does not have an icon defined for its class so the system can display a cursor while dragging.
WM_COMPAREITEM,Sent to determine the relative position of a new item in a sorted owner-drawn list box or combo box.
WM_WINDOWPOSCHANGING,"Sent to a window whose size, position, or place in the Z-order is about to change."
WM_WINDOWPOSCHANGED,"Sent to a window whose size, position, or place in the Z-order has changed."
WM_COPYDATA,Sends data from one application to another using inter-process communication.
WM_NOTIFY,Sent by a common control to its parent window when an event has occurred or the control requires information.
WM_CONTEXTMENU,Notifies a window that the user clicked the right mouse button (context menu request) in the window.
WM_STYLECHANGING,Sent to a window when the SetWindowLong function is about to change one or more of the window's styles.
WM_STYLECHANGED,Sent to a window after SetWindowLong has changed one or more of the window's styles.
WM_DISPLAYCHANGE,Sent to all windows when the display resolution has changed.
WM_GETICON,Sent to a window to retrieve a handle to the large or small icon associated with the window.
WM_SETICON,Associates a new large or small icon with a window.
WM_NCCREATE,Sent prior to WM_CREATE when a window is first created to initialize non-client area elements.
WM_NCDESTROY,Notifies a window that its non-client area is being destroyed; this is the final message received by a window.
WM_NCCALCSIZE,Sent when the size and position of a window's client area must be calculated.
WM_NCHITTEST,Sent to a window in order to determine what part of the window corresponds to a particular screen coordinate.
WM_NCPAINT,Sent to a window when its frame (non-client area) must be painted.
WM_NCACTIVATE,Sent to a window when its non-client area needs to be changed to indicate an active or inactive state.
WM_GETDLGCODE,Sent to the window procedure associated with a control to allow custom processing of direction and TAB keys.
WM_KEYDOWN,Posted to the window with the keyboard focus when a non-system key is pressed.
WM_KEYUP,Posted to the window with the keyboard focus when a non-system key is released.
WM_CHAR,Posted to the window with the keyboard focus when a WM_KEYDOWN message is translated by TranslateMessage.
WM_DEADCHAR,Posted when a WM_KEYUP message is translated by TranslateMessage for dead keys (such as accent marks).
WM_SYSKEYDOWN,Posted when the user presses the ALT key alongside another key or types when no window has focus.
WM_SYSKEYUP,Posted when the user releases a key that was pressed while the ALT key was held down.
WM_SYSCHAR,Posted when a WM_SYSKEYDOWN message is translated by the TranslateMessage function.
WM_SYSDEADCHAR,Sent when a WM_SYSKEYDOWN message is translated for a character that represents a dead key.
WM_INITDIALOG,Sent to the dialog box procedure immediately before a dialog box is displayed.
WM_COMMAND,"Sent when the user selects a command item from a menu, when a control sends a notification, or when an accelerator keystroke is translated."
WM_SYSCOMMAND,"Sent when the user selects a command from the Window menu or clicks the maximize, minimize, restore, or close buttons."
WM_TIMER,Posted to the installing thread's message queue when a timer set by SetTimer expires.
WM_HSCROLL,Sent to a window when an event occurs in the window's standard horizontal scroll bar or horizontal scroll control.
WM_VSCROLL,Sent to a window when an event occurs in the window's standard vertical scroll bar or vertical scroll control.
WM_INITMENU,"Sent when a menu is about to become active, allowing the application to modify it before display."
WM_INITMENUPOPUP,"Sent when a pop-up menu or submenu is about to become active, allowing modification prior to rendering."
WM_MENUSELECT,Sent to a menu's owner window when the user selects a menu item.
WM_MENUCHAR,Sent when a menu is active and the user presses a key that does not match any predefined mnemonic or accelerator.
WM_ENTERIDLE,Sent to an owner window when a modal dialog box or menu enters an idle state.
WM_MENURBUTTONUP,Sent when the user releases the right mouse button while the cursor is on a menu item.
WM_MENUDRAG,Sent to the owner of a drag-and-drop menu when the user drags a menu item.
WM_MENUGETOBJECT,Sent to the owner of a drag-and-drop menu when the mouse cursor enters a menu item or registers a drop target.
WM_UNINITMENUPOPUP,Sent when a drop-down menu or submenu has been destroyed.
WM_MENUCOMMAND,Sent when the user makes a selection from a menu configured with the MNS_NOTIFYBYPOS flag.
WM_CTLCOLORMSGBOX,Sent to the owner window of a message box before drawing to set color characteristics.
WM_CTLCOLOREDIT,"Sent to the parent window of an edit control when the control is about to be drawn, allowing custom text and background colors."
WM_CTLCOLORLISTBOX,Sent to the parent window of a list box before drawing to customize text and background colors.
WM_CTLCOLORBTN,Sent to the parent window of a button control before drawing to customize button colors.
WM_CTLCOLORDLG,Sent to a dialog box procedure before drawing the dialog background to customize dialog colors.
WM_CTLCOLORSCROLLBAR,Sent to the parent window of a scroll bar control before drawing to customize control colors.
WM_CTLCOLORSTATIC,Sent to the parent window of a static control when the control is about to be drawn.
WM_MOUSEMOVE,Posted to a window when the cursor moves across its client area.
WM_LBUTTONDOWN,Posted when the user presses the left mouse button while the cursor is in the client area of a window.
WM_LBUTTONUP,Posted when the user releases the left mouse button while the cursor is in the client area of a window.
WM_LBUTTONDBLCLK,Posted when the user double-clicks the left mouse button while the cursor is in the client area of a window.
WM_RBUTTONDOWN,Posted when the user presses the right mouse button while the cursor is in the client area of a window.
WM_RBUTTONUP,Posted when the user releases the right mouse button while the cursor is in the client area of a window.
WM_RBUTTONDBLCLK,Posted when the user double-clicks the right mouse button while the cursor is in the client area of a window.
WM_MBUTTONDOWN,Posted when the user presses the middle mouse button while the cursor is in the client area of a window.
WM_MBUTTONUP,Posted when the user releases the middle mouse button while the cursor is in the client area of a window.
WM_MBUTTONDBLCLK,Posted when the user double-clicks the middle mouse button while the cursor is in the client area of a window.
WM_MOUSEWHEEL,Sent to the active focus window when the mouse wheel is rotated.
WM_PARENTNOTIFY,"Sent to the parent window when a child window is created or destroyed, or when the user clicks a child window."
WM_ENTERMENULOOP,Notifies an application's main window procedure that a menu modal loop has been entered.
WM_EXITMENULOOP,Notifies an application's main window procedure that a menu modal loop has been exited.
WM_NEXTMENU,Sent to an application when the user switches between the menu bar and the system menu using arrow keys.
WM_SIZING,"Sent to a window that the user is currently resizing, allowing adjustment of sizing boundaries."
WM_PRINT,"Sent to a window to request that it draw itself in the specified device context (DC), typically for printing or capturing."
WM_PRINTCLIENT,Sent to a window to request that it draw its client area in the specified device context.
WM_USER,Defines the starting threshold boundary for private message IDs reserved for custom window classes.
WM_APP,Defines the starting threshold boundary for message IDs available for application-wide private messaging.
MB_OK,Message box style containing one push button: OK.
MB_OKCANCEL,Message box style containing two push buttons: OK and Cancel.
MB_ABORTRETRYIGNORE,"Message box style containing three push buttons: Abort, Retry, and Ignore."
MB_YESNOCANCEL,"Message box style containing three push buttons: Yes, No, and Cancel."
MB_YESNO,Message box style containing two push buttons: Yes and No.
MB_RETRYCANCEL,Message box style containing two push buttons: Retry and Cancel.
MB_CANCELTRYCONTINUE,"Message box style containing three push buttons: Cancel, Try Again, and Continue."
MB_ICONHAND,Message box flag displaying a stop-sign icon.
MB_ICONERROR,Message box flag displaying an error icon (same as MB_ICONHAND).
MB_ICONQUESTION,Message box flag displaying a question mark icon.
MB_ICONEXCLAMATION,Message box flag displaying an exclamation point icon.
MB_ICONWARNING,Message box flag displaying a warning icon (same as MB_ICONEXCLAMATION).
MB_ICONASTERISK,Message box flag displaying an information icon consisting of a lowercase 'i' inside a circle.
MB_ICONINFORMATION,Message box flag displaying an information icon (same as MB_ICONASTERISK).
MB_DEFBUTTON1,Sets the first button in the message box as the default focused button.
MB_DEFBUTTON2,Sets the second button in the message box as the default focused button.
MB_DEFBUTTON3,Sets the third button in the message box as the default focused button.
MB_DEFBUTTON4,Sets the fourth button in the message box as the default focused button.
MB_APPLMODAL,Makes the message box application-modal; user must respond before continuing work in the current application window.
MB_SYSTEMMODAL,Makes the message box system-modal; forces the message box to remain top-most on the system screen.
MB_TASKMODAL,Makes the message box task-modal; acts like application modal but targets thread-specific top-level windows.
MB_HELP,"Adds a Help button to the message box, generating a help event when pressed."
MB_SETFOREGROUND,Forces the message box window to become the foreground window upon display.
MB_DEFAULT_DESKTOP_ONLY,Constrains message box creation strictly to the default active desktop layout.
MB_TOPMOST,Specifies that the message box window is created with the WS_EX_TOPMOST extended style.
MB_RIGHT,Right-aligns the message box text.
MB_RTLREADING,Displays message box text in Right-to-Left reading order for RTL language systems.
MF_STRING,Specifies that a menu item uses a text string as its content.
MF_GRAYED,Disables and grays out a menu item so it cannot be selected.
MF_DISABLED,"Disables a menu item so that it cannot be selected, but does not gray it out."
MF_CHECKED,Places a check mark next to a menu item.
MF_POPUP,Specifies that a menu item opens a drop-down menu or submenu.
MF_MENUBARBREAK,Places the item on a new line (for menu bars) or in a new column (for pop-up menus) with a vertical dividing line.
MF_MENUBREAK,Places the item on a new line (for menu bars) or in a new column (for pop-up menus) without a dividing line.
MF_END,Indicates the final menu item in a menu bar or pop-up menu resource template (historical flag).
MF_OWNERDRAW,Specifies that the item is an owner-drawn menu item.
MF_SEPARATOR,Draws a horizontal dividing line in a pop-up menu or submenu.
MF_BYCOMMAND,Indicates that menu manipulation functions identify menu items by command ID (default behavior).
MF_BYPOSITION,Indicates that menu manipulation functions identify menu items by zero-based relative position.
MF_DEFAULT,Highlights the menu item as the default selection (typically displayed in bold text).
MF_SYSMENU,Indicates that the specified menu is the window system menu.
MF_HELP,Aligns a menu item to the right side of the menu bar (historically reserved for Help items).
MF_RIGHTJUSTIFY,Right-justifies a menu item or submenu on the main menu bar.
MF_MOUSESELECT,Flag indicating that a menu item was selected using mouse interaction.
MFT_STRING,Modern menu flag specifying that the menu item displays a text string (replaces MF_STRING).
MFT_BITMAP,Modern menu flag specifying that the menu item displays a bitmap (replaces MF_BITMAP).
MFT_MENUBARBREAK,Modern menu flag placing the item in a new column separated by a line (replaces MF_MENUBARBREAK).
MFT_MENUBREAK,Modern menu flag placing the item in a new column without a line (replaces MF_MENUBREAK).
MFT_OWNERDRAW,Modern menu flag assigning owner-drawn responsibilities to the item (replaces MF_OWNERDRAW).
MFT_RADIOCHECK,Displays a radio-button bullet next to the checked menu item instead of a standard check mark.
MFT_SEPARATOR,Modern menu flag creating a horizontal line separator inside a pop-up menu.
MFT_RIGHTORDER,Specifies that menu items are rendered in Right-to-Left order for cascading menus.
MFT_RIGHTJUSTIFY,Right-justifies the menu item and all subsequent items on the menu bar.
MFS_ENABLED,Menu state flag indicating the menu item is enabled and selectable.
MFS_DISABLED,Menu state flag indicating the menu item is disabled.
MFS_GRAYED,Menu state flag indicating the menu item is disabled and visually grayed out.
MFS_CHECKED,Menu state flag indicating the menu item has a check mark or radio mark.
MFS_UNCHECKED,Menu state flag indicating the menu item is not checked.
MFS_HILITE,Menu state flag indicating the menu item is currently highlighted/selected.
MFS_UNHILITE,Menu state flag indicating the menu item highlight state is removed.
MFS_DEFAULT,Menu state flag setting the menu item as the default bold action item.
FVIRTKEY,Indicates that the accelerator key structure code is a virtual key code rather than an ASCII character.
FNOINVERT,Prevents top-level menu item highlighting when an accelerator key combination is triggered.
FSHIFT,Specifies that the SHIFT key must be held down to activate the menu accelerator entry.
FCONTROL,Specifies that the CTRL key must be held down to activate the menu accelerator entry.
FALT,Specifies that the ALT key must be held down to activate the menu accelerator entry.
NOINVERT,Flag preventing menu item highlight state flipping during shortcut processing (same as FNOINVERT).
RT_CURSOR,Predefined resource type representing a hardware mouse cursor resource.
RT_BITMAP,Predefined resource type representing a bitmap graphics resource.
RT_ICON,Predefined resource type representing an application icon resource.


== CSV table of new identifiers to be added ==

Macro Name,Hex Value,Dec Value,Description
AFX_ID_PREVIEW_CLOSE,0xE300,58112,Command ID for the Print Preview Close button
AFX_ID_PREVIEW_NUMPAGE,0xE301,58113,Command ID for toggling one/two page view in Print Preview
AFX_ID_PREVIEW_NEXT,0xE302,58114,Command ID for the Print Preview Next Page button
AFX_ID_PREVIEW_PREV,0xE303,58115,Command ID for the Print Preview Previous Page button
AFX_ID_PREVIEW_PRINT,0xE304,58116,Command ID for the Print Preview Print button
AFX_ID_PREVIEW_ZOOMIN,0xE305,58117,Command ID for zooming in during Print Preview
AFX_ID_PREVIEW_ZOOMOUT,0xE306,58118,Command ID for zooming out during Print Preview
AFX_IDC_LISTBOX,0x0064,100,Control ID for standard List Box control
AFX_IDC_CHANGE,0x0065,101,Control ID for Change button
AFX_IDC_PRINT_DOCNAME,0x00C9,201,Control ID for Document Name static text in Print dialog
AFX_IDC_PRINT_PRINTERNAME,0x00CA,202,Control ID for Printer Name static text in Print dialog
AFX_IDC_PRINT_PORTNAME,0x00CB,203,Control ID for Port Name static text in Print dialog
AFX_IDC_PRINT_PAGENUM,0x00CC,204,Control ID for Page Number static text in Print dialog
AFX_IDC_FONTPROP,0x03E8,1000,Control ID for Font property page control
AFX_IDC_FONTNAMES,0x03E9,1001,Control ID for Font Name list
AFX_IDC_FONTSTYLES,0x03EA,1002,Control ID for Font Style list
AFX_IDC_FONTSIZES,0x03EB,1003,Control ID for Font Size list
AFX_IDC_STRIKEOUT,0x03EC,1004,Control ID for Strikeout checkbox
AFX_IDC_UNDERLINE,0x03ED,1005,Control ID for Underline checkbox
AFX_IDC_SAMPLEBOX,0x03EE,1006,Control ID for Font Sample display box
AFX_IDC_COLOR_BLACK,0x044C,1100,Control ID for Black color selection box
AFX_IDC_COLOR_WHITE,0x044D,1101,Control ID for White color selection box
AFX_IDC_COLOR_RED,0x044E,1102,Control ID for Red color selection box
AFX_IDC_COLOR_GREEN,0x044F,1103,Control ID for Green color selection box
AFX_IDC_COLOR_BLUE,0x0450,1104,Control ID for Blue color selection box
AFX_IDC_COLOR_YELLOW,0x0451,1105,Control ID for Yellow color selection box
AFX_IDC_COLOR_MAGENTA,0x0452,1106,Control ID for Magenta color selection box
AFX_IDC_COLOR_CYAN,0x0453,1107,Control ID for Cyan color selection box
AFX_IDC_COLOR_GRAY,0x0454,1108,Control ID for Gray color selection box
AFX_IDC_COLOR_LIGHTGRAY,0x0455,1109,Control ID for Light Gray color selection box
AFX_IDC_COLOR_DARKRED,0x0456,1110,Control ID for Dark Red color selection box
AFX_IDC_COLOR_DARKGREEN,0x0457,1111,Control ID for Dark Green color selection box
AFX_IDC_COLOR_DARKBLUE,0x0458,1112,Control ID for Dark Blue color selection box
AFX_IDC_COLOR_LIGHTBROWN,0x0459,1113,Control ID for Light Brown color selection box
AFX_IDC_COLOR_DARKMAGENTA,0x045A,1114,Control ID for Dark Magenta color selection box
AFX_IDC_COLOR_DARKCYAN,0x045B,1115,Control ID for Dark Cyan color selection box
AFX_IDC_COLORPROP,0x045C,1116,Control ID for Color property page control
AFX_IDC_SYSTEMCOLORS,0x045D,1117,Control ID for System Colors list
AFX_IDC_PROPNAME,0x04B1,1201,Control ID for Property Name static text
AFX_IDC_PICTURE,0x04B2,1202,Control ID for Picture property control
AFX_IDC_BROWSE,0x04B3,1203,Control ID for file browse button
AFX_IDC_CLEAR,0x04B4,1204,Control ID for Clear button
AFX_IDC_TAB_CONTROL,0x3020,12320,Control ID for Tab Control in property sheets
AFX_IDD_NEWTYPEDLG,0x7801,30721,Dialog Template ID for New File Type Selection dialog
AFX_IDD_PRINTDLG,0x7802,30722,Dialog Template ID for Print Status dialog
AFX_IDD_PREVIEW_TOOLBAR,0x7803,30723,Dialog Template ID for Print Preview toolbar
AFX_IDD_INSERTOBJECT,0x7804,30724,Dialog Template ID for OLE Insert Object dialog
AFX_IDD_CHANGEICON,0x7805,30725,Dialog Template ID for OLE Change Icon dialog
AFX_IDD_CONVERT,0x7806,30726,Dialog Template ID for OLE Convert Object dialog
AFX_IDD_PASTESPECIAL,0x7807,30727,Dialog Template ID for OLE Paste Special dialog
AFX_IDD_EDITLINKS,0x7808,30728,Dialog Template ID for OLE Edit Links dialog
AFX_IDD_FILEBROWSE,0x7809,30729,Dialog Template ID for File Browse dialog
AFX_IDD_BUSY,0x780A,30730,Dialog Template ID for OLE Busy dialog
AFX_IDD_OBJECTPROPERTIES,0x780C,30732,Dialog Template ID for OLE Object Properties dialog
AFX_IDD_CHANGESOURCE,0x780D,30733,Dialog Template ID for OLE Change Source dialog
AFX_IDC_CONTEXTHELP,0x7901,30977,Cursor ID for Context Help cursor
AFX_IDC_MAGNIFY,0x7902,30978,Cursor ID for Magnifier tool cursor
AFX_IDC_SMALLARROWS,0x7903,30979,Cursor ID for small directional adjustment arrows
AFX_IDC_HSPLITBAR,0x7904,30980,Cursor ID for Horizontal Splitter bar
AFX_IDC_VSPLITBAR,0x7905,30981,Cursor ID for Vertical Splitter bar
AFX_IDC_NODROPCRSR,0x7906,30982,"Cursor ID for ""No Drop"" drop-target indicator"
AFX_IDC_TRACKNWSE,0x7907,30983,Cursor ID for Northwest-Southeast tracking
AFX_IDC_TRACKNESW,0x7908,30984,Cursor ID for Northeast-Southwest tracking
AFX_IDC_TRACKNS,0x7909,30985,Cursor ID for North-South tracking
AFX_IDC_TRACKWE,0x790A,30986,Cursor ID for West-East tracking
AFX_IDC_TRACK4WAY,0x790B,30987,Cursor ID for 4-way object tracking
AFX_IDC_MOVE4WAY,0x790C,30988,Cursor ID for 4-way move operation
AFX_IDB_MINIFRAME_MENU,0x7912,30994,Bitmap ID for mini-frame window system menu button
AFX_IDB_CHECKLISTBOX_NT,0x7913,30995,Bitmap ID for CCheckListBox check images (Windows NT)
AFX_IDB_CHECKLISTBOX_95,0x7914,30996,Bitmap ID for CCheckListBox check images (Windows 95)
AFX_IDR_PREVIEW_ACCEL,0x7915,30997,Accelerator Table ID for Print Preview mode
AFX_IDI_STD_MDIFRAME,0x7A01,31233,Icon ID for standard MDI application frame window
AFX_IDI_STD_FRAME,0x7A02,31234,Icon ID for standard application frame window
AFX_IDS_OPENFILE,0xF000,61440,String ID for Open File dialog title
AFX_IDS_SAVEFILE,0xF001,61441,Prompt or title displayed in dialogs when saving an existing file
AFX_IDS_ALLFILTER,0xF002,61442,String ID for generic wildcard filter string
AFX_IDS_UNTITLED,0xF003,61443,"Default ""Untitled"" string prefix for new, unsaved documents"
AFX_IDS_SAVEFILECOPY,0xF004,61444,Prompt or title displayed in dialogs when saving a copy of a file
AFX_IDS_PREVIEW_CLOSE,0xF005,61445,"String ID for ""Close Print Preview"" tooltip label"
AFX_IDS_HIDE,0xF011,61457,String ID for Hide command label
AFX_IDP_NO_ERROR_AVAILABLE,0xF020,61472,Prompt ID for No error description available message
AFX_IDS_RESOURCE_EXCEPTION,0xF022,61474,Error message string displayed when a general resource exception occurs
AFX_IDS_USER_EXCEPTION,0xF024,61476,Standard string prompt for generic user-initiated exceptions
AFX_IDS_ONEPAGE,0xF041,61505,"String ID for ""One Page"" status text in Print Preview"
AFX_IDS_TWOPAGE,0xF042,61506,Button or UI string for toggling two-page view in Print Preview
AFX_IDS_PRINTDEFAULTEXT,0xF045,61509,String ID for Default Print File extension
AFX_IDS_PRINTDEFAULT,0xF046,61510,String ID for Default Printer description string
AFX_IDS_PRINTFILTER,0xF047,61511,String ID for Print File filter string
AFX_IDS_PRINTCAPTION,0xF048,61512,String ID for Print job progress window caption
AFX_IDS_OBJECT_MENUITEM,0xF080,61568,String ID for Dynamic OLE Object menu item label
AFX_IDS_EDIT_VERB,0xF081,61569,String ID for OLE Edit verb label
AFX_IDS_ACTIVATE_VERB,0xF082,61570,String ID for OLE Activate verb menu label
AFX_IDS_CHANGE_LINK,0xF083,61571,String ID for Change Link action label
AFX_IDS_AUTO,0xF084,61572,"String ID for ""Auto"" label"
AFX_IDS_MANUAL,0xF085,61573,"String ID for ""Manual"" label"
AFX_IDS_FROZEN,0xF086,61574,String ID for Frozen state indicator label
AFX_IDS_ALL_FILES,0xF087,61575,"String ID for ""All Files (.)"" filter string"
AFX_IDS_SAVE_MENU,0xF088,61576,Dynamic menu text used for saving the active document/object
AFX_IDS_UPDATE_MENU,0xF089,61577,Dynamic menu text for updating an embedded OLE object
AFX_IDS_SAVE_AS_MENU,0xF08A,61578,"Dynamic menu text used for the ""Save As"" command"
AFX_IDS_SAVE_COPY_AS_MENU,0xF08B,61579,"Dynamic menu text used for the ""Save Copy As"" command"
AFX_IDS_EXIT_MENU,0xF08C,61580,String ID for Exit command menu label
AFX_IDS_UPDATING_ITEMS,0xF08D,61581,Status message displayed while links/embedded objects update
AFX_IDS_METAFILE_FORMAT,0xF08E,61582,String ID for Metafile Clipboard format display
AFX_IDS_BITMAP_FORMAT,0xF090,61584,String ID for Bitmap Clipboard format name
AFX_IDS_LINKSOURCE_FORMAT,0xF091,61585,String ID for Link Source clipboard format display
AFX_IDS_EMBED_FORMAT,0xF092,61586,String ID for Embedded Object format display
AFX_IDS_PASTELINKEDTYPE,0xF094,61588,String ID for Paste Linked Object type description
AFX_IDS_UNKNOWNTYPE,0xF095,61589,Display string indicating an unknown or unrecognized data object
AFX_IDS_RTF_FORMAT,0xF096,61590,Display string describing Rich Text Format (RTF) data
AFX_IDS_TEXT_FORMAT,0xF097,61591,Display string describing plain unformatted text clipboard data
AFX_IDP_INVALID_FILENAME,0xF100,61696,Prompt ID for Invalid file name error prompt
AFX_IDP_FAILED_TO_OPEN_DOC,0xF101,61697,Prompt ID for Document open failure prompt
AFX_IDP_FAILED_TO_SAVE_DOC,0xF102,61698,Prompt ID for Document save failure prompt
AFX_IDP_ASK_TO_SAVE,0xF103,61699,Prompt ID for asking user to save modified document
AFX_IDP_FAILED_TO_CREATE_DOC,0xF104,61700,Prompt ID for Document creation failure prompt
AFX_IDP_FILE_TOO_LARGE,0xF105,61701,Prompt ID for File size exceeds maximum limit
AFX_IDP_FAILED_TO_START_PRINT,0xF106,61702,Prompt ID for Print job initialization failure prompt
AFX_IDP_FAILED_TO_LAUNCH_HELP,0xF107,61703,Prompt ID for Windows Help launch failure prompt
AFX_IDP_INTERNAL_FAILURE,0xF108,61704,Prompt ID for Internal application failure prompt
AFX_IDP_COMMAND_FAILURE,0xF109,61705,Prompt ID for Command execution failure message
AFX_IDP_FAILED_MEMORY_ALLOC,0xF10A,61706,Prompt ID for Memory allocation failure prompt
AFX_IDP_UNREG_DONE,0xF10B,61707,Prompt ID for Unregistration completed message
AFX_IDP_UNREG_FAILURE,0xF10C,61708,Prompt ID for Unregistration failed message
AFX_IDP_DLL_LOAD_FAILED,0xF10D,61709,Prompt ID for DLL loading failure message
AFX_IDP_DLL_BAD_VERSION,0xF10E,61710,Prompt ID for Incompatible DLL version error
AFX_IDP_PARSE_INT,0xF110,61712,Prompt ID for Data Validation: Integer parse error
AFX_IDP_PARSE_REAL,0xF111,61713,Prompt ID for Data Validation: Floating point parse error
AFX_IDP_PARSE_INT_RANGE,0xF112,61714,Prompt ID for Data Validation: Integer out of range error
AFX_IDP_PARSE_REAL_RANGE,0xF113,61715,Prompt ID for Data Validation: Floating point out of range error
AFX_IDP_PARSE_STRING_SIZE,0xF114,61716,Prompt ID for Data Validation: String length out of range error
AFX_IDP_PARSE_RADIO_BUTTON,0xF115,61717,Prompt ID for Data Validation: Radio button selection error
AFX_IDP_PARSE_BYTE,0xF116,61718,Prompt ID for Data Validation: Byte parse error
AFX_IDP_PARSE_UINT,0xF117,61719,Prompt ID for Data Validation: Unsigned integer parse error
AFX_IDP_PARSE_DATETIME,0xF118,61720,Prompt ID for Data Validation: Date/Time parse error
AFX_IDP_PARSE_CURRENCY,0xF119,61721,Prompt ID for Data Validation: Currency parse error
AFX_IDP_FILE_NONE,0xF1A0,61856,Prompt ID for CFileException: No error
AFX_IDP_FILE_GENERIC,0xF1A1,61857,Prompt ID for CFileException: Generic file error
AFX_IDP_FILE_NOT_FOUND,0xF1A2,61858,Prompt ID for CFileException: File not found
AFX_IDP_FILE_BAD_PATH,0xF1A3,61859,Prompt ID for CFileException: Bad path
AFX_IDP_FILE_TOO_MANY_OPEN,0xF1A4,61860,Prompt ID for CFileException: Too many open files
AFX_IDP_FILE_ACCESS_DENIED,0xF1A5,61861,Prompt ID for CFileException: Access denied
AFX_IDP_FILE_INVALID_FILE,0xF1A6,61862,Prompt ID for CFileException: Invalid file handle
AFX_IDP_FILE_REMOVE_CURRENT,0xF1A7,61863,Prompt ID for CFileException: Cannot remove current directory
AFX_IDP_FILE_DIR_FULL,0xF1A8,61864,Prompt ID for CFileException: Directory full
AFX_IDP_FILE_BAD_SEEK,0xF1A9,61865,Prompt ID for CFileException: Bad seek operation
AFX_IDP_FILE_HARD_IO,0xF1AA,61866,Prompt ID for CFileException: Hardware I/O error
AFX_IDP_FILE_SHARING,0xF1AB,61867,Prompt ID for CFileException: Sharing violation
AFX_IDP_FILE_LOCKING,0xF1AC,61868,Prompt ID for CFileException: File locking violation
AFX_IDP_FILE_DISKFULL,0xF1AD,61869,Prompt ID for CFileException: Disk full
AFX_IDP_FILE_EOF,0xF1AE,61870,Prompt ID for CFileException: Unexpected end of file
AFX_IDP_ARCH_NONE,0xF1B0,61872,Prompt ID for Archive Error: No error
AFX_IDP_ARCH_GENERIC,0xF1B1,61873,Prompt ID for Generic archive operation error
AFX_IDP_ARCH_READONLY,0xF1B2,61874,Prompt ID for Archive Error: Attempted write to read-only archive
AFX_IDP_ARCH_ENDOFFILE,0xF1B3,61875,Prompt ID for Archive Error: Unexpected end of file
AFX_IDP_ARCH_WRITEONLY,0xF1B4,61876,Prompt ID for Archive Error: Attempted read from write-only archive
AFX_IDP_ARCH_BADINDEX,0xF1B5,61877,Prompt ID for Archive Error: Bad file index
AFX_IDP_ARCH_BADCLASS,0xF1B6,61878,Prompt ID for Archive Error: Class bad or unexpected
AFX_IDP_ARCH_BADSCHEMA,0xF1B7,61879,Prompt ID for Archive Error: Bad schema version

== CSV table of System Defined Prompt String Resource Identifiers To Be Added ==
Macro Name,Value,Description
AFX_IDP_SQL_API_CONFORMANCE,0xF28E,ODBC driver is incompatible with MFC database classes (API conformance error).
AFX_IDP_SQL_BOOKMARKS_NOT_ENABLED,0xF29C,Bookmarks are not enabled on the recordset.
AFX_IDP_SQL_BOOKMARKS_NOT_SUPPORTED,0xF29B,Bookmarks are not supported by the underlying ODBC driver or recordset.
AFX_IDP_SQL_CONNECT_FAIL,0xF281,Attempt to connect to the SQL data source failed.
AFX_IDP_SQL_DATA_TRUNCATED,0xF295,Data returned from the SQL operation was truncated.
AFX_IDP_SQL_DYNAMIC_CURSOR_NOT_SUPPORTED,0xF299,Dynamic cursors are not supported by the ODBC driver.
AFX_IDP_SQL_DYNASET_NOT_SUPPORTED,0xF28C,Dynasets are not supported by the ODBC driver.
AFX_IDP_SQL_EMPTY_COLUMN_LIST,0xF283,"Attempted to open a table, but no columns were specified to retrieve."
AFX_IDP_SQL_FIELD_NOT_FOUND,0xF29A,The specified field or column name was not found.
AFX_IDP_SQL_FIELD_SCHEMA_MISMATCH,0xF284,Query returned an unexpected column data type (schema mismatch).
AFX_IDP_SQL_ILLEGAL_MODE,0xF285,Attempted update or delete in an illegal/unsupported recordset mode.
AFX_IDP_SQL_INCORRECT_ODBC,0xF297,Incompatible or incorrect ODBC driver installed.
AFX_IDP_SQL_LOCK_MODE_NOT_SUPPORTED,0xF294,The requested locking mode is not supported by the driver.
AFX_IDP_SQL_MULTIPLE_ROWS_AFFECTED,0xF286,Multiple rows were unexpectedly updated or deleted.
AFX_IDP_SQL_NO_CURRENT_RECORD,0xF287,Operation failed because there is no current record.
AFX_IDP_SQL_NO_DATA_FOUND,0xF290,No data was returned or found for the operation.
AFX_IDP_SQL_NO_POSITIONED_UPDATES,0xF293,Positioned updates or deletes are not supported.
AFX_IDP_SQL_NO_ROWS_AFFECTED,0xF288,No rows were affected by the update or delete operation.
AFX_IDP_SQL_ODBC_LOAD_FAILED,0xF28B,Failed to load the required ODBC driver library (ODBC32.DLL).
AFX_IDP_SQL_ODBC_V2_REQUIRED,0xF292,An ODBC 2.0 or higher compliant driver is required.
AFX_IDP_SQL_RECORDSET_FORWARD_ONLY,0xF282,The recordset only supports forward scrolling.
AFX_IDP_SQL_RECORDSET_READONLY,0xF289,The recordset is read-only and cannot be modified.
AFX_IDP_SQL_ROW_FETCH,0xF296,Error encountered while fetching rows from the database.
AFX_IDP_SQL_ROW_UPDATE_NOT_SUPPORTED,0xF291,Row update operations are not supported by the driver.
AFX_IDP_SQL_SNAPSHOT_NOT_SUPPORTED,0xF28D,Snapshots require static cursor support from the ODBC driver.
AFX_IDP_SQL_SQL_CONFORMANCE,0xF28F,ODBC driver does not meet required SQL conformance levels.
AFX_IDP_SQL_SQL_NO_TOTAL,0xF28A,ODBC driver does not support the MFC CLongBinary data model.
AFX_IDP_SQL_UPDATE_DELETE_FAILED,0xF298,The update or delete operation failed.

== CSV table of System Defined Resource Identifiers To Be Added ==
Macro Name,Value,Description
ID_APP_ABOUT,0xE140,"Displays the ""About"" dialog box for the application."
ID_APP_EXIT,0xE141,Prompts to save documents and exits the application.
ID_APPLY_NOW,0x3021,Applies changes in a property sheet without closing it.
ID_CONTEXT_HELP,0xE144,Enters context-sensitive Help mode (Shift+F1).
ID_DEFAULT_HELP,0xE147,Runs default help topic for the active window or control.
ID_EDIT_CLEAR,0xE120,Erases the selected content.
ID_EDIT_CLEAR_ALL,0xE121,Erases all content in the document or window.
ID_EDIT_COPY,0xE122,Copies the selection and places it on the Clipboard.
ID_EDIT_CUT,0xE123,Cuts the selection and places it on the Clipboard.
ID_EDIT_FIND,0xE124,Opens the Find dialog to search for specified text.
ID_EDIT_PASTE,0xE125,Inserts Clipboard contents at the insertion point.
ID_EDIT_PASTE_LINK,0xE126,Inserts Clipboard contents and creates a link to the source.
ID_EDIT_PASTE_SPECIAL,0xE127,Inserts Clipboard contents with formatting options.
ID_EDIT_REDO,0xE12C,Redoes the previously undone action.
ID_EDIT_REPEAT,0xE128,Repeats the last action performed.
ID_EDIT_REPLACE,0xE129,Replaces specific text with different text.
ID_EDIT_SELECT_ALL,0xE12A,Selects the entire document or contents.
ID_EDIT_UNDO,0xE12B,Undoes the last action performed.
ID_FILE_CLOSE,0xE102,Closes the active document.
ID_FILE_MRU_FILE1,0xE110,Opens most recently used file #1.
ID_FILE_MRU_FILE2,0xE111,Opens most recently used file #2.
ID_FILE_MRU_FILE3,0xE112,Opens most recently used file #3.
ID_FILE_MRU_FILE4,0xE113,Opens most recently used file #4.
ID_FILE_MRU_FILE5,0xE114,Opens most recently used file #5.
ID_FILE_MRU_FILE6,0xE115,Opens most recently used file #6.
ID_FILE_MRU_FILE7,0xE116,Opens most recently used file #7.
ID_FILE_MRU_FILE8,0xE117,Opens most recently used file #8.
ID_FILE_MRU_FILE9,0xE118,Opens most recently used file #9.
ID_FILE_MRU_FILE10,0xE119,Opens most recently used file #10.
ID_FILE_MRU_FILE11,0xE11A,Opens most recently used file #11.
ID_FILE_MRU_FILE12,0xE11B,Opens most recently used file #12.
ID_FILE_MRU_FILE13,0xE11C,Opens most recently used file #13.
ID_FILE_MRU_FILE14,0xE11D,Opens most recently used file #14.
ID_FILE_MRU_FILE15,0xE11E,Opens most recently used file #15.
ID_FILE_MRU_FILE16,0xE11F,Opens most recently used file #16.
ID_FILE_MRU_FIRST,0xE110,First ID reserved for the Most Recently Used (MRU) file list.
ID_FILE_MRU_LAST,0xE11F,Last ID reserved for the Most Recently Used (MRU) file list.
ID_FILE_NEW,0xE100,Creates a new document.
ID_FILE_NEW_FRAME,0xE10D,Opens a new frame window for an existing document.
ID_FILE_OPEN,0xE101,Opens an existing document.
ID_FILE_PAGE_SETUP,0xE105,Opens the Page Setup dialog box to alter printing parameters.
ID_FILE_PRINT,0xE107,Prints the active document.
ID_FILE_PRINT_DIRECT,0xE108,Prints the document directly using current printer settings.
ID_FILE_PRINT_PREVIEW,0xE109,Displays full pages on-screen to preview output before printing.
ID_FILE_PRINT_SETUP,0xE106,Opens the Print Setup dialog to choose a printer and options.
ID_FILE_SAVE,0xE103,Saves the active document under its current name.
ID_FILE_SAVE_AS,0xE104,Saves the active document under a new name.
ID_FILE_SAVE_COPY_AS,0xE10B,Saves a copy of the active document under a new name.
ID_FILE_SEND_MAIL,0xE10C,Sends the active document via electronic mail.
ID_FILE_UPDATE,0xE10A,Updates an embedded container document with object changes.
ID_FORMAT_FONT,0xE160,Selects fonts for current selection or document.
ID_HELP,0xE146,Displays help for the current task or command (F1).
ID_HELP_FINDER,0xE143,Displays Help contents/index or search prompt.
ID_HELP_INDEX,0xE142,Displays the main index for Help topics.
ID_HELP_USING,0xE144,Displays instructions on how to use Help.
ID_INDICATOR_CAPS,0xE701,Status bar indicator for Caps Lock state.
ID_INDICATOR_EXT,0xE700,Status bar indicator for extended selection mode.
ID_INDICATOR_KANA,0xE706,Status bar indicator for Kana input mode.
ID_INDICATOR_NUM,0xE702,Status bar indicator for Num Lock state.
ID_INDICATOR_OVR,0xE704,Status bar indicator for Overtype mode.
ID_INDICATOR_REC,0xE705,Status bar indicator for macro recording mode.
ID_INDICATOR_SCRL,0xE703,Status bar indicator for Scroll Lock state.
ID_NEXT_PANE,0xE150,Switches focus to the next window pane.
ID_OLE_EDIT_CHANGE_ICON,0xE203,Changes the icon displaying an OLE embedded item.
ID_OLE_EDIT_CONVERT,0xE202,Converts an embedded OLE object to a different type.
ID_OLE_EDIT_LINKS,0xE201,Displays options to edit or update linked OLE objects.
ID_OLE_EDIT_PROPERTIES,0xE204,Displays OLE item properties dialog.
ID_OLE_INSERT_NEW,0xE200,Inserts a new OLE embedded object into the document.
ID_OLE_VERB_FIRST,0xE210,First ID for dynamically populated OLE object verbs.
ID_PREV_PANE,0xE151,Switches focus back to the previous window pane.
ID_REC_FIRST,0xE240,Alias for ID_RECORD_FIRST; moves to the first record.
ID_RECORD_LAST,0xE243,Moves focus/selection to the last record in a database view.
ID_RECORD_NEXT,0xE242,Moves focus/selection to the next record in a database view.
ID_RECORD_PREV,0xE241,Moves focus/selection to the previous record in a database view.
ID_SEPARATOR,0x0000,Used as a visual separator line in menus and toolbars.
ID_VIEW_AUTOARRANGE,0xE816,Automatically arranges icons in a list/grid view.
ID_VIEW_BYNAME,0xE815,Sorts list view items alphabetically by name.
ID_VIEW_DETAILS,0xE813,Displays detailed information about each item in a list view.
ID_VIEW_LARGEICON,0xE810,Displays items using large icons in a list/grid view.
ID_VIEW_LINEUP,0xE814,Aligns icons to a invisible grid.
ID_VIEW_LIST,0xE812,Displays items as a simple list in a view window.
ID_VIEW_REBAR,0xE802,Toggles display of the rebar control bar.
ID_VIEW_SMALLICON,0xE811,Displays items using small icons in a list/grid view.
ID_VIEW_STATUS_BAR,0xE801,Toggles display of the application status bar.
ID_VIEW_TOOLBAR,0xE800,Toggles display of the application main toolbar.
ID_WINDOW_ARRANGE,0xE131,Arranges icons at the bottom of an MDI workspace.
ID_WINDOW_CASCADE,0xE132,Arranges open MDI windows in an overlapping cascade.
ID_WINDOW_NEW,0xE130,Opens another window for the active document.
ID_WINDOW_SPLIT,0xE135,Splits the active window into multiple panes.
ID_WINDOW_TILE_HORIZ,0xE133,Tiles open MDI windows horizontally.
ID_WINDOW_TILE_VERT,0xE134,Tiles open MDI windows vertically.
ID_WIZBACK,0x3023,Wizard control button ID for navigating to the previous page.
ID_WIZFINISH,0x3025,Wizard control button ID for completing and closing the wizard.
ID_WIZNEXT,0x3024,Wizard control button ID for navigating to the next page.
IDABORT,0x0003,"Dialog button result ID indicating ""Abort""."
IDC_STATIC,0xFFFFFFFF,Default ID reserved for non-interactive static controls.
IDCANCEL,0x0002,"Dialog button result ID indicating ""Cancel"" or closing a dialog."
IDD_ABOUTBOX,0x0064,Default resource ID assigned to an application's About dialog template.
IDIGNORE,0x0005,"Dialog button result ID indicating ""Ignore""."
IDNO,0x0007,"Dialog button result ID indicating ""No""."
IDOK,0x0001,"Dialog button result ID indicating ""OK"" / confirmation."
IDP_OLE_INIT_FAILED,0xE000,String resource ID for OLE initialization failure prompt message.
IDRETRY,0x0004,"Dialog button result ID indicating ""Retry""."
IDYES,0x0006,"Dialog button result ID indicating ""Yes""."

== CSV table of System Defined Bitmap Idenfiers To Be Added ==
Macro Name,Value,Description
IDB_STD_SMALL_COLOR,0x0000,Standard Common Controls toolbar bitmap containing 16x16-pixel color icons for file/edit operations.
IDB_STD_LARGE_COLOR,0x0001,Standard Common Controls toolbar bitmap containing 24x24-pixel color icons.
IDB_STD_SMALL_MONO,0x0002,Standard Common Controls bitmap containing 16x16 monochrome icons.
IDB_STD_LARGE_MONO,0x0003,Standard Common Controls bitmap containing 24x24 monochrome icons.
IDB_VIEW_SMALL_COLOR,0x0004,Standard toolbar bitmap containing 16x16 color icons for list-view styles.
IDB_VIEW_LARGE_COLOR,0x0005,Standard toolbar bitmap containing 24x24 color icons for list-view styles.
IDB_VIEW_SMALL_MONO,0x0006,Standard toolbar bitmap containing 16x16 monochrome icons for view styles.
IDB_VIEW_LARGE_MONO,0x0007,Standard toolbar bitmap containing 24x24 monochrome icons for view styles.
IDB_HIST_SMALL_COLOR,0x0008,Explorer-style navigation bitmap strip containing 16x16 color icons.
IDB_HIST_LARGE_COLOR,0x0009,Explorer-style navigation bitmap strip containing 24x24 color icons.
IDB_HIST_NORMAL,0x000C,Explorer travel buttons and favorites bitmaps in normal state.
IDB_HIST_HOT,0x000D,Explorer travel buttons and favorites bitmaps in hot-tracked state.
IDB_HIST_DISABLED,0x000E,Explorer travel buttons and favorites bitmaps in disabled state.
IDB_HIST_PRESSED,0x000F,Explorer travel buttons and favorites bitmaps in pressed state.
AFX_IDB_CHECKBOX,0x7801,"Standard MFC bitmap strip providing checkbox glyph states (checked, unchecked, indeterminate) for controls."
AFX_IDB_MINI_BTNS,0x7802,MFC resource strip containing small window control glyphs for mini frame windows/docking headers.
AFX_IDB_MINI_CALENDAR,0x7803,MFC navigation arrow and date glyph strip used in date pickers and calendar controls.
AFX_IDB_PROPERTY_GRID_BTNS,0x7804,"Expand (+), collapse (-), and drop-down button glyphs used in CMFCPropertyGridCtrl."
AFX_IDB_TASKPANE_SCROLL_BTNS,0x7805,Up and down scroll arrow buttons for scrollable task pane containers (CMFCTasksPane).
AFX_IDB_POPUP_SUBMENU,0x7806,Submenu expansion arrow glyphs used across MFC popup and context menus.
AFX_IDB_COLOR_PALETTE,0x7807,Stock color palette button icons for MFC color controls and property grid cells.
AFX_IDB_RIBBON_BAR_BTN_DEFAULT,0x7808,Generic placeholder image used by CMFCRibbonBar when an item image is missing.
AFX_IDB_RIBBON_PRINT_LARGE,0x7809,32x32-pixel print/print preview icon for Ribbon Application Menu.
AFX_IDB_RIBBON_PRINT_SMALL,0x780A,16x16-pixel print command icon for Ribbon menus and Quick Access Toolbars.
AFX_IDB_SCROLL_LEFT,0x780B,"Left scroll arrow icon for tabbed control bars, MDI tabs, and pane overflow navigation."
AFX_IDB_SCROLL_RIGHT,0x780C,Right scroll arrow icon for tabbed control bars and pane navigation.
AFX_IDB_SCROLL_UP,0x780D,Upward scroll arrow icon for vertically oriented toolbars and dockable panes.
AFX_IDB_SCROLL_DOWN,0x780E,Downward scroll arrow icon for vertically oriented toolbars and dockable panes.
AFX_IDB_MENU_IMAGES,0x780F,"Visual glyph strip containing caption-bar icons (Close, Minimize, Maximize, Pin, Menu Arrow)."
AFX_IDB_TASKPANE_CLOSE,0x7810,Close (X) button graphic used in task pane headers (CMFCTasksPane).
AFX_IDB_TASKPANE_EXPAND,0x7811,Expand/collapse indicator arrows used within CMFCTasksPane.
AFX_IDB_COLOR_TOOL,0x7812,Toolbar bitmap icon displaying color picker/fill tools.
AFX_IDB_TEAR,0x7813,Visual indicator graphic marking detachable tear-off menus and toolbars.
AFX_IDB_RIBBON_SLIDER_BTN_MINUS,0x7814,Zoom Out (-) button graphic used on Ribbon status bar zoom controls.
AFX_IDB_RIBBON_SLIDER_BTN_PLUS,0x7815,Zoom In (+) button graphic used on Ribbon status bar zoom controls.
AFX_IDB_HMENU_DOT,0x7F01,Internal MFC macro for radio item bullet checkmark glyphs in menus.
AFX_IDB_HMENU_ARROW,0x7F02,Internal MFC macro for submenu right-pointing expansion arrows.
OBM_OLD_LFARROW,0x7F14,Classic OEM left-arrow bitmap used in 16-bit / pre-Win95 scrollbars.
OBM_OLD_RGARROW,0x7F15,Classic OEM right-arrow bitmap used in legacy scrollbars.
OBM_OLD_UPARROW,0x7F16,Classic OEM up-arrow bitmap used in legacy scrollbars.
OBM_OLD_DNARROW,0x7F17,Classic OEM down-arrow bitmap used in legacy scrollbars.
OBM_OLD_CLOSE,0x7F18,Classic OEM close button bitmap for legacy title bars.
OBM_OLD_ZOOM,0x7F19,Classic OEM maximize (Zoom) button bitmap.
OBM_OLD_REDUCE,0x7F1A,Classic OEM minimize (Reduce) button bitmap.
OBM_OLD_RESTORE,0x7F1B,Classic OEM restore button bitmap.
OBM_SIZE,0x7F1C,OEM sizing frame bitmap asset.
OBM_BTSIZE,0x7F1D,OEM window resizing grip bitmap in corner of windows.
OBM_CHECKBOXES,0x7F1E,OEM system bitmap strip containing default checkbox state graphics.
OBM_BTNCORNERS,0x7F1F,Legacy OEM bitmap for rounded push-button corners.
OBM_CHECK,0x7F20,OEM checkmark bitmap drawn next to selected menu items.
OBM_RESTORE,0x7F23,System OEM restore button bitmap rendered on window caption bars.
OBM_ZOOM,0x7F24,System OEM maximize (Zoom) button bitmap.
OBM_REDUCE,0x7F25,System OEM minimize (Reduce) button bitmap.
OBM_LFARROW,0x7F26,OEM scrollbar left-arrow button bitmap.
OBM_RGARROW,0x7F27,OEM scrollbar right-arrow button bitmap.
OBM_DNARROW,0x7F28,OEM scrollbar down-arrow button bitmap.
OBM_UPARROW,0x7F29,OEM scrollbar up-arrow button bitmap.
OBM_CLOSE,0x7F2A,OEM title bar close button bitmap.
OBM_LFARROWI,0x7FE6,Inactive/disabled state left scrollbar arrow glyph.
OBM_RGARROWI,0x7FE7,Inactive/disabled state right scrollbar arrow glyph.
OBM_DNARROWI,0x7FE8,Inactive/disabled state down scrollbar arrow glyph.
OBM_UPARROWI,0x7FE9,Inactive/disabled state up scrollbar arrow glyph.
OBM_LFARROWD,0x7FEC,Pressed (down) state left scrollbar arrow button.
OBM_RGARROWD,0x7FED,Pressed (down) state right scrollbar arrow button.
OBM_ZOOMD,0x7FF1,Pressed state of standard Maximize/Zoom button.
OBM_REDUCED,0x7FF2,Pressed state of standard Minimize/Reduce button.
OBM_RESTORED,0x7FFA,Pressed state of standard Restore button.
OBM_COMBO,0x7FE2,OEM bitmap providing drop-down arrow graphic drawn inside standard combo box controls.
OBM_MNARROW,0x7FE3,OEM bitmap resource ID providing right-pointing submenu indicator arrow in popup menus.
OBM_UPARROWD,0x7FE0,OEM bitmap providing pressed/disabled state image for up-arrows.
OBM_DNARROWD,0x7FE1,OEM bitmap providing pressed/disabled state image for down-arrows.
*/


/* DO NOT DELETE THIS COMMENT BLOCK

=== Helpful information ===
Prefixes:
   AFX_ — Application Framework eXtension resource or symbol identifer
   IDP_ — Prompt resource identifer used for message box strings and error prompts
   IDC_ — Control resource identifer
   IDB_ — Bitmap resource identifer
   IDI_ — Icon resource identifer
   IDD_ — Dialog template identifer
   IDM_ — Menu command identifer
   IDR_ — Generic resource identifer
*/

enum class constant_category
{
  window_style,       // WS_*
  extended_style,     // WS_EX_*
  dialog_style,       // DS_*
  button_style,       // BS_*
  edit_style,         // ES_*
  static_style,       // SS_*
  listbox_style,      // LBS_*
  combobox_style,     // CBS_*
  scrollbar_style,    // SBS_*
  window_message,     // WM_*
  virtual_key,        // VK_*
  message_box,        // MB_*
  menu_flag,          // MF_*, MFT_*, MFS_*
  accelerator_flag,   // FVIRTKEY, FALT, etc.
  resource_type,      // RT_*
  control_message,    // CB_*, LB_*, EM_*, BM_*, STM_*, SBM_*, DM_*
  notification,       // CBN_*, LBN_*, EN_*, BN_*, STN_*
  dialog_id,          // IDOK, IDCANCEL, etc.
  system_id,          // IDC_STATIC, etc.
};

struct constant_entry
{
  std::string name;
  int64_t value;
  constant_category category;
  std::string description;
};

class constant_registry
{
public:
  static constant_registry& instance();

  void add(const std::string& name, int64_t value, constant_category cat,
           const std::string& desc = "");

  bool has_name(const std::string& name) const;
  bool has_value(int64_t value, constant_category cat = {}) const;

  int64_t resolve(const std::string& name) const;
  std::string resolve(int64_t value, constant_category cat = {}) const;

  std::vector<constant_entry> entries_by_category(constant_category cat) const;
  std::vector<constant_entry> all_entries() const;

  size_t size() const;

private:
  constant_registry();
  void register_all();

  std::unordered_map<std::string, int64_t> name_to_value_;
  std::vector<constant_entry> entries_;
};

}

#endif
