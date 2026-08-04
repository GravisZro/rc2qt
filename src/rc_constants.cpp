#include "rc_constants.h"

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

void constant_registry::add(category_t cat,
                            int64_t value,
                            const std::string& name,
                            const std::string& desc)
{
  m_name_to_value[name] = value;
  m_value_to_name[catval_t { cat, value }] = name;
  m_entries.push_back({name, value, cat, desc});
}

bool constant_registry::has_name(const std::string& name) const
{
  return m_name_to_value.count(name) > 0;
}

category_t constant_registry::resolve_category(const std::string& name)
{
  if(name.empty())
    return category_t::window_style;

  const std::string upper = to_upper(name);

  const std::unordered_map<std::string, category_t> categories =
  {
    // Standard control classes (ordinal references #128-#133)
    { "BUTTON", category_t::button_style },
    { "#128", category_t::button_style },
    { "EDIT", category_t::edit_style },
    { "#129", category_t::edit_style },
    { "STATIC", category_t::static_style },
    { "#130", category_t::static_style },
    { "LISTBOX", category_t::listbox_style },
    { "#131", category_t::listbox_style },
    { "SCROLLBAR", category_t::scrollbar_style },
    { "#132", category_t::scrollbar_style },
    { "COMBOBOX", category_t::combobox_style },
    { "COMBOBOXEX32", category_t::combobox_style },
    { "#133", category_t::combobox_style },

    // Common Control classes
    { "SYSLISTVIEW32", category_t::listview_style },
    { "SYSTREEVIEW32", category_t::treeview_style },
    { "MSCTLS_TRACKBAR32", category_t::trackbar_style },
    { "MSCTLS_PROGRESS32", category_t::progressbar_style },
    { "MSCTLS_UPDOWN32", category_t::updown_style },
    { "SYSDATETIMEPICK32", category_t::datetime_style },
    { "SYSMONTHCAL32", category_t::month_calendar_style },
    { "SYSTABCONTROL32", category_t::tabcontrol_style },
    { "SYSPAGER", category_t::pager_style },
    { "SYSMONTHCAL", category_t::month_calendar_style },
    { "SYSHEADER32", category_t::header_style },
    { "SYSANIMATE32", category_t::animate_style },
    { "TOOLBARCLASSNAME", category_t::toolbar_style },
    { "TOOLBARWINDOW32", category_t::toolbar_style },
    { "TOOLTIPS_CLASS32", category_t::tooltip_style },
    { "TOOLTIPS_CLASS", category_t::tooltip_style },
    { "REBARCLASSNAME", category_t::rebar_style },
    { "REBARWINDOW32", category_t::rebar_style },
    { "MSCTLS_STATUSBAR32", category_t::statusbar_style },
    { "STATUSCLASSNAME", category_t::statusbar_style },

    // Edit-based controls
    { "RICHEDIT", category_t::edit_style },
    { "RICHEDIT20A", category_t::edit_style },
    { "RICHEDIT20W", category_t::edit_style },
    { "RICHEDIT50W", category_t::edit_style },

    // RC resource statement keywords
    { "ACCELERATORS", category_t::accelerator_flag },
    { "DIALOG", category_t::dialog_style },
    { "DIALOGEX", category_t::dialog_style },

  };

  auto it = categories.find(upper);
  if(it != categories.end())
    return it->second | category_t::window_style;

  return category_t::bad_category;
}

int64_t constant_registry::resolve(const std::string& name) const
{
  auto it = m_name_to_value.find(name);
  if(it != m_name_to_value.end())
    return it->second;

  if(!name.empty())
  {
    try
    {
      size_t pos = 0;
      long long val = std::stoll(name, &pos, 0);
      if(pos == name.size())
        return static_cast<int64_t>(val);
    }
    catch(...)
    {
    }
  }

  return -1;
}


std::string constant_registry::resolve(category_t cat, int64_t value) const
{
  for(uint64_t bit = 1; bit != 0; bit <<= 1)
  {
    if(static_cast<uint64_t>(cat) & bit)
    {
      auto it = m_value_to_name.find(catval_t
        { static_cast<category_t>(bit), value });
      if(it != m_value_to_name.end())
        return it->second;
    }
  }
  return {};
}

std::vector<constant_entry> constant_registry::entries_by_category(category_t cat) const
{
  std::vector<constant_entry> result;
  for(const auto& e : m_entries)
    if(static_cast<uint64_t>(e.category) & static_cast<uint64_t>(cat))
      result.push_back(e);
  return result;
}

std::vector<constant_entry> constant_registry::all_entries() const
{
  return m_entries;
}

size_t constant_registry::size() const
{
  return m_entries.size();
}

