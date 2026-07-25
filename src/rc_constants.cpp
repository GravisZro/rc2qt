#include "rc_constants.h"

#include <algorithm>

namespace rc
{

constant_registry& constant_registry::instance()
{
  static constant_registry reg;
  return reg;
}

constant_registry::constant_registry()
{
  register_all();
}

void constant_registry::add(const std::string& name, int64_t value,
                            constant_category cat, const std::string& desc)
{
  name_to_value_[name] = value;
  entries_.push_back({name, value, cat, desc});
}

bool constant_registry::has_name(const std::string& name) const
{
  return name_to_value_.count(name) > 0;
}

int64_t constant_registry::resolve(const std::string& name) const
{
  auto it = name_to_value_.find(name);
  if(it != name_to_value_.end())
    return it->second;
  return -1;
}

std::string constant_registry::resolve(int64_t value, constant_category cat) const
{
  for(const auto& e : entries_)
  {
    if(e.value == value)
    {
      if(cat == constant_category{} || e.category == cat)
        return e.name;
    }
  }
  return {};
}

std::vector<constant_entry> constant_registry::entries_by_category(
  constant_category cat) const
{
  std::vector<constant_entry> result;
  for(const auto& e : entries_)
    if(e.category == cat)
      result.push_back(e);
  return result;
}

std::vector<constant_entry> constant_registry::all_entries() const
{
  return entries_;
}

size_t constant_registry::size() const
{
  return entries_.size();
}

void constant_registry::register_all()
{
  /* ── WS_* Window Styles ───────────────────────────────────────── */
  add("WS_BORDER", 0x00800000, constant_category::window_style, "Creates a window that has a thin-line border.");
  add("WS_CAPTION", 0x00C00000, constant_category::window_style, "Creates a window that has a title bar (includes the WS_BORDER style).");
  add("WS_CHILD", 0x40000000, constant_category::window_style, "Creates a child window. A window with this style cannot have a menu bar and cannot be used with the WS_POPUP style.");
  add("WS_CLIPCHILDREN", 0x02000000, constant_category::window_style, "Excludes the area occupied by child windows when drawing occurs within the parent window. Used when creating the parent window.");
  add("WS_CLIPSIBLINGS", 0x04000000, constant_category::window_style, "Clips child windows relative to each other; that is");
  add("WS_DISABLED", 0x08000000, constant_category::window_style, "Creates a window that is initially disabled. A disabled window cannot receive input from the user.");
  add("WS_DLGFRAME", 0x00400000, constant_category::window_style, "Creates a window that has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar.");
  add("WS_GROUP", 0x00020000, constant_category::window_style, "Specifies the first control of a group of controls. The group consists of this first control and all controls defined after it");
  add("WS_HSCROLL", 0x00100000, constant_category::window_style, "Creates a window that has a horizontal scroll bar.");
  add("WS_MAXIMIZE", 0x01000000, constant_category::window_style, "Creates a window that is initially maximized.");
  add("WS_MAXIMIZEBOX", 0x00010000, constant_category::window_style, "Creates a window that has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU.");
  add("WS_MINIMIZE", 0x20000000, constant_category::window_style, "Creates a window that is initially minimized.");
  add("WS_MINIMIZEBOX", 0x00020000, constant_category::window_style, "Creates a window that has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU.");
  add("WS_OVERLAPPED", 0x00000000, constant_category::window_style, "Creates an overlapped window. An overlapped window has a title bar and a border.");
  add("WS_POPUP", 0x80000000, constant_category::window_style, "Creates a pop-up window. Cannot be used with the WS_CHILD style.");
  add("WS_SIZEBOX", 0x00040000, constant_category::window_style, "Creates a window that has a sizing border (same as WS_THICKFRAME).");
  add("WS_SYSMENU", 0x00080000, constant_category::window_style, "Creates a window that has a window menu on its title bar. Requires WS_CAPTION.");
  add("WS_TABSTOP", 0x00010000, constant_category::window_style, "Specifies a control that can receive the keyboard focus when the user presses the TAB key. Pressing TAB changes the focus to the next control with this style.");
  add("WS_VISIBLE", 0x10000000, constant_category::window_style, "Creates a window that is initially visible.");
  add("WS_VSCROLL", 0x00200000, constant_category::window_style, "Creates a window that has a vertical scroll bar.");
  add("WS_THICKFRAME", 0x00040000, constant_category::window_style, "Creates a window that has a sizing border (same as WS_SIZEBOX).");
  add("WS_OVERLAPPEDWINDOW", 0x00CF0000, constant_category::window_style, "Creates an overlapped window with WS_OVERLAPPED");
  add("WS_POPUPWINDOW", 0x80880000, constant_category::window_style, "Creates a pop-up window with WS_POPUP");

  /* ── WS_EX_* Extended Window Styles ───────────────────────────── */
  add("WS_EX_ACCEPTFILES", 0x00000010, constant_category::extended_style, "Specifies that a window created with this style accepts dropped files.");
  add("WS_EX_APPWINDOW", 0x00040000, constant_category::extended_style, "Forces a top-level window onto the taskbar when the window is visible.");
  add("WS_EX_CLIENTEDGE", 0x00000200, constant_category::extended_style, "Specifies that a window has a border with a sunken edge.");
  add("WS_EX_COMPOSITED", 0x02000000, constant_category::extended_style, "Paints all descendants of a window in bottom-to-top painting order using double-buffering.");
  add("WS_EX_CONTEXTHELP", 0x00000400, constant_category::extended_style, "Includes a question mark in the title bar of the window. When clicked");
  add("WS_EX_CONTROLPARENT", 0x00010000, constant_category::extended_style, "Allows the user to navigate among the child windows of the window by using the TAB key.");
  add("WS_EX_DLGMODALFRAME", 0x00000001, constant_category::extended_style, "Creates a window that has a double border; can optionally be created with a title bar by specifying WS_CAPTION.");
  add("WS_EX_LAYERED", 0x00080000, constant_category::extended_style, "Creates a layered window. Note that this cannot be used if the window has a window class style of CS_OWNDC or CS_CLASSDC.");
  add("WS_EX_LAYOUTRTL", 0x00400000, constant_category::extended_style, "Sets the horizontal origin to the right edge. Increasing horizontal values advance to the left.");
  add("WS_EX_LEFT", 0x00000000, constant_category::extended_style, "Gives the window left-aligned properties (default layout).");
  add("WS_EX_LEFTSCROLLBAR", 0x00004000, constant_category::extended_style, "Places the vertical scroll bar (if present) to the left of the client area.");
  add("WS_EX_LTRREADING", 0x00000000, constant_category::extended_style, "Displays window text using left-to-right reading order properties (default).");
  add("WS_EX_MDICHILD", 0x00000040, constant_category::extended_style, "Creates a Multiple-Document Interface (MDI) child window.");
  add("WS_EX_NOACTIVATE", 0x08000000, constant_category::extended_style, "A top-level window created with this style does not become the active window when the user clicks it.");
  add("WS_EX_NOINHERITLAYOUT", 0x00100000, constant_category::extended_style, "Prevents the window layout from being inherited by child windows.");
  add("WS_EX_NOPARENTNOTIFY", 0x00000004, constant_category::extended_style, "Specifies that a child window created with this style does not send the WM_PARENTNOTIFY message to its parent when created or destroyed.");
  add("WS_EX_PALETTEWINDOW", 0x00000188, constant_category::extended_style, "Combines WS_EX_WINDOWEDGE");
  add("WS_EX_RIGHT", 0x00001000, constant_category::extended_style, "Gives the window right-aligned properties depending on window class.");
  add("WS_EX_RIGHTSCROLLBAR", 0x00000000, constant_category::extended_style, "Places the vertical scroll bar to the right of the client area (default).");
  add("WS_EX_RTLREADING", 0x00002000, constant_category::extended_style, "Displays window text using Right-to-Left reading order properties.");
  add("WS_EX_STATICEDGE", 0x00020000, constant_category::extended_style, "Gives a window a three-dimensional border style intended to be used for items that do not accept user input.");
  add("WS_EX_TOOLWINDOW", 0x00000080, constant_category::extended_style, "Creates a tool window intended as a floating toolbar; does not appear on the taskbar or in the ALT+TAB dialog.");
  add("WS_EX_TOPMOST", 0x00000008, constant_category::extended_style, "Specifies that a window created with this style should be placed above all non-topmost windows and remain above them");
  add("WS_EX_TRANSPARENT", 0x00000020, constant_category::extended_style, "Specifies that a window created with this style should not be painted until siblings beneath it have been painted.");
  add("WS_EX_WINDOWEDGE", 0x00000100, constant_category::extended_style, "Specifies that a window has a border with a raised edge.");

  /* ── DS_* Dialog Styles ───────────────────────────────────────── */
  add("DS_3DLOOK", 0x0004, constant_category::dialog_style, "Gives dialog box controls a 3D look (historical compatibility style).");
  add("DS_ABSALIGN", 0x0001, constant_category::dialog_style, "Indicates that the coordinates of the dialog box are screen coordinates (otherwise client coordinates relative to owner).");
  add("DS_CENTER", 0x0800, constant_category::dialog_style, "Centers the dialog box on the display screen or monitor.");
  add("DS_CENTERMOUSE", 0x1000, constant_category::dialog_style, "Centers the dialog box under the mouse cursor.");
  add("DS_CONTEXTHELP", 0x2000, constant_category::dialog_style, "Includes a Help button in the title bar of the dialog box.");
  add("DS_CONTROL", 0x0400, constant_category::dialog_style, "Creates a dialog box that can work as a control child inside another dialog box (like a tab control page).");
  add("DS_FIXEDSYS", 0x0008, constant_category::dialog_style, "Causes the dialog box to use the system font with fixed spacing instead of proportional spacing.");
  add("DS_LOCALEDIT", 0x0020, constant_category::dialog_style, "Applies to edit controls in the dialog box");
  add("DS_MODALFRAME", 0x0080, constant_category::dialog_style, "Creates a dialog box with a modal dialog frame border.");
  add("DS_NOFAILCREATE", 0x0010, constant_category::dialog_style, "Causes CreateDialog or DialogBox to succeed even if some child controls cannot be created.");
  add("DS_NOIDLEMSG", 0x0100, constant_category::dialog_style, "Suppresses WM_ENTERIDLE messages that the system otherwise sends to the owner while the dialog box is displayed.");
  add("DS_SETFONT", 0x0040, constant_category::dialog_style, "Indicates that the dialog template specifies a custom font for rendering text in the dialog box.");
  add("DS_SETFOREGROUND", 0x0200, constant_category::dialog_style, "Brings the dialog box to the foreground upon creation.");
  add("DS_SYSMODAL", 0x0002, constant_category::dialog_style, "Creates a system-modal dialog box (historical style");
  add("DS_SHELLFONT", 0x0048, constant_category::dialog_style, "Uses the system shell font for rendering text inside the dialog box (requires DS_SETFONT).");

  /* ── BS_* Button Styles ───────────────────────────────────────── */
  add("BS_3STATE", 0x0005, constant_category::button_style, "Creates a button that functions like a checkbox but can be dimmed (indeterminate state) as well as checked or unchecked.");
  add("BS_AUTO3STATE", 0x0006, constant_category::button_style, "Same as BS_3STATE");
  add("BS_AUTOCHECKBOX", 0x0003, constant_category::button_style, "Same as BS_CHECKBOX");
  add("BS_AUTORADIOBUTTON", 0x0009, constant_category::button_style, "Same as BS_RADIOBUTTON");
  add("BS_BITMAP", 0x0080, constant_category::button_style, "Specifies that the button displays a bitmap instead of text.");
  add("BS_BOTTOM", 0x0800, constant_category::button_style, "Places text at the bottom of the button rectangle.");
  add("BS_CENTER", 0x0300, constant_category::button_style, "Horizontally centers text in the button rectangle.");
  add("BS_CHECKBOX", 0x0002, constant_category::button_style, "Creates a small square box with adjacent text. Typically used to allow multi-select options.");
  add("BS_DEFPUSHBUTTON", 0x0001, constant_category::button_style, "Creates a push button with a heavy black border representing the default action when ENTER is pressed.");
  add("BS_GROUPBOX", 0x0007, constant_category::button_style, "Creates a rectangle in which other controls can be grouped");
  add("BS_ICON", 0x0040, constant_category::button_style, "Specifies that the button displays an icon instead of text.");
  add("BS_LEFT", 0x0100, constant_category::button_style, "Left-aligns the text in the button rectangle.");
  add("BS_LEFTTEXT", 0x0020, constant_category::button_style, "Places the radio button circle or checkbox square on the right side of the control text.");
  add("BS_MULTILINE", 0x2000, constant_category::button_style, "Wraps the button text onto multiple lines if it is too long to fit on a single line.");
  add("BS_NOTIFY", 0x4000, constant_category::button_style, "Enables a button to send BN_KILLFOCUS");
  add("BS_OWNERDRAW", 0x000B, constant_category::button_style, "Creates an owner-drawn button where the parent window is responsible for painting the button's appearance.");
  add("BS_PUSHBUTTON", 0x0000, constant_category::button_style, "Creates a standard push button that posts a WM_COMMAND message to its owner window when clicked.");
  add("BS_PUSHLIKE", 0x1000, constant_category::button_style, "Makes a checkbox or radio button look and act like a standard push button.");
  add("BS_RADIOBUTTON", 0x0004, constant_category::button_style, "Creates a small circle with adjacent text");
  add("BS_RIGHT", 0x0200, constant_category::button_style, "Right-aligns the text in the button rectangle.");
  add("BS_RIGHTBUTTON", 0x0020, constant_category::button_style, "Positions the button box on the right side of the text (same as BS_LEFTTEXT).");
  add("BS_TEXT", 0x0000, constant_category::button_style, "Specifies that the button displays text (default behavior).");
  add("BS_TOP", 0x0400, constant_category::button_style, "Places text at the top of the button rectangle.");
  add("BS_VCENTER", 0x0C00, constant_category::button_style, "Vertically centers text in the button rectangle.");
  add("BS_FLAT", 0x8000, constant_category::button_style, "Specifies that the button is drawn flat");

  /* ── ES_* Edit Control Styles ─────────────────────────────────── */
  add("ES_AUTOHSCROLL", 0x0080, constant_category::edit_style, "Automatically scrolls edit control text to the right when typing reaches the right border.");
  add("ES_AUTOVSCROLL", 0x0040, constant_category::edit_style, "Automatically scrolls edit control text up when pressing ENTER on the last visible line.");
  add("ES_CENTER", 0x0001, constant_category::edit_style, "Centers text in a single-line or multiline edit control.");
  add("ES_LEFT", 0x0000, constant_category::edit_style, "Left-aligns text in an edit control.");
  add("ES_LOWERCASE", 0x0010, constant_category::edit_style, "Converts all characters entered into the edit control to lowercase.");
  add("ES_MULTILINE", 0x0004, constant_category::edit_style, "Designates a multiline edit control allowing text entry on multiple lines.");
  add("ES_NOHIDESEL", 0x0100, constant_category::edit_style, "Prevents the edit control from hiding the text selection when it loses input focus.");
  add("ES_NUMBER", 0x2000, constant_category::edit_style, "Restricts the edit control input to digits only.");
  add("ES_OEMCONVERT", 0x0400, constant_category::edit_style, "Converts text typed in the edit control from ANSI to OEM character set and back.");
  add("ES_PASSWORD", 0x0020, constant_category::edit_style, "Displays an asterisk or custom mask character for every character typed into the edit control.");
  add("ES_READONLY", 0x0800, constant_category::edit_style, "Prevents the user from typing or editing text in the edit control.");
  add("ES_RIGHT", 0x0002, constant_category::edit_style, "Right-aligns text in a single-line or multiline edit control.");
  add("ES_UPPERCASE", 0x0008, constant_category::edit_style, "Converts all characters entered into the edit control to uppercase.");
  add("ES_WANTRETURN", 0x1000, constant_category::edit_style, "Inserts a carriage return when the user presses ENTER in a multiline edit control instead of submitting the dialog.");

  /* ── SS_* Static Control Styles ───────────────────────────────── */
  add("SS_BITMAP", 0x000E, constant_category::static_style, "Specifies that the static control displays a bitmap resource.");
  add("SS_BLACKFRAME", 0x0007, constant_category::static_style, "Displays a box drawn with a frame of the same color as window frames (default black).");
  add("SS_BLACKRECT", 0x0004, constant_category::static_style, "Displays a solid rectangle filled with the color used to draw window frames.");
  add("SS_CENTER", 0x0001, constant_category::static_style, "Designates a simple control displaying centered text wrapped automatically across lines.");
  add("SS_CENTERIMAGE", 0x0200, constant_category::static_style, "Centers the image or text vertically and horizontally inside the static control bounding area.");
  add("SS_EDITCONTROL", 0x2000, constant_category::static_style, "Mimics the text-wrapping and rendering behavior of a standard edit control in a static control.");
  add("SS_ENHMETAFILE", 0x000F, constant_category::static_style, "Specifies that the static control displays an enhanced metafile.");
  add("SS_ETCHEDFRAME", 0x0012, constant_category::static_style, "Draws the frame of the static control using the EDGE_ETCHED border style.");
  add("SS_ETCHEDHORZ", 0x0010, constant_category::static_style, "Draws a horizontal line using the EDGE_ETCHED style.");
  add("SS_ETCHEDVERT", 0x0011, constant_category::static_style, "Draws a vertical line using the EDGE_ETCHED style.");
  add("SS_GRAYFRAME", 0x0008, constant_category::static_style, "Displays a box drawn with a frame of the current desktop background color.");
  add("SS_GRAYRECT", 0x0005, constant_category::static_style, "Displays a solid rectangle filled with the current desktop background color.");
  add("SS_ICON", 0x0003, constant_category::static_style, "Displays an icon in the static control.");
  add("SS_LEFT", 0x0000, constant_category::static_style, "Left-aligns text in a static control with word wrapping.");
  add("SS_LEFTNOWORDWRAP", 0x000C, constant_category::static_style, "Left-aligns text without wrapping long lines; excess text is clipped.");
  add("SS_NOPREFIX", 0x0080, constant_category::static_style, "Prevents interpreting ampersands (&) as accelerator prefix characters in static control text.");
  add("SS_NOTIFY", 0x0100, constant_category::static_style, "Sends mouse click notification messages (STN_CLICKED");
  add("SS_OWNERDRAW", 0x000D, constant_category::static_style, "Delegates control painting responsibility to the parent window via WM_DRAWITEM.");
  add("SS_REALSIZECONTROL", 0x0040, constant_category::static_style, "Prevents resizing bitmap or icon images inside the static control");
  add("SS_REALSIZEIMAGE", 0x0800, constant_category::static_style, "Clips the image if it is larger than the static control bounds rather than scaling it.");
  add("SS_RIGHT", 0x0002, constant_category::static_style, "Right-aligns text in a static control with word wrapping.");
  add("SS_RIGHTJUST", 0x0400, constant_category::static_style, "Specifies that the upper-right corner of the static image/icon remains fixed when resized.");
  add("SS_SIMPLE", 0x000B, constant_category::static_style, "Displays a single line of left-aligned static text that cannot be wrapped or modified dynamically.");
  add("SS_SUNKEN", 0x1000, constant_category::static_style, "Draws a sunken 3D border around the static control.");
  add("SS_WHITEFRAME", 0x0009, constant_category::static_style, "Displays a box drawn with a frame matching the window background color (default white).");
  add("SS_WHITERECT", 0x0006, constant_category::static_style, "Displays a solid rectangle filled with the window background color.");
  add("SS_TYPEMASK", 0x0000001F, constant_category::static_style, "Bitmask used to isolate static control style types.");
  add("SS_ENDELLIPSIS", 0x00004000, constant_category::static_style, "Replaces characters at the end of static text with an ellipsis if they exceed control bounds.");
  add("SS_PATHELLIPSIS", 0x00008000, constant_category::static_style, "Replaces characters in the middle of a file path with an ellipsis to fit inside the static control.");
  add("SS_WORDELLIPSIS", 0x0000C000, constant_category::static_style, "Truncates text that extends beyond the control end and inserts an ellipsis after the last visible word.");

  /* ── LBS_* List Box Styles ────────────────────────────────────── */
  add("LBS_DISABLENOSCROLL", 0x1000, constant_category::listbox_style, "Shows a disabled vertical scroll bar in the list box when it contains too few items to scroll.");
  add("LBS_EXTENDEDSEL", 0x0800, constant_category::listbox_style, "Allows multiple selection using SHIFT and CTRL key combinations or mouse dragging.");
  add("LBS_HASSTRINGS", 0x0040, constant_category::listbox_style, "Specifies that an owner-drawn list box contains items consisting of strings.");
  add("LBS_MULTICOLUMN", 0x0200, constant_category::listbox_style, "Specifies a multi-column list box that scrolls horizontally.");
  add("LBS_MULTIPLESEL", 0x0008, constant_category::listbox_style, "Toggles item selection on each click without requiring SHIFT or CTRL modifier keys.");
  add("LBS_NODATA", 0x2000, constant_category::listbox_style, "Specifies a virtual list box");
  add("LBS_NOINTEGRALHEIGHT", 0x0100, constant_category::listbox_style, "Prevents the list box from automatically resizing to avoid showing partial items.");
  add("LBS_NOREDRAW", 0x0004, constant_category::listbox_style, "Prevents updating the list box visual representation when changes are made.");
  add("LBS_NOSEL", 0x4000, constant_category::listbox_style, "Specifies that items in the list box can be viewed but not selected.");
  add("LBS_NOTIFY", 0x0001, constant_category::listbox_style, "Sends notification messages to the parent window whenever the user clicks or double-clicks an item.");
  add("LBS_OWNERDRAWFIXED", 0x0010, constant_category::listbox_style, "Specifies an owner-drawn list box where all items have the same uniform height.");
  add("LBS_OWNERDRAWVARIABLE", 0x0020, constant_category::listbox_style, "Specifies an owner-drawn list box where individual items can vary in height.");
  add("LBS_SORT", 0x0002, constant_category::listbox_style, "Alphabetically sorts items added to the list box.");
  add("LBS_STANDARD", 0xA00003, constant_category::listbox_style, "Combines LBS_NOTIFY");
  add("LBS_USETABSTOPS", 0x0080, constant_category::listbox_style, "Enables the list box to recognize and expand tab characters within item strings.");
  add("LBS_WANTKEYBOARDINPUT", 0x0400, constant_category::listbox_style, "Sends WM_VKEYTOITEM messages to the parent window whenever a key is pressed while the list box has focus.");

  /* ── CBS_* Combo Box Styles ───────────────────────────────────── */
  add("CBS_AUTOHSCROLL", 0x0040, constant_category::combobox_style, "Automatically scrolls text in the combo box edit control horizontally when typed past the right edge.");
  add("CBS_DISABLENOSCROLL", 0x0800, constant_category::combobox_style, "Shows a disabled scroll bar in the drop-down list when it does not contain enough items to scroll.");
  add("CBS_DROPDOWN", 0x0002, constant_category::combobox_style, "Displays a drop-down list with an editable text field.");
  add("CBS_DROPDOWNLIST", 0x0003, constant_category::combobox_style, "Displays a drop-down list where the user can only select existing items (edit box is read-only).");
  add("CBS_HASSTRINGS", 0x0200, constant_category::combobox_style, "Specifies that an owner-drawn combo box contains items consisting of strings.");
  add("CBS_LOWERCASE", 0x4000, constant_category::combobox_style, "Converts all entered text in the combo box edit field to lowercase.");
  add("CBS_NOINTEGRALHEIGHT", 0x0400, constant_category::combobox_style, "Prevents the drop-down list from resizing automatically to show only complete items.");
  add("CBS_OEMCONVERT", 0x0080, constant_category::combobox_style, "Converts character sets from ANSI to OEM for text entered into the combo box edit field.");
  add("CBS_OWNERDRAWFIXED", 0x0010, constant_category::combobox_style, "Delegates drop-down item drawing to the parent window");
  add("CBS_OWNERDRAWVARIABLE", 0x0020, constant_category::combobox_style, "Delegates item drawing to the parent window");
  add("CBS_SIMPLE", 0x0001, constant_category::combobox_style, "Displays the list box at all times underneath the edit control.");
  add("CBS_SORT", 0x0100, constant_category::combobox_style, "Alphabetically sorts items added to the combo box list.");
  add("CBS_UPPERCASE", 0x2000, constant_category::combobox_style, "Converts all entered text in the combo box edit field to uppercase.");

  /* ── SBS_* Scroll Bar Styles ──────────────────────────────────── */
  add("SBS_BOTTOMALIGN", 0x0004, constant_category::scrollbar_style, "Aligns the scroll bar along the bottom edge of the defined rectangle.");
  add("SBS_HORZ", 0x0000, constant_category::scrollbar_style, "Specifies a horizontal scroll bar.");
  add("SBS_LEFTALIGN", 0x0002, constant_category::scrollbar_style, "Aligns the scroll bar along the left edge of the defined rectangle.");
  add("SBS_RIGHTALIGN", 0x0004, constant_category::scrollbar_style, "Aligns the scroll bar along the right edge of the defined rectangle.");
  add("SBS_SIZEBOX", 0x0008, constant_category::scrollbar_style, "Specifies a sizing box control instead of a scroll bar.");
  add("SBS_SIZEBOXBOTTOMRIGHTALIGN", 0x0004, constant_category::scrollbar_style, "Aligns the size box with the bottom-right corner of the window.");
  add("SBS_SIZEBOXTOPLEFTALIGN", 0x0002, constant_category::scrollbar_style, "Aligns the size box with the top-left corner of the window.");
  add("SBS_SIZEGRIP", 0x0010, constant_category::scrollbar_style, "Displays a sizing box featuring a visible grip pattern.");
  add("SBS_TOPALIGN", 0x0002, constant_category::scrollbar_style, "Aligns the scroll bar along the top edge of the defined rectangle.");
  add("SBS_VERT", 0x0001, constant_category::scrollbar_style, "Specifies a vertical scroll bar.");

  /* ── LVS_* List-View Styles ───────────────────────────────────── */
  add("LVS_ALIGNLEFT", 0x0800, constant_category::listview_style, "Left-aligns items in the list-view control.");
  add("LVS_ALIGNMASK", 0x0c00, constant_category::listview_style, "Mask used to retrieve the alignment style bits of a list-view control.");
  add("LVS_ALIGNTOP", 0x0000, constant_category::listview_style, "Top-aligns items in the list-view control.");
  add("LVS_AUTOARRANGE", 0x0100, constant_category::listview_style, "Automatically arranges items in icon view when the list-view window content changes.");
  add("LVS_EDITLABELS", 0x0080, constant_category::listview_style, "Allows item text to be edited in-place by the user.");
  add("LVS_ICON", 0x0000, constant_category::listview_style, "Displays items as large icons with labels.");
  add("LVS_LIST", 0x0003, constant_category::listview_style, "Displays items as a small-icon list with labels.");
  add("LVS_NOCOLUMNHEADER", 0x4000, constant_category::listview_style, "Hides the column header in report view.");
  add("LVS_NOLABELWRAP", 0x0080, constant_category::listview_style, "Prevents item labels from wrapping in icon view; text is truncated instead.");
  add("LVS_NOSCROLL", 0x2000, constant_category::listview_style, "Disables scrolling in the list-view control.");
  add("LVS_NOSORTHEADER", 0x8000, constant_category::listview_style, "Disables column header click sorting in report view.");
  add("LVS_OWNERDATA", 0x1000, constant_category::listview_style, "Creates a virtual list-view control where the application supplies item data on demand.");
  add("LVS_REPORT", 0x0001, constant_category::listview_style, "Displays items in report view with columns.");
  add("LVS_SHAREIMAGELISTS", 0x0040, constant_category::listview_style, "Does not destroy the image list when the control is destroyed; allows sharing image lists.");
  add("LVS_SHOWSELALWAYS", 0x0008, constant_category::listview_style, "Keeps the selection visible even when the control loses focus.");
  add("LVS_SINGLESEL", 0x0004, constant_category::listview_style, "Allows only one item to be selected at a time.");
  add("LVS_SORTASCENDING", 0x0010, constant_category::listview_style, "Sorts items in ascending order using the current sort column.");
  add("LVS_SORTDESCENDING", 0x0020, constant_category::listview_style, "Sorts items in descending order using the current sort column.");
  add("LVS_TYPEMASK", 0x0003, constant_category::listview_style, "Mask used to retrieve the view-type bits of a list-view control.");
  add("LVS_TYPESTYLEMASK", 0xfc00, constant_category::listview_style, "Mask used to retrieve the type and style bits that affect arranging in list-view.");

  /* ── TVS_* Tree-View Styles ──────────────────────────────────── */
  add("TVS_CHECKBOXES", 0x0100, constant_category::treeview_style, "Displays check boxes next to each item in the tree-view control.");
  add("TVS_DISABLEDRAGDROP", 0x0010, constant_category::treeview_style, "Prevents the user from dragging items in the tree-view control.");
  add("TVS_EDITLABELS", 0x0008, constant_category::treeview_style, "Allows item text to be edited in-place by the user.");
  add("TVS_FULLROWSELECT", 0x1000, constant_category::treeview_style, "Selects the entire row of the item when clicked, not just the icon or text.");
  add("TVS_HASBUTTONS", 0x0001, constant_category::treeview_style, "Displays plus/minus buttons next to parent items for expanding and collapsing.");
  add("TVS_HASLINES", 0x0002, constant_category::treeview_style, "Displays lines connecting parent items to their children in the tree.");
  add("TVS_LINESATROOT", 0x0004, constant_category::treeview_style, "Displays lines connecting root-level items to each other.");
  add("TVS_NOHSCROLL", 0x8000, constant_category::treeview_style, "Disables horizontal scrolling in the tree-view control.");
  add("TVS_NOSCROLL", 0x2000, constant_category::treeview_style, "Disables both horizontal and vertical scrolling in the tree-view control.");
  add("TVS_NONEVENHEIGHT", 0x4000, constant_category::treeview_style, "Allows tree-view items to have odd heights; otherwise items are forced to even heights.");
  add("TVS_RTLREADING", 0x0040, constant_category::treeview_style, "Displays text using right-to-left reading order.");
  add("TVS_SHOWSELALWAYS", 0x0020, constant_category::treeview_style, "Keeps the selected item visible even when the control loses focus.");
  add("TVS_SINGLEEXPAND", 0x0400, constant_category::treeview_style, "Expands the selected item when it is expanded; collapses other expanded items.");
  add("TVS_TRACKSELECT", 0x0200, constant_category::treeview_style, "Enables hot-tracking of tree-view items as the mouse pointer moves over them.");

  /* ── TBS_* Trackbar Styles ───────────────────────────────────── */
  add("TBS_AUTOTICKS", 0x0001, constant_category::trackbar_style, "Automatically creates tick marks at each increment along the trackbar.");
  add("TBS_BOTH", 0x0008, constant_category::trackbar_style, "Displays tick marks on both sides of the trackbar.");
  add("TBS_BOTTOM", 0x0000, constant_category::trackbar_style, "Displays tick marks on the bottom of the trackbar.");
  add("TBS_ENABLESELRANGE", 0x0020, constant_category::trackbar_style, "Enables a selection range in the trackbar control.");
  add("TBS_FIXEDLENGTH", 0x0040, constant_category::trackbar_style, "Allows the trackbar thumb to have a fixed size, not scaled to the trackbar range.");
  add("TBS_HORZ", 0x0000, constant_category::trackbar_style, "Displays a horizontal trackbar.");
  add("TBS_LEFT", 0x0004, constant_category::trackbar_style, "Displays tick marks on the left side of the trackbar.");
  add("TBS_NOTHUMB", 0x0080, constant_category::trackbar_style, "Hides the trackbar thumb (slider) so the user cannot drag it.");
  add("TBS_NOTICKS", 0x0010, constant_category::trackbar_style, "Hides the tick marks along the trackbar.");
  add("TBS_TOP", 0x0004, constant_category::trackbar_style, "Displays tick marks on the top of the trackbar.");
  add("TBS_VERT", 0x0002, constant_category::trackbar_style, "Displays a vertical trackbar.");

  /* ── PBS_* Progress Bar Styles ───────────────────────────────── */
  add("PBS_MARQUEE", 0x0008, constant_category::progressbar_style, "Enables marquee mode where the progress bar animates continuously without a specific position.");
  add("PBS_SMOOTH", 0x0001, constant_category::progressbar_style, "Fills the progress bar with a smooth bar rather than segmented blocks.");
  add("PBS_SMOOTHREVERSE", 0x0010, constant_category::progressbar_style, "Fills the progress bar with a smooth bar in reverse direction for smooth animation.");
  add("PBS_VERTICAL", 0x0004, constant_category::progressbar_style, "Displays the progress bar vertically.");

  /* ── UDS_* Up-Down Control Styles ────────────────────────────── */
  add("UDS_ALIGNLEFT", 0x0008, constant_category::updown_style, "Aligns the up-down control to the left of the buddy edit control.");
  add("UDS_ALIGNRIGHT", 0x0004, constant_category::updown_style, "Aligns the up-down control to the right of the buddy edit control.");
  add("UDS_ARROWKEYS", 0x0020, constant_category::updown_style, "Enables arrow key support to increment or decrement the up-down control value.");
  add("UDS_AUTOBUDDY", 0x0010, constant_category::updown_style, "Automatically selects the previous control in the dialog template as the buddy window.");
  add("UDS_HORZ", 0x0040, constant_category::updown_style, "Displays the up and down arrows horizontally instead of vertically.");
  add("UDS_NOTHOUSANDS", 0x0080, constant_category::updown_style, "Does not insert a thousands separator in the buddy edit control.");
  add("UDS_SETBUDDYINT", 0x0002, constant_category::updown_style, "Sets the buddy edit control text to an integer value whenever the up-down control changes.");
  add("UDS_WRAP", 0x0001, constant_category::updown_style, "Causes the control to wrap around to the other end when reaching the maximum or minimum value.");

  /* ── DTS_* Date/Time Picker Styles ───────────────────────────── */
  add("DTS_APPCANPARSE", 0x0010, constant_category::datetimepicker_style, "Enables the owner to parse user input in the date/time picker control.");
  add("DTS_LONGDATEFORMAT", 0x0004, constant_category::datetimepicker_style, "Uses the long date format string as specified in the user's locale settings.");
  add("DTS_RIGHTALIGN", 0x0020, constant_category::datetimepicker_style, "Right-aligns the drop-down calendar with the control.");
  add("DTS_SHOWNONE", 0x0002, constant_category::datetimepicker_style, "Displays a check box next to the control; when unchecked, no date is selected.");
  add("DTS_TIMEFORMAT", 0x0008, constant_category::datetimepicker_style, "Uses the time format string as specified in the user's locale settings.");
  add("DTS_UPDOWN", 0x0001, constant_category::datetimepicker_style, "Displays an up-down control rather than a drop-down calendar for date selection.");

  /* ── TCS_* Tab Control Styles ────────────────────────────────── */
  add("TCS_BOTTOM", 0x0002, constant_category::tabcontrol_style, "Places tabs at the bottom of the control.");
  add("TCS_BUTTONS", 0x0002, constant_category::tabcontrol_style, "Displays tabs as push buttons.");
  add("TCS_EX_FLATSEPARATORS", 0x00000001, constant_category::tabcontrol_style, "Draws flat separators between tabs in the tab control.");
  add("TCS_EX_REGISTERDROP", 0x00000002, constant_category::tabcontrol_style, "Enables tab items to receive WM_QUERYDROPPOSITION notifications.");
  add("TCS_FIXEDWIDTH", 0x0040, constant_category::tabcontrol_style, "All tabs are the same width; individual text length is ignored.");
  add("TCS_FLATBUTTONS", 0x0008, constant_category::tabcontrol_style, "Displays tabs as flat push buttons in tab controls without borders.");
  add("TCS_FOCUSNEVER", 0x0800, constant_category::tabcontrol_style, "Prevents the tab control from receiving input focus.");
  add("TCS_FOCUSONBUTTONDOWN", 0x0100, constant_category::tabcontrol_style, "Gives input focus to the selected tab when the user clicks on a tab.");
  add("TCS_FORCESELECTLEFT", 0x0010, constant_category::tabcontrol_style, "Forces the selected tab to be the leftmost visible tab when the tab control gains focus.");
  add("TCS_MULTILINE", 0x0100, constant_category::tabcontrol_style, "Displays multiple rows of tabs if necessary so all tabs are visible.");
  add("TCS_MULTISELECT", 0x0004, constant_category::tabcontrol_style, "Enables multiple tab selection using CTRL+click in the tab control.");
  add("TCS_OWNERDRAWFIXED", 0x0200, constant_category::tabcontrol_style, "Enables the tab control owner to draw tab items in a fixed size.");
  add("TCS_RAGGEDRIGHT", 0x0080, constant_category::tabcontrol_style, "Prevents tabs from being stretched to fill the tab control width.");
  add("TCS_RIGHTJUSTIFY", 0x0020, constant_category::tabcontrol_style, "Right-justifies tabs when the tab control is multiline.");
  add("TCS_SCROLLOP", 0x0001, constant_category::tabcontrol_style, "Enables horizontal scrolling of multiple rows of tabs.");
  add("TCS_TOOLTIPS", 0x0400, constant_category::tabcontrol_style, "Enables tooltips for each tab in the tab control.");

  /* ── VK_* Virtual Key Codes ───────────────────────────────────── */
  add("VK_LBUTTON", 0x01, constant_category::virtual_key, "Virtual key code representing the left mouse button.");
  add("VK_RBUTTON", 0x02, constant_category::virtual_key, "Virtual key code representing the right mouse button.");
  add("VK_CANCEL", 0x03, constant_category::virtual_key, "Virtual key code representing Control-break processing.");
  add("VK_MBUTTON", 0x04, constant_category::virtual_key, "Virtual key code representing the middle mouse button.");
  add("VK_XBUTTON1", 0x05, constant_category::virtual_key, "Virtual key code representing the first X mouse button (back button on multi-button mice).");
  add("VK_XBUTTON2", 0x06, constant_category::virtual_key, "Virtual key code representing the second X mouse button (forward button on multi-button mice).");
  add("VK_BACK", 0x08, constant_category::virtual_key, "Virtual key code representing the BACKSPACE key.");
  add("VK_TAB", 0x09, constant_category::virtual_key, "Virtual key code representing the TAB key.");
  add("VK_CLEAR", 0x0C, constant_category::virtual_key, "Virtual key code representing the CLEAR key (typically Numpad 5 with Num Lock off).");
  add("VK_RETURN", 0x0D, constant_category::virtual_key, "Virtual key code representing the ENTER key.");
  add("VK_SHIFT", 0x10, constant_category::virtual_key, "Virtual key code representing the SHIFT key.");
  add("VK_CONTROL", 0x11, constant_category::virtual_key, "Virtual key code representing the CTRL key.");
  add("VK_MENU", 0x12, constant_category::virtual_key, "Virtual key code representing the ALT key.");
  add("VK_PAUSE", 0x13, constant_category::virtual_key, "Virtual key code representing the PAUSE key.");
  add("VK_CAPITAL", 0x14, constant_category::virtual_key, "Virtual key code representing the CAPS LOCK key.");
  add("VK_ESCAPE", 0x1B, constant_category::virtual_key, "Virtual key code representing the ESC key.");
  add("VK_SPACE", 0x20, constant_category::virtual_key, "Virtual key code representing the SPACEBAR key.");
  add("VK_PRIOR", 0x21, constant_category::virtual_key, "Virtual key code representing the PAGE UP key.");
  add("VK_NEXT", 0x22, constant_category::virtual_key, "Virtual key code representing the PAGE DOWN key.");
  add("VK_END", 0x23, constant_category::virtual_key, "Virtual key code representing the END key.");
  add("VK_HOME", 0x24, constant_category::virtual_key, "Virtual key code representing the HOME key.");
  add("VK_LEFT", 0x25, constant_category::virtual_key, "Virtual key code representing the LEFT ARROW key.");
  add("VK_UP", 0x26, constant_category::virtual_key, "Virtual key code representing the UP ARROW key.");
  add("VK_RIGHT", 0x27, constant_category::virtual_key, "Virtual key code representing the RIGHT ARROW key.");
  add("VK_DOWN", 0x28, constant_category::virtual_key, "Virtual key code representing the DOWN ARROW key.");
  add("VK_SELECT", 0x29, constant_category::virtual_key, "Virtual key code representing the SELECT key.");
  add("VK_PRINT", 0x2A, constant_category::virtual_key, "Virtual key code representing the PRINT key.");
  add("VK_EXECUTE", 0x2B, constant_category::virtual_key, "Virtual key code representing the EXECUTE key.");
  add("VK_SNAPSHOT", 0x2C, constant_category::virtual_key, "Virtual key code representing the PRINT SCREEN key.");
  add("VK_INSERT", 0x2D, constant_category::virtual_key, "Virtual key code representing the INS key.");
  add("VK_DELETE", 0x2E, constant_category::virtual_key, "Virtual key code representing the DEL key.");
  add("VK_HELP", 0x2F, constant_category::virtual_key, "Virtual key code representing the HELP key.");
  add("VK_0", 0x30, constant_category::virtual_key, "Virtual key code representing the '0' key.");
  add("VK_1", 0x31, constant_category::virtual_key, "Virtual key code representing the '1' key.");
  add("VK_2", 0x32, constant_category::virtual_key, "Virtual key code representing the '2' key.");
  add("VK_3", 0x33, constant_category::virtual_key, "Virtual key code representing the '3' key.");
  add("VK_4", 0x34, constant_category::virtual_key, "Virtual key code representing the '4' key.");
  add("VK_5", 0x35, constant_category::virtual_key, "Virtual key code representing the '5' key.");
  add("VK_6", 0x36, constant_category::virtual_key, "Virtual key code representing the '6' key.");
  add("VK_7", 0x37, constant_category::virtual_key, "Virtual key code representing the '7' key.");
  add("VK_8", 0x38, constant_category::virtual_key, "Virtual key code representing the '8' key.");
  add("VK_9", 0x39, constant_category::virtual_key, "Virtual key code representing the '9' key.");
  add("VK_A", 0x41, constant_category::virtual_key, "Virtual key code representing the 'A' key.");
  add("VK_B", 0x42, constant_category::virtual_key, "Virtual key code representing the 'B' key.");
  add("VK_C", 0x43, constant_category::virtual_key, "Virtual key code representing the 'C' key.");
  add("VK_D", 0x44, constant_category::virtual_key, "Virtual key code representing the 'D' key.");
  add("VK_E", 0x45, constant_category::virtual_key, "Virtual key code representing the 'E' key.");
  add("VK_F", 0x46, constant_category::virtual_key, "Virtual key code representing the 'F' key.");
  add("VK_G", 0x47, constant_category::virtual_key, "Virtual key code representing the 'G' key.");
  add("VK_H", 0x48, constant_category::virtual_key, "Virtual key code representing the 'H' key.");
  add("VK_I", 0x49, constant_category::virtual_key, "Virtual key code representing the 'I' key.");
  add("VK_J", 0x4A, constant_category::virtual_key, "Virtual key code representing the 'J' key.");
  add("VK_K", 0x4B, constant_category::virtual_key, "Virtual key code representing the 'K' key.");
  add("VK_L", 0x4C, constant_category::virtual_key, "Virtual key code representing the 'L' key.");
  add("VK_M", 0x4D, constant_category::virtual_key, "Virtual key code representing the 'M' key.");
  add("VK_N", 0x4E, constant_category::virtual_key, "Virtual key code representing the 'N' key.");
  add("VK_O", 0x4F, constant_category::virtual_key, "Virtual key code representing the 'O' key.");
  add("VK_P", 0x50, constant_category::virtual_key, "Virtual key code representing the 'P' key.");
  add("VK_Q", 0x51, constant_category::virtual_key, "Virtual key code representing the 'Q' key.");
  add("VK_R", 0x52, constant_category::virtual_key, "Virtual key code representing the 'R' key.");
  add("VK_S", 0x53, constant_category::virtual_key, "Virtual key code representing the 'S' key.");
  add("VK_T", 0x54, constant_category::virtual_key, "Virtual key code representing the 'T' key.");
  add("VK_U", 0x55, constant_category::virtual_key, "Virtual key code representing the 'U' key.");
  add("VK_V", 0x56, constant_category::virtual_key, "Virtual key code representing the 'V' key.");
  add("VK_W", 0x57, constant_category::virtual_key, "Virtual key code representing the 'W' key.");
  add("VK_X", 0x58, constant_category::virtual_key, "Virtual key code representing the 'X' key.");
  add("VK_Y", 0x59, constant_category::virtual_key, "Virtual key code representing the 'Y' key.");
  add("VK_Z", 0x5A, constant_category::virtual_key, "Virtual key code representing the 'Z' key.");
  add("VK_LWIN", 0x5B, constant_category::virtual_key, "Virtual key code representing the Left Windows key.");
  add("VK_RWIN", 0x5C, constant_category::virtual_key, "Virtual key code representing the Right Windows key.");
  add("VK_APPS", 0x5D, constant_category::virtual_key, "Virtual key code representing the Applications key (context menu key).");
  add("VK_SLEEP", 0x5F, constant_category::virtual_key, "Virtual key code representing the Computer Sleep key.");
  add("VK_NUMPAD0", 0x60, constant_category::virtual_key, "Virtual key code representing the '0' key on the numeric keypad.");
  add("VK_NUMPAD1", 0x61, constant_category::virtual_key, "Virtual key code representing the '1' key on the numeric keypad.");
  add("VK_NUMPAD2", 0x62, constant_category::virtual_key, "Virtual key code representing the '2' key on the numeric keypad.");
  add("VK_NUMPAD3", 0x63, constant_category::virtual_key, "Virtual key code representing the '3' key on the numeric keypad.");
  add("VK_NUMPAD4", 0x64, constant_category::virtual_key, "Virtual key code representing the '4' key on the numeric keypad.");
  add("VK_NUMPAD5", 0x65, constant_category::virtual_key, "Virtual key code representing the '5' key on the numeric keypad.");
  add("VK_NUMPAD6", 0x66, constant_category::virtual_key, "Virtual key code representing the '6' key on the numeric keypad.");
  add("VK_NUMPAD7", 0x67, constant_category::virtual_key, "Virtual key code representing the '7' key on the numeric keypad.");
  add("VK_NUMPAD8", 0x68, constant_category::virtual_key, "Virtual key code representing the '8' key on the numeric keypad.");
  add("VK_NUMPAD9", 0x69, constant_category::virtual_key, "Virtual key code representing the '9' key on the numeric keypad.");
  add("VK_MULTIPLY", 0x6A, constant_category::virtual_key, "Virtual key code representing the Multiply key (*) on the numeric keypad.");
  add("VK_ADD", 0x6B, constant_category::virtual_key, "Virtual key code representing the Add key (+) on the numeric keypad.");
  add("VK_SEPARATOR", 0x6C, constant_category::virtual_key, "Virtual key code representing the Separator key.");
  add("VK_SUBTRACT", 0x6D, constant_category::virtual_key, "Virtual key code representing the Subtract key (-) on the numeric keypad.");
  add("VK_DECIMAL", 0x6E, constant_category::virtual_key, "Virtual key code representing the Decimal key (.) on the numeric keypad.");
  add("VK_DIVIDE", 0x6F, constant_category::virtual_key, "Virtual key code representing the Divide key (/) on the numeric keypad.");
  add("VK_F1", 0x70, constant_category::virtual_key, "Virtual key code representing the F1 function key.");
  add("VK_F2", 0x71, constant_category::virtual_key, "Virtual key code representing the F2 function key.");
  add("VK_F3", 0x72, constant_category::virtual_key, "Virtual key code representing the F3 function key.");
  add("VK_F4", 0x73, constant_category::virtual_key, "Virtual key code representing the F4 function key.");
  add("VK_F5", 0x74, constant_category::virtual_key, "Virtual key code representing the F5 function key.");
  add("VK_F6", 0x75, constant_category::virtual_key, "Virtual key code representing the F6 function key.");
  add("VK_F7", 0x76, constant_category::virtual_key, "Virtual key code representing the F7 function key.");
  add("VK_F8", 0x77, constant_category::virtual_key, "Virtual key code representing the F8 function key.");
  add("VK_F9", 0x78, constant_category::virtual_key, "Virtual key code representing the F9 function key.");
  add("VK_F10", 0x79, constant_category::virtual_key, "Virtual key code representing the F10 function key.");
  add("VK_F11", 0x7A, constant_category::virtual_key, "Virtual key code representing the F11 function key.");
  add("VK_F12", 0x7B, constant_category::virtual_key, "Virtual key code representing the F12 function key.");
  add("VK_NUMLOCK", 0x90, constant_category::virtual_key, "Virtual key code representing the NUM LOCK key.");
  add("VK_SCROLL", 0x91, constant_category::virtual_key, "Virtual key code representing the SCROLL LOCK key.");
  add("VK_LSHIFT", 0xA0, constant_category::virtual_key, "Virtual key code representing the Left SHIFT key.");
  add("VK_RSHIFT", 0xA1, constant_category::virtual_key, "Virtual key code representing the Right SHIFT key.");
  add("VK_LCONTROL", 0xA2, constant_category::virtual_key, "Virtual key code representing the Left CTRL key.");
  add("VK_RCONTROL", 0xA3, constant_category::virtual_key, "Virtual key code representing the Right CTRL key.");
  add("VK_LMENU", 0xA4, constant_category::virtual_key, "Virtual key code representing the Left ALT key.");
  add("VK_RMENU", 0xA5, constant_category::virtual_key, "Virtual key code representing the Right ALT key.");

  /* ── WM_* Window Messages ─────────────────────────────────────── */
  add("WM_NULL", 0x0000, constant_category::window_message, "Performs no operation. Used as a placeholder or to test message routing.");
  add("WM_CREATE", 0x0001, constant_category::window_message, "Sent when an application requests that a window be created by calling CreateWindowEx.");
  add("WM_DESTROY", 0x0002, constant_category::window_message, "Sent when a window is being destroyed");
  add("WM_MOVE", 0x0003, constant_category::window_message, "Sent after a window has been moved.");
  add("WM_SIZE", 0x0005, constant_category::window_message, "Sent to a window after its size has changed.");
  add("WM_ACTIVATE", 0x0006, constant_category::window_message, "Sent to both the window being activated and the window being deactivated.");
  add("WM_SETFOCUS", 0x0007, constant_category::window_message, "Sent to a window after it has gained keyboard focus.");
  add("WM_KILLFOCUS", 0x0008, constant_category::window_message, "Sent to a window immediately before it loses keyboard focus.");
  add("WM_ENABLE", 0x000A, constant_category::window_message, "Sent when an application changes the enabled state of a window.");
  add("WM_SETREDRAW", 0x000B, constant_category::window_message, "Sent to a window to allow or prevent changes in that window from being redrawn.");
  add("WM_SETTEXT", 0x000C, constant_category::window_message, "Sets the text of a window (title bar or control text).");
  add("WM_GETTEXT", 0x000D, constant_category::window_message, "Copies the text corresponding to a window into a caller-supplied buffer.");
  add("WM_GETTEXTLENGTH", 0x000E, constant_category::window_message, "Determines the length");
  add("WM_PAINT", 0x000F, constant_category::window_message, "Sent when the system or another application makes a request to paint a portion of an application's window.");
  add("WM_CLOSE", 0x0010, constant_category::window_message, "Sent as a signal that a window or an application should terminate.");
  add("WM_QUERYENDSESSION", 0x0011, constant_category::window_message, "Sent when the user chooses to end the session or when an application calls one of the shutdown functions.");
  add("WM_QUIT", 0x0012, constant_category::window_message, "Indicates a request to terminate an application and is generated when PostQuitMessage is called.");
  add("WM_ERASEBKGND", 0x0014, constant_category::window_message, "Sent when the window background must be erased (for example");
  add("WM_SHOWWINDOW", 0x0018, constant_category::window_message, "Sent to a window when it is about to be hidden or shown.");
  add("WM_ACTIVATEAPP", 0x001C, constant_category::window_message, "Sent when a window belonging to a different application than the active window is about to be activated.");
  add("WM_CANCELMODE", 0x001F, constant_category::window_message, "Sent to cancel certain modes");
  add("WM_SETCURSOR", 0x0020, constant_category::window_message, "Sent to a window if the mouse cursor moves within a window and mouse input is not captured.");
  add("WM_MOUSEACTIVATE", 0x0021, constant_category::window_message, "Sent when the cursor is in an inactive window and the user presses a mouse button.");
  add("WM_CHILDACTIVATE", 0x0022, constant_category::window_message, "Sent to a child window when the user clicks its title bar or when the window is created");
  add("WM_GETMINMAXINFO", 0x0024, constant_category::window_message, "Sent to a window when the size or position is about to change");
  add("WM_SETFONT", 0x0030, constant_category::window_message, "Sets the font that a control is to use when drawing text.");
  add("WM_GETFONT", 0x0031, constant_category::window_message, "Retrieves the font with which a control is currently drawing its text.");
  add("WM_SETHOTKEY", 0x0032, constant_category::window_message, "Associates a hot key with a window.");
  add("WM_GETHOTKEY", 0x0033, constant_category::window_message, "Determines the hot key associated with a window.");
  add("WM_QUERYDRAGICON", 0x0037, constant_category::window_message, "Sent to a minimized window that does not have an icon defined for its class so the system can display a cursor while dragging.");
  add("WM_COMPAREITEM", 0x0039, constant_category::window_message, "Sent to determine the relative position of a new item in a sorted owner-drawn list box or combo box.");
  add("WM_WINDOWPOSCHANGING", 0x0046, constant_category::window_message, "Sent to a window whose size");
  add("WM_WINDOWPOSCHANGED", 0x0047, constant_category::window_message, "Sent to a window whose size");
  add("WM_COPYDATA", 0x004A, constant_category::window_message, "Sends data from one application to another using inter-process communication.");
  add("WM_NOTIFY", 0x004E, constant_category::window_message, "Sent by a common control to its parent window when an event has occurred or the control requires information.");
  add("WM_CONTEXTMENU", 0x007B, constant_category::window_message, "Notifies a window that the user clicked the right mouse button (context menu request) in the window.");
  add("WM_STYLECHANGING", 0x007C, constant_category::window_message, "Sent to a window when the SetWindowLong function is about to change one or more of the window's styles.");
  add("WM_STYLECHANGED", 0x007D, constant_category::window_message, "Sent to a window after SetWindowLong has changed one or more of the window's styles.");
  add("WM_DISPLAYCHANGE", 0x007E, constant_category::window_message, "Sent to all windows when the display resolution has changed.");
  add("WM_GETICON", 0x007F, constant_category::window_message, "Sent to a window to retrieve a handle to the large or small icon associated with the window.");
  add("WM_SETICON", 0x0080, constant_category::window_message, "Associates a new large or small icon with a window.");
  add("WM_NCCREATE", 0x0081, constant_category::window_message, "Sent prior to WM_CREATE when a window is first created to initialize non-client area elements.");
  add("WM_NCDESTROY", 0x0082, constant_category::window_message, "Notifies a window that its non-client area is being destroyed; this is the final message received by a window.");
  add("WM_NCCALCSIZE", 0x0083, constant_category::window_message, "Sent when the size and position of a window's client area must be calculated.");
  add("WM_NCHITTEST", 0x0084, constant_category::window_message, "Sent to a window in order to determine what part of the window corresponds to a particular screen coordinate.");
  add("WM_NCPAINT", 0x0085, constant_category::window_message, "Sent to a window when its frame (non-client area) must be painted.");
  add("WM_NCACTIVATE", 0x0086, constant_category::window_message, "Sent to a window when its non-client area needs to be changed to indicate an active or inactive state.");
  add("WM_GETDLGCODE", 0x0087, constant_category::window_message, "Sent to the window procedure associated with a control to allow custom processing of direction and TAB keys.");
  add("WM_KEYDOWN", 0x0100, constant_category::window_message, "Posted to the window with the keyboard focus when a non-system key is pressed.");
  add("WM_KEYUP", 0x0101, constant_category::window_message, "Posted to the window with the keyboard focus when a non-system key is released.");
  add("WM_CHAR", 0x0102, constant_category::window_message, "Posted to the window with the keyboard focus when a WM_KEYDOWN message is translated by TranslateMessage.");
  add("WM_DEADCHAR", 0x0103, constant_category::window_message, "Posted when a WM_KEYUP message is translated by TranslateMessage for dead keys (such as accent marks).");
  add("WM_SYSKEYDOWN", 0x0104, constant_category::window_message, "Posted when the user presses the ALT key alongside another key or types when no window has focus.");
  add("WM_SYSKEYUP", 0x0105, constant_category::window_message, "Posted when the user releases a key that was pressed while the ALT key was held down.");
  add("WM_SYSCHAR", 0x0106, constant_category::window_message, "Posted when a WM_SYSKEYDOWN message is translated by the TranslateMessage function.");
  add("WM_SYSDEADCHAR", 0x0107, constant_category::window_message, "Sent when a WM_SYSKEYDOWN message is translated for a character that represents a dead key.");
  add("WM_INITDIALOG", 0x0110, constant_category::window_message, "Sent to the dialog box procedure immediately before a dialog box is displayed.");
  add("WM_COMMAND", 0x0111, constant_category::window_message, "Sent when the user selects a command item from a menu");
  add("WM_SYSCOMMAND", 0x0112, constant_category::window_message, "Sent when the user selects a command from the Window menu or clicks the maximize");
  add("WM_TIMER", 0x0113, constant_category::window_message, "Posted to the installing thread's message queue when a timer set by SetTimer expires.");
  add("WM_HSCROLL", 0x0114, constant_category::window_message, "Sent to a window when an event occurs in the window's standard horizontal scroll bar or horizontal scroll control.");
  add("WM_VSCROLL", 0x0115, constant_category::window_message, "Sent to a window when an event occurs in the window's standard vertical scroll bar or vertical scroll control.");
  add("WM_INITMENU", 0x0116, constant_category::window_message, "Sent when a menu is about to become active");
  add("WM_INITMENUPOPUP", 0x0117, constant_category::window_message, "Sent when a pop-up menu or submenu is about to become active");
  add("WM_MENUSELECT", 0x011F, constant_category::window_message, "Sent to a menu's owner window when the user selects a menu item.");
  add("WM_MENUCHAR", 0x0120, constant_category::window_message, "Sent when a menu is active and the user presses a key that does not match any predefined mnemonic or accelerator.");
  add("WM_ENTERIDLE", 0x0121, constant_category::window_message, "Sent to an owner window when a modal dialog box or menu enters an idle state.");
  add("WM_MENURBUTTONUP", 0x0122, constant_category::window_message, "Sent when the user releases the right mouse button while the cursor is on a menu item.");
  add("WM_MENUDRAG", 0x0123, constant_category::window_message, "Sent to the owner of a drag-and-drop menu when the user drags a menu item.");
  add("WM_MENUGETOBJECT", 0x0124, constant_category::window_message, "Sent to the owner of a drag-and-drop menu when the mouse cursor enters a menu item or registers a drop target.");
  add("WM_UNINITMENUPOPUP", 0x0125, constant_category::window_message, "Sent when a drop-down menu or submenu has been destroyed.");
  add("WM_MENUCOMMAND", 0x0126, constant_category::window_message, "Sent when the user makes a selection from a menu configured with the MNS_NOTIFYBYPOS flag.");
  add("WM_CTLCOLORMSGBOX", 0x0132, constant_category::window_message, "Sent to the owner window of a message box before drawing to set color characteristics.");
  add("WM_CTLCOLOREDIT", 0x0133, constant_category::window_message, "Sent to the parent window of an edit control when the control is about to be drawn");
  add("WM_CTLCOLORLISTBOX", 0x0134, constant_category::window_message, "Sent to the parent window of a list box before drawing to customize text and background colors.");
  add("WM_CTLCOLORBTN", 0x0135, constant_category::window_message, "Sent to the parent window of a button control before drawing to customize button colors.");
  add("WM_CTLCOLORDLG", 0x0136, constant_category::window_message, "Sent to a dialog box procedure before drawing the dialog background to customize dialog colors.");
  add("WM_CTLCOLORSCROLLBAR", 0x0137, constant_category::window_message, "Sent to the parent window of a scroll bar control before drawing to customize control colors.");
  add("WM_CTLCOLORSTATIC", 0x0138, constant_category::window_message, "Sent to the parent window of a static control when the control is about to be drawn.");
  add("WM_MOUSEMOVE", 0x0200, constant_category::window_message, "Posted to a window when the cursor moves across its client area.");
  add("WM_LBUTTONDOWN", 0x0201, constant_category::window_message, "Posted when the user presses the left mouse button while the cursor is in the client area of a window.");
  add("WM_LBUTTONUP", 0x0202, constant_category::window_message, "Posted when the user releases the left mouse button while the cursor is in the client area of a window.");
  add("WM_LBUTTONDBLCLK", 0x0203, constant_category::window_message, "Posted when the user double-clicks the left mouse button while the cursor is in the client area of a window.");
  add("WM_RBUTTONDOWN", 0x0204, constant_category::window_message, "Posted when the user presses the right mouse button while the cursor is in the client area of a window.");
  add("WM_RBUTTONUP", 0x0205, constant_category::window_message, "Posted when the user releases the right mouse button while the cursor is in the client area of a window.");
  add("WM_RBUTTONDBLCLK", 0x0206, constant_category::window_message, "Posted when the user double-clicks the right mouse button while the cursor is in the client area of a window.");
  add("WM_MBUTTONDOWN", 0x0207, constant_category::window_message, "Posted when the user presses the middle mouse button while the cursor is in the client area of a window.");
  add("WM_MBUTTONUP", 0x0208, constant_category::window_message, "Posted when the user releases the middle mouse button while the cursor is in the client area of a window.");
  add("WM_MBUTTONDBLCLK", 0x0209, constant_category::window_message, "Posted when the user double-clicks the middle mouse button while the cursor is in the client area of a window.");
  add("WM_MOUSEWHEEL", 0x020A, constant_category::window_message, "Sent to the active focus window when the mouse wheel is rotated.");
  add("WM_PARENTNOTIFY", 0x0210, constant_category::window_message, "Sent to the parent window when a child window is created or destroyed");
  add("WM_ENTERMENULOOP", 0x0211, constant_category::window_message, "Notifies an application's main window procedure that a menu modal loop has been entered.");
  add("WM_EXITMENULOOP", 0x0212, constant_category::window_message, "Notifies an application's main window procedure that a menu modal loop has been exited.");
  add("WM_NEXTMENU", 0x0213, constant_category::window_message, "Sent to an application when the user switches between the menu bar and the system menu using arrow keys.");
  add("WM_SIZING", 0x0214, constant_category::window_message, "Sent to a window that the user is currently resizing");
  add("WM_PRINT", 0x0317, constant_category::window_message, "Sent to a window to request that it draw itself in the specified device context (DC)");
  add("WM_PRINTCLIENT", 0x0318, constant_category::window_message, "Sent to a window to request that it draw its client area in the specified device context.");
  add("WM_USER", 0x0400, constant_category::window_message, "Defines the starting threshold boundary for private message IDs reserved for custom window classes.");
  add("WM_APP", 0x8000, constant_category::window_message, "Defines the starting threshold boundary for message IDs available for application-wide private messaging.");

  /* ── MB_* Message Box Flags ───────────────────────────────────── */
  add("MB_OK", 0x00000000, constant_category::message_box, "Message box style containing one push button: OK.");
  add("MB_OKCANCEL", 0x00000001, constant_category::message_box, "Message box style containing two push buttons: OK and Cancel.");
  add("MB_ABORTRETRYIGNORE", 0x00000002, constant_category::message_box, "Message box style containing three push buttons: Abort");
  add("MB_YESNOCANCEL", 0x00000003, constant_category::message_box, "Message box style containing three push buttons: Yes");
  add("MB_YESNO", 0x00000004, constant_category::message_box, "Message box style containing two push buttons: Yes and No.");
  add("MB_RETRYCANCEL", 0x00000005, constant_category::message_box, "Message box style containing two push buttons: Retry and Cancel.");
  add("MB_CANCELTRYCONTINUE", 0x00000006, constant_category::message_box, "Message box style containing three push buttons: Cancel");
  add("MB_ICONHAND", 0x00000010, constant_category::message_box, "Message box flag displaying a stop-sign icon.");
  add("MB_ICONERROR", 0x00000010, constant_category::message_box, "Message box flag displaying an error icon (same as MB_ICONHAND).");
  add("MB_ICONQUESTION", 0x00000020, constant_category::message_box, "Message box flag displaying a question mark icon.");
  add("MB_ICONEXCLAMATION", 0x00000030, constant_category::message_box, "Message box flag displaying an exclamation point icon.");
  add("MB_ICONWARNING", 0x00000030, constant_category::message_box, "Message box flag displaying a warning icon (same as MB_ICONEXCLAMATION).");
  add("MB_ICONASTERISK", 0x00000040, constant_category::message_box, "Message box flag displaying an information icon consisting of a lowercase 'i' inside a circle.");
  add("MB_ICONINFORMATION", 0x00000040, constant_category::message_box, "Message box flag displaying an information icon (same as MB_ICONASTERISK).");
  add("MB_DEFBUTTON1", 0x00000000, constant_category::message_box, "Sets the first button in the message box as the default focused button.");
  add("MB_DEFBUTTON2", 0x00000100, constant_category::message_box, "Sets the second button in the message box as the default focused button.");
  add("MB_DEFBUTTON3", 0x00000200, constant_category::message_box, "Sets the third button in the message box as the default focused button.");
  add("MB_DEFBUTTON4", 0x00000300, constant_category::message_box, "Sets the fourth button in the message box as the default focused button.");
  add("MB_APPLMODAL", 0x00000000, constant_category::message_box, "Makes the message box application-modal; user must respond before continuing work in the current application window.");
  add("MB_SYSTEMMODAL", 0x00001000, constant_category::message_box, "Makes the message box system-modal; forces the message box to remain top-most on the system screen.");
  add("MB_TASKMODAL", 0x00002000, constant_category::message_box, "Makes the message box task-modal; acts like application modal but targets thread-specific top-level windows.");
  add("MB_HELP", 0x00004000, constant_category::message_box, "Adds a Help button to the message box");
  add("MB_SETFOREGROUND", 0x00010000, constant_category::message_box, "Forces the message box window to become the foreground window upon display.");
  add("MB_DEFAULT_DESKTOP_ONLY", 0x00020000, constant_category::message_box, "Constrains message box creation strictly to the default active desktop layout.");
  add("MB_TOPMOST", 0x00040000, constant_category::message_box, "Specifies that the message box window is created with the WS_EX_TOPMOST extended style.");
  add("MB_RIGHT", 0x00080000, constant_category::message_box, "Right-aligns the message box text.");
  add("MB_RTLREADING", 0x00100000, constant_category::message_box, "Displays message box text in Right-to-Left reading order for RTL language systems.");

  /* ── MF_* Menu Flags ──────────────────────────────────────────── */
  add("MF_STRING", 0x0000, constant_category::menu_flag, "Specifies that a menu item uses a text string as its content.");
  add("MF_GRAYED", 0x0001, constant_category::menu_flag, "Disables and grays out a menu item so it cannot be selected.");
  add("MF_DISABLED", 0x0002, constant_category::menu_flag, "Disables a menu item so that it cannot be selected");
  add("MF_CHECKED", 0x0008, constant_category::menu_flag, "Places a check mark next to a menu item.");
  add("MF_POPUP", 0x0010, constant_category::menu_flag, "Specifies that a menu item opens a drop-down menu or submenu.");
  add("MF_MENUBARBREAK", 0x0020, constant_category::menu_flag, "Places the item on a new line (for menu bars) or in a new column (for pop-up menus) with a vertical dividing line.");
  add("MF_MENUBREAK", 0x0040, constant_category::menu_flag, "Places the item on a new line (for menu bars) or in a new column (for pop-up menus) without a dividing line.");
  add("MF_END", 0x0080, constant_category::menu_flag, "Indicates the final menu item in a menu bar or pop-up menu resource template (historical flag).");
  add("MF_OWNERDRAW", 0x0100, constant_category::menu_flag, "Specifies that the item is an owner-drawn menu item.");
  add("MF_SEPARATOR", 0x0800, constant_category::menu_flag, "Draws a horizontal dividing line in a pop-up menu or submenu.");
  add("MF_BYCOMMAND", 0x0000, constant_category::menu_flag, "Indicates that menu manipulation functions identify menu items by command ID (default behavior).");
  add("MF_BYPOSITION", 0x0400, constant_category::menu_flag, "Indicates that menu manipulation functions identify menu items by zero-based relative position.");
  add("MF_DEFAULT", 0x1000, constant_category::menu_flag, "Highlights the menu item as the default selection (typically displayed in bold text).");
  add("MF_SYSMENU", 0x2000, constant_category::menu_flag, "Indicates that the specified menu is the window system menu.");
  add("MF_HELP", 0x4000, constant_category::menu_flag, "Aligns a menu item to the right side of the menu bar (historically reserved for Help items).");
  add("MF_RIGHTJUSTIFY", 0x4000, constant_category::menu_flag, "Right-justifies a menu item or submenu on the main menu bar.");
  add("MF_MOUSESELECT", 0x8000, constant_category::menu_flag, "Flag indicating that a menu item was selected using mouse interaction.");

  /* ── MFT_* Extended Menu Item Types ───────────────────────────── */
  add("MFT_STRING", 0x0000, constant_category::menu_flag, "Modern menu flag specifying that the menu item displays a text string (replaces MF_STRING).");
  add("MFT_BITMAP", 0x0004, constant_category::menu_flag, "Modern menu flag specifying that the menu item displays a bitmap (replaces MF_BITMAP).");
  add("MFT_MENUBARBREAK", 0x0020, constant_category::menu_flag, "Modern menu flag placing the item in a new column separated by a line (replaces MF_MENUBARBREAK).");
  add("MFT_MENUBREAK", 0x0040, constant_category::menu_flag, "Modern menu flag placing the item in a new column without a line (replaces MF_MENUBREAK).");
  add("MFT_OWNERDRAW", 0x0100, constant_category::menu_flag, "Modern menu flag assigning owner-drawn responsibilities to the item (replaces MF_OWNERDRAW).");
  add("MFT_RADIOCHECK", 0x0200, constant_category::menu_flag, "Displays a radio-button bullet next to the checked menu item instead of a standard check mark.");
  add("MFT_SEPARATOR", 0x0800, constant_category::menu_flag, "Modern menu flag creating a horizontal line separator inside a pop-up menu.");
  add("MFT_RIGHTORDER", 0x2000, constant_category::menu_flag, "Specifies that menu items are rendered in Right-to-Left order for cascading menus.");
  add("MFT_RIGHTJUSTIFY", 0x4000, constant_category::menu_flag, "Right-justifies the menu item and all subsequent items on the menu bar.");

  /* ── MFS_* Extended Menu Item States ──────────────────────────── */
  add("MFS_ENABLED", 0x0000, constant_category::menu_flag, "Menu state flag indicating the menu item is enabled and selectable.");
  add("MFS_DISABLED", 0x0003, constant_category::menu_flag, "Menu state flag indicating the menu item is disabled.");
  add("MFS_GRAYED", 0x0003, constant_category::menu_flag, "Menu state flag indicating the menu item is disabled and visually grayed out.");
  add("MFS_CHECKED", 0x0008, constant_category::menu_flag, "Menu state flag indicating the menu item has a check mark or radio mark.");
  add("MFS_UNCHECKED", 0x0000, constant_category::menu_flag, "Menu state flag indicating the menu item is not checked.");
  add("MFS_HILITE", 0x0080, constant_category::menu_flag, "Menu state flag indicating the menu item is currently highlighted/selected.");
  add("MFS_UNHILITE", 0x0000, constant_category::menu_flag, "Menu state flag indicating the menu item highlight state is removed.");
  add("MFS_DEFAULT", 0x1000, constant_category::menu_flag, "Menu state flag setting the menu item as the default bold action item.");

  /* ── Accelerator Flags ────────────────────────────────────────── */
  add("FVIRTKEY", 0x01, constant_category::accelerator_flag, "Indicates that the accelerator key structure code is a virtual key code rather than an ASCII character.");
  add("FNOINVERT", 0x02, constant_category::accelerator_flag, "Prevents top-level menu item highlighting when an accelerator key combination is triggered.");
  add("FSHIFT", 0x04, constant_category::accelerator_flag, "Specifies that the SHIFT key must be held down to activate the menu accelerator entry.");
  add("FCONTROL", 0x08, constant_category::accelerator_flag, "Specifies that the CTRL key must be held down to activate the menu accelerator entry.");
  add("FALT", 0x10, constant_category::accelerator_flag, "Specifies that the ALT key must be held down to activate the menu accelerator entry.");
  add("NOINVERT", 0x02, constant_category::accelerator_flag, "Flag preventing menu item highlight state flipping during shortcut processing (same as FNOINVERT).");

  /* ── RT_* Predefined Resource Types ───────────────────────────── */
  add("RT_CURSOR", 1, constant_category::resource_type, "Predefined resource type representing a hardware mouse cursor resource.");
  add("RT_BITMAP", 2, constant_category::resource_type, "Predefined resource type representing a bitmap graphics resource.");
  add("RT_ICON", 3, constant_category::resource_type, "Predefined resource type representing an application icon resource.");
  add("RT_MENU",              4,  constant_category::resource_type);
  add("RT_DIALOG",            5,  constant_category::resource_type);
  add("RT_STRING",            6,  constant_category::resource_type);
  add("RT_FONTDIR",           7,  constant_category::resource_type);
  add("RT_FONT",              8,  constant_category::resource_type);
  add("RT_ACCELERATOR",       9,  constant_category::resource_type);
  add("RT_RCDATA",            10, constant_category::resource_type);
  add("RT_MESSAGETABLE",      11, constant_category::resource_type);
  add("RT_GROUP_CURSOR",      12, constant_category::resource_type);
  add("RT_GROUP_ICON",        14, constant_category::resource_type);
  add("RT_VERSION",           16, constant_category::resource_type);
  add("RT_DLGINCLUDE",        17, constant_category::resource_type);
  add("RT_PLUGPLAY",          19, constant_category::resource_type);
  add("RT_VXD",               20, constant_category::resource_type);
  add("RT_ANICURSOR",         21, constant_category::resource_type);
  add("RT_ANIICON",           22, constant_category::resource_type);
  add("RT_HTML",              23, constant_category::resource_type);
  add("RT_MANIFEST",          24, constant_category::resource_type);

  /* ── ID* Dialog Button Identifiers ────────────────────────────── */
  add("IDOK",                 1,  constant_category::dialog_id);
  add("IDCANCEL",             2,  constant_category::dialog_id);
  add("IDABORT",              3,  constant_category::dialog_id);
  add("IDRETRY",              4,  constant_category::dialog_id);
  add("IDIGNORE",             5,  constant_category::dialog_id);
  add("IDYES",                6,  constant_category::dialog_id);
  add("IDNO",                 7,  constant_category::dialog_id);
  add("IDCLOSE",              8,  constant_category::dialog_id);
  add("IDHELP",               9,  constant_category::dialog_id);
  add("IDTRYAGAIN",           10, constant_category::dialog_id);
  add("IDCONTINUE",           11, constant_category::dialog_id);
  add("IDTIMEOUT",            32000, constant_category::dialog_id);

  /* ── IDC_* System Control Identifiers ─────────────────────────── */

  /* ── AFX_IDC_* MFC Control Identifiers ──────────────────────── */
  add("AFX_IDC_BROWSE", 0x4b3, constant_category::control_id, "Control ID for file browse button");
  add("AFX_IDC_CHANGE", 0x65, constant_category::control_id, "Control ID for Change button");
  add("AFX_IDC_CLEAR", 0x4b4, constant_category::control_id, "Control ID for Clear button");
  add("AFX_IDC_COLORPROP", 0x45c, constant_category::control_id, "Control ID for Color property page control");
  add("AFX_IDC_COLOR_BLACK", 0x44c, constant_category::control_id, "Control ID for Black color selection box");
  add("AFX_IDC_COLOR_BLUE", 0x450, constant_category::control_id, "Control ID for Blue color selection box");
  add("AFX_IDC_COLOR_CYAN", 0x453, constant_category::control_id, "Control ID for Cyan color selection box");
  add("AFX_IDC_COLOR_DARKBLUE", 0x458, constant_category::control_id, "Control ID for Dark Blue color selection box");
  add("AFX_IDC_COLOR_DARKCYAN", 0x45b, constant_category::control_id, "Control ID for Dark Cyan color selection box");
  add("AFX_IDC_COLOR_DARKGREEN", 0x457, constant_category::control_id, "Control ID for Dark Green color selection box");
  add("AFX_IDC_COLOR_DARKMAGENTA", 0x45a, constant_category::control_id, "Control ID for Dark Magenta color selection box");
  add("AFX_IDC_COLOR_DARKRED", 0x456, constant_category::control_id, "Control ID for Dark Red color selection box");
  add("AFX_IDC_COLOR_GRAY", 0x454, constant_category::control_id, "Control ID for Gray color selection box");
  add("AFX_IDC_COLOR_GREEN", 0x44f, constant_category::control_id, "Control ID for Green color selection box");
  add("AFX_IDC_COLOR_LIGHTBROWN", 0x459, constant_category::control_id, "Control ID for Light Brown color selection box");
  add("AFX_IDC_COLOR_LIGHTGRAY", 0x455, constant_category::control_id, "Control ID for Light Gray color selection box");
  add("AFX_IDC_COLOR_MAGENTA", 0x452, constant_category::control_id, "Control ID for Magenta color selection box");
  add("AFX_IDC_COLOR_RED", 0x44e, constant_category::control_id, "Control ID for Red color selection box");
  add("AFX_IDC_COLOR_WHITE", 0x44d, constant_category::control_id, "Control ID for White color selection box");
  add("AFX_IDC_COLOR_YELLOW", 0x451, constant_category::control_id, "Control ID for Yellow color selection box");
  add("AFX_IDC_CONTEXTHELP", 0x7901, constant_category::control_id, "Cursor ID for Context Help cursor");
  add("AFX_IDC_FONTNAMES", 0x3e9, constant_category::control_id, "Control ID for Font Name list");
  add("AFX_IDC_FONTPROP", 0x3e8, constant_category::control_id, "Control ID for Font property page control");
  add("AFX_IDC_FONTSIZES", 0x3eb, constant_category::control_id, "Control ID for Font Size list");
  add("AFX_IDC_FONTSTYLES", 0x3ea, constant_category::control_id, "Control ID for Font Style list");
  add("AFX_IDC_HSPLITBAR", 0x7904, constant_category::control_id, "Cursor ID for Horizontal Splitter bar");
  add("AFX_IDC_LISTBOX", 0x64, constant_category::control_id, "Control ID for standard List Box control");
  add("AFX_IDC_MAGNIFY", 0x7902, constant_category::control_id, "Cursor ID for Magnifier tool cursor");
  add("AFX_IDC_MOVE4WAY", 0x790c, constant_category::control_id, "Cursor ID for 4-way move operation");
  add("AFX_IDC_NODROPCRSR", 0x7906, constant_category::control_id, "Cursor ID for No Drop drop-target indicator");
  add("AFX_IDC_PICTURE", 0x4b2, constant_category::control_id, "Control ID for Picture property control");
  add("AFX_IDC_PRINT_DOCNAME", 0xc9, constant_category::control_id, "Control ID for Document Name static text in Print dialog");
  add("AFX_IDC_PRINT_PAGENUM", 0xcc, constant_category::control_id, "Control ID for Page Number static text in Print dialog");
  add("AFX_IDC_PRINT_PORTNAME", 0xcb, constant_category::control_id, "Control ID for Port Name static text in Print dialog");
  add("AFX_IDC_PRINT_PRINTERNAME", 0xca, constant_category::control_id, "Control ID for Printer Name static text in Print dialog");
  add("AFX_IDC_PROPNAME", 0x4b1, constant_category::control_id, "Control ID for Property Name static text");
  add("AFX_IDC_SAMPLEBOX", 0x3ee, constant_category::control_id, "Control ID for Font Sample display box");
  add("AFX_IDC_SMALLARROWS", 0x7903, constant_category::control_id, "Cursor ID for small directional adjustment arrows");
  add("AFX_IDC_STRIKEOUT", 0x3ec, constant_category::control_id, "Control ID for Strikeout checkbox");
  add("AFX_IDC_SYSTEMCOLORS", 0x45d, constant_category::control_id, "Control ID for System Colors list");
  add("AFX_IDC_TAB_CONTROL", 0x3020, constant_category::control_id, "Control ID for Tab Control in property sheets");
  add("AFX_IDC_TRACK4WAY", 0x790b, constant_category::control_id, "Cursor ID for 4-way object tracking");
  add("AFX_IDC_TRACKNESW", 0x7908, constant_category::control_id, "Cursor ID for Northeast-Southwest tracking");
  add("AFX_IDC_TRACKNS", 0x7909, constant_category::control_id, "Cursor ID for North-South tracking");
  add("AFX_IDC_TRACKNWSE", 0x7907, constant_category::control_id, "Cursor ID for Northwest-Southeast tracking");
  add("AFX_IDC_TRACKWE", 0x790a, constant_category::control_id, "Cursor ID for West-East tracking");
  add("AFX_IDC_UNDERLINE", 0x3ed, constant_category::control_id, "Control ID for Underline checkbox");
  add("AFX_IDC_VSPLITBAR", 0x7905, constant_category::control_id, "Cursor ID for Vertical Splitter bar");

  /* ── AFX_IDD_* MFC Dialog Template IDs ──────────────────────── */
  add("AFX_IDD_BUSY", 0x780a, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Busy dialog");
  add("AFX_IDD_CHANGEICON", 0x7805, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Change Icon dialog");
  add("AFX_IDD_CHANGESOURCE", 0x780d, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Change Source dialog");
  add("AFX_IDD_CONVERT", 0x7806, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Convert Object dialog");
  add("AFX_IDD_EDITLINKS", 0x7808, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Edit Links dialog");
  add("AFX_IDD_FILEBROWSE", 0x7809, constant_category::mfc_dialog_id, "Dialog Template ID for File Browse dialog");
  add("AFX_IDD_INSERTOBJECT", 0x7804, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Insert Object dialog");
  add("AFX_IDD_NEWTYPEDLG", 0x7801, constant_category::mfc_dialog_id, "Dialog Template ID for New File Type Selection dialog");
  add("AFX_IDD_OBJECTPROPERTIES", 0x780c, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Object Properties dialog");
  add("AFX_IDD_PASTESPECIAL", 0x7807, constant_category::mfc_dialog_id, "Dialog Template ID for OLE Paste Special dialog");
  add("AFX_IDD_PREVIEW_TOOLBAR", 0x7803, constant_category::mfc_dialog_id, "Dialog Template ID for Print Preview toolbar");
  add("AFX_IDD_PRINTDLG", 0x7802, constant_category::mfc_dialog_id, "Dialog Template ID for Print Status dialog");

  /* ── AFX_IDB_* MFC Bitmap IDs ───────────────────────────────── */
  add("AFX_IDB_CHECKBOX", 0x7801, constant_category::mfc_bitmap_id, "Standard MFC bitmap strip providing checkbox glyph states");
  add("AFX_IDB_CHECKLISTBOX_95", 0x7914, constant_category::mfc_bitmap_id, "Bitmap ID for CCheckListBox check images (Windows 95)");
  add("AFX_IDB_CHECKLISTBOX_NT", 0x7913, constant_category::mfc_bitmap_id, "Bitmap ID for CCheckListBox check images (Windows NT)");
  add("AFX_IDB_COLOR_PALETTE", 0x7807, constant_category::mfc_bitmap_id, "Stock color palette button icons for MFC color controls");
  add("AFX_IDB_COLOR_TOOL", 0x7812, constant_category::mfc_bitmap_id, "Toolbar bitmap icon displaying color picker/fill tools");
  add("AFX_IDB_HMENU_ARROW", 0x7f02, constant_category::mfc_bitmap_id, "Internal MFC macro for submenu right-pointing expansion arrows");
  add("AFX_IDB_HMENU_DOT", 0x7f01, constant_category::mfc_bitmap_id, "Internal MFC macro for radio item bullet checkmark glyphs in menus");
  add("AFX_IDB_MENU_IMAGES", 0x780f, constant_category::mfc_bitmap_id, "Visual glyph strip containing caption-bar icons");
  add("AFX_IDB_MINIFRAME_MENU", 0x7912, constant_category::mfc_bitmap_id, "Bitmap ID for mini-frame window system menu button");
  add("AFX_IDB_MINI_BTNS", 0x7802, constant_category::mfc_bitmap_id, "MFC resource strip containing small window control glyphs for mini frame windows");
  add("AFX_IDB_MINI_CALENDAR", 0x7803, constant_category::mfc_bitmap_id, "MFC navigation arrow and date glyph strip used in date pickers");
  add("AFX_IDB_POPUP_SUBMENU", 0x7806, constant_category::mfc_bitmap_id, "Submenu expansion arrow glyphs used across MFC popup and context menus");
  add("AFX_IDB_PROPERTY_GRID_BTNS", 0x7804, constant_category::mfc_bitmap_id, "Expand/collapse and drop-down button glyphs for CMFCPropertyGridCtrl");
  add("AFX_IDB_RIBBON_BAR_BTN_DEFAULT", 0x7808, constant_category::mfc_bitmap_id, "Generic placeholder image used by CMFCRibbonBar when item image is missing");
  add("AFX_IDB_RIBBON_PRINT_LARGE", 0x7809, constant_category::mfc_bitmap_id, "Print/print preview icon for Ribbon Application Menu");
  add("AFX_IDB_RIBBON_PRINT_SMALL", 0x780a, constant_category::mfc_bitmap_id, "Print command icon for Ribbon menus and Quick Access Toolbars");
  add("AFX_IDB_RIBBON_SLIDER_BTN_MINUS", 0x7814, constant_category::mfc_bitmap_id, "Zoom Out button graphic used on Ribbon status bar zoom controls");
  add("AFX_IDB_RIBBON_SLIDER_BTN_PLUS", 0x7815, constant_category::mfc_bitmap_id, "Zoom In button graphic used on Ribbon status bar zoom controls");
  add("AFX_IDB_SCROLL_DOWN", 0x780e, constant_category::mfc_bitmap_id, "Downward scroll arrow icon for vertically oriented toolbars");
  add("AFX_IDB_SCROLL_LEFT", 0x780b, constant_category::mfc_bitmap_id, "Left scroll arrow icon for tabbed control bars");
  add("AFX_IDB_SCROLL_RIGHT", 0x780c, constant_category::mfc_bitmap_id, "Right scroll arrow icon for tabbed control bars");
  add("AFX_IDB_SCROLL_UP", 0x780d, constant_category::mfc_bitmap_id, "Upward scroll arrow icon for vertically oriented toolbars");
  add("AFX_IDB_TASKPANE_CLOSE", 0x7810, constant_category::mfc_bitmap_id, "Close button graphic used in task pane headers");
  add("AFX_IDB_TASKPANE_EXPAND", 0x7811, constant_category::mfc_bitmap_id, "Expand/collapse indicator arrows used within CMFCTasksPane");
  add("AFX_IDB_TASKPANE_SCROLL_BTNS", 0x7805, constant_category::mfc_bitmap_id, "Up and down scroll arrow buttons for scrollable task pane containers");
  add("AFX_IDB_TEAR", 0x7813, constant_category::mfc_bitmap_id, "Visual indicator graphic marking detachable tear-off menus and toolbars");

  /* ── IDB_* Standard Common Controls Bitmap IDs ───────────────── */
  add("IDB_HIST_DISABLED", 0xe, constant_category::mfc_bitmap_id, "Explorer travel buttons and favorites bitmaps in disabled state");
  add("IDB_HIST_HOT", 0xd, constant_category::mfc_bitmap_id, "Explorer travel buttons and favorites bitmaps in hot-tracked state");
  add("IDB_HIST_LARGE_COLOR", 0x9, constant_category::mfc_bitmap_id, "Explorer-style navigation bitmap with 24x24 color icons");
  add("IDB_HIST_NORMAL", 0xc, constant_category::mfc_bitmap_id, "Explorer travel buttons and favorites bitmaps in normal state");
  add("IDB_HIST_PRESSED", 0xf, constant_category::mfc_bitmap_id, "Explorer travel buttons and favorites bitmaps in pressed state");
  add("IDB_HIST_SMALL_COLOR", 0x8, constant_category::mfc_bitmap_id, "Explorer-style navigation bitmap with 16x16 color icons");
  add("IDB_STD_LARGE_COLOR", 0x1, constant_category::mfc_bitmap_id, "Standard Common Controls toolbar bitmap with 24x24 color icons");
  add("IDB_STD_LARGE_MONO", 0x3, constant_category::mfc_bitmap_id, "Standard Common Controls bitmap with 24x24 monochrome icons");
  add("IDB_STD_SMALL_COLOR", 0x0, constant_category::mfc_bitmap_id, "Standard Common Controls toolbar bitmap with 16x16 color icons");
  add("IDB_STD_SMALL_MONO", 0x2, constant_category::mfc_bitmap_id, "Standard Common Controls bitmap with 16x16 monochrome icons");
  add("IDB_VIEW_LARGE_COLOR", 0x5, constant_category::mfc_bitmap_id, "Standard toolbar bitmap with 24x24 color icons for list-view styles");
  add("IDB_VIEW_LARGE_MONO", 0x7, constant_category::mfc_bitmap_id, "Standard toolbar bitmap with 24x24 monochrome icons for view styles");
  add("IDB_VIEW_SMALL_COLOR", 0x4, constant_category::mfc_bitmap_id, "Standard toolbar bitmap with 16x16 color icons for list-view styles");
  add("IDB_VIEW_SMALL_MONO", 0x6, constant_category::mfc_bitmap_id, "Standard toolbar bitmap with 16x16 monochrome icons for view styles");

  /* ── AFX_IDR_* MFC Accelerator Table IDs ────────────────────── */
  add("AFX_IDR_PREVIEW_ACCEL", 0x7915, constant_category::mfc_accel_id, "Accelerator Table ID for Print Preview mode");

  /* ── AFX_IDI_* MFC Icon IDs ─────────────────────────────────── */
  add("AFX_IDI_STD_MDIFRAME", 0x7A01, constant_category::mfc_icon_id, "Icon ID for standard MDI application frame window");
  add("AFX_IDI_STD_FRAME", 0x7A02, constant_category::mfc_icon_id, "Icon ID for standard application frame window");

  /* ── IDD_* Standard Dialog Template IDs ──────────────────────── */
  add("IDD_ABOUTBOX", 0x0064, constant_category::mfc_dialog_id, "Default resource ID assigned to an application About dialog template");

  /* ── AFX_IDP_* MFC Prompt IDs ───────────────────────────────── */
  add("AFX_IDP_ASK_TO_SAVE", 0xf103, constant_category::mfc_prompt_id, "Prompt ID for asking user to save modified document");
  add("AFX_IDP_COMMAND_FAILURE", 0xf109, constant_category::mfc_prompt_id, "Prompt ID for Command execution failure message");
  add("AFX_IDP_DLL_BAD_VERSION", 0xf10e, constant_category::mfc_prompt_id, "Prompt ID for Incompatible DLL version error");
  add("AFX_IDP_DLL_LOAD_FAILED", 0xf10d, constant_category::mfc_prompt_id, "Prompt ID for DLL loading failure message");
  add("AFX_IDP_FAILED_MEMORY_ALLOC", 0xf10a, constant_category::mfc_prompt_id, "Prompt ID for Memory allocation failure prompt");
  add("AFX_IDP_FAILED_TO_CREATE_DOC", 0xf104, constant_category::mfc_prompt_id, "Prompt ID for Document creation failure prompt");
  add("AFX_IDP_FAILED_TO_LAUNCH_HELP", 0xf107, constant_category::mfc_prompt_id, "Prompt ID for Windows Help launch failure prompt");
  add("AFX_IDP_FAILED_TO_OPEN_DOC", 0xf101, constant_category::mfc_prompt_id, "Prompt ID for Document open failure prompt");
  add("AFX_IDP_FAILED_TO_SAVE_DOC", 0xf102, constant_category::mfc_prompt_id, "Prompt ID for Document save failure prompt");
  add("AFX_IDP_FAILED_TO_START_PRINT", 0xf106, constant_category::mfc_prompt_id, "Prompt ID for Print job initialization failure prompt");
  add("AFX_IDP_FILE_TOO_LARGE", 0xf105, constant_category::mfc_prompt_id, "Prompt ID for File size exceeds maximum limit");
  add("AFX_IDP_INTERNAL_FAILURE", 0xf108, constant_category::mfc_prompt_id, "Prompt ID for Internal application failure prompt");
  add("AFX_IDP_INVALID_FILENAME", 0xf100, constant_category::mfc_prompt_id, "Prompt ID for Invalid file name error prompt");
  add("AFX_IDP_NO_ERROR_AVAILABLE", 0xf020, constant_category::mfc_prompt_id, "Prompt ID for No error description available message");
  add("AFX_IDP_PARSE_BYTE", 0xf116, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Byte parse error");
  add("AFX_IDP_PARSE_CURRENCY", 0xf119, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Currency parse error");
  add("AFX_IDP_PARSE_DATETIME", 0xf118, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Date/Time parse error");
  add("AFX_IDP_PARSE_INT", 0xf110, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Integer parse error");
  add("AFX_IDP_PARSE_INT_RANGE", 0xf112, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Integer out of range error");
  add("AFX_IDP_PARSE_RADIO_BUTTON", 0xf115, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Radio button selection error");
  add("AFX_IDP_PARSE_REAL", 0xf111, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Floating point parse error");
  add("AFX_IDP_PARSE_REAL_RANGE", 0xf113, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Floating point out of range error");
  add("AFX_IDP_PARSE_STRING_SIZE", 0xf114, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: String length out of range error");
  add("AFX_IDP_PARSE_UINT", 0xf117, constant_category::mfc_prompt_id, "Prompt ID for Data Validation: Unsigned integer parse error");
  add("AFX_IDP_UNREG_DONE", 0xf10b, constant_category::mfc_prompt_id, "Prompt ID for Unregistration completed message");
  add("AFX_IDP_UNREG_FAILURE", 0xf10c, constant_category::mfc_prompt_id, "Prompt ID for Unregistration failed message");
  add("AFX_IDS_RESOURCE_EXCEPTION", 0xf022, constant_category::mfc_prompt_id, "Error message string displayed when a general resource exception occurs");
  add("AFX_IDS_USER_EXCEPTION", 0xf024, constant_category::mfc_prompt_id, "Standard string prompt for generic user-initiated exceptions");

  /* ── AFX_IDP_FILE_* / AFX_IDP_ARCH_* File/Archive Exception Prompts */
  add("AFX_IDP_ARCH_BADCLASS", 0xf1b6, constant_category::mfc_prompt_id, "Archive Error: Class bad or unexpected");
  add("AFX_IDP_ARCH_BADINDEX", 0xf1b5, constant_category::mfc_prompt_id, "Archive Error: Bad file index");
  add("AFX_IDP_ARCH_BADSCHEMA", 0xf1b7, constant_category::mfc_prompt_id, "Archive Error: Bad schema version");
  add("AFX_IDP_ARCH_ENDOFFILE", 0xf1b3, constant_category::mfc_prompt_id, "Archive Error: Unexpected end of file");
  add("AFX_IDP_ARCH_GENERIC", 0xf1b1, constant_category::mfc_prompt_id, "Generic archive operation error");
  add("AFX_IDP_ARCH_NONE", 0xf1b0, constant_category::mfc_prompt_id, "Archive Error: No error");
  add("AFX_IDP_ARCH_READONLY", 0xf1b2, constant_category::mfc_prompt_id, "Archive Error: Attempted write to read-only archive");
  add("AFX_IDP_ARCH_WRITEONLY", 0xf1b4, constant_category::mfc_prompt_id, "Archive Error: Attempted read from write-only archive");
  add("AFX_IDP_FILE_ACCESS_DENIED", 0xf1a5, constant_category::mfc_prompt_id, "CFileException: Access denied");
  add("AFX_IDP_FILE_BAD_PATH", 0xf1a3, constant_category::mfc_prompt_id, "CFileException: Bad path");
  add("AFX_IDP_FILE_BAD_SEEK", 0xf1a9, constant_category::mfc_prompt_id, "CFileException: Bad seek operation");
  add("AFX_IDP_FILE_DIR_FULL", 0xf1a8, constant_category::mfc_prompt_id, "CFileException: Directory full");
  add("AFX_IDP_FILE_DISKFULL", 0xf1ad, constant_category::mfc_prompt_id, "CFileException: Disk full");
  add("AFX_IDP_FILE_EOF", 0xf1ae, constant_category::mfc_prompt_id, "CFileException: Unexpected end of file");
  add("AFX_IDP_FILE_GENERIC", 0xf1a1, constant_category::mfc_prompt_id, "CFileException: Generic file error");
  add("AFX_IDP_FILE_HARD_IO", 0xf1aa, constant_category::mfc_prompt_id, "CFileException: Hardware I/O error");
  add("AFX_IDP_FILE_INVALID_FILE", 0xf1a6, constant_category::mfc_prompt_id, "CFileException: Invalid file handle");
  add("AFX_IDP_FILE_LOCKING", 0xf1ac, constant_category::mfc_prompt_id, "CFileException: File locking violation");
  add("AFX_IDP_FILE_NONE", 0xf1a0, constant_category::mfc_prompt_id, "CFileException: No error");
  add("AFX_IDP_FILE_NOT_FOUND", 0xf1a2, constant_category::mfc_prompt_id, "CFileException: File not found");
  add("AFX_IDP_FILE_REMOVE_CURRENT", 0xf1a7, constant_category::mfc_prompt_id, "CFileException: Cannot remove current directory");
  add("AFX_IDP_FILE_SHARING", 0xf1ab, constant_category::mfc_prompt_id, "CFileException: Sharing violation");
  add("AFX_IDP_FILE_TOO_MANY_OPEN", 0xf1a4, constant_category::mfc_prompt_id, "CFileException: Too many open files");

  /* ── AFX_IDP_SQL_* MFC SQL Prompt IDs ───────────────────────── */
  add("AFX_IDP_SQL_API_CONFORMANCE", 0xf28e, constant_category::mfc_prompt_id, "ODBC driver is incompatible with MFC database classes");
  add("AFX_IDP_SQL_BOOKMARKS_NOT_ENABLED", 0xf29c, constant_category::mfc_prompt_id, "Bookmarks are not enabled on the recordset");
  add("AFX_IDP_SQL_BOOKMARKS_NOT_SUPPORTED", 0xf29b, constant_category::mfc_prompt_id, "Bookmarks are not supported by the underlying ODBC driver");
  add("AFX_IDP_SQL_CONNECT_FAIL", 0xf281, constant_category::mfc_prompt_id, "Attempt to connect to the SQL data source failed");
  add("AFX_IDP_SQL_DATA_TRUNCATED", 0xf295, constant_category::mfc_prompt_id, "Data returned from the SQL operation was truncated");
  add("AFX_IDP_SQL_DYNAMIC_CURSOR_NOT_SUPPORTED", 0xf299, constant_category::mfc_prompt_id, "Dynamic cursors are not supported by the ODBC driver");
  add("AFX_IDP_SQL_DYNASET_NOT_SUPPORTED", 0xf28c, constant_category::mfc_prompt_id, "Dynasets are not supported by the ODBC driver");
  add("AFX_IDP_SQL_EMPTY_COLUMN_LIST", 0xf283, constant_category::mfc_prompt_id, "Attempted to open a table but no columns were specified");
  add("AFX_IDP_SQL_FIELD_NOT_FOUND", 0xf29a, constant_category::mfc_prompt_id, "The specified field or column name was not found");
  add("AFX_IDP_SQL_FIELD_SCHEMA_MISMATCH", 0xf284, constant_category::mfc_prompt_id, "Query returned an unexpected column data type");
  add("AFX_IDP_SQL_ILLEGAL_MODE", 0xf285, constant_category::mfc_prompt_id, "Attempted update or delete in an illegal recordset mode");
  add("AFX_IDP_SQL_INCORRECT_ODBC", 0xf297, constant_category::mfc_prompt_id, "Incompatible or incorrect ODBC driver installed");
  add("AFX_IDP_SQL_LOCK_MODE_NOT_SUPPORTED", 0xf294, constant_category::mfc_prompt_id, "The requested locking mode is not supported");
  add("AFX_IDP_SQL_MULTIPLE_ROWS_AFFECTED", 0xf286, constant_category::mfc_prompt_id, "Multiple rows were unexpectedly updated or deleted");
  add("AFX_IDP_SQL_NO_CURRENT_RECORD", 0xf287, constant_category::mfc_prompt_id, "Operation failed because there is no current record");
  add("AFX_IDP_SQL_NO_DATA_FOUND", 0xf290, constant_category::mfc_prompt_id, "No data was returned or found for the operation");
  add("AFX_IDP_SQL_NO_POSITIONED_UPDATES", 0xf293, constant_category::mfc_prompt_id, "Positioned updates or deletes are not supported");
  add("AFX_IDP_SQL_NO_ROWS_AFFECTED", 0xf288, constant_category::mfc_prompt_id, "No rows were affected by the update or delete operation");
  add("AFX_IDP_SQL_ODBC_LOAD_FAILED", 0xf28b, constant_category::mfc_prompt_id, "Failed to load the required ODBC driver library");
  add("AFX_IDP_SQL_ODBC_V2_REQUIRED", 0xf292, constant_category::mfc_prompt_id, "An ODBC 2.0 or higher compliant driver is required");
  add("AFX_IDP_SQL_RECORDSET_FORWARD_ONLY", 0xf282, constant_category::mfc_prompt_id, "The recordset only supports forward scrolling");
  add("AFX_IDP_SQL_RECORDSET_READONLY", 0xf289, constant_category::mfc_prompt_id, "The recordset is read-only and cannot be modified");
  add("AFX_IDP_SQL_ROW_FETCH", 0xf296, constant_category::mfc_prompt_id, "Error encountered while fetching rows from the database");
  add("AFX_IDP_SQL_ROW_UPDATE_NOT_SUPPORTED", 0xf291, constant_category::mfc_prompt_id, "Row update operations are not supported by the driver");
  add("AFX_IDP_SQL_SNAPSHOT_NOT_SUPPORTED", 0xf28d, constant_category::mfc_prompt_id, "Snapshots require static cursor support from the ODBC driver");
  add("AFX_IDP_SQL_SQL_CONFORMANCE", 0xf28f, constant_category::mfc_prompt_id, "ODBC driver does not meet required SQL conformance levels");
  add("AFX_IDP_SQL_SQL_NO_TOTAL", 0xf28a, constant_category::mfc_prompt_id, "ODBC driver does not support the MFC CLongBinary data model");
  add("AFX_IDP_SQL_UPDATE_DELETE_FAILED", 0xf298, constant_category::mfc_prompt_id, "The update or delete operation failed");

  /* ── AFX_IDS_* MFC String IDs ───────────────────────────────── */
  add("AFX_IDS_ACTIVATE_VERB", 0xf082, constant_category::mfc_string_id, "String ID for OLE Activate verb menu label");
  add("AFX_IDS_ALLFILTER", 0xf002, constant_category::mfc_string_id, "String ID for generic wildcard filter string");
  add("AFX_IDS_ALL_FILES", 0xf087, constant_category::mfc_string_id, "String ID for All Files filter string");
  add("AFX_IDS_AUTO", 0xf084, constant_category::mfc_string_id, "String ID for Auto label");
  add("AFX_IDS_BITMAP_FORMAT", 0xf090, constant_category::mfc_string_id, "String ID for Bitmap Clipboard format name");
  add("AFX_IDS_CHANGE_LINK", 0xf083, constant_category::mfc_string_id, "String ID for Change Link action label");
  add("AFX_IDS_EDIT_VERB", 0xf081, constant_category::mfc_string_id, "String ID for OLE Edit verb label");
  add("AFX_IDS_EMBED_FORMAT", 0xf092, constant_category::mfc_string_id, "String ID for Embedded Object format display");
  add("AFX_IDS_EXIT_MENU", 0xf08c, constant_category::mfc_string_id, "String ID for Exit command menu label");
  add("AFX_IDS_FROZEN", 0xf086, constant_category::mfc_string_id, "String ID for Frozen state indicator label");
  add("AFX_IDS_HIDE", 0xf011, constant_category::mfc_string_id, "String ID for Hide command label");
  add("AFX_IDS_LINKSOURCE_FORMAT", 0xf091, constant_category::mfc_string_id, "String ID for Link Source clipboard format display");
  add("AFX_IDS_MANUAL", 0xf085, constant_category::mfc_string_id, "String ID for Manual label");
  add("AFX_IDS_METAFILE_FORMAT", 0xf08e, constant_category::mfc_string_id, "String ID for Metafile Clipboard format display");
  add("AFX_IDS_OBJECT_MENUITEM", 0xf080, constant_category::mfc_string_id, "String ID for Dynamic OLE Object menu item label");
  add("AFX_IDS_ONEPAGE", 0xf041, constant_category::mfc_string_id, "String ID for One Page status text in Print Preview");
  add("AFX_IDS_OPENFILE", 0xf000, constant_category::mfc_string_id, "String ID for Open File dialog title");
  add("AFX_IDS_PASTELINKEDTYPE", 0xf094, constant_category::mfc_string_id, "String ID for Paste Linked Object type description");
  add("AFX_IDS_PREVIEW_CLOSE", 0xf005, constant_category::mfc_string_id, "String ID for Close Print Preview tooltip label");
  add("AFX_IDS_PRINTCAPTION", 0xf048, constant_category::mfc_string_id, "String ID for Print job progress window caption");
  add("AFX_IDS_PRINTDEFAULT", 0xf046, constant_category::mfc_string_id, "String ID for Default Printer description string");
  add("AFX_IDS_PRINTDEFAULTEXT", 0xf045, constant_category::mfc_string_id, "String ID for Default Print File extension");
  add("AFX_IDS_PRINTFILTER", 0xf047, constant_category::mfc_string_id, "String ID for Print File filter string");
  add("AFX_IDS_RTF_FORMAT", 0xf096, constant_category::mfc_string_id, "Display string describing Rich Text Format (RTF) data");
  add("AFX_IDS_SAVEFILE", 0xf001, constant_category::mfc_string_id, "Prompt or title displayed in dialogs when saving an existing file");
  add("AFX_IDS_SAVEFILECOPY", 0xf004, constant_category::mfc_string_id, "Prompt or title displayed in dialogs when saving a copy of a file");
  add("AFX_IDS_SAVE_AS_MENU", 0xf08a, constant_category::mfc_string_id, "Dynamic menu text used for the Save As command");
  add("AFX_IDS_SAVE_COPY_AS_MENU", 0xf08b, constant_category::mfc_string_id, "Dynamic menu text used for the Save Copy As command");
  add("AFX_IDS_SAVE_MENU", 0xf088, constant_category::mfc_string_id, "Dynamic menu text used for saving the active document");
  add("AFX_IDS_TEXT_FORMAT", 0xf097, constant_category::mfc_string_id, "Display string describing plain unformatted text clipboard data");
  add("AFX_IDS_TWOPAGE", 0xf042, constant_category::mfc_string_id, "Button or UI string for toggling two-page view in Print Preview");
  add("AFX_IDS_UNKNOWNTYPE", 0xf095, constant_category::mfc_string_id, "Display string indicating an unknown or unrecognized data object");
  add("AFX_IDS_UNTITLED", 0xf003, constant_category::mfc_string_id, "Default Untitled string prefix for new unsaved documents");
  add("AFX_IDS_UPDATE_MENU", 0xf089, constant_category::mfc_string_id, "Dynamic menu text for updating an embedded OLE object");
  add("AFX_IDS_UPDATING_ITEMS", 0xf08d, constant_category::mfc_string_id, "Status message displayed while links/embedded objects update");

  /* ── AFX_ID_PREVIEW_* Print Preview Command IDs ─────────────── */
  add("AFX_ID_PREVIEW_CLOSE", 0xe300, constant_category::system_resource_id, "Command ID for the Print Preview Close button");
  add("AFX_ID_PREVIEW_NEXT", 0xe302, constant_category::system_resource_id, "Command ID for the Print Preview Next Page button");
  add("AFX_ID_PREVIEW_NUMPAGE", 0xe301, constant_category::system_resource_id, "Command ID for toggling one/two page view in Print Preview");
  add("AFX_ID_PREVIEW_PREV", 0xe303, constant_category::system_resource_id, "Command ID for the Print Preview Previous Page button");
  add("AFX_ID_PREVIEW_PRINT", 0xe304, constant_category::system_resource_id, "Command ID for the Print Preview Print button");
  add("AFX_ID_PREVIEW_ZOOMIN", 0xe305, constant_category::system_resource_id, "Command ID for zooming in during Print Preview");
  add("AFX_ID_PREVIEW_ZOOMOUT", 0xe306, constant_category::system_resource_id, "Command ID for zooming out during Print Preview");

  /* ── ID_* System Resource Identifiers ────────────────────────── */
  add("IDP_OLE_INIT_FAILED", 0xe000, constant_category::system_resource_id, "String resource ID for OLE initialization failure prompt message");
  add("ID_APPLY_NOW", 0x3021, constant_category::system_resource_id, "Applies changes in a property sheet without closing it");
  add("ID_APP_ABOUT", 0xe140, constant_category::system_resource_id, "Displays the About dialog box for the application");
  add("ID_APP_EXIT", 0xe141, constant_category::system_resource_id, "Prompts to save documents and exits the application");
  add("ID_CONTEXT_HELP", 0xe144, constant_category::system_resource_id, "Enters context-sensitive Help mode (Shift+F1)");
  add("ID_DEFAULT_HELP", 0xe147, constant_category::system_resource_id, "Runs default help topic for the active window or control");
  add("ID_EDIT_CLEAR", 0xe120, constant_category::system_resource_id, "Erases the selected content");
  add("ID_EDIT_CLEAR_ALL", 0xe121, constant_category::system_resource_id, "Erases all content in the document or window");
  add("ID_EDIT_COPY", 0xe122, constant_category::system_resource_id, "Copies the selection and places it on the Clipboard");
  add("ID_EDIT_CUT", 0xe123, constant_category::system_resource_id, "Cuts the selection and places it on the Clipboard");
  add("ID_EDIT_FIND", 0xe124, constant_category::system_resource_id, "Opens the Find dialog to search for specified text");
  add("ID_EDIT_PASTE", 0xe125, constant_category::system_resource_id, "Inserts Clipboard contents at the insertion point");
  add("ID_EDIT_PASTE_LINK", 0xe126, constant_category::system_resource_id, "Inserts Clipboard contents and creates a link to the source");
  add("ID_EDIT_PASTE_SPECIAL", 0xe127, constant_category::system_resource_id, "Inserts Clipboard contents with formatting options");
  add("ID_EDIT_REDO", 0xe12c, constant_category::system_resource_id, "Redoes the previously undone action");
  add("ID_EDIT_REPEAT", 0xe128, constant_category::system_resource_id, "Repeats the last action performed");
  add("ID_EDIT_REPLACE", 0xe129, constant_category::system_resource_id, "Replaces specific text with different text");
  add("ID_EDIT_SELECT_ALL", 0xe12a, constant_category::system_resource_id, "Selects the entire document or contents");
  add("ID_EDIT_UNDO", 0xe12b, constant_category::system_resource_id, "Undoes the last action performed");
  add("ID_FILE_CLOSE", 0xe102, constant_category::system_resource_id, "Closes the active document");
  add("ID_FILE_NEW", 0xe100, constant_category::system_resource_id, "Creates a new document");
  add("ID_FILE_NEW_FRAME", 0xe10d, constant_category::system_resource_id, "Opens a new frame window for an existing document");
  add("ID_FILE_OPEN", 0xe101, constant_category::system_resource_id, "Opens an existing document");
  add("ID_FILE_PAGE_SETUP", 0xe105, constant_category::system_resource_id, "Opens the Page Setup dialog box");
  add("ID_FILE_PRINT", 0xe107, constant_category::system_resource_id, "Prints the active document");
  add("ID_FILE_PRINT_DIRECT", 0xe108, constant_category::system_resource_id, "Prints the document directly using current printer settings");
  add("ID_FILE_PRINT_PREVIEW", 0xe109, constant_category::system_resource_id, "Displays full pages on-screen to preview output");
  add("ID_FILE_PRINT_SETUP", 0xe106, constant_category::system_resource_id, "Opens the Print Setup dialog to choose a printer");
  add("ID_FILE_SAVE", 0xe103, constant_category::system_resource_id, "Saves the active document under its current name");
  add("ID_FILE_SAVE_AS", 0xe104, constant_category::system_resource_id, "Saves the active document under a new name");
  add("ID_FILE_SAVE_COPY_AS", 0xe10b, constant_category::system_resource_id, "Saves a copy of the active document under a new name");
  add("ID_FILE_SEND_MAIL", 0xe10c, constant_category::system_resource_id, "Sends the active document via electronic mail");
  add("ID_FILE_UPDATE", 0xe10a, constant_category::system_resource_id, "Updates an embedded container document with object changes");
  add("ID_FORMAT_FONT", 0xe160, constant_category::system_resource_id, "Selects fonts for current selection or document");
  add("ID_HELP", 0xe146, constant_category::system_resource_id, "Displays help for the current task or command (F1)");
  add("ID_HELP_FINDER", 0xe143, constant_category::system_resource_id, "Displays Help contents/index or search prompt");
  add("ID_HELP_INDEX", 0xe142, constant_category::system_resource_id, "Displays the main index for Help topics");
  add("ID_HELP_USING", 0xe144, constant_category::system_resource_id, "Displays instructions on how to use Help");
  add("ID_INDICATOR_CAPS", 0xe701, constant_category::system_resource_id, "Status bar indicator for Caps Lock state");
  add("ID_INDICATOR_EXT", 0xe700, constant_category::system_resource_id, "Status bar indicator for extended selection mode");
  add("ID_INDICATOR_KANA", 0xe706, constant_category::system_resource_id, "Status bar indicator for Kana input mode");
  add("ID_INDICATOR_NUM", 0xe702, constant_category::system_resource_id, "Status bar indicator for Num Lock state");
  add("ID_INDICATOR_OVR", 0xe704, constant_category::system_resource_id, "Status bar indicator for Overtype mode");
  add("ID_INDICATOR_REC", 0xe705, constant_category::system_resource_id, "Status bar indicator for macro recording mode");
  add("ID_INDICATOR_SCRL", 0xe703, constant_category::system_resource_id, "Status bar indicator for Scroll Lock state");
  add("ID_NEXT_PANE", 0xe150, constant_category::system_resource_id, "Switches focus to the next window pane");
  add("ID_OLE_EDIT_CHANGE_ICON", 0xe203, constant_category::system_resource_id, "Changes the icon displaying an OLE embedded item");
  add("ID_OLE_EDIT_CONVERT", 0xe202, constant_category::system_resource_id, "Converts an embedded OLE object to a different type");
  add("ID_OLE_EDIT_LINKS", 0xe201, constant_category::system_resource_id, "Displays options to edit or update linked OLE objects");
  add("ID_OLE_EDIT_PROPERTIES", 0xe204, constant_category::system_resource_id, "Displays OLE item properties dialog");
  add("ID_OLE_INSERT_NEW", 0xe200, constant_category::system_resource_id, "Inserts a new OLE embedded object into the document");
  add("ID_OLE_VERB_FIRST", 0xe210, constant_category::system_resource_id, "First ID for dynamically populated OLE object verbs");
  add("ID_PREV_PANE", 0xe151, constant_category::system_resource_id, "Switches focus back to the previous window pane");
  add("ID_RECORD_LAST", 0xe243, constant_category::system_resource_id, "Moves focus/selection to the last record");
  add("ID_RECORD_NEXT", 0xe242, constant_category::system_resource_id, "Moves focus/selection to the next record");
  add("ID_RECORD_PREV", 0xe241, constant_category::system_resource_id, "Moves focus/selection to the previous record");
  add("ID_REC_FIRST", 0xe240, constant_category::system_resource_id, "Alias for ID_RECORD_FIRST; moves to the first record");
  add("ID_SEPARATOR", 0x0, constant_category::system_resource_id, "Used as a visual separator line in menus and toolbars");
  add("ID_VIEW_AUTOARRANGE", 0xe816, constant_category::system_resource_id, "Automatically arranges icons in a list/grid view");
  add("ID_VIEW_BYNAME", 0xe815, constant_category::system_resource_id, "Sorts list view items alphabetically by name");
  add("ID_VIEW_DETAILS", 0xe813, constant_category::system_resource_id, "Displays detailed information about each item");
  add("ID_VIEW_LARGEICON", 0xe810, constant_category::system_resource_id, "Displays items using large icons");
  add("ID_VIEW_LINEUP", 0xe814, constant_category::system_resource_id, "Aligns icons to an invisible grid");
  add("ID_VIEW_LIST", 0xe812, constant_category::system_resource_id, "Displays items as a simple list");
  add("ID_VIEW_REBAR", 0xe802, constant_category::system_resource_id, "Toggles display of the rebar control bar");
  add("ID_VIEW_SMALLICON", 0xe811, constant_category::system_resource_id, "Displays items using small icons");
  add("ID_VIEW_STATUS_BAR", 0xe801, constant_category::system_resource_id, "Toggles display of the application status bar");
  add("ID_VIEW_TOOLBAR", 0xe800, constant_category::system_resource_id, "Toggles display of the application main toolbar");
  add("ID_WINDOW_ARRANGE", 0xe131, constant_category::system_resource_id, "Arranges icons at the bottom of an MDI workspace");
  add("ID_WINDOW_CASCADE", 0xe132, constant_category::system_resource_id, "Arranges open MDI windows in an overlapping cascade");
  add("ID_WINDOW_NEW", 0xe130, constant_category::system_resource_id, "Opens another window for the active document");
  add("ID_WINDOW_SPLIT", 0xe135, constant_category::system_resource_id, "Splits the active window into multiple panes");
  add("ID_WINDOW_TILE_HORIZ", 0xe133, constant_category::system_resource_id, "Tiles open MDI windows horizontally");
  add("ID_WINDOW_TILE_VERT", 0xe134, constant_category::system_resource_id, "Tiles open MDI windows vertically");
  add("ID_WIZBACK", 0x3023, constant_category::system_resource_id, "Wizard control button ID for navigating to the previous page");
  add("ID_WIZFINISH", 0x3025, constant_category::system_resource_id, "Wizard control button ID for completing and closing the wizard");
  add("ID_WIZNEXT", 0x3024, constant_category::system_resource_id, "Wizard control button ID for navigating to the next page");

  /* ── ID_FILE_MRU_* Most Recently Used File List IDs ──────────── */
  add("ID_FILE_MRU_FILE1", 0xe110, constant_category::system_resource_id, "Opens most recently used file #1");
  add("ID_FILE_MRU_FILE2", 0xe111, constant_category::system_resource_id, "Opens most recently used file #2");
  add("ID_FILE_MRU_FILE3", 0xe112, constant_category::system_resource_id, "Opens most recently used file #3");
  add("ID_FILE_MRU_FILE4", 0xe113, constant_category::system_resource_id, "Opens most recently used file #4");
  add("ID_FILE_MRU_FILE5", 0xe114, constant_category::system_resource_id, "Opens most recently used file #5");
  add("ID_FILE_MRU_FILE6", 0xe115, constant_category::system_resource_id, "Opens most recently used file #6");
  add("ID_FILE_MRU_FILE7", 0xe116, constant_category::system_resource_id, "Opens most recently used file #7");
  add("ID_FILE_MRU_FILE8", 0xe117, constant_category::system_resource_id, "Opens most recently used file #8");
  add("ID_FILE_MRU_FILE9", 0xe118, constant_category::system_resource_id, "Opens most recently used file #9");
  add("ID_FILE_MRU_FILE10", 0xe119, constant_category::system_resource_id, "Opens most recently used file #10");
  add("ID_FILE_MRU_FILE11", 0xe11a, constant_category::system_resource_id, "Opens most recently used file #11");
  add("ID_FILE_MRU_FILE12", 0xe11b, constant_category::system_resource_id, "Opens most recently used file #12");
  add("ID_FILE_MRU_FILE13", 0xe11c, constant_category::system_resource_id, "Opens most recently used file #13");
  add("ID_FILE_MRU_FILE14", 0xe11d, constant_category::system_resource_id, "Opens most recently used file #14");
  add("ID_FILE_MRU_FILE15", 0xe11e, constant_category::system_resource_id, "Opens most recently used file #15");
  add("ID_FILE_MRU_FILE16", 0xe11f, constant_category::system_resource_id, "Opens most recently used file #16");
  add("ID_FILE_MRU_FIRST", 0xE110, constant_category::system_resource_id, "First ID reserved for the Most Recently Used file list");
  add("ID_FILE_MRU_LAST", 0xE11F, constant_category::system_resource_id, "Last ID reserved for the Most Recently Used file list");

  /* ── OBM_* OEM Bitmap IDs ───────────────────────────────────── */
  add("OBM_BTNCORNERS", 0x7f1f, constant_category::oem_bitmap, "Legacy OEM bitmap for rounded push-button corners");
  add("OBM_BTSIZE", 0x7f1d, constant_category::oem_bitmap, "OEM window resizing grip bitmap");
  add("OBM_CHECK", 0x7f20, constant_category::oem_bitmap, "OEM checkmark bitmap drawn next to selected menu items");
  add("OBM_CHECKBOXES", 0x7f1e, constant_category::oem_bitmap, "OEM system bitmap strip containing default checkbox state graphics");
  add("OBM_CLOSE", 0x7f2a, constant_category::oem_bitmap, "OEM title bar close button bitmap");
  add("OBM_COMBO", 0x7fe2, constant_category::oem_bitmap, "OEM bitmap providing drop-down arrow graphic for combo box controls");
  add("OBM_DNARROW", 0x7f28, constant_category::oem_bitmap, "OEM scrollbar down-arrow button bitmap");
  add("OBM_DNARROWD", 0x7fe1, constant_category::oem_bitmap, "OEM bitmap providing pressed state for down-arrows");
  add("OBM_DNARROWI", 0x7fe8, constant_category::oem_bitmap, "Inactive/disabled state down scrollbar arrow glyph");
  add("OBM_LFARROW", 0x7f26, constant_category::oem_bitmap, "OEM scrollbar left-arrow button bitmap");
  add("OBM_LFARROWD", 0x7fec, constant_category::oem_bitmap, "Pressed state left scrollbar arrow button");
  add("OBM_LFARROWI", 0x7fe6, constant_category::oem_bitmap, "Inactive/disabled state left scrollbar arrow glyph");
  add("OBM_MNARROW", 0x7fe3, constant_category::oem_bitmap, "OEM bitmap providing right-pointing submenu indicator arrow");
  add("OBM_OLD_CLOSE", 0x7f18, constant_category::oem_bitmap, "Classic OEM close button bitmap for legacy title bars");
  add("OBM_OLD_DNARROW", 0x7f17, constant_category::oem_bitmap, "Classic OEM down-arrow bitmap used in legacy scrollbars");
  add("OBM_OLD_LFARROW", 0x7f14, constant_category::oem_bitmap, "Classic OEM left-arrow bitmap used in 16-bit scrollbars");
  add("OBM_OLD_REDUCE", 0x7f1a, constant_category::oem_bitmap, "Classic OEM minimize button bitmap");
  add("OBM_OLD_RESTORE", 0x7f1b, constant_category::oem_bitmap, "Classic OEM restore button bitmap");
  add("OBM_OLD_RGARROW", 0x7f15, constant_category::oem_bitmap, "Classic OEM right-arrow bitmap used in legacy scrollbars");
  add("OBM_OLD_UPARROW", 0x7f16, constant_category::oem_bitmap, "Classic OEM up-arrow bitmap used in legacy scrollbars");
  add("OBM_OLD_ZOOM", 0x7f19, constant_category::oem_bitmap, "Classic OEM maximize button bitmap");
  add("OBM_REDUCE", 0x7f25, constant_category::oem_bitmap, "System OEM minimize button bitmap");
  add("OBM_REDUCED", 0x7ff2, constant_category::oem_bitmap, "Pressed state of standard Minimize button");
  add("OBM_RESTORE", 0x7f23, constant_category::oem_bitmap, "System OEM restore button bitmap");
  add("OBM_RESTORED", 0x7ffa, constant_category::oem_bitmap, "Pressed state of standard Restore button");
  add("OBM_RGARROW", 0x7f27, constant_category::oem_bitmap, "OEM scrollbar right-arrow button bitmap");
  add("OBM_RGARROWD", 0x7fed, constant_category::oem_bitmap, "Pressed state right scrollbar arrow button");
  add("OBM_RGARROWI", 0x7fe7, constant_category::oem_bitmap, "Inactive/disabled state right scrollbar arrow glyph");
  add("OBM_SIZE", 0x7f1c, constant_category::oem_bitmap, "OEM sizing frame bitmap asset");
  add("OBM_UPARROW", 0x7f29, constant_category::oem_bitmap, "OEM scrollbar up-arrow button bitmap");
  add("OBM_UPARROWD", 0x7fe0, constant_category::oem_bitmap, "OEM bitmap providing pressed state for up-arrows");
  add("OBM_UPARROWI", 0x7fe9, constant_category::oem_bitmap, "Inactive/disabled state up scrollbar arrow glyph");
  add("OBM_ZOOM", 0x7f24, constant_category::oem_bitmap, "System OEM maximize button bitmap");
  add("OBM_ZOOMD", 0x7ff1, constant_category::oem_bitmap, "Pressed state of standard Maximize button");
  add("IDC_STATIC",           -1, constant_category::system_id);
}

}