void constant_registry::register_all()
{
/* These are purely descriptive for the purpose of mapping values to Qt attributes/options/etc.
add(category_t::dialog_args, 0x00000401, "LB_ADDSTRING"?, "Sends the accompanying string or data to a List Box control (CListBox), adding it as a new list item."
add(category_t::dialog_args, 0x00000402, "LB_INSERTSTRING"?, "Used in internal structures for insertion tasks or specialized list box initialization commands."
add(category_t::dialog_args, 0x00000403, "CB_ADDSTRING"?, "Sends the accompanying string data to a Combo Box control (CComboBox), adding it as an entry in the drop-down list."
add(category_t::dialog_args, 0x00000404, "CB_INSERTSTRING"?, "Used for inserting items at specific indices rather than appending them to the end of a combo box list."
*/

  /* ── WS_* Window Styles ───────────────────────────────────────── */
  // docs/win32/desktop-src/winmsg/window-styles.md
  add(category_t::window_style, 0x00800000, "WS_BORDER", "Creates a window that has a thin-line border.");
  add(category_t::window_style, 0x00C00000, "WS_CAPTION", "Creates a window that has a title bar (includes the WS_BORDER style).");
  add(category_t::window_style, 0x40000000, "WS_CHILD", "Creates a child window. A window with this style cannot have a menu bar and cannot be used with the WS_POPUP style.");
  add(category_t::window_style, 0x02000000, "WS_CLIPCHILDREN", "Excludes the area occupied by child windows when drawing occurs within the parent window. Used when creating the parent window.");
  add(category_t::window_style, 0x04000000, "WS_CLIPSIBLINGS", "Clips child windows relative to each other; that is");
  add(category_t::window_style, 0x08000000, "WS_DISABLED", "Creates a window that is initially disabled. A disabled window cannot receive input from the user.");
  add(category_t::window_style, 0x00400000, "WS_DLGFRAME", "Creates a window that has a border of a style typically used with dialog boxes. A window with this style cannot have a title bar.");
  add(category_t::window_style, 0x00020000, "WS_GROUP", "Specifies the first control of a group of controls. The group consists of this first control and all controls defined after it");
  add(category_t::window_style, 0x00100000, "WS_HSCROLL", "Creates a window that has a horizontal scroll bar.");
  add(category_t::window_style, 0x01000000, "WS_MAXIMIZE", "Creates a window that is initially maximized.");
  add(category_t::window_style, 0x00010000, "WS_MAXIMIZEBOX", "Creates a window that has a maximize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU.");
  add(category_t::window_style, 0x20000000, "WS_MINIMIZE", "Creates a window that is initially minimized.");
  add(category_t::window_style, 0x00020000, "WS_MINIMIZEBOX", "Creates a window that has a minimize button. Cannot be combined with the WS_EX_CONTEXTHELP style. Requires WS_SYSMENU.");
  add(category_t::window_style, 0x00000000, "WS_OVERLAPPED", "Creates an overlapped window. An overlapped window has a title bar and a border.");
  add(category_t::window_style, 0x80000000, "WS_POPUP", "Creates a pop-up window. Cannot be used with the WS_CHILD style.");
  add(category_t::window_style, 0x00040000, "WS_SIZEBOX", "Creates a window that has a sizing border (same as WS_THICKFRAME).");
  add(category_t::window_style, 0x00080000, "WS_SYSMENU", "Creates a window that has a window menu on its title bar. Requires WS_CAPTION.");
  add(category_t::window_style, 0x00010000, "WS_TABSTOP", "Specifies a control that can receive the keyboard focus when the user presses the TAB key. Pressing TAB changes the focus to the next control with this style.");
  add(category_t::window_style, 0x10000000, "WS_VISIBLE", "Creates a window that is initially visible.");
  add(category_t::window_style, 0x00200000, "WS_VSCROLL", "Creates a window that has a vertical scroll bar.");
  add(category_t::window_style, 0x00040000, "WS_THICKFRAME", "Creates a window that has a sizing border (same as WS_SIZEBOX).");
  add(category_t::window_style, 0x00CF0000, "WS_OVERLAPPEDWINDOW", "Creates an overlapped window with WS_OVERLAPPED");
  add(category_t::window_style, 0x80880000, "WS_POPUPWINDOW", "Creates a pop-up window with WS_POPUP");

  /* ── WS_EX_* Extended Window Styles ───────────────────────────── */
  // docs/win32/desktop-src/winmsg/extended-window-styles.md
  add(category_t::extended_style, 0x00000010, "WS_EX_ACCEPTFILES", "Specifies that a window created with this style accepts dropped files.");
  add(category_t::extended_style, 0x00040000, "WS_EX_APPWINDOW", "Forces a top-level window onto the taskbar when the window is visible.");
  add(category_t::extended_style, 0x00000200, "WS_EX_CLIENTEDGE", "Specifies that a window has a border with a sunken edge.");
  add(category_t::extended_style, 0x02000000, "WS_EX_COMPOSITED", "Paints all descendants of a window in bottom-to-top painting order using double-buffering.");
  add(category_t::extended_style, 0x00000400, "WS_EX_CONTEXTHELP", "Includes a question mark in the title bar of the window. When clicked");
  add(category_t::extended_style, 0x00010000, "WS_EX_CONTROLPARENT", "Allows the user to navigate among the child windows of the window by using the TAB key.");
  add(category_t::extended_style, 0x00000001, "WS_EX_DLGMODALFRAME", "Creates a window that has a double border; can optionally be created with a title bar by specifying WS_CAPTION.");
  add(category_t::extended_style, 0x00080000, "WS_EX_LAYERED", "Creates a layered window. Note that this cannot be used if the window has a window class style of CS_OWNDC or CS_CLASSDC.");
  add(category_t::extended_style, 0x00400000, "WS_EX_LAYOUTRTL", "Sets the horizontal origin to the right edge. Increasing horizontal values advance to the left.");
  add(category_t::extended_style, 0x00000000, "WS_EX_LEFT", "Gives the window left-aligned properties (default layout).");
  add(category_t::extended_style, 0x00004000, "WS_EX_LEFTSCROLLBAR", "Places the vertical scroll bar (if present) to the left of the client area.");
  add(category_t::extended_style, 0x00000000, "WS_EX_LTRREADING", "Displays window text using left-to-right reading order properties (default).");
  add(category_t::extended_style, 0x00000040, "WS_EX_MDICHILD", "Creates a Multiple-Document Interface (MDI) child window.");
  add(category_t::extended_style, 0x08000000, "WS_EX_NOACTIVATE", "A top-level window created with this style does not become the active window when the user clicks it.");
  add(category_t::extended_style, 0x00100000, "WS_EX_NOINHERITLAYOUT", "Prevents the window layout from being inherited by child windows.");
  add(category_t::extended_style, 0x00000004, "WS_EX_NOPARENTNOTIFY", "Specifies that a child window created with this style does not send the WM_PARENTNOTIFY message to its parent when created or destroyed.");
  add(category_t::extended_style, 0x00000188, "WS_EX_PALETTEWINDOW", "Combines WS_EX_WINDOWEDGE");
  add(category_t::extended_style, 0x00001000, "WS_EX_RIGHT", "Gives the window right-aligned properties depending on window class.");
  add(category_t::extended_style, 0x00000000, "WS_EX_RIGHTSCROLLBAR", "Places the vertical scroll bar to the right of the client area (default).");
  add(category_t::extended_style, 0x00002000, "WS_EX_RTLREADING", "Displays window text using Right-to-Left reading order properties.");
  add(category_t::extended_style, 0x00020000, "WS_EX_STATICEDGE", "Gives a window a three-dimensional border style intended to be used for items that do not accept user input.");
  add(category_t::extended_style, 0x00000080, "WS_EX_TOOLWINDOW", "Creates a tool window intended as a floating toolbar; does not appear on the taskbar or in the ALT+TAB dialog.");
  add(category_t::extended_style, 0x00000008, "WS_EX_TOPMOST", "Specifies that a window created with this style should be placed above all non-topmost windows and remain above them");
  add(category_t::extended_style, 0x00000020, "WS_EX_TRANSPARENT", "Specifies that a window created with this style should not be painted until siblings beneath it have been painted.");
  add(category_t::extended_style, 0x00000100, "WS_EX_WINDOWEDGE", "Specifies that a window has a border with a raised edge.");

  /* ── DS_* Dialog Styles ───────────────────────────────────────── */
  add(category_t::dialog_style, 0x0004, "DS_3DLOOK", "Gives dialog box controls a 3D look (historical compatibility style).");
  add(category_t::dialog_style, 0x0001, "DS_ABSALIGN", "Indicates that the coordinates of the dialog box are screen coordinates (otherwise client coordinates relative to owner).");
  add(category_t::dialog_style, 0x0800, "DS_CENTER", "Centers the dialog box on the display screen or monitor.");
  add(category_t::dialog_style, 0x1000, "DS_CENTERMOUSE", "Centers the dialog box under the mouse cursor.");
  add(category_t::dialog_style, 0x2000, "DS_CONTEXTHELP", "Includes a Help button in the title bar of the dialog box.");
  add(category_t::dialog_style, 0x0400, "DS_CONTROL", "Creates a dialog box that can work as a control child inside another dialog box (like a tab control page).");
  add(category_t::dialog_style, 0x0008, "DS_FIXEDSYS", "Causes the dialog box to use the system font with fixed spacing instead of proportional spacing.");
  add(category_t::dialog_style, 0x0020, "DS_LOCALEDIT", "Applies to edit controls in the dialog box");
  add(category_t::dialog_style, 0x0080, "DS_MODALFRAME", "Creates a dialog box with a modal dialog frame border.");
  add(category_t::dialog_style, 0x0010, "DS_NOFAILCREATE", "Causes CreateDialog or DialogBox to succeed even if some child controls cannot be created.");
  add(category_t::dialog_style, 0x0100, "DS_NOIDLEMSG", "Suppresses WM_ENTERIDLE messages that the system otherwise sends to the owner while the dialog box is displayed.");
  add(category_t::dialog_style, 0x0040, "DS_SETFONT", "Indicates that the dialog template specifies a custom font for rendering text in the dialog box.");
  add(category_t::dialog_style, 0x0200, "DS_SETFOREGROUND", "Brings the dialog box to the foreground upon creation.");
  add(category_t::dialog_style, 0x0002, "DS_SYSMODAL", "Creates a system-modal dialog box (historical style");
  add(category_t::dialog_style, 0x0048, "DS_SHELLFONT", "Uses the system shell font for rendering text inside the dialog box (requires DS_SETFONT).");

  /* ── BS_* Button Styles ───────────────────────────────────────── */
  add(category_t::button_style, 0x0005, "BS_3STATE", "Creates a button that functions like a checkbox but can be dimmed (indeterminate state) as well as checked or unchecked.");
  add(category_t::button_style, 0x0006, "BS_AUTO3STATE", "Same as BS_3STATE");
  add(category_t::button_style, 0x0003, "BS_AUTOCHECKBOX", "Same as BS_CHECKBOX");
  add(category_t::button_style, 0x0009, "BS_AUTORADIOBUTTON", "Same as BS_RADIOBUTTON");
  add(category_t::button_style, 0x0080, "BS_BITMAP", "Specifies that the button displays a bitmap instead of text.");
  add(category_t::button_style, 0x0800, "BS_BOTTOM", "Places text at the bottom of the button rectangle.");
  add(category_t::button_style, 0x0300, "BS_CENTER", "Horizontally centers text in the button rectangle.");
  add(category_t::button_style, 0x0002, "BS_CHECKBOX", "Creates a small square box with adjacent text. Typically used to allow multi-select options.");
  add(category_t::button_style, 0x0001, "BS_DEFPUSHBUTTON", "Creates a push button with a heavy black border representing the default action when ENTER is pressed.");
  add(category_t::button_style, 0x0007, "BS_GROUPBOX", "Creates a rectangle in which other controls can be grouped");
  add(category_t::button_style, 0x0040, "BS_ICON", "Specifies that the button displays an icon instead of text.");
  add(category_t::button_style, 0x0100, "BS_LEFT", "Left-aligns the text in the button rectangle.");
  add(category_t::button_style, 0x0020, "BS_LEFTTEXT", "Places the radio button circle or checkbox square on the right side of the control text.");
  add(category_t::button_style, 0x2000, "BS_MULTILINE", "Wraps the button text onto multiple lines if it is too long to fit on a single line.");
  add(category_t::button_style, 0x4000, "BS_NOTIFY", "Enables a button to send BN_KILLFOCUS");
  add(category_t::button_style, 0x000B, "BS_OWNERDRAW", "Creates an owner-drawn button where the parent window is responsible for painting the button's appearance.");
  add(category_t::button_style, 0x0000, "BS_PUSHBUTTON", "Creates a standard push button that posts a WM_COMMAND message to its owner window when clicked.");
  add(category_t::button_style, 0x1000, "BS_PUSHLIKE", "Makes a checkbox or radio button look and act like a standard push button.");
  add(category_t::button_style, 0x0004, "BS_RADIOBUTTON", "Creates a small circle with adjacent text");
  add(category_t::button_style, 0x0200, "BS_RIGHT", "Right-aligns the text in the button rectangle.");
  add(category_t::button_style, 0x0020, "BS_RIGHTBUTTON", "Positions the button box on the right side of the text (same as BS_LEFTTEXT).");
  add(category_t::button_style, 0x0000, "BS_TEXT", "Specifies that the button displays text (default behavior).");
  add(category_t::button_style, 0x0400, "BS_TOP", "Places text at the top of the button rectangle.");
  add(category_t::button_style, 0x0C00, "BS_VCENTER", "Vertically centers text in the button rectangle.");
  add(category_t::button_style, 0x8000, "BS_FLAT", "Specifies that the button is drawn flat");
  add(category_t::button_style, 0x0000000E, "BS_COMMANDLINK", "Creates a command link button with a large glyph and multi-line text.");
  add(category_t::button_style, 0x0000000F, "BS_DEFCOMMANDLINK", "Creates a default command link button that receives the default action when the user presses ENTER.");
  add(category_t::button_style, 0x0000000D, "BS_DEFSPLITBUTTON", "Same as BS_SPLITBUTTON, except the button is the default button.");
  add(category_t::button_style, 0x0000000C, "BS_SPLITBUTTON", "Creates a split button. A split button has a drop-down arrow.");
  add(category_t::button_style, 0x0000000F, "BS_TYPEMASK", "Mask used to retrieve the button type style bits.");
  add(category_t::button_style, 0x00000008, "BS_USERBUTTON", "Obsolete user-defined button style; the value is reserved.");

  /* ── ES_* Edit Control Styles ─────────────────────────────────── */
  add(category_t::edit_style, 0x0080, "ES_AUTOHSCROLL", "Automatically scrolls edit control text to the right when typing reaches the right border.");
  add(category_t::edit_style, 0x0040, "ES_AUTOVSCROLL", "Automatically scrolls edit control text up when pressing ENTER on the last visible line.");
  add(category_t::edit_style, 0x0001, "ES_CENTER", "Centers text in a single-line or multiline edit control.");
  add(category_t::edit_style, 0x0000, "ES_LEFT", "Left-aligns text in an edit control.");
  add(category_t::edit_style, 0x0010, "ES_LOWERCASE", "Converts all characters entered into the edit control to lowercase.");
  add(category_t::edit_style, 0x0004, "ES_MULTILINE", "Designates a multiline edit control allowing text entry on multiple lines.");
  add(category_t::edit_style, 0x0100, "ES_NOHIDESEL", "Prevents the edit control from hiding the text selection when it loses input focus.");
  add(category_t::edit_style, 0x2000, "ES_NUMBER", "Restricts the edit control input to digits only.");
  add(category_t::edit_style, 0x0400, "ES_OEMCONVERT", "Converts text typed in the edit control from ANSI to OEM character set and back.");
  add(category_t::edit_style, 0x0020, "ES_PASSWORD", "Displays an asterisk or custom mask character for every character typed into the edit control.");
  add(category_t::edit_style, 0x0800, "ES_READONLY", "Prevents the user from typing or editing text in the edit control.");
  add(category_t::edit_style, 0x0002, "ES_RIGHT", "Right-aligns text in a single-line or multiline edit control.");
  add(category_t::edit_style, 0x0008, "ES_UPPERCASE", "Converts all characters entered into the edit control to uppercase.");
  add(category_t::edit_style, 0x1000, "ES_WANTRETURN", "Inserts a carriage return when the user presses ENTER in a multiline edit control instead of submitting the dialog.");
  add(category_t::edit_style, 0x00002000, "ES_DISABLENOSCROLL", "Disables scroll bars instead of hiding them when they are not needed.");
  add(category_t::edit_style, 0x00080000, "ES_NOIME", "Disables the IME operation. This style is available for Asian language support only.");
  add(category_t::edit_style, 0x00000008, "ES_NOOLEDRAGDROP", "Disables OLE drag-and-drop support for the control.");
  add(category_t::edit_style, 0x00008000, "ES_SAVESEL", "Preserves the text selection when the control loses focus.");
  add(category_t::edit_style, 0x01000000, "ES_SELECTIONBAR", "Displays a selection bar at the left margin of the control.");
  add(category_t::edit_style, 0x00040000, "ES_SELFIME", "Directs the rich edit control to let the application handle all IME operations.");
  add(category_t::edit_style, 0x00004000, "ES_SUNKEN", "Displays a sunken edge border around the control.");
  add(category_t::edit_style, 0x00400000, "ES_VERTICAL", "Draws text vertically from top to bottom.");

  /* ── SS_* Static Control Styles ───────────────────────────────── */
  add(category_t::static_style, 0x000E, "SS_BITMAP", "Specifies that the static control displays a bitmap resource.");
  add(category_t::static_style, 0x0007, "SS_BLACKFRAME", "Displays a box drawn with a frame of the same color as window frames (default black).");
  add(category_t::static_style, 0x0004, "SS_BLACKRECT", "Displays a solid rectangle filled with the color used to draw window frames.");
  add(category_t::static_style, 0x0001, "SS_CENTER", "Designates a simple control displaying centered text wrapped automatically across lines.");
  add(category_t::static_style, 0x0200, "SS_CENTERIMAGE", "Centers the image or text vertically and horizontally inside the static control bounding area.");
  add(category_t::static_style, 0x2000, "SS_EDITCONTROL", "Mimics the text-wrapping and rendering behavior of a standard edit control in a static control.");
  add(category_t::static_style, 0x000F, "SS_ENHMETAFILE", "Specifies that the static control displays an enhanced metafile.");
  add(category_t::static_style, 0x0012, "SS_ETCHEDFRAME", "Draws the frame of the static control using the EDGE_ETCHED border style.");
  add(category_t::static_style, 0x0010, "SS_ETCHEDHORZ", "Draws a horizontal line using the EDGE_ETCHED style.");
  add(category_t::static_style, 0x0011, "SS_ETCHEDVERT", "Draws a vertical line using the EDGE_ETCHED style.");
  add(category_t::static_style, 0x0008, "SS_GRAYFRAME", "Displays a box drawn with a frame of the current desktop background color.");
  add(category_t::static_style, 0x0005, "SS_GRAYRECT", "Displays a solid rectangle filled with the current desktop background color.");
  add(category_t::static_style, 0x0003, "SS_ICON", "Displays an icon in the static control.");
  add(category_t::static_style, 0x0000, "SS_LEFT", "Left-aligns text in a static control with word wrapping.");
  add(category_t::static_style, 0x000C, "SS_LEFTNOWORDWRAP", "Left-aligns text without wrapping long lines; excess text is clipped.");
  add(category_t::static_style, 0x0080, "SS_NOPREFIX", "Prevents interpreting ampersands (&) as accelerator prefix characters in static control text.");
  add(category_t::static_style, 0x0100, "SS_NOTIFY", "Sends mouse click notification messages (STN_CLICKED");
  add(category_t::static_style, 0x000D, "SS_OWNERDRAW", "Delegates control painting responsibility to the parent window via WM_DRAWITEM.");
  add(category_t::static_style, 0x0040, "SS_REALSIZECONTROL", "Prevents resizing bitmap or icon images inside the static control");
  add(category_t::static_style, 0x0800, "SS_REALSIZEIMAGE", "Clips the image if it is larger than the static control bounds rather than scaling it.");
  add(category_t::static_style, 0x0002, "SS_RIGHT", "Right-aligns text in a static control with word wrapping.");
  add(category_t::static_style, 0x0400, "SS_RIGHTJUST", "Specifies that the upper-right corner of the static image/icon remains fixed when resized.");
  add(category_t::static_style, 0x000B, "SS_SIMPLE", "Displays a single line of left-aligned static text that cannot be wrapped or modified dynamically.");
  add(category_t::static_style, 0x1000, "SS_SUNKEN", "Draws a sunken 3D border around the static control.");
  add(category_t::static_style, 0x0009, "SS_WHITEFRAME", "Displays a box drawn with a frame matching the window background color (default white).");
  add(category_t::static_style, 0x0006, "SS_WHITERECT", "Displays a solid rectangle filled with the window background color.");
  add(category_t::static_style, 0x0000001F, "SS_TYPEMASK", "Bitmask used to isolate static control style types.");
  add(category_t::static_style, 0x00004000, "SS_ENDELLIPSIS", "Replaces characters at the end of static text with an ellipsis if they exceed control bounds.");
  add(category_t::static_style, 0x00008000, "SS_PATHELLIPSIS", "Replaces characters in the middle of a file path with an ellipsis to fit inside the static control.");
  add(category_t::static_style, 0x0000C000, "SS_WORDELLIPSIS", "Truncates text that extends beyond the control end and inserts an ellipsis after the last visible word.");
  add(category_t::static_style, 0x00040000, "SS_INMOTION", "Indicates the static control displays an animated content in motion (Vista+ animation flag).");

  /* ── LBS_* List Box Styles ────────────────────────────────────── */
  add(category_t::listbox_style, 0x1000, "LBS_DISABLENOSCROLL", "Shows a disabled vertical scroll bar in the list box when it contains too few items to scroll.");
  add(category_t::listbox_style, 0x0800, "LBS_EXTENDEDSEL", "Allows multiple selection using SHIFT and CTRL key combinations or mouse dragging.");
  add(category_t::listbox_style, 0x0040, "LBS_HASSTRINGS", "Specifies that an owner-drawn list box contains items consisting of strings.");
  add(category_t::listbox_style, 0x0200, "LBS_MULTICOLUMN", "Specifies a multi-column list box that scrolls horizontally.");
  add(category_t::listbox_style, 0x0008, "LBS_MULTIPLESEL", "Toggles item selection on each click without requiring SHIFT or CTRL modifier keys.");
  add(category_t::listbox_style, 0x2000, "LBS_NODATA", "Specifies a virtual list box");
  add(category_t::listbox_style, 0x0100, "LBS_NOINTEGRALHEIGHT", "Prevents the list box from automatically resizing to avoid showing partial items.");
  add(category_t::listbox_style, 0x0004, "LBS_NOREDRAW", "Prevents updating the list box visual representation when changes are made.");
  add(category_t::listbox_style, 0x4000, "LBS_NOSEL", "Specifies that items in the list box can be viewed but not selected.");
  add(category_t::listbox_style, 0x0001, "LBS_NOTIFY", "Sends notification messages to the parent window whenever the user clicks or double-clicks an item.");
  add(category_t::listbox_style, 0x0010, "LBS_OWNERDRAWFIXED", "Specifies an owner-drawn list box where all items have the same uniform height.");
  add(category_t::listbox_style, 0x0020, "LBS_OWNERDRAWVARIABLE", "Specifies an owner-drawn list box where individual items can vary in height.");
  add(category_t::listbox_style, 0x0002, "LBS_SORT", "Alphabetically sorts items added to the list box.");
  add(category_t::listbox_style, 0x00A00003, "LBS_STANDARD", "Combines LBS_NOTIFY");
  add(category_t::listbox_style, 0x0080, "LBS_USETABSTOPS", "Enables the list box to recognize and expand tab characters within item strings.");
  add(category_t::listbox_style, 0x0400, "LBS_WANTKEYBOARDINPUT", "Sends WM_VKEYTOITEM messages to the parent window whenever a key is pressed while the list box has focus.");
  add(category_t::listbox_style, 0x8000, "LBS_COMBOBOX", "Notifies a list box that it is part of a combo box. This allows coordination between the two controls so that they present a unified UI.");

  /* ── CBS_* Combo Box Styles ───────────────────────────────────── */
  add(category_t::combobox_style, 0x0040, "CBS_AUTOHSCROLL", "Automatically scrolls text in the combo box edit control horizontally when typed past the right edge.");
  add(category_t::combobox_style, 0x0800, "CBS_DISABLENOSCROLL", "Shows a disabled scroll bar in the drop-down list when it does not contain enough items to scroll.");
  add(category_t::combobox_style, 0x0002, "CBS_DROPDOWN", "Displays a drop-down list with an editable text field.");
  add(category_t::combobox_style, 0x0003, "CBS_DROPDOWNLIST", "Displays a drop-down list where the user can only select existing items (edit box is read-only).");
  add(category_t::combobox_style, 0x0200, "CBS_HASSTRINGS", "Specifies that an owner-drawn combo box contains items consisting of strings.");
  add(category_t::combobox_style, 0x4000, "CBS_LOWERCASE", "Converts all entered text in the combo box edit field to lowercase.");
  add(category_t::combobox_style, 0x0400, "CBS_NOINTEGRALHEIGHT", "Prevents the drop-down list from resizing automatically to show only complete items.");
  add(category_t::combobox_style, 0x0080, "CBS_OEMCONVERT", "Converts character sets from ANSI to OEM for text entered into the combo box edit field.");
  add(category_t::combobox_style, 0x0010, "CBS_OWNERDRAWFIXED", "Delegates drop-down item drawing to the parent window");
  add(category_t::combobox_style, 0x0020, "CBS_OWNERDRAWVARIABLE", "Delegates item drawing to the parent window");
  add(category_t::combobox_style, 0x0001, "CBS_SIMPLE", "Displays the list box at all times underneath the edit control.");
  add(category_t::combobox_style, 0x0100, "CBS_SORT", "Alphabetically sorts items added to the combo box list.");
  add(category_t::combobox_style, 0x2000, "CBS_UPPERCASE", "Converts all entered text in the combo box edit field to uppercase.");

  /* ── SBS_* Scroll Bar Styles ──────────────────────────────────── */
  add(category_t::scrollbar_style, 0x0004, "SBS_BOTTOMALIGN", "Aligns the scroll bar along the bottom edge of the defined rectangle.");
  add(category_t::scrollbar_style, 0x0000, "SBS_HORZ", "Specifies a horizontal scroll bar.");
  add(category_t::scrollbar_style, 0x0002, "SBS_LEFTALIGN", "Aligns the scroll bar along the left edge of the defined rectangle.");
  add(category_t::scrollbar_style, 0x0004, "SBS_RIGHTALIGN", "Aligns the scroll bar along the right edge of the defined rectangle.");
  add(category_t::scrollbar_style, 0x0008, "SBS_SIZEBOX", "Specifies a sizing box control instead of a scroll bar.");
  add(category_t::scrollbar_style, 0x0004, "SBS_SIZEBOXBOTTOMRIGHTALIGN", "Aligns the size box with the bottom-right corner of the window.");
  add(category_t::scrollbar_style, 0x0002, "SBS_SIZEBOXTOPLEFTALIGN", "Aligns the size box with the top-left corner of the window.");
  add(category_t::scrollbar_style, 0x0010, "SBS_SIZEGRIP", "Displays a sizing box featuring a visible grip pattern.");
  add(category_t::scrollbar_style, 0x0002, "SBS_TOPALIGN", "Aligns the scroll bar along the top edge of the defined rectangle.");
  add(category_t::scrollbar_style, 0x0001, "SBS_VERT", "Specifies a vertical scroll bar.");

  /* ── LVS_* List-View Styles ───────────────────────────────────── */
  add(category_t::listview_style, 0x0800, "LVS_ALIGNLEFT", "Left-aligns items in the list-view control.");
  add(category_t::listview_style, 0x0c00, "LVS_ALIGNMASK", "Mask used to retrieve the alignment style bits of a list-view control.");
  add(category_t::listview_style, 0x0000, "LVS_ALIGNTOP", "Top-aligns items in the list-view control.");
  add(category_t::listview_style, 0x0100, "LVS_AUTOARRANGE", "Automatically arranges items in icon view when the list-view window content changes.");
  add(category_t::listview_style, 0x0080, "LVS_EDITLABELS", "Allows item text to be edited in-place by the user.");
  add(category_t::listview_style, 0x0000, "LVS_ICON", "Displays items as large icons with labels.");
  add(category_t::listview_style, 0x0003, "LVS_LIST", "Displays items as a small-icon list with labels.");
  add(category_t::listview_style, 0x4000, "LVS_NOCOLUMNHEADER", "Hides the column header in report view.");
  add(category_t::listview_style, 0x0080, "LVS_NOLABELWRAP", "Prevents item labels from wrapping in icon view; text is truncated instead.");
  add(category_t::listview_style, 0x2000, "LVS_NOSCROLL", "Disables scrolling in the list-view control.");
  add(category_t::listview_style, 0x8000, "LVS_NOSORTHEADER", "Disables column header click sorting in report view.");
  add(category_t::listview_style, 0x1000, "LVS_OWNERDATA", "Creates a virtual list-view control where the application supplies item data on demand.");
  add(category_t::listview_style, 0x0001, "LVS_REPORT", "Displays items in report view with columns.");
  add(category_t::listview_style, 0x0040, "LVS_SHAREIMAGELISTS", "Does not destroy the image list when the control is destroyed; allows sharing image lists.");
  add(category_t::listview_style, 0x0008, "LVS_SHOWSELALWAYS", "Keeps the selection visible even when the control loses focus.");
  add(category_t::listview_style, 0x0004, "LVS_SINGLESEL", "Allows only one item to be selected at a time.");
  add(category_t::listview_style, 0x0010, "LVS_SORTASCENDING", "Sorts items in ascending order using the current sort column.");
  add(category_t::listview_style, 0x0020, "LVS_SORTDESCENDING", "Sorts items in descending order using the current sort column.");
  add(category_t::listview_style, 0x0003, "LVS_TYPEMASK", "Mask used to retrieve the view-type bits of a list-view control.");
  add(category_t::listview_style, 0xfc00, "LVS_TYPESTYLEMASK", "Mask used to retrieve the type and style bits that affect arranging in list-view.");
  add(category_t::listview_style, 0x0400, "LVS_OWNERDRAWFIXED", "Specifies that the owner window paints items in report view.");
  add(category_t::listview_style, 0x0002, "LVS_SMALLICON", "Displays items as small icons with labels.");

  /* ── TVS_* Tree-View Styles ──────────────────────────────────── */
  add(category_t::treeview_style, 0x0100, "TVS_CHECKBOXES", "Displays check boxes next to each item in the tree-view control.");
  add(category_t::treeview_style, 0x0010, "TVS_DISABLEDRAGDROP", "Prevents the user from dragging items in the tree-view control.");
  add(category_t::treeview_style, 0x0008, "TVS_EDITLABELS", "Allows item text to be edited in-place by the user.");
  add(category_t::treeview_style, 0x1000, "TVS_FULLROWSELECT", "Selects the entire row of the item when clicked, not just the icon or text.");
  add(category_t::treeview_style, 0x0001, "TVS_HASBUTTONS", "Displays plus/minus buttons next to parent items for expanding and collapsing.");
  add(category_t::treeview_style, 0x0002, "TVS_HASLINES", "Displays lines connecting parent items to their children in the tree.");
  add(category_t::treeview_style, 0x0004, "TVS_LINESATROOT", "Displays lines connecting root-level items to each other.");
  add(category_t::treeview_style, 0x8000, "TVS_NOHSCROLL", "Disables horizontal scrolling in the tree-view control.");
  add(category_t::treeview_style, 0x2000, "TVS_NOSCROLL", "Disables both horizontal and vertical scrolling in the tree-view control.");
  add(category_t::treeview_style, 0x4000, "TVS_NONEVENHEIGHT", "Allows tree-view items to have odd heights; otherwise items are forced to even heights.");
  add(category_t::treeview_style, 0x0040, "TVS_RTLREADING", "Displays text using right-to-left reading order.");
  add(category_t::treeview_style, 0x0020, "TVS_SHOWSELALWAYS", "Keeps the selected item visible even when the control loses focus.");
  add(category_t::treeview_style, 0x0400, "TVS_SINGLEEXPAND", "Expands the selected item when it is expanded; collapses other expanded items.");
  add(category_t::treeview_style, 0x0200, "TVS_TRACKSELECT", "Enables hot-tracking of tree-view items as the mouse pointer moves over them.");
  add(category_t::treeview_style, 0x0080, "TVS_NOTOOLTIPS", "Disables tooltips for tree-view items.");
  add(category_t::treeview_style, 0x0800, "TVS_INFOTIP", "Enables information tooltips for tree-view items that display the full description of each item.");

  /* ── TBS_* Trackbar Styles ───────────────────────────────────── */
  add(category_t::trackbar_style, 0x0001, "TBS_AUTOTICKS", "Automatically creates tick marks at each increment along the trackbar.");
  add(category_t::trackbar_style, 0x0008, "TBS_BOTH", "Displays tick marks on both sides of the trackbar.");
  add(category_t::trackbar_style, 0x0000, "TBS_BOTTOM", "Displays tick marks on the bottom of the trackbar.");
  add(category_t::trackbar_style, 0x0020, "TBS_ENABLESELRANGE", "Enables a selection range in the trackbar control.");
  add(category_t::trackbar_style, 0x0040, "TBS_FIXEDLENGTH", "Allows the trackbar thumb to have a fixed size, not scaled to the trackbar range.");
  add(category_t::trackbar_style, 0x0000, "TBS_HORZ", "Displays a horizontal trackbar.");
  add(category_t::trackbar_style, 0x0004, "TBS_LEFT", "Displays tick marks on the left side of the trackbar.");
  add(category_t::trackbar_style, 0x0080, "TBS_NOTHUMB", "Hides the trackbar thumb (slider) so the user cannot drag it.");
  add(category_t::trackbar_style, 0x0010, "TBS_NOTICKS", "Hides the tick marks along the trackbar.");
  add(category_t::trackbar_style, 0x0004, "TBS_TOP", "Displays tick marks on the top of the trackbar.");
  add(category_t::trackbar_style, 0x0002, "TBS_VERT", "Displays a vertical trackbar.");
  add(category_t::trackbar_style, 0x0000, "TBS_TOPTOP", "Displays tick marks on the top and positions the thumb at the top.");
  add(category_t::trackbar_style, 0x0000, "TBS_RIGHT", "Displays tick marks on the right side of the trackbar (same as TBS_BOTTOM).");
  add(category_t::trackbar_style, 0x0400, "TBS_DOWNISLEFT", "Places down and left on the left side of the control (reversed layout).");
  add(category_t::trackbar_style, 0x0800, "TBS_NOTIFYBEFOREMOVE", "Notifies the parent before repositioning the slider due to user action (enables snapping).");
  add(category_t::trackbar_style, 0x0200, "TBS_REVERSED", "Reverses the trackbar orientation so smaller values are at the high end.");
  add(category_t::trackbar_style, 0x0100, "TBS_TOOLTIPS", "Enables tooltips for the trackbar.");
  add(category_t::trackbar_style, 0x1000, "TBS_TRANSPARENTBKGND", "The background is painted by the parent via WM_PRINTCLIENT.");

  /* ── PBS_* Progress Bar Styles ───────────────────────────────── */
  add(category_t::progressbar_style, 0x0008, "PBS_MARQUEE", "Enables marquee mode where the progress bar animates continuously without a specific position.");
  add(category_t::progressbar_style, 0x0001, "PBS_SMOOTH", "Fills the progress bar with a smooth bar rather than segmented blocks.");
  add(category_t::progressbar_style, 0x0010, "PBS_SMOOTHREVERSE", "Fills the progress bar with a smooth bar in reverse direction for smooth animation.");
  add(category_t::progressbar_style, 0x0004, "PBS_VERTICAL", "Displays the progress bar vertically.");
  add(category_t::progressbar_style, 0x0100, "PBS_TEXTONLY", "Displays only text on the progress bar instead of the bar graphic.");

  /* ── UDS_* Up-Down Control Styles ────────────────────────────── */
  add(category_t::updown_style, 0x0008, "UDS_ALIGNLEFT", "Aligns the up-down control to the left of the buddy edit control.");
  add(category_t::updown_style, 0x0004, "UDS_ALIGNRIGHT", "Aligns the up-down control to the right of the buddy edit control.");
  add(category_t::updown_style, 0x0020, "UDS_ARROWKEYS", "Enables arrow key support to increment or decrement the up-down control value.");
  add(category_t::updown_style, 0x0010, "UDS_AUTOBUDDY", "Automatically selects the previous control in the dialog template as the buddy window.");
  add(category_t::updown_style, 0x0040, "UDS_HORZ", "Displays the up and down arrows horizontally instead of vertically.");
  add(category_t::updown_style, 0x0080, "UDS_NOTHOUSANDS", "Does not insert a thousands separator in the buddy edit control.");
  add(category_t::updown_style, 0x0002, "UDS_SETBUDDYINT", "Sets the buddy edit control text to an integer value whenever the up-down control changes.");
  add(category_t::updown_style, 0x0001, "UDS_WRAP", "Causes the control to wrap around to the other end when reaching the maximum or minimum value.");
  add(category_t::updown_style, 0x0100, "UDS_HOTTRACK", "Displays a hot-tracking border on the up-down control when the pointer hovers over it.");

  /* ── DTS_* Date/Time Picker Styles ───────────────────────────── */
  add(category_t::datetime_style, 0x0010, "DTS_APPCANPARSE", "Enables the owner to parse user input in the date/time picker control.");
  add(category_t::datetime_style, 0x0004, "DTS_LONGDATEFORMAT", "Uses the long date format string as specified in the user's locale settings.");
  add(category_t::datetime_style, 0x0020, "DTS_RIGHTALIGN", "Right-aligns the drop-down calendar with the control.");
  add(category_t::datetime_style, 0x0002, "DTS_SHOWNONE", "Displays a check box next to the control; when unchecked, no date is selected.");
  // add(category_t::datetime_style, 0x0008, "DTS_TIMEFORMAT", "Uses the time format string as specified in the user's locale settings.");
  add(category_t::datetime_style, 0x0009, "DTS_TIMEFORMAT", "Uses the time format string as specified in the user's locale settings.");
  add(category_t::datetime_style, 0x0001, "DTS_UPDOWN", "Displays an up-down control rather than a drop-down calendar for date selection.");
  add(category_t::datetime_style, 0x0000, "DTS_SHORTDATEFORMAT", "Uses the short date format as specified in the user's locale settings (default).");
  // add(category_t::datetime_style, 0x0020, "DTS_MONTHCAL", "Displays a month calendar control as a drop-down rather than a simple drop-down list."); // Not defined in CommCtrl.h; conflicts with DTS_RIGHTALIGN.
  add(category_t::datetime_style, 0x000C, "DTS_SHORTDATECENTURYFORMAT", "Similar to DTS_SHORTDATEFORMAT, except the year is a four-digit field.");

  /* ── TCS_* Tab Control Styles ────────────────────────────────── */
  add(category_t::tabcontrol_style, 0x0002, "TCS_BOTTOM", "Places tabs at the bottom of the control.");
  add(category_t::tabcontrol_style, 0x0002, "TCS_BUTTONS", "Displays tabs as push buttons.");
  // add(category_t::tabcontrol_style, 0x00000001, "TCS_EX_FLATSEPARATORS", "Draws flat separators between tabs in the tab control.");
  // add(category_t::tabcontrol_style, 0x00000002, "TCS_EX_REGISTERDROP", "Enables tab items to receive WM_QUERYDROPPOSITION notifications.");
  add(category_t::tabcontrol_style, 0x0040, "TCS_FIXEDWIDTH", "All tabs are the same width; individual text length is ignored.");
  add(category_t::tabcontrol_style, 0x0008, "TCS_FLATBUTTONS", "Displays tabs as flat push buttons in tab controls without borders.");
  add(category_t::tabcontrol_style, 0x0800, "TCS_FOCUSNEVER", "Prevents the tab control from receiving input focus.");
  add(category_t::tabcontrol_style, 0x0100, "TCS_FOCUSONBUTTONDOWN", "Gives input focus to the selected tab when the user clicks on a tab.");
  add(category_t::tabcontrol_style, 0x0010, "TCS_FORCESELECTLEFT", "Forces the selected tab to be the leftmost visible tab when the tab control gains focus.");
  add(category_t::tabcontrol_style, 0x0100, "TCS_MULTILINE", "Displays multiple rows of tabs if necessary so all tabs are visible.");
  add(category_t::tabcontrol_style, 0x0004, "TCS_MULTISELECT", "Enables multiple tab selection using CTRL+click in the tab control.");
  add(category_t::tabcontrol_style, 0x0200, "TCS_OWNERDRAWFIXED", "Enables the tab control owner to draw tab items in a fixed size.");
  add(category_t::tabcontrol_style, 0x0080, "TCS_RAGGEDRIGHT", "Prevents tabs from being stretched to fill the tab control width.");
  add(category_t::tabcontrol_style, 0x0020, "TCS_RIGHTJUSTIFY", "Right-justifies tabs when the tab control is multiline.");
  add(category_t::tabcontrol_style, 0x0001, "TCS_SCROLLOP", "Enables horizontal scrolling of multiple rows of tabs.");
  add(category_t::tabcontrol_style, 0x0400, "TCS_TOOLTIPS", "Enables tooltips for each tab in the tab control.");
  add(category_t::tabcontrol_style, 0x0010, "TCS_FORCEICONLEFT", "Forces the icon to be left-aligned in each tab.");
  add(category_t::tabcontrol_style, 0x0020, "TCS_FORCELABELLEFT", "Forces the label to be left-aligned in each tab.");
  add(category_t::tabcontrol_style, 0x0040, "TCS_HOTTRACK", "Underlines the tab label under the pointer to indicate that the tab can be selected.");
  add(category_t::tabcontrol_style, 0x0002, "TCS_RIGHT", "Places tabs on the right side of the control (same as TCS_BOTTOM).");
  add(category_t::tabcontrol_style, 0x0001, "TCS_SCROLLOPPOSITE", "Enables horizontal scrolling of multiple rows of tabs (same as TCS_SCROLLOP).");
  add(category_t::tabcontrol_style, 0x0000, "TCS_SINGLELINE", "Displays only one row of tabs.");
  add(category_t::tabcontrol_style, 0x0000, "TCS_TABS", "Displays tabs as normal tabs (default).");
  add(category_t::tabcontrol_style, 0x0080, "TCS_VERTICAL", "Displays tabs vertically.");

  /* ── TCS_EX_* Extended Tab Control Styles ─────────────────────── */
  add(category_t::tabcontrol_ex_style, 0x00000001, "TCS_EX_FLATSEPARATORS", "Draws flat separators between tabs in the tab control.");
  add(category_t::tabcontrol_ex_style, 0x00000002, "TCS_EX_REGISTERDROP", "Enables tab items to receive WM_QUERYDROPPOSITION notifications.");

  /* ── SB_* Scroll Bar Constants ───────────────────────────────── */
  add(category_t::scrollbar_style, 0x0000, "SB_HORZ", "Identifies a horizontal scroll bar for scroll bar messages.");
  add(category_t::scrollbar_style, 0x0001, "SB_VERT", "Identifies a vertical scroll bar for scroll bar messages.");
  add(category_t::scrollbar_style, 0x0002, "SB_CTL", "Identifies a scroll bar control for scroll bar messages.");
  add(category_t::scrollbar_style, 0x0003, "SB_BOTH", "Identifies both horizontal and vertical scroll bars for scroll bar messages.");

  /* ── SBARS_* Status Bar Styles ──────────────────────────────── */
  // add(category_t::scrollbar_style, 0x0100, "SBARS_SIZEGRIP", "Displays a size grip in the lower-right corner of the status bar window.");
  // add(category_t::scrollbar_style, 0x0800, "SBARS_TOOLTIPS", "Enables tooltips for status bar parts when the mouse hovers over them.");
  add(category_t::statusbar_style, 0x0100, "SBARS_SIZEGRIP", "Displays a size grip in the lower-right corner of the status bar window.");
  add(category_t::statusbar_style, 0x0800, "SBARS_TOOLTIPS", "Enables tooltips for status bar parts when the mouse hovers over them.");
  add(category_t::statusbar_style, 0x0800, "SBT_TOOLTIPS", "Identical to SBARS_TOOLTIPS; use for version 5.00 or later.");

  /* ── MCS_* Month Calendar Styles ─────────────────────────────── */
  // add(category_t::datetime_style, 0x0001, "MCS_DAYSTATE", "Requests the parent to supply day state information to display bolded dates.");
  // add(category_t::datetime_style, 0x0002, "MCS_MULTISELECT", "Enables selection of a range of dates in the month calendar control.");
  // add(category_t::datetime_style, 0x0004, "MCS_WEEKNUMBERS", "Displays week numbers along the left edge of the calendar.");
  // add(category_t::datetime_style, 0x0008, "MCS_NOTODAYCIRCLE", "Disables the circle that highlights today's date in the month calendar.");
  // add(category_t::datetime_style, 0x0010, "MCS_NOTODAY", "Hides the today selection row at the bottom of the month calendar control.");
  // add(category_t::datetime_style, 0x0020, "MCS_NORROWHEAD", "Hides the left column that shows the week numbers in the month calendar."); // Not defined in CommCtrl.h.
  // add(category_t::datetime_style, 0x0040, "MCS_NODATEPICK", "Hides the date picker at the top of the month calendar control."); // Not defined in CommCtrl.h; conflicts with MCS_NOTRAILINGDATES.
  // add(category_t::datetime_style, 0x0100, "MCS_NOSELCHANGEONNAV", "The selection is not changed when the user navigates next or previous in the calendar.");
  // add(category_t::datetime_style, 0x0040, "MCS_NOTRAILINGDATES", "Dates from the previous and next months are not displayed in the current month's calendar.");
  // add(category_t::datetime_style, 0x0080, "MCS_SHORTDAYSOFWEEK", "Short day names are displayed in the header.");
  add(category_t::month_calendar_style, 0x0001, "MCS_DAYSTATE", "Requests the parent to supply day state information to display bolded dates.");
  add(category_t::month_calendar_style, 0x0002, "MCS_MULTISELECT", "Enables selection of a range of dates in the month calendar control.");
  add(category_t::month_calendar_style, 0x0004, "MCS_WEEKNUMBERS", "Displays week numbers along the left edge of the calendar.");
  add(category_t::month_calendar_style, 0x0008, "MCS_NOTODAYCIRCLE", "Disables the circle that highlights today's date in the month calendar.");
  add(category_t::month_calendar_style, 0x0010, "MCS_NOTODAY", "Hides the today selection row at the bottom of the month calendar control.");
  add(category_t::month_calendar_style, 0x0040, "MCS_NOTRAILINGDATES", "Dates from the previous and next months are not displayed in the current month's calendar.");
  add(category_t::month_calendar_style, 0x0080, "MCS_SHORTDAYSOFWEEK", "Short day names are displayed in the header.");
  add(category_t::month_calendar_style, 0x0100, "MCS_NOSELCHANGEONNAV", "The selection is not changed when the user navigates next or previous in the calendar.");

  /* ── CCS_* Common Control Styles ─────────────────────────────── */
  // docs/win32/desktop-src/Controls/common-control-styles.md
  add(category_t::common_control_style, 0x00000020, "CCS_ADJUSTABLE", "Enables a toolbar's built-in customization features.");
  add(category_t::common_control_style, 0x00000003, "CCS_BOTTOM", "Positions the control at the bottom of the parent window's client area.");
  add(category_t::common_control_style, 0x00000081, "CCS_LEFT", "Displays the control vertically on the left side of the parent window.");
  add(category_t::common_control_style, 0x00000040, "CCS_NODIVIDER", "Prevents a two-pixel highlight from being drawn at the top of the control.");
  add(category_t::common_control_style, 0x00000082, "CCS_NOMOVEX", "Resizes and moves the control vertically, but not horizontally, in response to WM_SIZE.");
  add(category_t::common_control_style, 0x00000002, "CCS_NOMOVEY", "Resizes and moves the control horizontally, but not vertically, in response to WM_SIZE.");
  add(category_t::common_control_style, 0x00000008, "CCS_NOPARENTALIGN", "Prevents the control from automatically moving to the top or bottom of the parent window.");
  add(category_t::common_control_style, 0x00000004, "CCS_NORESIZE", "Prevents the control from using the default width and height when setting its size.");
  add(category_t::common_control_style, 0x00000083, "CCS_RIGHT", "Displays the control vertically on the right side of the parent window.");
  add(category_t::common_control_style, 0x00000001, "CCS_TOP", "Positions the control at the top of the parent window's client area.");
  add(category_t::common_control_style, 0x00000080, "CCS_VERT", "Displays the control vertically.");

  /* ── HDS_* Header Control Styles ─────────────────────────────── */
  // docs/win32/desktop-src/Controls/header-control-styles.md
  add(category_t::header_style, 0x0002, "HDS_BUTTONS", "Each item in the control looks and behaves like a push button.");
  add(category_t::header_style, 0x0400, "HDS_CHECKBOXES", "Allows the placing of checkboxes on header items.");
  add(category_t::header_style, 0x0040, "HDS_DRAGDROP", "Allows drag-and-drop reordering of header items.");
  add(category_t::header_style, 0x0100, "HDS_FILTERBAR", "Includes a filter bar as part of the standard header control.");
  add(category_t::header_style, 0x0200, "HDS_FLAT", "Causes the header control to be drawn flat in classic mode.");
  add(category_t::header_style, 0x0080, "HDS_FULLDRAG", "Displays column contents even while the user resizes a column.");
  add(category_t::header_style, 0x0008, "HDS_HIDDEN", "Indicates a header control that is intended to be hidden.");
  add(category_t::header_style, 0x0000, "HDS_HORZ", "Creates a header control with a horizontal orientation.");
  add(category_t::header_style, 0x0004, "HDS_HOTTRACK", "Enables hot tracking.");
  add(category_t::header_style, 0x0800, "HDS_NOSIZING", "The user cannot drag the divider on the header control.");
  add(category_t::header_style, 0x1000, "HDS_OVERFLOW", "Displays a button when not all items fit within the header control's rectangle.");

  /* ── LVS_EX_* Extended List-View Styles ──────────────────────── */
  // docs/win32/desktop-src/Controls/extended-list-view-styles.md
  add(category_t::listview_ex_style, 0x01000000, "LVS_EX_AUTOAUTOARRANGE", "Automatically arranges icons if no icon positions have been set.");
  add(category_t::listview_ex_style, 0x08000000, "LVS_EX_AUTOCHECKSELECT", "Automatically selects check boxes on single click.");
  add(category_t::listview_ex_style, 0x10000000, "LVS_EX_AUTOSIZECOLUMNS", "Automatically sizes listview columns.");
  add(category_t::listview_ex_style, 0x00008000, "LVS_EX_BORDERSELECT", "Changes border color when an item is selected instead of highlighting the item.");
  add(category_t::listview_ex_style, 0x00000004, "LVS_EX_CHECKBOXES", "Enables check boxes for items in a list-view control.");
  add(category_t::listview_ex_style, 0x80000000, "LVS_EX_COLUMNOVERFLOW", "Displays an overflow button in icon/tile view when there is not enough client width.");
  add(category_t::listview_ex_style, 0x40000000, "LVS_EX_COLUMNSNAPPOINTS", "Snaps to minimum column width when the user resizes a column.");
  add(category_t::listview_ex_style, 0x00010000, "LVS_EX_DOUBLEBUFFER", "Paints via double-buffering, which reduces flicker.");
  add(category_t::listview_ex_style, 0x00000100, "LVS_EX_FLATSB", "Enables flat scroll bars in the list view.");
  add(category_t::listview_ex_style, 0x00000020, "LVS_EX_FULLROWSELECT", "Highlights the item and all its subitems when selected (LVS_REPORT only).");
  add(category_t::listview_ex_style, 0x00000001, "LVS_EX_GRIDLINES", "Displays gridlines around items and subitems (LVS_REPORT only).");
  add(category_t::listview_ex_style, 0x00000010, "LVS_EX_HEADERDRAGDROP", "Enables drag-and-drop reordering of columns (LVS_REPORT only).");
  add(category_t::listview_ex_style, 0x02000000, "LVS_EX_HEADERINALLVIEWS", "Shows column headers in all view modes.");
  add(category_t::listview_ex_style, 0x00020000, "LVS_EX_HIDELABELS", "Hides the labels in icon and small icon view.");
  add(category_t::listview_ex_style, 0x00000400, "LVS_EX_INFOTIP", "Sends LVN_GETINFOTIP to the parent before displaying an item's tooltip.");
  add(category_t::listview_ex_style, 0x00200000, "LVS_EX_JUSTIFYCOLUMNS", "Icons are lined up in columns that use up the whole view.");
  add(category_t::listview_ex_style, 0x00004000, "LVS_EX_LABELTIP", "Unfolds partly hidden labels in any list view mode.");
  add(category_t::listview_ex_style, 0x00002000, "LVS_EX_MULTIWORKAREAS", "Does not autoarrange icons until one or more work areas are defined.");
  add(category_t::listview_ex_style, 0x00000040, "LVS_EX_ONECLICKACTIVATE", "Sends LVN_ITEMACTIVATE when the user clicks an item; enables hot tracking.");
  add(category_t::listview_ex_style, 0x00000200, "LVS_EX_REGIONAL", "Sets the list view window region to include only item icons and text.");
  add(category_t::listview_ex_style, 0x00100000, "LVS_EX_SIMPLESELECT", "Moves the state image to the top right of the large icon rendering in icon view.");
  add(category_t::listview_ex_style, 0x00040000, "LVS_EX_SINGLEROW", "Reserved; not used.");
  add(category_t::listview_ex_style, 0x00080000, "LVS_EX_SNAPTOGRID", "Icons automatically snap into a grid in icon view.");
  add(category_t::listview_ex_style, 0x00000002, "LVS_EX_SUBITEMIMAGES", "Allows images to be displayed for subitems (LVS_REPORT only).");
  add(category_t::listview_ex_style, 0x00000008, "LVS_EX_TRACKSELECT", "Enables hot-track selection in a list-view control.");
  add(category_t::listview_ex_style, 0x00400000, "LVS_EX_TRANSPARENTBKGND", "The background is painted by the parent via WM_PRINTCLIENT.");
  add(category_t::listview_ex_style, 0x00800000, "LVS_EX_TRANSPARENTSHADOWTEXT", "Enables shadow text on transparent backgrounds only.");
  add(category_t::listview_ex_style, 0x00000080, "LVS_EX_TWOCLICKACTIVATE", "Sends LVN_ITEMACTIVATE when the user double-clicks an item; enables hot tracking.");
  add(category_t::listview_ex_style, 0x00001000, "LVS_EX_UNDERLINECOLD", "Underlines non-hot items that may be activated (requires LVS_EX_TWOCLICKACTIVATE).");
  add(category_t::listview_ex_style, 0x00000800, "LVS_EX_UNDERLINEHOT", "Underlines hot items that may be activated (requires click/two-click activate).");

  /* ── TVS_EX_* Extended Tree-View Styles ──────────────────────── */
  // docs/win32/desktop-src/Controls/tree-view-control-window-extended-styles.md
  add(category_t::treeview_ex_style, 0x0020, "TVS_EX_AUTOHSCROLL", "Removes the horizontal scroll bar and auto-scrolls depending on mouse position.");
  add(category_t::treeview_ex_style, 0x0200, "TVS_EX_DIMMEDCHECKBOXES", "Adds a dimmed checkbox state indicating a node selected because its parent is selected.");
  add(category_t::treeview_ex_style, 0x0004, "TVS_EX_DOUBLEBUFFER", "Specifies how the background is erased or filled.");
  add(category_t::treeview_ex_style, 0x0400, "TVS_EX_DRAWIMAGEASYNC", "Specifies that images are drawn asynchronously.");
  add(category_t::treeview_ex_style, 0x0100, "TVS_EX_EXCLUSIONCHECKBOXES", "Adds an exclusion checkbox state (red X) in addition to the normal checkbox states.");
  add(category_t::treeview_ex_style, 0x0040, "TVS_EX_FADEINOUTEXPANDOS", "Fades expando buttons in or out when the mouse enters or leaves the control.");
  add(category_t::treeview_ex_style, 0x0002, "TVS_EX_MULTISELECT", "Not supported; do not use.");
  add(category_t::treeview_ex_style, 0x0008, "TVS_EX_NOINDENTSTATE", "Does not indent the tree view for the expando buttons.");
  add(category_t::treeview_ex_style, 0x0001, "TVS_EX_NOSINGLECOLLAPSE", "Does not collapse the previously selected item unless it shares the same parent.");
  add(category_t::treeview_ex_style, 0x0080, "TVS_EX_PARTIALCHECKBOXES", "Adds a partial (square) checkbox state in addition to the normal checkbox states.");
  add(category_t::treeview_ex_style, 0x0010, "TVS_EX_RICHTOOLTIP", "Allows rich tooltips in the tree view (custom drawn with icon and text).");

  /* ── ES_EX_* Extended Edit Control Styles ────────────────────── */
  // docs/win32/desktop-src/Controls/edit-control-window-extended-styles.md
  add(category_t::edit_ex_style, 0x0001, "ES_EX_ALLOWEOL_CR", "Enables support for carriage return (CR) end-of-line characters to break lines.");
  add(category_t::edit_ex_style, 0x0002, "ES_EX_ALLOWEOL_LF", "Enables support for linefeed (LF) end-of-line characters to break lines.");
  add(category_t::edit_ex_style, 0x0003, "ES_EX_ALLOWEOL_ALL", "Enables support for both CR and LF end-of-line characters to break lines.");
  add(category_t::edit_ex_style, 0x0004, "ES_EX_CONVERT_EOL_ON_PASTE", "Converts end-of-line characters in pasted content to match the current document.");
  add(category_t::edit_ex_style, 0x0010, "ES_EX_ZOOMABLE", "Enables zooming using Ctrl+MouseWheel and the EM_GETZOOM/EM_SETZOOM messages.");
  add(category_t::edit_ex_style, 0x01000000, "ES_EX_NOCALLOLEINIT", "Prevents the control from calling OleInitialize when created (dialog templates only).");

  /* ── PGS_* Pager Control Styles ──────────────────────────────── */
  // docs/win32/desktop-src/Controls/pager-control-styles.md
  add(category_t::pager_style, 0x00000002, "PGS_AUTOSCROLL", "The pager control scrolls when the user hovers the mouse over one of the scroll buttons.");
  add(category_t::pager_style, 0x00000004, "PGS_DRAGNDROP", "The contained window can be a drag-and-drop target.");
  add(category_t::pager_style, 0x00000001, "PGS_HORZ", "Creates a pager control that can be scrolled horizontally.");
  add(category_t::pager_style, 0x00000000, "PGS_VERT", "Creates a pager control that can be scrolled vertically (default).");

  /* ── RBS_* Rebar Control Styles ──────────────────────────────── */
  // docs/win32/desktop-src/Controls/rebar-control-styles.md
  add(category_t::rebar_style, 0x00002000, "RBS_AUTOSIZE", "The rebar control automatically changes the layout of the bands when the size changes.");
  add(category_t::rebar_style, 0x00000400, "RBS_BANDBORDERS", "The rebar control displays narrow lines to separate adjacent bands.");
  add(category_t::rebar_style, 0x00008000, "RBS_DBLCLKTOGGLE", "A rebar band toggles its maximized/minimized state when double-clicked.");
  add(category_t::rebar_style, 0x00000800, "RBS_FIXEDORDER", "The rebar control always displays bands in the same order.");
  add(category_t::rebar_style, 0x00001000, "RBS_REGISTERDROP", "Generates RBN_GETOBJECT notifications when an object is dragged over a band.");
  add(category_t::rebar_style, 0x00000100, "RBS_TOOLTIPS", "Not yet supported.");
  add(category_t::rebar_style, 0x00000200, "RBS_VARHEIGHT", "Displays bands at the minimum required height, when possible.");
  add(category_t::rebar_style, 0x00004000, "RBS_VERTICALGRIPPER", "Displays the size grip vertically in a vertical rebar control.");

  /* ── TTS_* Tooltip Styles ────────────────────────────────────── */
  // docs/win32/desktop-src/Controls/tooltip-styles.md
  add(category_t::tooltip_style, 0x01, "TTS_ALWAYSTIP", "The tooltip appears when the cursor is on a tool even if the owner window is inactive.");
  add(category_t::tooltip_style, 0x40, "TTS_BALLOON", "The tooltip has the appearance of a cartoon balloon with a stem.");
  add(category_t::tooltip_style, 0x80, "TTS_CLOSE", "Displays a Close button on the tooltip (requires TTS_BALLOON and a title).");
  add(category_t::tooltip_style, 0x10, "TTS_NOANIMATE", "Disables sliding tooltip animation.");
  add(category_t::tooltip_style, 0x20, "TTS_NOFADE", "Disables fading tooltip animation.");
  add(category_t::tooltip_style, 0x02, "TTS_NOPREFIX", "Prevents the system from stripping ampersands and terminating strings at tabs.");
  add(category_t::tooltip_style, 0x100, "TTS_USEVISUALSTYLE", "Uses themed hyperlinks (requires TTF_PARSELINKS).");

  /* ── TBSTYLE_* / BTNS_* Toolbar Styles ───────────────────────── */
  // docs/win32/desktop-src/Controls/toolbar-control-and-button-styles.md
  add(category_t::toolbar_style, 0x0400, "TBSTYLE_ALTDRAG", "Allows users to change a toolbar button's position by dragging with ALT held down.");
  add(category_t::toolbar_style, 0x0010, "TBSTYLE_AUTOSIZE", "Calculates the button's width based on the width of the text plus the image.");
  add(category_t::toolbar_style, 0x0000, "TBSTYLE_BUTTON", "Creates a standard button (no other flags set).");
  add(category_t::toolbar_style, 0x0002, "TBSTYLE_CHECK", "Creates a dual-state push button that toggles between pressed and nonpressed states.");
  add(category_t::toolbar_style, 0x0006, "TBSTYLE_CHECKGROUP", "Creates a check button that stays pressed until another button in the group is pressed.");
  add(category_t::toolbar_style, 0x2000, "TBSTYLE_CUSTOMERASE", "Generates NM_CUSTOMDRAW notification codes when the toolbar processes WM_ERASEBKGND.");
  add(category_t::toolbar_style, 0x0008, "TBSTYLE_DROPDOWN", "Creates a drop-down style button that can display a list when clicked.");
  add(category_t::toolbar_style, 0x0800, "TBSTYLE_FLAT", "Creates a flat toolbar with transparent buttons and hot-tracking enabled.");
  add(category_t::toolbar_style, 0x0004, "TBSTYLE_GROUP", "Creates a button that stays pressed until another button in the group is pressed.");
  add(category_t::toolbar_style, 0x1000, "TBSTYLE_LIST", "Creates a flat toolbar with button text to the right of the bitmap.");
  add(category_t::toolbar_style, 0x0020, "TBSTYLE_NOPREFIX", "Specifies that the button text will not have an accelerator prefix.");
  add(category_t::toolbar_style, 0x4000, "TBSTYLE_REGISTERDROP", "Generates TBN_GETOBJECT notifications to request drop target objects.");
  add(category_t::toolbar_style, 0x0001, "TBSTYLE_SEP", "Creates a separator, providing a small gap between button groups.");
  add(category_t::toolbar_style, 0x0100, "TBSTYLE_TOOLTIPS", "Creates a tooltip control for displaying descriptive text for the toolbar buttons.");
  add(category_t::toolbar_style, 0x8000, "TBSTYLE_TRANSPARENT", "Creates a transparent toolbar where the toolbar is transparent but the buttons are not.");
  add(category_t::toolbar_style, 0x0200, "TBSTYLE_WRAPABLE", "Creates a toolbar that can wrap buttons onto multiple lines.");
  add(category_t::toolbar_style, 0x0000, "BTNS_BUTTON", "Creates a standard button (equivalent to TBSTYLE_BUTTON).");
  add(category_t::toolbar_style, 0x0002, "BTNS_CHECK", "Creates a dual-state push button (equivalent to TBSTYLE_CHECK).");
  add(category_t::toolbar_style, 0x0006, "BTNS_CHECKGROUP", "Creates a check button that stays pressed until another button in the group is pressed.");
  add(category_t::toolbar_style, 0x0008, "BTNS_DROPDOWN", "Creates a drop-down style button (equivalent to TBSTYLE_DROPDOWN).");
  add(category_t::toolbar_style, 0x0004, "BTNS_GROUP", "Creates a button that stays pressed until another button in the group is pressed.");
  add(category_t::toolbar_style, 0x0020, "BTNS_NOPREFIX", "Specifies that the button text will not have an accelerator prefix.");
  add(category_t::toolbar_style, 0x0001, "BTNS_SEP", "Creates a separator (equivalent to TBSTYLE_SEP).");
  add(category_t::toolbar_style, 0x0010, "BTNS_AUTOSIZE", "Calculates the button's width based on text plus image (equivalent to TBSTYLE_AUTOSIZE).");
  add(category_t::toolbar_style, 0x0040, "BTNS_SHOWTEXT", "Specifies that button text should be displayed.");
  add(category_t::toolbar_style, 0x0080, "BTNS_WHOLEDROPDOWN", "Specifies that the button will have a drop-down arrow, but not as a separate section.");

  /* ── ACS_* Animation Control Styles ───────────────────────────── */
  // docs/win32/desktop-src/Controls/animation-control-styles.md
  add(category_t::animate_style, 0x0004, "ACS_AUTOPLAY", "Starts playing the animation as soon as the AVI clip is opened.");
  add(category_t::animate_style, 0x0001, "ACS_CENTER", "Centers the animation in the animation control's window.");
  add(category_t::animate_style, 0x0008, "ACS_TIMER", "Plays the clip using a Win32 timer instead of creating a thread.");
  add(category_t::animate_style, 0x0002, "ACS_TRANSPARENT", "Matches the animation's background color to that of the underlying window.");

  /* ── VK_* Virtual Key Codes ───────────────────────────────────── */
  add(category_t::virtual_key, 0x01, "VK_LBUTTON", "Virtual key code representing the left mouse button.");
  add(category_t::virtual_key, 0x02, "VK_RBUTTON", "Virtual key code representing the right mouse button.");
  add(category_t::virtual_key, 0x03, "VK_CANCEL", "Virtual key code representing Control-break processing.");
  add(category_t::virtual_key, 0x04, "VK_MBUTTON", "Virtual key code representing the middle mouse button.");
  add(category_t::virtual_key, 0x05, "VK_XBUTTON1", "Virtual key code representing the first X mouse button (back button on multi-button mice).");
  add(category_t::virtual_key, 0x06, "VK_XBUTTON2", "Virtual key code representing the second X mouse button (forward button on multi-button mice).");
  add(category_t::virtual_key, 0x08, "VK_BACK", "Virtual key code representing the BACKSPACE key.");
  add(category_t::virtual_key, 0x09, "VK_TAB", "Virtual key code representing the TAB key.");
  add(category_t::virtual_key, 0x0C, "VK_CLEAR", "Virtual key code representing the CLEAR key (typically Numpad 5 with Num Lock off).");
  add(category_t::virtual_key, 0x0D, "VK_RETURN", "Virtual key code representing the ENTER key.");
  add(category_t::virtual_key, 0x10, "VK_SHIFT", "Virtual key code representing the SHIFT key.");
  add(category_t::virtual_key, 0x11, "VK_CONTROL", "Virtual key code representing the CTRL key.");
  add(category_t::virtual_key, 0x12, "VK_MENU", "Virtual key code representing the ALT key.");
  add(category_t::virtual_key, 0x13, "VK_PAUSE", "Virtual key code representing the PAUSE key.");
  add(category_t::virtual_key, 0x14, "VK_CAPITAL", "Virtual key code representing the CAPS LOCK key.");
  add(category_t::virtual_key, 0x1B, "VK_ESCAPE", "Virtual key code representing the ESC key.");
  add(category_t::virtual_key, 0x20, "VK_SPACE", "Virtual key code representing the SPACEBAR key.");
  add(category_t::virtual_key, 0x21, "VK_PRIOR", "Virtual key code representing the PAGE UP key.");
  add(category_t::virtual_key, 0x22, "VK_NEXT", "Virtual key code representing the PAGE DOWN key.");
  add(category_t::virtual_key, 0x23, "VK_END", "Virtual key code representing the END key.");
  add(category_t::virtual_key, 0x24, "VK_HOME", "Virtual key code representing the HOME key.");
  add(category_t::virtual_key, 0x25, "VK_LEFT", "Virtual key code representing the LEFT ARROW key.");
  add(category_t::virtual_key, 0x26, "VK_UP", "Virtual key code representing the UP ARROW key.");
  add(category_t::virtual_key, 0x27, "VK_RIGHT", "Virtual key code representing the RIGHT ARROW key.");
  add(category_t::virtual_key, 0x28, "VK_DOWN", "Virtual key code representing the DOWN ARROW key.");
  add(category_t::virtual_key, 0x29, "VK_SELECT", "Virtual key code representing the SELECT key.");
  add(category_t::virtual_key, 0x2A, "VK_PRINT", "Virtual key code representing the PRINT key.");
  add(category_t::virtual_key, 0x2B, "VK_EXECUTE", "Virtual key code representing the EXECUTE key.");
  add(category_t::virtual_key, 0x2C, "VK_SNAPSHOT", "Virtual key code representing the PRINT SCREEN key.");
  add(category_t::virtual_key, 0x2D, "VK_INSERT", "Virtual key code representing the INS key.");
  add(category_t::virtual_key, 0x2E, "VK_DELETE", "Virtual key code representing the DEL key.");
  add(category_t::virtual_key, 0x2F, "VK_HELP", "Virtual key code representing the HELP key.");
  add(category_t::virtual_key, 0x30, "VK_0", "Virtual key code representing the '0' key.");
  add(category_t::virtual_key, 0x31, "VK_1", "Virtual key code representing the '1' key.");
  add(category_t::virtual_key, 0x32, "VK_2", "Virtual key code representing the '2' key.");
  add(category_t::virtual_key, 0x33, "VK_3", "Virtual key code representing the '3' key.");
  add(category_t::virtual_key, 0x34, "VK_4", "Virtual key code representing the '4' key.");
  add(category_t::virtual_key, 0x35, "VK_5", "Virtual key code representing the '5' key.");
  add(category_t::virtual_key, 0x36, "VK_6", "Virtual key code representing the '6' key.");
  add(category_t::virtual_key, 0x37, "VK_7", "Virtual key code representing the '7' key.");
  add(category_t::virtual_key, 0x38, "VK_8", "Virtual key code representing the '8' key.");
  add(category_t::virtual_key, 0x39, "VK_9", "Virtual key code representing the '9' key.");
  add(category_t::virtual_key, 0x41, "VK_A", "Virtual key code representing the 'A' key.");
  add(category_t::virtual_key, 0x42, "VK_B", "Virtual key code representing the 'B' key.");
  add(category_t::virtual_key, 0x43, "VK_C", "Virtual key code representing the 'C' key.");
  add(category_t::virtual_key, 0x44, "VK_D", "Virtual key code representing the 'D' key.");
  add(category_t::virtual_key, 0x45, "VK_E", "Virtual key code representing the 'E' key.");
  add(category_t::virtual_key, 0x46, "VK_F", "Virtual key code representing the 'F' key.");
  add(category_t::virtual_key, 0x47, "VK_G", "Virtual key code representing the 'G' key.");
  add(category_t::virtual_key, 0x48, "VK_H", "Virtual key code representing the 'H' key.");
  add(category_t::virtual_key, 0x49, "VK_I", "Virtual key code representing the 'I' key.");
  add(category_t::virtual_key, 0x4A, "VK_J", "Virtual key code representing the 'J' key.");
  add(category_t::virtual_key, 0x4B, "VK_K", "Virtual key code representing the 'K' key.");
  add(category_t::virtual_key, 0x4C, "VK_L", "Virtual key code representing the 'L' key.");
  add(category_t::virtual_key, 0x4D, "VK_M", "Virtual key code representing the 'M' key.");
  add(category_t::virtual_key, 0x4E, "VK_N", "Virtual key code representing the 'N' key.");
  add(category_t::virtual_key, 0x4F, "VK_O", "Virtual key code representing the 'O' key.");
  add(category_t::virtual_key, 0x50, "VK_P", "Virtual key code representing the 'P' key.");
  add(category_t::virtual_key, 0x51, "VK_Q", "Virtual key code representing the 'Q' key.");
  add(category_t::virtual_key, 0x52, "VK_R", "Virtual key code representing the 'R' key.");
  add(category_t::virtual_key, 0x53, "VK_S", "Virtual key code representing the 'S' key.");
  add(category_t::virtual_key, 0x54, "VK_T", "Virtual key code representing the 'T' key.");
  add(category_t::virtual_key, 0x55, "VK_U", "Virtual key code representing the 'U' key.");
  add(category_t::virtual_key, 0x56, "VK_V", "Virtual key code representing the 'V' key.");
  add(category_t::virtual_key, 0x57, "VK_W", "Virtual key code representing the 'W' key.");
  add(category_t::virtual_key, 0x58, "VK_X", "Virtual key code representing the 'X' key.");
  add(category_t::virtual_key, 0x59, "VK_Y", "Virtual key code representing the 'Y' key.");
  add(category_t::virtual_key, 0x5A, "VK_Z", "Virtual key code representing the 'Z' key.");
  add(category_t::virtual_key, 0x5B, "VK_LWIN", "Virtual key code representing the Left Windows key.");
  add(category_t::virtual_key, 0x5C, "VK_RWIN", "Virtual key code representing the Right Windows key.");
  add(category_t::virtual_key, 0x5D, "VK_APPS", "Virtual key code representing the Applications key (context menu key).");
  add(category_t::virtual_key, 0x5F, "VK_SLEEP", "Virtual key code representing the Computer Sleep key.");
  add(category_t::virtual_key, 0x60, "VK_NUMPAD0", "Virtual key code representing the '0' key on the numeric keypad.");
  add(category_t::virtual_key, 0x61, "VK_NUMPAD1", "Virtual key code representing the '1' key on the numeric keypad.");
  add(category_t::virtual_key, 0x62, "VK_NUMPAD2", "Virtual key code representing the '2' key on the numeric keypad.");
  add(category_t::virtual_key, 0x63, "VK_NUMPAD3", "Virtual key code representing the '3' key on the numeric keypad.");
  add(category_t::virtual_key, 0x64, "VK_NUMPAD4", "Virtual key code representing the '4' key on the numeric keypad.");
  add(category_t::virtual_key, 0x65, "VK_NUMPAD5", "Virtual key code representing the '5' key on the numeric keypad.");
  add(category_t::virtual_key, 0x66, "VK_NUMPAD6", "Virtual key code representing the '6' key on the numeric keypad.");
  add(category_t::virtual_key, 0x67, "VK_NUMPAD7", "Virtual key code representing the '7' key on the numeric keypad.");
  add(category_t::virtual_key, 0x68, "VK_NUMPAD8", "Virtual key code representing the '8' key on the numeric keypad.");
  add(category_t::virtual_key, 0x69, "VK_NUMPAD9", "Virtual key code representing the '9' key on the numeric keypad.");
  add(category_t::virtual_key, 0x6A, "VK_MULTIPLY", "Virtual key code representing the Multiply key (*) on the numeric keypad.");
  add(category_t::virtual_key, 0x6B, "VK_ADD", "Virtual key code representing the Add key (+) on the numeric keypad.");
  add(category_t::virtual_key, 0x6C, "VK_SEPARATOR", "Virtual key code representing the Separator key.");
  add(category_t::virtual_key, 0x6D, "VK_SUBTRACT", "Virtual key code representing the Subtract key (-) on the numeric keypad.");
  add(category_t::virtual_key, 0x6E, "VK_DECIMAL", "Virtual key code representing the Decimal key (.) on the numeric keypad.");
  add(category_t::virtual_key, 0x6F, "VK_DIVIDE", "Virtual key code representing the Divide key (/) on the numeric keypad.");
  add(category_t::virtual_key, 0x70, "VK_F1", "Virtual key code representing the F1 function key.");
  add(category_t::virtual_key, 0x71, "VK_F2", "Virtual key code representing the F2 function key.");
  add(category_t::virtual_key, 0x72, "VK_F3", "Virtual key code representing the F3 function key.");
  add(category_t::virtual_key, 0x73, "VK_F4", "Virtual key code representing the F4 function key.");
  add(category_t::virtual_key, 0x74, "VK_F5", "Virtual key code representing the F5 function key.");
  add(category_t::virtual_key, 0x75, "VK_F6", "Virtual key code representing the F6 function key.");
  add(category_t::virtual_key, 0x76, "VK_F7", "Virtual key code representing the F7 function key.");
  add(category_t::virtual_key, 0x77, "VK_F8", "Virtual key code representing the F8 function key.");
  add(category_t::virtual_key, 0x78, "VK_F9", "Virtual key code representing the F9 function key.");
  add(category_t::virtual_key, 0x79, "VK_F10", "Virtual key code representing the F10 function key.");
  add(category_t::virtual_key, 0x7A, "VK_F11", "Virtual key code representing the F11 function key.");
  add(category_t::virtual_key, 0x7B, "VK_F12", "Virtual key code representing the F12 function key.");
  add(category_t::virtual_key, 0x90, "VK_NUMLOCK", "Virtual key code representing the NUM LOCK key.");
  add(category_t::virtual_key, 0x91, "VK_SCROLL", "Virtual key code representing the SCROLL LOCK key.");
  add(category_t::virtual_key, 0xA0, "VK_LSHIFT", "Virtual key code representing the Left SHIFT key.");
  add(category_t::virtual_key, 0xA1, "VK_RSHIFT", "Virtual key code representing the Right SHIFT key.");
  add(category_t::virtual_key, 0xA2, "VK_LCONTROL", "Virtual key code representing the Left CTRL key.");
  add(category_t::virtual_key, 0xA3, "VK_RCONTROL", "Virtual key code representing the Right CTRL key.");
  add(category_t::virtual_key, 0xA4, "VK_LMENU", "Virtual key code representing the Left ALT key.");
  add(category_t::virtual_key, 0xA5, "VK_RMENU", "Virtual key code representing the Right ALT key.");

  /* ── WM_* Window Messages ─────────────────────────────────────── */
  add(category_t::window_message, 0x0000, "WM_NULL", "Performs no operation. Used as a placeholder or to test message routing.");
  add(category_t::window_message, 0x0001, "WM_CREATE", "Sent when an application requests that a window be created by calling CreateWindowEx.");
  add(category_t::window_message, 0x0002, "WM_DESTROY", "Sent when a window is being destroyed");
  add(category_t::window_message, 0x0003, "WM_MOVE", "Sent after a window has been moved.");
  add(category_t::window_message, 0x0005, "WM_SIZE", "Sent to a window after its size has changed.");
  add(category_t::window_message, 0x0006, "WM_ACTIVATE", "Sent to both the window being activated and the window being deactivated.");
  add(category_t::window_message, 0x0007, "WM_SETFOCUS", "Sent to a window after it has gained keyboard focus.");
  add(category_t::window_message, 0x0008, "WM_KILLFOCUS", "Sent to a window immediately before it loses keyboard focus.");
  add(category_t::window_message, 0x000A, "WM_ENABLE", "Sent when an application changes the enabled state of a window.");
  add(category_t::window_message, 0x000B, "WM_SETREDRAW", "Sent to a window to allow or prevent changes in that window from being redrawn.");
  add(category_t::window_message, 0x000C, "WM_SETTEXT", "Sets the text of a window (title bar or control text).");
  add(category_t::window_message, 0x000D, "WM_GETTEXT", "Copies the text corresponding to a window into a caller-supplied buffer.");
  add(category_t::window_message, 0x000E, "WM_GETTEXTLENGTH", "Determines the length");
  add(category_t::window_message, 0x000F, "WM_PAINT", "Sent when the system or another application makes a request to paint a portion of an application's window.");
  add(category_t::window_message, 0x0010, "WM_CLOSE", "Sent as a signal that a window or an application should terminate.");
  add(category_t::window_message, 0x0011, "WM_QUERYENDSESSION", "Sent when the user chooses to end the session or when an application calls one of the shutdown functions.");
  add(category_t::window_message, 0x0012, "WM_QUIT", "Indicates a request to terminate an application and is generated when PostQuitMessage is called.");
  add(category_t::window_message, 0x0014, "WM_ERASEBKGND", "Sent when the window background must be erased (for example");
  add(category_t::window_message, 0x0018, "WM_SHOWWINDOW", "Sent to a window when it is about to be hidden or shown.");
  add(category_t::window_message, 0x001C, "WM_ACTIVATEAPP", "Sent when a window belonging to a different application than the active window is about to be activated.");
  add(category_t::window_message, 0x001F, "WM_CANCELMODE", "Sent to cancel certain modes");
  add(category_t::window_message, 0x0020, "WM_SETCURSOR", "Sent to a window if the mouse cursor moves within a window and mouse input is not captured.");
  add(category_t::window_message, 0x0021, "WM_MOUSEACTIVATE", "Sent when the cursor is in an inactive window and the user presses a mouse button.");
  add(category_t::window_message, 0x0022, "WM_CHILDACTIVATE", "Sent to a child window when the user clicks its title bar or when the window is created");
  add(category_t::window_message, 0x0024, "WM_GETMINMAXINFO", "Sent to a window when the size or position is about to change");
  add(category_t::window_message, 0x0030, "WM_SETFONT", "Sets the font that a control is to use when drawing text.");
  add(category_t::window_message, 0x0031, "WM_GETFONT", "Retrieves the font with which a control is currently drawing its text.");
  add(category_t::window_message, 0x0032, "WM_SETHOTKEY", "Associates a hot key with a window.");
  add(category_t::window_message, 0x0033, "WM_GETHOTKEY", "Determines the hot key associated with a window.");
  add(category_t::window_message, 0x0037, "WM_QUERYDRAGICON", "Sent to a minimized window that does not have an icon defined for its class so the system can display a cursor while dragging.");
  add(category_t::window_message, 0x0039, "WM_COMPAREITEM", "Sent to determine the relative position of a new item in a sorted owner-drawn list box or combo box.");
  add(category_t::window_message, 0x0046, "WM_WINDOWPOSCHANGING", "Sent to a window whose size");
  add(category_t::window_message, 0x0047, "WM_WINDOWPOSCHANGED", "Sent to a window whose size");
  add(category_t::window_message, 0x004A, "WM_COPYDATA", "Sends data from one application to another using inter-process communication.");
  add(category_t::window_message, 0x004E, "WM_NOTIFY", "Sent by a common control to its parent window when an event has occurred or the control requires information.");
  add(category_t::window_message, 0x007B, "WM_CONTEXTMENU", "Notifies a window that the user clicked the right mouse button (context menu request) in the window.");
  add(category_t::window_message, 0x007C, "WM_STYLECHANGING", "Sent to a window when the SetWindowLong function is about to change one or more of the window's styles.");
  add(category_t::window_message, 0x007D, "WM_STYLECHANGED", "Sent to a window after SetWindowLong has changed one or more of the window's styles.");
  add(category_t::window_message, 0x007E, "WM_DISPLAYCHANGE", "Sent to all windows when the display resolution has changed.");
  add(category_t::window_message, 0x007F, "WM_GETICON", "Sent to a window to retrieve a handle to the large or small icon associated with the window.");
  add(category_t::window_message, 0x0080, "WM_SETICON", "Associates a new large or small icon with a window.");
  add(category_t::window_message, 0x0081, "WM_NCCREATE", "Sent prior to WM_CREATE when a window is first created to initialize non-client area elements.");
  add(category_t::window_message, 0x0082, "WM_NCDESTROY", "Notifies a window that its non-client area is being destroyed; this is the final message received by a window.");
  add(category_t::window_message, 0x0083, "WM_NCCALCSIZE", "Sent when the size and position of a window's client area must be calculated.");
  add(category_t::window_message, 0x0084, "WM_NCHITTEST", "Sent to a window in order to determine what part of the window corresponds to a particular screen coordinate.");
  add(category_t::window_message, 0x0085, "WM_NCPAINT", "Sent to a window when its frame (non-client area) must be painted.");
  add(category_t::window_message, 0x0086, "WM_NCACTIVATE", "Sent to a window when its non-client area needs to be changed to indicate an active or inactive state.");
  add(category_t::window_message, 0x0087, "WM_GETDLGCODE", "Sent to the window procedure associated with a control to allow custom processing of direction and TAB keys.");
  add(category_t::window_message, 0x0100, "WM_KEYDOWN", "Posted to the window with the keyboard focus when a non-system key is pressed.");
  add(category_t::window_message, 0x0101, "WM_KEYUP", "Posted to the window with the keyboard focus when a non-system key is released.");
  add(category_t::window_message, 0x0102, "WM_CHAR", "Posted to the window with the keyboard focus when a WM_KEYDOWN message is translated by TranslateMessage.");
  add(category_t::window_message, 0x0103, "WM_DEADCHAR", "Posted when a WM_KEYUP message is translated by TranslateMessage for dead keys (such as accent marks).");
  add(category_t::window_message, 0x0104, "WM_SYSKEYDOWN", "Posted when the user presses the ALT key alongside another key or types when no window has focus.");
  add(category_t::window_message, 0x0105, "WM_SYSKEYUP", "Posted when the user releases a key that was pressed while the ALT key was held down.");
  add(category_t::window_message, 0x0106, "WM_SYSCHAR", "Posted when a WM_SYSKEYDOWN message is translated by the TranslateMessage function.");
  add(category_t::window_message, 0x0107, "WM_SYSDEADCHAR", "Sent when a WM_SYSKEYDOWN message is translated for a character that represents a dead key.");
  add(category_t::window_message, 0x0110, "WM_INITDIALOG", "Sent to the dialog box procedure immediately before a dialog box is displayed.");
  add(category_t::window_message, 0x0111, "WM_COMMAND", "Sent when the user selects a command item from a menu");
  add(category_t::window_message, 0x0112, "WM_SYSCOMMAND", "Sent when the user selects a command from the Window menu or clicks the maximize");
  add(category_t::window_message, 0x0113, "WM_TIMER", "Posted to the installing thread's message queue when a timer set by SetTimer expires.");
  add(category_t::window_message, 0x0114, "WM_HSCROLL", "Sent to a window when an event occurs in the window's standard horizontal scroll bar or horizontal scroll control.");
  add(category_t::window_message, 0x0115, "WM_VSCROLL", "Sent to a window when an event occurs in the window's standard vertical scroll bar or vertical scroll control.");
  add(category_t::window_message, 0x0116, "WM_INITMENU", "Sent when a menu is about to become active");
  add(category_t::window_message, 0x0117, "WM_INITMENUPOPUP", "Sent when a pop-up menu or submenu is about to become active");
  add(category_t::window_message, 0x011F, "WM_MENUSELECT", "Sent to a menu's owner window when the user selects a menu item.");
  add(category_t::window_message, 0x0120, "WM_MENUCHAR", "Sent when a menu is active and the user presses a key that does not match any predefined mnemonic or accelerator.");
  add(category_t::window_message, 0x0121, "WM_ENTERIDLE", "Sent to an owner window when a modal dialog box or menu enters an idle state.");
  add(category_t::window_message, 0x0122, "WM_MENURBUTTONUP", "Sent when the user releases the right mouse button while the cursor is on a menu item.");
  add(category_t::window_message, 0x0123, "WM_MENUDRAG", "Sent to the owner of a drag-and-drop menu when the user drags a menu item.");
  add(category_t::window_message, 0x0124, "WM_MENUGETOBJECT", "Sent to the owner of a drag-and-drop menu when the mouse cursor enters a menu item or registers a drop target.");
  add(category_t::window_message, 0x0125, "WM_UNINITMENUPOPUP", "Sent when a drop-down menu or submenu has been destroyed.");
  add(category_t::window_message, 0x0126, "WM_MENUCOMMAND", "Sent when the user makes a selection from a menu configured with the MNS_NOTIFYBYPOS flag.");
  add(category_t::window_message, 0x0132, "WM_CTLCOLORMSGBOX", "Sent to the owner window of a message box before drawing to set color characteristics.");
  add(category_t::window_message, 0x0133, "WM_CTLCOLOREDIT", "Sent to the parent window of an edit control when the control is about to be drawn");
  add(category_t::window_message, 0x0134, "WM_CTLCOLORLISTBOX", "Sent to the parent window of a list box before drawing to customize text and background colors.");
  add(category_t::window_message, 0x0135, "WM_CTLCOLORBTN", "Sent to the parent window of a button control before drawing to customize button colors.");
  add(category_t::window_message, 0x0136, "WM_CTLCOLORDLG", "Sent to a dialog box procedure before drawing the dialog background to customize dialog colors.");
  add(category_t::window_message, 0x0137, "WM_CTLCOLORSCROLLBAR", "Sent to the parent window of a scroll bar control before drawing to customize control colors.");
  add(category_t::window_message, 0x0138, "WM_CTLCOLORSTATIC", "Sent to the parent window of a static control when the control is about to be drawn.");
  add(category_t::window_message, 0x0200, "WM_MOUSEMOVE", "Posted to a window when the cursor moves across its client area.");
  add(category_t::window_message, 0x0201, "WM_LBUTTONDOWN", "Posted when the user presses the left mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0202, "WM_LBUTTONUP", "Posted when the user releases the left mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0203, "WM_LBUTTONDBLCLK", "Posted when the user double-clicks the left mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0204, "WM_RBUTTONDOWN", "Posted when the user presses the right mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0205, "WM_RBUTTONUP", "Posted when the user releases the right mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0206, "WM_RBUTTONDBLCLK", "Posted when the user double-clicks the right mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0207, "WM_MBUTTONDOWN", "Posted when the user presses the middle mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0208, "WM_MBUTTONUP", "Posted when the user releases the middle mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x0209, "WM_MBUTTONDBLCLK", "Posted when the user double-clicks the middle mouse button while the cursor is in the client area of a window.");
  add(category_t::window_message, 0x020A, "WM_MOUSEWHEEL", "Sent to the active focus window when the mouse wheel is rotated.");
  add(category_t::window_message, 0x0210, "WM_PARENTNOTIFY", "Sent to the parent window when a child window is created or destroyed");
  add(category_t::window_message, 0x0211, "WM_ENTERMENULOOP", "Notifies an application's main window procedure that a menu modal loop has been entered.");
  add(category_t::window_message, 0x0212, "WM_EXITMENULOOP", "Notifies an application's main window procedure that a menu modal loop has been exited.");
  add(category_t::window_message, 0x0213, "WM_NEXTMENU", "Sent to an application when the user switches between the menu bar and the system menu using arrow keys.");
  add(category_t::window_message, 0x0214, "WM_SIZING", "Sent to a window that the user is currently resizing");
  add(category_t::window_message, 0x0317, "WM_PRINT", "Sent to a window to request that it draw itself in the specified device context (DC)");
  add(category_t::window_message, 0x0318, "WM_PRINTCLIENT", "Sent to a window to request that it draw its client area in the specified device context.");
  add(category_t::window_message, 0x0400, "WM_USER", "Defines the starting threshold boundary for private message IDs reserved for custom window classes.");
  add(category_t::window_message, 0x8000, "WM_APP", "Defines the starting threshold boundary for message IDs available for application-wide private messaging.");

  /* ── MB_* Message Box Flags ───────────────────────────────────── */
  add(category_t::message_box, 0x00000000, "MB_OK", "Message box style containing one push button: OK.");
  add(category_t::message_box, 0x00000001, "MB_OKCANCEL", "Message box style containing two push buttons: OK and Cancel.");
  add(category_t::message_box, 0x00000002, "MB_ABORTRETRYIGNORE", "Message box style containing three push buttons: Abort");
  add(category_t::message_box, 0x00000003, "MB_YESNOCANCEL", "Message box style containing three push buttons: Yes");
  add(category_t::message_box, 0x00000004, "MB_YESNO", "Message box style containing two push buttons: Yes and No.");
  add(category_t::message_box, 0x00000005, "MB_RETRYCANCEL", "Message box style containing two push buttons: Retry and Cancel.");
  add(category_t::message_box, 0x00000006, "MB_CANCELTRYCONTINUE", "Message box style containing three push buttons: Cancel");
  add(category_t::message_box, 0x00000010, "MB_ICONHAND", "Message box flag displaying a stop-sign icon.");
  add(category_t::message_box, 0x00000010, "MB_ICONERROR", "Message box flag displaying an error icon (same as MB_ICONHAND).");
  add(category_t::message_box, 0x00000020, "MB_ICONQUESTION", "Message box flag displaying a question mark icon.");
  add(category_t::message_box, 0x00000030, "MB_ICONEXCLAMATION", "Message box flag displaying an exclamation point icon.");
  add(category_t::message_box, 0x00000030, "MB_ICONWARNING", "Message box flag displaying a warning icon (same as MB_ICONEXCLAMATION).");
  add(category_t::message_box, 0x00000040, "MB_ICONASTERISK", "Message box flag displaying an information icon consisting of a lowercase 'i' inside a circle.");
  add(category_t::message_box, 0x00000040, "MB_ICONINFORMATION", "Message box flag displaying an information icon (same as MB_ICONASTERISK).");
  add(category_t::message_box, 0x00000000, "MB_DEFBUTTON1", "Sets the first button in the message box as the default focused button.");
  add(category_t::message_box, 0x00000100, "MB_DEFBUTTON2", "Sets the second button in the message box as the default focused button.");
  add(category_t::message_box, 0x00000200, "MB_DEFBUTTON3", "Sets the third button in the message box as the default focused button.");
  add(category_t::message_box, 0x00000300, "MB_DEFBUTTON4", "Sets the fourth button in the message box as the default focused button.");
  add(category_t::message_box, 0x00000000, "MB_APPLMODAL", "Makes the message box application-modal; user must respond before continuing work in the current application window.");
  add(category_t::message_box, 0x00001000, "MB_SYSTEMMODAL", "Makes the message box system-modal; forces the message box to remain top-most on the system screen.");
  add(category_t::message_box, 0x00002000, "MB_TASKMODAL", "Makes the message box task-modal; acts like application modal but targets thread-specific top-level windows.");
  add(category_t::message_box, 0x00004000, "MB_HELP", "Adds a Help button to the message box");
  add(category_t::message_box, 0x00010000, "MB_SETFOREGROUND", "Forces the message box window to become the foreground window upon display.");
  add(category_t::message_box, 0x00020000, "MB_DEFAULT_DESKTOP_ONLY", "Constrains message box creation strictly to the default active desktop layout.");
  add(category_t::message_box, 0x00040000, "MB_TOPMOST", "Specifies that the message box window is created with the WS_EX_TOPMOST extended style.");
  add(category_t::message_box, 0x00080000, "MB_RIGHT", "Right-aligns the message box text.");
  add(category_t::message_box, 0x00100000, "MB_RTLREADING", "Displays message box text in Right-to-Left reading order for RTL language systems.");

  /* ── MF_* Menu Flags ──────────────────────────────────────────── */
  add(category_t::menu_flag, 0x0000, "MF_STRING", "Specifies that a menu item uses a text string as its content.");
  add(category_t::menu_flag, 0x0001, "MF_GRAYED", "Disables and grays out a menu item so it cannot be selected.");
  add(category_t::menu_flag, 0x0002, "MF_DISABLED", "Disables a menu item so that it cannot be selected");
  add(category_t::menu_flag, 0x0008, "MF_CHECKED", "Places a check mark next to a menu item.");
  add(category_t::menu_flag, 0x0010, "MF_POPUP", "Specifies that a menu item opens a drop-down menu or submenu.");
  add(category_t::menu_flag, 0x0020, "MF_MENUBARBREAK", "Places the item on a new line (for menu bars) or in a new column (for pop-up menus) with a vertical dividing line.");
  add(category_t::menu_flag, 0x0040, "MF_MENUBREAK", "Places the item on a new line (for menu bars) or in a new column (for pop-up menus) without a dividing line.");
  add(category_t::menu_flag, 0x0080, "MF_END", "Indicates the final menu item in a menu bar or pop-up menu resource template (historical flag).");
  add(category_t::menu_flag, 0x0100, "MF_OWNERDRAW", "Specifies that the item is an owner-drawn menu item.");
  add(category_t::menu_flag, 0x0800, "MF_SEPARATOR", "Draws a horizontal dividing line in a pop-up menu or submenu.");
  add(category_t::menu_flag, 0x0000, "MF_BYCOMMAND", "Indicates that menu manipulation functions identify menu items by command ID (default behavior).");
  add(category_t::menu_flag, 0x0400, "MF_BYPOSITION", "Indicates that menu manipulation functions identify menu items by zero-based relative position.");
  add(category_t::menu_flag, 0x1000, "MF_DEFAULT", "Highlights the menu item as the default selection (typically displayed in bold text).");
  add(category_t::menu_flag, 0x2000, "MF_SYSMENU", "Indicates that the specified menu is the window system menu.");
  add(category_t::menu_flag, 0x4000, "MF_HELP", "Aligns a menu item to the right side of the menu bar (historically reserved for Help items).");
  add(category_t::menu_flag, 0x4000, "MF_RIGHTJUSTIFY", "Right-justifies a menu item or submenu on the main menu bar.");
  add(category_t::menu_flag, 0x8000, "MF_MOUSESELECT", "Flag indicating that a menu item was selected using mouse interaction.");

  /* ── MFT_* Extended Menu Item Types ───────────────────────────── */
  add(category_t::menu_flag, 0x0000, "MFT_STRING", "Modern menu flag specifying that the menu item displays a text string (replaces MF_STRING).");
  add(category_t::menu_flag, 0x0004, "MFT_BITMAP", "Modern menu flag specifying that the menu item displays a bitmap (replaces MF_BITMAP).");
  add(category_t::menu_flag, 0x0020, "MFT_MENUBARBREAK", "Modern menu flag placing the item in a new column separated by a line (replaces MF_MENUBARBREAK).");
  add(category_t::menu_flag, 0x0040, "MFT_MENUBREAK", "Modern menu flag placing the item in a new column without a line (replaces MF_MENUBREAK).");
  add(category_t::menu_flag, 0x0100, "MFT_OWNERDRAW", "Modern menu flag assigning owner-drawn responsibilities to the item (replaces MF_OWNERDRAW).");
  add(category_t::menu_flag, 0x0200, "MFT_RADIOCHECK", "Displays a radio-button bullet next to the checked menu item instead of a standard check mark.");
  add(category_t::menu_flag, 0x0800, "MFT_SEPARATOR", "Modern menu flag creating a horizontal line separator inside a pop-up menu.");
  add(category_t::menu_flag, 0x2000, "MFT_RIGHTORDER", "Specifies that menu items are rendered in Right-to-Left order for cascading menus.");
  add(category_t::menu_flag, 0x4000, "MFT_RIGHTJUSTIFY", "Right-justifies the menu item and all subsequent items on the menu bar.");

  /* ── MFS_* Extended Menu Item States ──────────────────────────── */
  add(category_t::menu_flag, 0x0000, "MFS_ENABLED", "Menu state flag indicating the menu item is enabled and selectable.");
  add(category_t::menu_flag, 0x0003, "MFS_DISABLED", "Menu state flag indicating the menu item is disabled.");
  add(category_t::menu_flag, 0x0003, "MFS_GRAYED", "Menu state flag indicating the menu item is disabled and visually grayed out.");
  add(category_t::menu_flag, 0x0008, "MFS_CHECKED", "Menu state flag indicating the menu item has a check mark or radio mark.");
  add(category_t::menu_flag, 0x0000, "MFS_UNCHECKED", "Menu state flag indicating the menu item is not checked.");
  add(category_t::menu_flag, 0x0080, "MFS_HILITE", "Menu state flag indicating the menu item is currently highlighted/selected.");
  add(category_t::menu_flag, 0x0000, "MFS_UNHILITE", "Menu state flag indicating the menu item highlight state is removed.");
  add(category_t::menu_flag, 0x1000, "MFS_DEFAULT", "Menu state flag setting the menu item as the default bold action item.");

  /* ── Accelerator Flags ────────────────────────────────────────── */
  add(category_t::accelerator_flag, 0x01, "FVIRTKEY", "Indicates that the accelerator key structure code is a virtual key code rather than an ASCII character.");
  add(category_t::accelerator_flag, 0x02, "FNOINVERT", "Prevents top-level menu item highlighting when an accelerator key combination is triggered.");
  add(category_t::accelerator_flag, 0x04, "FSHIFT", "Specifies that the SHIFT key must be held down to activate the menu accelerator entry.");
  add(category_t::accelerator_flag, 0x08, "FCONTROL", "Specifies that the CTRL key must be held down to activate the menu accelerator entry.");
  add(category_t::accelerator_flag, 0x10, "FALT", "Specifies that the ALT key must be held down to activate the menu accelerator entry.");
  add(category_t::accelerator_flag, 0x02, "NOINVERT", "Flag preventing menu item highlight state flipping during shortcut processing (same as FNOINVERT).");

  /* ── RT_* Predefined Resource Types ───────────────────────────── */
  add(category_t::resource_type, 1, "RT_CURSOR", "Predefined resource type representing a hardware mouse cursor resource.");
  add(category_t::resource_type, 2, "RT_BITMAP", "Predefined resource type representing a bitmap graphics resource.");
  add(category_t::resource_type, 3, "RT_ICON", "Predefined resource type representing an application icon resource.");
  add(category_t::resource_type, 4, "RT_MENU");
  add(category_t::resource_type, 5, "RT_DIALOG");
  add(category_t::resource_type, 6, "RT_STRING");
  add(category_t::resource_type, 7, "RT_FONTDIR");
  add(category_t::resource_type, 8, "RT_FONT");
  add(category_t::resource_type, 9, "RT_ACCELERATOR");
  add(category_t::resource_type, 10, "RT_RCDATA");
  add(category_t::resource_type, 11, "RT_MESSAGETABLE");
  add(category_t::resource_type, 12, "RT_GROUP_CURSOR");
  add(category_t::resource_type, 14, "RT_GROUP_ICON");
  add(category_t::resource_type, 16, "RT_VERSION");
  add(category_t::resource_type, 17, "RT_DLGINCLUDE");
  add(category_t::resource_type, 19, "RT_PLUGPLAY");
  add(category_t::resource_type, 20, "RT_VXD");
  add(category_t::resource_type, 21, "RT_ANICURSOR");
  add(category_t::resource_type, 22, "RT_ANIICON");
  add(category_t::resource_type, 23, "RT_HTML");
  add(category_t::resource_type, 24, "RT_MANIFEST");

  /* ── ID* Dialog Button Identifiers ────────────────────────────── */
  add(category_t::dialog_id, 1, "IDOK");
  add(category_t::dialog_id, 2, "IDCANCEL");
  add(category_t::dialog_id, 3, "IDABORT");
  add(category_t::dialog_id, 4, "IDRETRY");
  add(category_t::dialog_id, 5, "IDIGNORE");
  add(category_t::dialog_id, 6, "IDYES");
  add(category_t::dialog_id, 7, "IDNO");
  add(category_t::dialog_id, 8, "IDCLOSE");
  add(category_t::dialog_id, 9, "IDHELP");
  add(category_t::dialog_id, 10, "IDTRYAGAIN");
  add(category_t::dialog_id, 11, "IDCONTINUE");
  add(category_t::dialog_id, 32000, "IDTIMEOUT");

  /* ── IDC_* System Control Identifiers ─────────────────────────── */

  /* ── AFX_IDC_* MFC Control Identifiers ──────────────────────── */
  add(category_t::control_id, 0x4b3, "AFX_IDC_BROWSE", "Control ID for file browse button");
  add(category_t::control_id, 0x65, "AFX_IDC_CHANGE", "Control ID for Change button");
  add(category_t::control_id, 0x4b4, "AFX_IDC_CLEAR", "Control ID for Clear button");
  add(category_t::control_id, 0x45c, "AFX_IDC_COLORPROP", "Control ID for Color property page control");
  add(category_t::control_id, 0x44c, "AFX_IDC_COLOR_BLACK", "Control ID for Black color selection box");
  add(category_t::control_id, 0x450, "AFX_IDC_COLOR_BLUE", "Control ID for Blue color selection box");
  add(category_t::control_id, 0x453, "AFX_IDC_COLOR_CYAN", "Control ID for Cyan color selection box");
  add(category_t::control_id, 0x458, "AFX_IDC_COLOR_DARKBLUE", "Control ID for Dark Blue color selection box");
  add(category_t::control_id, 0x45b, "AFX_IDC_COLOR_DARKCYAN", "Control ID for Dark Cyan color selection box");
  add(category_t::control_id, 0x457, "AFX_IDC_COLOR_DARKGREEN", "Control ID for Dark Green color selection box");
  add(category_t::control_id, 0x45a, "AFX_IDC_COLOR_DARKMAGENTA", "Control ID for Dark Magenta color selection box");
  add(category_t::control_id, 0x456, "AFX_IDC_COLOR_DARKRED", "Control ID for Dark Red color selection box");
  add(category_t::control_id, 0x454, "AFX_IDC_COLOR_GRAY", "Control ID for Gray color selection box");
  add(category_t::control_id, 0x44f, "AFX_IDC_COLOR_GREEN", "Control ID for Green color selection box");
  add(category_t::control_id, 0x459, "AFX_IDC_COLOR_LIGHTBROWN", "Control ID for Light Brown color selection box");
  add(category_t::control_id, 0x455, "AFX_IDC_COLOR_LIGHTGRAY", "Control ID for Light Gray color selection box");
  add(category_t::control_id, 0x452, "AFX_IDC_COLOR_MAGENTA", "Control ID for Magenta color selection box");
  add(category_t::control_id, 0x44e, "AFX_IDC_COLOR_RED", "Control ID for Red color selection box");
  add(category_t::control_id, 0x44d, "AFX_IDC_COLOR_WHITE", "Control ID for White color selection box");
  add(category_t::control_id, 0x451, "AFX_IDC_COLOR_YELLOW", "Control ID for Yellow color selection box");
  add(category_t::control_id, 0x7901, "AFX_IDC_CONTEXTHELP", "Cursor ID for Context Help cursor");
  add(category_t::control_id, 0x3e9, "AFX_IDC_FONTNAMES", "Control ID for Font Name list");
  add(category_t::control_id, 0x3e8, "AFX_IDC_FONTPROP", "Control ID for Font property page control");
  add(category_t::control_id, 0x3eb, "AFX_IDC_FONTSIZES", "Control ID for Font Size list");
  add(category_t::control_id, 0x3ea, "AFX_IDC_FONTSTYLES", "Control ID for Font Style list");
  add(category_t::control_id, 0x7904, "AFX_IDC_HSPLITBAR", "Cursor ID for Horizontal Splitter bar");
  add(category_t::control_id, 0x64, "AFX_IDC_LISTBOX", "Control ID for standard List Box control");
  add(category_t::control_id, 0x7902, "AFX_IDC_MAGNIFY", "Cursor ID for Magnifier tool cursor");
  add(category_t::control_id, 0x790c, "AFX_IDC_MOVE4WAY", "Cursor ID for 4-way move operation");
  add(category_t::control_id, 0x7906, "AFX_IDC_NODROPCRSR", "Cursor ID for No Drop drop-target indicator");
  add(category_t::control_id, 0x4b2, "AFX_IDC_PICTURE", "Control ID for Picture property control");
  add(category_t::control_id, 0xc9, "AFX_IDC_PRINT_DOCNAME", "Control ID for Document Name static text in Print dialog");
  add(category_t::control_id, 0xcc, "AFX_IDC_PRINT_PAGENUM", "Control ID for Page Number static text in Print dialog");
  add(category_t::control_id, 0xcb, "AFX_IDC_PRINT_PORTNAME", "Control ID for Port Name static text in Print dialog");
  add(category_t::control_id, 0xca, "AFX_IDC_PRINT_PRINTERNAME", "Control ID for Printer Name static text in Print dialog");
  add(category_t::control_id, 0x4b1, "AFX_IDC_PROPNAME", "Control ID for Property Name static text");
  add(category_t::control_id, 0x3ee, "AFX_IDC_SAMPLEBOX", "Control ID for Font Sample display box");
  add(category_t::control_id, 0x7903, "AFX_IDC_SMALLARROWS", "Cursor ID for small directional adjustment arrows");
  add(category_t::control_id, 0x3ec, "AFX_IDC_STRIKEOUT", "Control ID for Strikeout checkbox");
  add(category_t::control_id, 0x45d, "AFX_IDC_SYSTEMCOLORS", "Control ID for System Colors list");
  add(category_t::control_id, 0x3020, "AFX_IDC_TAB_CONTROL", "Control ID for Tab Control in property sheets");
  add(category_t::control_id, 0x790b, "AFX_IDC_TRACK4WAY", "Cursor ID for 4-way object tracking");
  add(category_t::control_id, 0x7908, "AFX_IDC_TRACKNESW", "Cursor ID for Northeast-Southwest tracking");
  add(category_t::control_id, 0x7909, "AFX_IDC_TRACKNS", "Cursor ID for North-South tracking");
  add(category_t::control_id, 0x7907, "AFX_IDC_TRACKNWSE", "Cursor ID for Northwest-Southeast tracking");
  add(category_t::control_id, 0x790a, "AFX_IDC_TRACKWE", "Cursor ID for West-East tracking");
  add(category_t::control_id, 0x3ed, "AFX_IDC_UNDERLINE", "Control ID for Underline checkbox");
  add(category_t::control_id, 0x7905, "AFX_IDC_VSPLITBAR", "Cursor ID for Vertical Splitter bar");

  /* ── AFX_IDD_* MFC Dialog Template IDs ──────────────────────── */
  add(category_t::mfc_dialog_id, 0x780a, "AFX_IDD_BUSY", "Dialog Template ID for OLE Busy dialog");
  add(category_t::mfc_dialog_id, 0x7805, "AFX_IDD_CHANGEICON", "Dialog Template ID for OLE Change Icon dialog");
  add(category_t::mfc_dialog_id, 0x780d, "AFX_IDD_CHANGESOURCE", "Dialog Template ID for OLE Change Source dialog");
  add(category_t::mfc_dialog_id, 0x7806, "AFX_IDD_CONVERT", "Dialog Template ID for OLE Convert Object dialog");
  add(category_t::mfc_dialog_id, 0x7808, "AFX_IDD_EDITLINKS", "Dialog Template ID for OLE Edit Links dialog");
  add(category_t::mfc_dialog_id, 0x7809, "AFX_IDD_FILEBROWSE", "Dialog Template ID for File Browse dialog");
  add(category_t::mfc_dialog_id, 0x7804, "AFX_IDD_INSERTOBJECT", "Dialog Template ID for OLE Insert Object dialog");
  add(category_t::mfc_dialog_id, 0x7801, "AFX_IDD_NEWTYPEDLG", "Dialog Template ID for New File Type Selection dialog");
  add(category_t::mfc_dialog_id, 0x780c, "AFX_IDD_OBJECTPROPERTIES", "Dialog Template ID for OLE Object Properties dialog");
  add(category_t::mfc_dialog_id, 0x7807, "AFX_IDD_PASTESPECIAL", "Dialog Template ID for OLE Paste Special dialog");
  add(category_t::mfc_dialog_id, 0x7803, "AFX_IDD_PREVIEW_TOOLBAR", "Dialog Template ID for Print Preview toolbar");
  add(category_t::mfc_dialog_id, 0x7802, "AFX_IDD_PRINTDLG", "Dialog Template ID for Print Status dialog");

  /* ── AFX_IDB_* MFC Bitmap IDs ───────────────────────────────── */
  add(category_t::mfc_bitmap_id, 0x7801, "AFX_IDB_CHECKBOX", "Standard MFC bitmap strip providing checkbox glyph states");
  add(category_t::mfc_bitmap_id, 0x7914, "AFX_IDB_CHECKLISTBOX_95", "Bitmap ID for CCheckListBox check images (Windows 95)");
  add(category_t::mfc_bitmap_id, 0x7913, "AFX_IDB_CHECKLISTBOX_NT", "Bitmap ID for CCheckListBox check images (Windows NT)");
  add(category_t::mfc_bitmap_id, 0x7807, "AFX_IDB_COLOR_PALETTE", "Stock color palette button icons for MFC color controls");
  add(category_t::mfc_bitmap_id, 0x7812, "AFX_IDB_COLOR_TOOL", "Toolbar bitmap icon displaying color picker/fill tools");
  add(category_t::mfc_bitmap_id, 0x7f02, "AFX_IDB_HMENU_ARROW", "Internal MFC macro for submenu right-pointing expansion arrows");
  add(category_t::mfc_bitmap_id, 0x7f01, "AFX_IDB_HMENU_DOT", "Internal MFC macro for radio item bullet checkmark glyphs in menus");
  add(category_t::mfc_bitmap_id, 0x780f, "AFX_IDB_MENU_IMAGES", "Visual glyph strip containing caption-bar icons");
  add(category_t::mfc_bitmap_id, 0x7912, "AFX_IDB_MINIFRAME_MENU", "Bitmap ID for mini-frame window system menu button");
  add(category_t::mfc_bitmap_id, 0x7802, "AFX_IDB_MINI_BTNS", "MFC resource strip containing small window control glyphs for mini frame windows");
  add(category_t::mfc_bitmap_id, 0x7803, "AFX_IDB_MINI_CALENDAR", "MFC navigation arrow and date glyph strip used in date pickers");
  add(category_t::mfc_bitmap_id, 0x7806, "AFX_IDB_POPUP_SUBMENU", "Submenu expansion arrow glyphs used across MFC popup and context menus");
  add(category_t::mfc_bitmap_id, 0x7804, "AFX_IDB_PROPERTY_GRID_BTNS", "Expand/collapse and drop-down button glyphs for CMFCPropertyGridCtrl");
  add(category_t::mfc_bitmap_id, 0x7808, "AFX_IDB_RIBBON_BAR_BTN_DEFAULT", "Generic placeholder image used by CMFCRibbonBar when item image is missing");
  add(category_t::mfc_bitmap_id, 0x7809, "AFX_IDB_RIBBON_PRINT_LARGE", "Print/print preview icon for Ribbon Application Menu");
  add(category_t::mfc_bitmap_id, 0x780a, "AFX_IDB_RIBBON_PRINT_SMALL", "Print command icon for Ribbon menus and Quick Access Toolbars");
  add(category_t::mfc_bitmap_id, 0x7814, "AFX_IDB_RIBBON_SLIDER_BTN_MINUS", "Zoom Out button graphic used on Ribbon status bar zoom controls");
  add(category_t::mfc_bitmap_id, 0x7815, "AFX_IDB_RIBBON_SLIDER_BTN_PLUS", "Zoom In button graphic used on Ribbon status bar zoom controls");
  add(category_t::mfc_bitmap_id, 0x780e, "AFX_IDB_SCROLL_DOWN", "Downward scroll arrow icon for vertically oriented toolbars");
  add(category_t::mfc_bitmap_id, 0x780b, "AFX_IDB_SCROLL_LEFT", "Left scroll arrow icon for tabbed control bars");
  add(category_t::mfc_bitmap_id, 0x780c, "AFX_IDB_SCROLL_RIGHT", "Right scroll arrow icon for tabbed control bars");
  add(category_t::mfc_bitmap_id, 0x780d, "AFX_IDB_SCROLL_UP", "Upward scroll arrow icon for vertically oriented toolbars");
  add(category_t::mfc_bitmap_id, 0x7810, "AFX_IDB_TASKPANE_CLOSE", "Close button graphic used in task pane headers");
  add(category_t::mfc_bitmap_id, 0x7811, "AFX_IDB_TASKPANE_EXPAND", "Expand/collapse indicator arrows used within CMFCTasksPane");
  add(category_t::mfc_bitmap_id, 0x7805, "AFX_IDB_TASKPANE_SCROLL_BTNS", "Up and down scroll arrow buttons for scrollable task pane containers");
  add(category_t::mfc_bitmap_id, 0x7813, "AFX_IDB_TEAR", "Visual indicator graphic marking detachable tear-off menus and toolbars");

  /* ── IDB_* Standard Common Controls Bitmap IDs ───────────────── */
  add(category_t::mfc_bitmap_id, 0xe, "IDB_HIST_DISABLED", "Explorer travel buttons and favorites bitmaps in disabled state");
  add(category_t::mfc_bitmap_id, 0xd, "IDB_HIST_HOT", "Explorer travel buttons and favorites bitmaps in hot-tracked state");
  add(category_t::mfc_bitmap_id, 0x9, "IDB_HIST_LARGE_COLOR", "Explorer-style navigation bitmap with 24x24 color icons");
  add(category_t::mfc_bitmap_id, 0xc, "IDB_HIST_NORMAL", "Explorer travel buttons and favorites bitmaps in normal state");
  add(category_t::mfc_bitmap_id, 0xf, "IDB_HIST_PRESSED", "Explorer travel buttons and favorites bitmaps in pressed state");
  add(category_t::mfc_bitmap_id, 0x8, "IDB_HIST_SMALL_COLOR", "Explorer-style navigation bitmap with 16x16 color icons");
  add(category_t::mfc_bitmap_id, 0x1, "IDB_STD_LARGE_COLOR", "Standard Common Controls toolbar bitmap with 24x24 color icons");
  add(category_t::mfc_bitmap_id, 0x3, "IDB_STD_LARGE_MONO", "Standard Common Controls bitmap with 24x24 monochrome icons");
  add(category_t::mfc_bitmap_id, 0x0, "IDB_STD_SMALL_COLOR", "Standard Common Controls toolbar bitmap with 16x16 color icons");
  add(category_t::mfc_bitmap_id, 0x2, "IDB_STD_SMALL_MONO", "Standard Common Controls bitmap with 16x16 monochrome icons");
  add(category_t::mfc_bitmap_id, 0x5, "IDB_VIEW_LARGE_COLOR", "Standard toolbar bitmap with 24x24 color icons for list-view styles");
  add(category_t::mfc_bitmap_id, 0x7, "IDB_VIEW_LARGE_MONO", "Standard toolbar bitmap with 24x24 monochrome icons for view styles");
  add(category_t::mfc_bitmap_id, 0x4, "IDB_VIEW_SMALL_COLOR", "Standard toolbar bitmap with 16x16 color icons for list-view styles");
  add(category_t::mfc_bitmap_id, 0x6, "IDB_VIEW_SMALL_MONO", "Standard toolbar bitmap with 16x16 monochrome icons for view styles");

  /* ── AFX_IDR_* MFC Accelerator Table IDs ────────────────────── */
  add(category_t::mfc_accel_id, 0x7915, "AFX_IDR_PREVIEW_ACCEL", "Accelerator Table ID for Print Preview mode");

  /* ── AFX_IDI_* MFC Icon IDs ─────────────────────────────────── */
  add(category_t::mfc_icon_id, 0x7A01, "AFX_IDI_STD_MDIFRAME", "Icon ID for standard MDI application frame window");
  add(category_t::mfc_icon_id, 0x7A02, "AFX_IDI_STD_FRAME", "Icon ID for standard application frame window");

  /* ── IDD_* Standard Dialog Template IDs ──────────────────────── */
  add(category_t::mfc_dialog_id, 0x0064, "IDD_ABOUTBOX", "Default resource ID assigned to an application About dialog template");

  /* ── AFX_IDP_* MFC Prompt IDs ───────────────────────────────── */
  add(category_t::mfc_prompt_id, 0xf103, "AFX_IDP_ASK_TO_SAVE", "Prompt ID for asking user to save modified document");
  add(category_t::mfc_prompt_id, 0xf109, "AFX_IDP_COMMAND_FAILURE", "Prompt ID for Command execution failure message");
  add(category_t::mfc_prompt_id, 0xf10e, "AFX_IDP_DLL_BAD_VERSION", "Prompt ID for Incompatible DLL version error");
  add(category_t::mfc_prompt_id, 0xf10d, "AFX_IDP_DLL_LOAD_FAILED", "Prompt ID for DLL loading failure message");
  add(category_t::mfc_prompt_id, 0xf10a, "AFX_IDP_FAILED_MEMORY_ALLOC", "Prompt ID for Memory allocation failure prompt");
  add(category_t::mfc_prompt_id, 0xf104, "AFX_IDP_FAILED_TO_CREATE_DOC", "Prompt ID for Document creation failure prompt");
  add(category_t::mfc_prompt_id, 0xf107, "AFX_IDP_FAILED_TO_LAUNCH_HELP", "Prompt ID for Windows Help launch failure prompt");
  add(category_t::mfc_prompt_id, 0xf101, "AFX_IDP_FAILED_TO_OPEN_DOC", "Prompt ID for Document open failure prompt");
  add(category_t::mfc_prompt_id, 0xf102, "AFX_IDP_FAILED_TO_SAVE_DOC", "Prompt ID for Document save failure prompt");
  add(category_t::mfc_prompt_id, 0xf106, "AFX_IDP_FAILED_TO_START_PRINT", "Prompt ID for Print job initialization failure prompt");
  add(category_t::mfc_prompt_id, 0xf105, "AFX_IDP_FILE_TOO_LARGE", "Prompt ID for File size exceeds maximum limit");
  add(category_t::mfc_prompt_id, 0xf108, "AFX_IDP_INTERNAL_FAILURE", "Prompt ID for Internal application failure prompt");
  add(category_t::mfc_prompt_id, 0xf100, "AFX_IDP_INVALID_FILENAME", "Prompt ID for Invalid file name error prompt");
  add(category_t::mfc_prompt_id, 0xf020, "AFX_IDP_NO_ERROR_AVAILABLE", "Prompt ID for No error description available message");
  add(category_t::mfc_prompt_id, 0xf116, "AFX_IDP_PARSE_BYTE", "Prompt ID for Data Validation: Byte parse error");
  add(category_t::mfc_prompt_id, 0xf119, "AFX_IDP_PARSE_CURRENCY", "Prompt ID for Data Validation: Currency parse error");
  add(category_t::mfc_prompt_id, 0xf118, "AFX_IDP_PARSE_DATETIME", "Prompt ID for Data Validation: Date/Time parse error");
  add(category_t::mfc_prompt_id, 0xf110, "AFX_IDP_PARSE_INT", "Prompt ID for Data Validation: Integer parse error");
  add(category_t::mfc_prompt_id, 0xf112, "AFX_IDP_PARSE_INT_RANGE", "Prompt ID for Data Validation: Integer out of range error");
  add(category_t::mfc_prompt_id, 0xf115, "AFX_IDP_PARSE_RADIO_BUTTON", "Prompt ID for Data Validation: Radio button selection error");
  add(category_t::mfc_prompt_id, 0xf111, "AFX_IDP_PARSE_REAL", "Prompt ID for Data Validation: Floating point parse error");
  add(category_t::mfc_prompt_id, 0xf113, "AFX_IDP_PARSE_REAL_RANGE", "Prompt ID for Data Validation: Floating point out of range error");
  add(category_t::mfc_prompt_id, 0xf114, "AFX_IDP_PARSE_STRING_SIZE", "Prompt ID for Data Validation: String length out of range error");
  add(category_t::mfc_prompt_id, 0xf117, "AFX_IDP_PARSE_UINT", "Prompt ID for Data Validation: Unsigned integer parse error");
  add(category_t::mfc_prompt_id, 0xf10b, "AFX_IDP_UNREG_DONE", "Prompt ID for Unregistration completed message");
  add(category_t::mfc_prompt_id, 0xf10c, "AFX_IDP_UNREG_FAILURE", "Prompt ID for Unregistration failed message");
  add(category_t::mfc_prompt_id, 0xf022, "AFX_IDS_RESOURCE_EXCEPTION", "Error message string displayed when a general resource exception occurs");
  add(category_t::mfc_prompt_id, 0xf024, "AFX_IDS_USER_EXCEPTION", "Standard string prompt for generic user-initiated exceptions");

  /* ── AFX_IDP_FILE_* / AFX_IDP_ARCH_* File/Archive Exception Prompts */
  add(category_t::mfc_prompt_id, 0xf1b6, "AFX_IDP_ARCH_BADCLASS", "Archive Error: Class bad or unexpected");
  add(category_t::mfc_prompt_id, 0xf1b5, "AFX_IDP_ARCH_BADINDEX", "Archive Error: Bad file index");
  add(category_t::mfc_prompt_id, 0xf1b7, "AFX_IDP_ARCH_BADSCHEMA", "Archive Error: Bad schema version");
  add(category_t::mfc_prompt_id, 0xf1b3, "AFX_IDP_ARCH_ENDOFFILE", "Archive Error: Unexpected end of file");
  add(category_t::mfc_prompt_id, 0xf1b1, "AFX_IDP_ARCH_GENERIC", "Generic archive operation error");
  add(category_t::mfc_prompt_id, 0xf1b0, "AFX_IDP_ARCH_NONE", "Archive Error: No error");
  add(category_t::mfc_prompt_id, 0xf1b2, "AFX_IDP_ARCH_READONLY", "Archive Error: Attempted write to read-only archive");
  add(category_t::mfc_prompt_id, 0xf1b4, "AFX_IDP_ARCH_WRITEONLY", "Archive Error: Attempted read from write-only archive");
  add(category_t::mfc_prompt_id, 0xf1a5, "AFX_IDP_FILE_ACCESS_DENIED", "CFileException: Access denied");
  add(category_t::mfc_prompt_id, 0xf1a3, "AFX_IDP_FILE_BAD_PATH", "CFileException: Bad path");
  add(category_t::mfc_prompt_id, 0xf1a9, "AFX_IDP_FILE_BAD_SEEK", "CFileException: Bad seek operation");
  add(category_t::mfc_prompt_id, 0xf1a8, "AFX_IDP_FILE_DIR_FULL", "CFileException: Directory full");
  add(category_t::mfc_prompt_id, 0xf1ad, "AFX_IDP_FILE_DISKFULL", "CFileException: Disk full");
  add(category_t::mfc_prompt_id, 0xf1ae, "AFX_IDP_FILE_EOF", "CFileException: Unexpected end of file");
  add(category_t::mfc_prompt_id, 0xf1a1, "AFX_IDP_FILE_GENERIC", "CFileException: Generic file error");
  add(category_t::mfc_prompt_id, 0xf1aa, "AFX_IDP_FILE_HARD_IO", "CFileException: Hardware I/O error");
  add(category_t::mfc_prompt_id, 0xf1a6, "AFX_IDP_FILE_INVALID_FILE", "CFileException: Invalid file handle");
  add(category_t::mfc_prompt_id, 0xf1ac, "AFX_IDP_FILE_LOCKING", "CFileException: File locking violation");
  add(category_t::mfc_prompt_id, 0xf1a0, "AFX_IDP_FILE_NONE", "CFileException: No error");
  add(category_t::mfc_prompt_id, 0xf1a2, "AFX_IDP_FILE_NOT_FOUND", "CFileException: File not found");
  add(category_t::mfc_prompt_id, 0xf1a7, "AFX_IDP_FILE_REMOVE_CURRENT", "CFileException: Cannot remove current directory");
  add(category_t::mfc_prompt_id, 0xf1ab, "AFX_IDP_FILE_SHARING", "CFileException: Sharing violation");
  add(category_t::mfc_prompt_id, 0xf1a4, "AFX_IDP_FILE_TOO_MANY_OPEN", "CFileException: Too many open files");

  /* ── AFX_IDP_SQL_* MFC SQL Prompt IDs ───────────────────────── */
  add(category_t::mfc_prompt_id, 0xf28e, "AFX_IDP_SQL_API_CONFORMANCE", "ODBC driver is incompatible with MFC database classes");
  add(category_t::mfc_prompt_id, 0xf29c, "AFX_IDP_SQL_BOOKMARKS_NOT_ENABLED", "Bookmarks are not enabled on the recordset");
  add(category_t::mfc_prompt_id, 0xf29b, "AFX_IDP_SQL_BOOKMARKS_NOT_SUPPORTED", "Bookmarks are not supported by the underlying ODBC driver");
  add(category_t::mfc_prompt_id, 0xf281, "AFX_IDP_SQL_CONNECT_FAIL", "Attempt to connect to the SQL data source failed");
  add(category_t::mfc_prompt_id, 0xf295, "AFX_IDP_SQL_DATA_TRUNCATED", "Data returned from the SQL operation was truncated");
  add(category_t::mfc_prompt_id, 0xf299, "AFX_IDP_SQL_DYNAMIC_CURSOR_NOT_SUPPORTED", "Dynamic cursors are not supported by the ODBC driver");
  add(category_t::mfc_prompt_id, 0xf28c, "AFX_IDP_SQL_DYNASET_NOT_SUPPORTED", "Dynasets are not supported by the ODBC driver");
  add(category_t::mfc_prompt_id, 0xf283, "AFX_IDP_SQL_EMPTY_COLUMN_LIST", "Attempted to open a table but no columns were specified");
  add(category_t::mfc_prompt_id, 0xf29a, "AFX_IDP_SQL_FIELD_NOT_FOUND", "The specified field or column name was not found");
  add(category_t::mfc_prompt_id, 0xf284, "AFX_IDP_SQL_FIELD_SCHEMA_MISMATCH", "Query returned an unexpected column data type");
  add(category_t::mfc_prompt_id, 0xf285, "AFX_IDP_SQL_ILLEGAL_MODE", "Attempted update or delete in an illegal recordset mode");
  add(category_t::mfc_prompt_id, 0xf297, "AFX_IDP_SQL_INCORRECT_ODBC", "Incompatible or incorrect ODBC driver installed");
  add(category_t::mfc_prompt_id, 0xf294, "AFX_IDP_SQL_LOCK_MODE_NOT_SUPPORTED", "The requested locking mode is not supported");
  add(category_t::mfc_prompt_id, 0xf286, "AFX_IDP_SQL_MULTIPLE_ROWS_AFFECTED", "Multiple rows were unexpectedly updated or deleted");
  add(category_t::mfc_prompt_id, 0xf287, "AFX_IDP_SQL_NO_CURRENT_RECORD", "Operation failed because there is no current record");
  add(category_t::mfc_prompt_id, 0xf290, "AFX_IDP_SQL_NO_DATA_FOUND", "No data was returned or found for the operation");
  add(category_t::mfc_prompt_id, 0xf293, "AFX_IDP_SQL_NO_POSITIONED_UPDATES", "Positioned updates or deletes are not supported");
  add(category_t::mfc_prompt_id, 0xf288, "AFX_IDP_SQL_NO_ROWS_AFFECTED", "No rows were affected by the update or delete operation");
  add(category_t::mfc_prompt_id, 0xf28b, "AFX_IDP_SQL_ODBC_LOAD_FAILED", "Failed to load the required ODBC driver library");
  add(category_t::mfc_prompt_id, 0xf292, "AFX_IDP_SQL_ODBC_V2_REQUIRED", "An ODBC 2.0 or higher compliant driver is required");
  add(category_t::mfc_prompt_id, 0xf282, "AFX_IDP_SQL_RECORDSET_FORWARD_ONLY", "The recordset only supports forward scrolling");
  add(category_t::mfc_prompt_id, 0xf289, "AFX_IDP_SQL_RECORDSET_READONLY", "The recordset is read-only and cannot be modified");
  add(category_t::mfc_prompt_id, 0xf296, "AFX_IDP_SQL_ROW_FETCH", "Error encountered while fetching rows from the database");
  add(category_t::mfc_prompt_id, 0xf291, "AFX_IDP_SQL_ROW_UPDATE_NOT_SUPPORTED", "Row update operations are not supported by the driver");
  add(category_t::mfc_prompt_id, 0xf28d, "AFX_IDP_SQL_SNAPSHOT_NOT_SUPPORTED", "Snapshots require static cursor support from the ODBC driver");
  add(category_t::mfc_prompt_id, 0xf28f, "AFX_IDP_SQL_SQL_CONFORMANCE", "ODBC driver does not meet required SQL conformance levels");
  add(category_t::mfc_prompt_id, 0xf28a, "AFX_IDP_SQL_SQL_NO_TOTAL", "ODBC driver does not support the MFC CLongBinary data model");
  add(category_t::mfc_prompt_id, 0xf298, "AFX_IDP_SQL_UPDATE_DELETE_FAILED", "The update or delete operation failed");

  /* ── AFX_IDS_* MFC String IDs ───────────────────────────────── */
  add(category_t::mfc_string_id, 0xf082, "AFX_IDS_ACTIVATE_VERB", "String ID for OLE Activate verb menu label");
  add(category_t::mfc_string_id, 0xf002, "AFX_IDS_ALLFILTER", "String ID for generic wildcard filter string");
  add(category_t::mfc_string_id, 0xf087, "AFX_IDS_ALL_FILES", "String ID for All Files filter string");
  add(category_t::mfc_string_id, 0xf084, "AFX_IDS_AUTO", "String ID for Auto label");
  add(category_t::mfc_string_id, 0xf090, "AFX_IDS_BITMAP_FORMAT", "String ID for Bitmap Clipboard format name");
  add(category_t::mfc_string_id, 0xf083, "AFX_IDS_CHANGE_LINK", "String ID for Change Link action label");
  add(category_t::mfc_string_id, 0xf081, "AFX_IDS_EDIT_VERB", "String ID for OLE Edit verb label");
  add(category_t::mfc_string_id, 0xf092, "AFX_IDS_EMBED_FORMAT", "String ID for Embedded Object format display");
  add(category_t::mfc_string_id, 0xf08c, "AFX_IDS_EXIT_MENU", "String ID for Exit command menu label");
  add(category_t::mfc_string_id, 0xf086, "AFX_IDS_FROZEN", "String ID for Frozen state indicator label");
  add(category_t::mfc_string_id, 0xf011, "AFX_IDS_HIDE", "String ID for Hide command label");
  add(category_t::mfc_string_id, 0xf091, "AFX_IDS_LINKSOURCE_FORMAT", "String ID for Link Source clipboard format display");
  add(category_t::mfc_string_id, 0xf085, "AFX_IDS_MANUAL", "String ID for Manual label");
  add(category_t::mfc_string_id, 0xf08e, "AFX_IDS_METAFILE_FORMAT", "String ID for Metafile Clipboard format display");
  add(category_t::mfc_string_id, 0xf080, "AFX_IDS_OBJECT_MENUITEM", "String ID for Dynamic OLE Object menu item label");
  add(category_t::mfc_string_id, 0xf041, "AFX_IDS_ONEPAGE", "String ID for One Page status text in Print Preview");
  add(category_t::mfc_string_id, 0xf000, "AFX_IDS_OPENFILE", "String ID for Open File dialog title");
  add(category_t::mfc_string_id, 0xf094, "AFX_IDS_PASTELINKEDTYPE", "String ID for Paste Linked Object type description");
  add(category_t::mfc_string_id, 0xf005, "AFX_IDS_PREVIEW_CLOSE", "String ID for Close Print Preview tooltip label");
  add(category_t::mfc_string_id, 0xf048, "AFX_IDS_PRINTCAPTION", "String ID for Print job progress window caption");
  add(category_t::mfc_string_id, 0xf046, "AFX_IDS_PRINTDEFAULT", "String ID for Default Printer description string");
  add(category_t::mfc_string_id, 0xf045, "AFX_IDS_PRINTDEFAULTEXT", "String ID for Default Print File extension");
  add(category_t::mfc_string_id, 0xf047, "AFX_IDS_PRINTFILTER", "String ID for Print File filter string");
  add(category_t::mfc_string_id, 0xf096, "AFX_IDS_RTF_FORMAT", "Display string describing Rich Text Format (RTF) data");
  add(category_t::mfc_string_id, 0xf001, "AFX_IDS_SAVEFILE", "Prompt or title displayed in dialogs when saving an existing file");
  add(category_t::mfc_string_id, 0xf004, "AFX_IDS_SAVEFILECOPY", "Prompt or title displayed in dialogs when saving a copy of a file");
  add(category_t::mfc_string_id, 0xf08a, "AFX_IDS_SAVE_AS_MENU", "Dynamic menu text used for the Save As command");
  add(category_t::mfc_string_id, 0xf08b, "AFX_IDS_SAVE_COPY_AS_MENU", "Dynamic menu text used for the Save Copy As command");
  add(category_t::mfc_string_id, 0xf088, "AFX_IDS_SAVE_MENU", "Dynamic menu text used for saving the active document");
  add(category_t::mfc_string_id, 0xf097, "AFX_IDS_TEXT_FORMAT", "Display string describing plain unformatted text clipboard data");
  add(category_t::mfc_string_id, 0xf042, "AFX_IDS_TWOPAGE", "Button or UI string for toggling two-page view in Print Preview");
  add(category_t::mfc_string_id, 0xf095, "AFX_IDS_UNKNOWNTYPE", "Display string indicating an unknown or unrecognized data object");
  add(category_t::mfc_string_id, 0xf003, "AFX_IDS_UNTITLED", "Default Untitled string prefix for new unsaved documents");
  add(category_t::mfc_string_id, 0xf089, "AFX_IDS_UPDATE_MENU", "Dynamic menu text for updating an embedded OLE object");
  add(category_t::mfc_string_id, 0xf08d, "AFX_IDS_UPDATING_ITEMS", "Status message displayed while links/embedded objects update");

  /* ── AFX_ID_PREVIEW_* Print Preview Command IDs ─────────────── */
  add(category_t::system_resource_id, 0xe300, "AFX_ID_PREVIEW_CLOSE", "Command ID for the Print Preview Close button");
  add(category_t::system_resource_id, 0xe302, "AFX_ID_PREVIEW_NEXT", "Command ID for the Print Preview Next Page button");
  add(category_t::system_resource_id, 0xe301, "AFX_ID_PREVIEW_NUMPAGE", "Command ID for toggling one/two page view in Print Preview");
  add(category_t::system_resource_id, 0xe303, "AFX_ID_PREVIEW_PREV", "Command ID for the Print Preview Previous Page button");
  add(category_t::system_resource_id, 0xe304, "AFX_ID_PREVIEW_PRINT", "Command ID for the Print Preview Print button");
  add(category_t::system_resource_id, 0xe305, "AFX_ID_PREVIEW_ZOOMIN", "Command ID for zooming in during Print Preview");
  add(category_t::system_resource_id, 0xe306, "AFX_ID_PREVIEW_ZOOMOUT", "Command ID for zooming out during Print Preview");

  /* ── ID_* System Resource Identifiers ────────────────────────── */
  add(category_t::system_resource_id, 0xe000, "IDP_OLE_INIT_FAILED", "String resource ID for OLE initialization failure prompt message");
  add(category_t::system_resource_id, 0x3021, "ID_APPLY_NOW", "Applies changes in a property sheet without closing it");
  add(category_t::system_resource_id, 0xe140, "ID_APP_ABOUT", "Displays the About dialog box for the application");
  add(category_t::system_resource_id, 0xe141, "ID_APP_EXIT", "Prompts to save documents and exits the application");
  add(category_t::system_resource_id, 0xe144, "ID_CONTEXT_HELP", "Enters context-sensitive Help mode (Shift+F1)");
  add(category_t::system_resource_id, 0xe147, "ID_DEFAULT_HELP", "Runs default help topic for the active window or control");
  add(category_t::system_resource_id, 0xe120, "ID_EDIT_CLEAR", "Erases the selected content");
  add(category_t::system_resource_id, 0xe121, "ID_EDIT_CLEAR_ALL", "Erases all content in the document or window");
  add(category_t::system_resource_id, 0xe122, "ID_EDIT_COPY", "Copies the selection and places it on the Clipboard");
  add(category_t::system_resource_id, 0xe123, "ID_EDIT_CUT", "Cuts the selection and places it on the Clipboard");
  add(category_t::system_resource_id, 0xe124, "ID_EDIT_FIND", "Opens the Find dialog to search for specified text");
  add(category_t::system_resource_id, 0xe125, "ID_EDIT_PASTE", "Inserts Clipboard contents at the insertion point");
  add(category_t::system_resource_id, 0xe126, "ID_EDIT_PASTE_LINK", "Inserts Clipboard contents and creates a link to the source");
  add(category_t::system_resource_id, 0xe127, "ID_EDIT_PASTE_SPECIAL", "Inserts Clipboard contents with formatting options");
  add(category_t::system_resource_id, 0xe12c, "ID_EDIT_REDO", "Redoes the previously undone action");
  add(category_t::system_resource_id, 0xe128, "ID_EDIT_REPEAT", "Repeats the last action performed");
  add(category_t::system_resource_id, 0xe129, "ID_EDIT_REPLACE", "Replaces specific text with different text");
  add(category_t::system_resource_id, 0xe12a, "ID_EDIT_SELECT_ALL", "Selects the entire document or contents");
  add(category_t::system_resource_id, 0xe12b, "ID_EDIT_UNDO", "Undoes the last action performed");
  add(category_t::system_resource_id, 0xe102, "ID_FILE_CLOSE", "Closes the active document");
  add(category_t::system_resource_id, 0xe100, "ID_FILE_NEW", "Creates a new document");
  add(category_t::system_resource_id, 0xe10d, "ID_FILE_NEW_FRAME", "Opens a new frame window for an existing document");
  add(category_t::system_resource_id, 0xe101, "ID_FILE_OPEN", "Opens an existing document");
  add(category_t::system_resource_id, 0xe105, "ID_FILE_PAGE_SETUP", "Opens the Page Setup dialog box");
  add(category_t::system_resource_id, 0xe107, "ID_FILE_PRINT", "Prints the active document");
  add(category_t::system_resource_id, 0xe108, "ID_FILE_PRINT_DIRECT", "Prints the document directly using current printer settings");
  add(category_t::system_resource_id, 0xe109, "ID_FILE_PRINT_PREVIEW", "Displays full pages on-screen to preview output");
  add(category_t::system_resource_id, 0xe106, "ID_FILE_PRINT_SETUP", "Opens the Print Setup dialog to choose a printer");
  add(category_t::system_resource_id, 0xe103, "ID_FILE_SAVE", "Saves the active document under its current name");
  add(category_t::system_resource_id, 0xe104, "ID_FILE_SAVE_AS", "Saves the active document under a new name");
  add(category_t::system_resource_id, 0xe10b, "ID_FILE_SAVE_COPY_AS", "Saves a copy of the active document under a new name");
  add(category_t::system_resource_id, 0xe10c, "ID_FILE_SEND_MAIL", "Sends the active document via electronic mail");
  add(category_t::system_resource_id, 0xe10a, "ID_FILE_UPDATE", "Updates an embedded container document with object changes");
  add(category_t::system_resource_id, 0xe160, "ID_FORMAT_FONT", "Selects fonts for current selection or document");
  add(category_t::system_resource_id, 0xe146, "ID_HELP", "Displays help for the current task or command (F1)");
  add(category_t::system_resource_id, 0xe143, "ID_HELP_FINDER", "Displays Help contents/index or search prompt");
  add(category_t::system_resource_id, 0xe142, "ID_HELP_INDEX", "Displays the main index for Help topics");
  add(category_t::system_resource_id, 0xe144, "ID_HELP_USING", "Displays instructions on how to use Help");
  add(category_t::system_resource_id, 0xe701, "ID_INDICATOR_CAPS", "Status bar indicator for Caps Lock state");
  add(category_t::system_resource_id, 0xe700, "ID_INDICATOR_EXT", "Status bar indicator for extended selection mode");
  add(category_t::system_resource_id, 0xe706, "ID_INDICATOR_KANA", "Status bar indicator for Kana input mode");
  add(category_t::system_resource_id, 0xe702, "ID_INDICATOR_NUM", "Status bar indicator for Num Lock state");
  add(category_t::system_resource_id, 0xe704, "ID_INDICATOR_OVR", "Status bar indicator for Overtype mode");
  add(category_t::system_resource_id, 0xe705, "ID_INDICATOR_REC", "Status bar indicator for macro recording mode");
  add(category_t::system_resource_id, 0xe703, "ID_INDICATOR_SCRL", "Status bar indicator for Scroll Lock state");
  add(category_t::system_resource_id, 0xe150, "ID_NEXT_PANE", "Switches focus to the next window pane");
  add(category_t::system_resource_id, 0xe203, "ID_OLE_EDIT_CHANGE_ICON", "Changes the icon displaying an OLE embedded item");
  add(category_t::system_resource_id, 0xe202, "ID_OLE_EDIT_CONVERT", "Converts an embedded OLE object to a different type");
  add(category_t::system_resource_id, 0xe201, "ID_OLE_EDIT_LINKS", "Displays options to edit or update linked OLE objects");
  add(category_t::system_resource_id, 0xe204, "ID_OLE_EDIT_PROPERTIES", "Displays OLE item properties dialog");
  add(category_t::system_resource_id, 0xe200, "ID_OLE_INSERT_NEW", "Inserts a new OLE embedded object into the document");
  add(category_t::system_resource_id, 0xe210, "ID_OLE_VERB_FIRST", "First ID for dynamically populated OLE object verbs");
  add(category_t::system_resource_id, 0xe151, "ID_PREV_PANE", "Switches focus back to the previous window pane");
  add(category_t::system_resource_id, 0xe243, "ID_RECORD_LAST", "Moves focus/selection to the last record");
  add(category_t::system_resource_id, 0xe242, "ID_RECORD_NEXT", "Moves focus/selection to the next record");
  add(category_t::system_resource_id, 0xe241, "ID_RECORD_PREV", "Moves focus/selection to the previous record");
  add(category_t::system_resource_id, 0xe240, "ID_REC_FIRST", "Alias for ID_RECORD_FIRST; moves to the first record");
  add(category_t::system_resource_id, 0x0, "ID_SEPARATOR", "Used as a visual separator line in menus and toolbars");
  add(category_t::system_resource_id, 0xe816, "ID_VIEW_AUTOARRANGE", "Automatically arranges icons in a list/grid view");
  add(category_t::system_resource_id, 0xe815, "ID_VIEW_BYNAME", "Sorts list view items alphabetically by name");
  add(category_t::system_resource_id, 0xe813, "ID_VIEW_DETAILS", "Displays detailed information about each item");
  add(category_t::system_resource_id, 0xe810, "ID_VIEW_LARGEICON", "Displays items using large icons");
  add(category_t::system_resource_id, 0xe814, "ID_VIEW_LINEUP", "Aligns icons to an invisible grid");
  add(category_t::system_resource_id, 0xe812, "ID_VIEW_LIST", "Displays items as a simple list");
  add(category_t::system_resource_id, 0xe802, "ID_VIEW_REBAR", "Toggles display of the rebar control bar");
  add(category_t::system_resource_id, 0xe811, "ID_VIEW_SMALLICON", "Displays items using small icons");
  add(category_t::system_resource_id, 0xe801, "ID_VIEW_STATUS_BAR", "Toggles display of the application status bar");
  add(category_t::system_resource_id, 0xe800, "ID_VIEW_TOOLBAR", "Toggles display of the application main toolbar");
  add(category_t::system_resource_id, 0xe131, "ID_WINDOW_ARRANGE", "Arranges icons at the bottom of an MDI workspace");
  add(category_t::system_resource_id, 0xe132, "ID_WINDOW_CASCADE", "Arranges open MDI windows in an overlapping cascade");
  add(category_t::system_resource_id, 0xe130, "ID_WINDOW_NEW", "Opens another window for the active document");
  add(category_t::system_resource_id, 0xe135, "ID_WINDOW_SPLIT", "Splits the active window into multiple panes");
  add(category_t::system_resource_id, 0xe133, "ID_WINDOW_TILE_HORIZ", "Tiles open MDI windows horizontally");
  add(category_t::system_resource_id, 0xe134, "ID_WINDOW_TILE_VERT", "Tiles open MDI windows vertically");
  add(category_t::system_resource_id, 0x3023, "ID_WIZBACK", "Wizard control button ID for navigating to the previous page");
  add(category_t::system_resource_id, 0x3025, "ID_WIZFINISH", "Wizard control button ID for completing and closing the wizard");
  add(category_t::system_resource_id, 0x3024, "ID_WIZNEXT", "Wizard control button ID for navigating to the next page");

  /* ── ID_FILE_MRU_* Most Recently Used File List IDs ──────────── */
  add(category_t::system_resource_id, 0xe110, "ID_FILE_MRU_FILE1", "Opens most recently used file #1");
  add(category_t::system_resource_id, 0xe111, "ID_FILE_MRU_FILE2", "Opens most recently used file #2");
  add(category_t::system_resource_id, 0xe112, "ID_FILE_MRU_FILE3", "Opens most recently used file #3");
  add(category_t::system_resource_id, 0xe113, "ID_FILE_MRU_FILE4", "Opens most recently used file #4");
  add(category_t::system_resource_id, 0xe114, "ID_FILE_MRU_FILE5", "Opens most recently used file #5");
  add(category_t::system_resource_id, 0xe115, "ID_FILE_MRU_FILE6", "Opens most recently used file #6");
  add(category_t::system_resource_id, 0xe116, "ID_FILE_MRU_FILE7", "Opens most recently used file #7");
  add(category_t::system_resource_id, 0xe117, "ID_FILE_MRU_FILE8", "Opens most recently used file #8");
  add(category_t::system_resource_id, 0xe118, "ID_FILE_MRU_FILE9", "Opens most recently used file #9");
  add(category_t::system_resource_id, 0xe119, "ID_FILE_MRU_FILE10", "Opens most recently used file #10");
  add(category_t::system_resource_id, 0xe11a, "ID_FILE_MRU_FILE11", "Opens most recently used file #11");
  add(category_t::system_resource_id, 0xe11b, "ID_FILE_MRU_FILE12", "Opens most recently used file #12");
  add(category_t::system_resource_id, 0xe11c, "ID_FILE_MRU_FILE13", "Opens most recently used file #13");
  add(category_t::system_resource_id, 0xe11d, "ID_FILE_MRU_FILE14", "Opens most recently used file #14");
  add(category_t::system_resource_id, 0xe11e, "ID_FILE_MRU_FILE15", "Opens most recently used file #15");
  add(category_t::system_resource_id, 0xe11f, "ID_FILE_MRU_FILE16", "Opens most recently used file #16");
  add(category_t::system_resource_id, 0xE110, "ID_FILE_MRU_FIRST", "First ID reserved for the Most Recently Used file list");
  add(category_t::system_resource_id, 0xE11F, "ID_FILE_MRU_LAST", "Last ID reserved for the Most Recently Used file list");

  /* ── OBM_* OEM Bitmap IDs ───────────────────────────────────── */
  add(category_t::oem_bitmap, 0x7f1f, "OBM_BTNCORNERS", "Legacy OEM bitmap for rounded push-button corners");
  add(category_t::oem_bitmap, 0x7f1d, "OBM_BTSIZE", "OEM window resizing grip bitmap");
  add(category_t::oem_bitmap, 0x7f20, "OBM_CHECK", "OEM checkmark bitmap drawn next to selected menu items");
  add(category_t::oem_bitmap, 0x7f1e, "OBM_CHECKBOXES", "OEM system bitmap strip containing default checkbox state graphics");
  add(category_t::oem_bitmap, 0x7f2a, "OBM_CLOSE", "OEM title bar close button bitmap");
  add(category_t::oem_bitmap, 0x7fe2, "OBM_COMBO", "OEM bitmap providing drop-down arrow graphic for combo box controls");
  add(category_t::oem_bitmap, 0x7f28, "OBM_DNARROW", "OEM scrollbar down-arrow button bitmap");
  add(category_t::oem_bitmap, 0x7fe1, "OBM_DNARROWD", "OEM bitmap providing pressed state for down-arrows");
  add(category_t::oem_bitmap, 0x7fe8, "OBM_DNARROWI", "Inactive/disabled state down scrollbar arrow glyph");
  add(category_t::oem_bitmap, 0x7f26, "OBM_LFARROW", "OEM scrollbar left-arrow button bitmap");
  add(category_t::oem_bitmap, 0x7fec, "OBM_LFARROWD", "Pressed state left scrollbar arrow button");
  add(category_t::oem_bitmap, 0x7fe6, "OBM_LFARROWI", "Inactive/disabled state left scrollbar arrow glyph");
  add(category_t::oem_bitmap, 0x7fe3, "OBM_MNARROW", "OEM bitmap providing right-pointing submenu indicator arrow");
  add(category_t::oem_bitmap, 0x7f18, "OBM_OLD_CLOSE", "Classic OEM close button bitmap for legacy title bars");
  add(category_t::oem_bitmap, 0x7f17, "OBM_OLD_DNARROW", "Classic OEM down-arrow bitmap used in legacy scrollbars");
  add(category_t::oem_bitmap, 0x7f14, "OBM_OLD_LFARROW", "Classic OEM left-arrow bitmap used in 16-bit scrollbars");
  add(category_t::oem_bitmap, 0x7f1a, "OBM_OLD_REDUCE", "Classic OEM minimize button bitmap");
  add(category_t::oem_bitmap, 0x7f1b, "OBM_OLD_RESTORE", "Classic OEM restore button bitmap");
  add(category_t::oem_bitmap, 0x7f15, "OBM_OLD_RGARROW", "Classic OEM right-arrow bitmap used in legacy scrollbars");
  add(category_t::oem_bitmap, 0x7f16, "OBM_OLD_UPARROW", "Classic OEM up-arrow bitmap used in legacy scrollbars");
  add(category_t::oem_bitmap, 0x7f19, "OBM_OLD_ZOOM", "Classic OEM maximize button bitmap");
  add(category_t::oem_bitmap, 0x7f25, "OBM_REDUCE", "System OEM minimize button bitmap");
  add(category_t::oem_bitmap, 0x7ff2, "OBM_REDUCED", "Pressed state of standard Minimize button");
  add(category_t::oem_bitmap, 0x7f23, "OBM_RESTORE", "System OEM restore button bitmap");
  add(category_t::oem_bitmap, 0x7ffa, "OBM_RESTORED", "Pressed state of standard Restore button");
  add(category_t::oem_bitmap, 0x7f27, "OBM_RGARROW", "OEM scrollbar right-arrow button bitmap");
  add(category_t::oem_bitmap, 0x7fed, "OBM_RGARROWD", "Pressed state right scrollbar arrow button");
  add(category_t::oem_bitmap, 0x7fe7, "OBM_RGARROWI", "Inactive/disabled state right scrollbar arrow glyph");
  add(category_t::oem_bitmap, 0x7f1c, "OBM_SIZE", "OEM sizing frame bitmap asset");
  add(category_t::oem_bitmap, 0x7f29, "OBM_UPARROW", "OEM scrollbar up-arrow button bitmap");
  add(category_t::oem_bitmap, 0x7fe0, "OBM_UPARROWD", "OEM bitmap providing pressed state for up-arrows");
  add(category_t::oem_bitmap, 0x7fe9, "OBM_UPARROWI", "Inactive/disabled state up scrollbar arrow glyph");
  add(category_t::oem_bitmap, 0x7f24, "OBM_ZOOM", "System OEM maximize button bitmap");
  add(category_t::oem_bitmap, 0x7ff1, "OBM_ZOOMD", "Pressed state of standard Maximize button");
  add(category_t::system_id,      -1, "IDC_STATIC");
}
/*
DESIGNINFO
LEFTMARGIN, RIGHTMARGIN, TOPMARGIN, BOTTOMMARGIN

    VERTGUIDE,

"VERTGUIDE", "Defines a vertical alignment reference line placed at a specific coordinate (in dialog units) across the dialog. This allows multiple controls (like text boxes or buttons) to snap to a shared vertical axis."
"HORZGUIDE", "Defines a horizontal alignment reference line at a specific coordinate. Used to keep rows or groups of controls level with one another."

GUIDELINES DESIGNINFO DISCARDABLE
BEGIN
    IDD_ABOUTBOX, DIALOG
    BEGIN
       LEFTMARGIN, 7
       RIGHTMARGIN, 228
       TOPMARGIN, 7
       BOTTOMMARGIN, 62
    END

    IDD_MAIN_DIALOG, DIALOG
    BEGIN
        LEFTMARGIN, 7
        RIGHTMARGIN, 313
        TOPMARGIN, 7
        BOTTOMMARGIN, 183
    END
END


<layout class="QVBoxLayout" name="verticalLayout">
  <property name="leftMargin">
    <number>9</number>
  </property>
  <property name="topMargin">
    <number>9</number>
  </property>
  <property name="rightMargin">
    <number>9</number>
  </property>
  <property name="bottomMargin">
    <number>9</number>
  </property>
</layout>

*/
}
