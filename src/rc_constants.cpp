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
  add("WS_BORDER",            0x00800000, constant_category::window_style);
  add("WS_CAPTION",           0x00C00000, constant_category::window_style);
  add("WS_CHILD",             0x40000000, constant_category::window_style);
  add("WS_CLIPCHILDREN",      0x02000000, constant_category::window_style);
  add("WS_CLIPSIBLINGS",      0x04000000, constant_category::window_style);
  add("WS_DISABLED",          0x08000000, constant_category::window_style);
  add("WS_DLGFRAME",          0x00400000, constant_category::window_style);
  add("WS_GROUP",             0x00020000, constant_category::window_style);
  add("WS_HSCROLL",           0x00100000, constant_category::window_style);
  add("WS_MAXIMIZE",          0x01000000, constant_category::window_style);
  add("WS_MAXIMIZEBOX",       0x00010000, constant_category::window_style);
  add("WS_MINIMIZE",          0x20000000, constant_category::window_style);
  add("WS_MINIMIZEBOX",       0x00020000, constant_category::window_style);
  add("WS_OVERLAPPED",        0x00000000, constant_category::window_style);
  add("WS_POPUP",             0x80000000, constant_category::window_style);
  add("WS_SIZEBOX",           0x00040000, constant_category::window_style);
  add("WS_SYSMENU",           0x00080000, constant_category::window_style);
  add("WS_TABSTOP",           0x00010000, constant_category::window_style);
  add("WS_VISIBLE",           0x10000000, constant_category::window_style);
  add("WS_VSCROLL",           0x00200000, constant_category::window_style);
  add("WS_THICKFRAME",        0x00040000, constant_category::window_style);
  add("WS_OVERLAPPEDWINDOW",  0x00CF0000, constant_category::window_style);
  add("WS_POPUPWINDOW",       0x80880000, constant_category::window_style);

  /* ── WS_EX_* Extended Window Styles ───────────────────────────── */
  add("WS_EX_ACCEPTFILES",    0x00000010, constant_category::extended_style);
  add("WS_EX_APPWINDOW",      0x00040000, constant_category::extended_style);
  add("WS_EX_CLIENTEDGE",     0x00000200, constant_category::extended_style);
  add("WS_EX_COMPOSITED",     0x02000000, constant_category::extended_style);
  add("WS_EX_CONTEXTHELP",    0x00000400, constant_category::extended_style);
  add("WS_EX_CONTROLPARENT",  0x00010000, constant_category::extended_style);
  add("WS_EX_DLGMODALFRAME",  0x00000001, constant_category::extended_style);
  add("WS_EX_LAYERED",        0x00080000, constant_category::extended_style);
  add("WS_EX_LAYOUTRTL",      0x00400000, constant_category::extended_style);
  add("WS_EX_LEFT",           0x00000000, constant_category::extended_style);
  add("WS_EX_LEFTSCROLLBAR",  0x00004000, constant_category::extended_style);
  add("WS_EX_LTRREADING",     0x00000000, constant_category::extended_style);
  add("WS_EX_MDICHILD",       0x00000040, constant_category::extended_style);
  add("WS_EX_NOACTIVATE",     0x08000000, constant_category::extended_style);
  add("WS_EX_NOINHERITLAYOUT",0x00100000, constant_category::extended_style);
  add("WS_EX_NOPARENTNOTIFY", 0x00000004, constant_category::extended_style);
  add("WS_EX_PALETTEWINDOW",  0x00000188, constant_category::extended_style);
  add("WS_EX_RIGHT",          0x00001000, constant_category::extended_style);
  add("WS_EX_RIGHTSCROLLBAR", 0x00000000, constant_category::extended_style);
  add("WS_EX_RTLREADING",     0x00002000, constant_category::extended_style);
  add("WS_EX_STATICEDGE",     0x00020000, constant_category::extended_style);
  add("WS_EX_TOOLWINDOW",     0x00000080, constant_category::extended_style);
  add("WS_EX_TOPMOST",        0x00000008, constant_category::extended_style);
  add("WS_EX_TRANSPARENT",    0x00000020, constant_category::extended_style);
  add("WS_EX_WINDOWEDGE",     0x00000100, constant_category::extended_style);

  /* ── DS_* Dialog Styles ───────────────────────────────────────── */
  add("DS_3DLOOK",            0x0004, constant_category::dialog_style);
  add("DS_ABSALIGN",          0x0001, constant_category::dialog_style);
  add("DS_CENTER",            0x0800, constant_category::dialog_style);
  add("DS_CENTERMOUSE",       0x1000, constant_category::dialog_style);
  add("DS_CONTEXTHELP",       0x2000, constant_category::dialog_style);
  add("DS_CONTROL",           0x0400, constant_category::dialog_style);
  add("DS_FIXEDSYS",          0x0008, constant_category::dialog_style);
  add("DS_LOCALEDIT",         0x0020, constant_category::dialog_style);
  add("DS_MODALFRAME",        0x0080, constant_category::dialog_style);
  add("DS_NOFAILCREATE",      0x0010, constant_category::dialog_style);
  add("DS_NOIDLEMSG",         0x0100, constant_category::dialog_style);
  add("DS_SETFONT",           0x0040, constant_category::dialog_style);
  add("DS_SETFOREGROUND",     0x0200, constant_category::dialog_style);
  add("DS_SYSMODAL",          0x0002, constant_category::dialog_style);
  add("DS_SHELLFONT",         0x0048, constant_category::dialog_style);

  /* ── BS_* Button Styles ───────────────────────────────────────── */
  add("BS_3STATE",            0x0005, constant_category::button_style);
  add("BS_AUTO3STATE",        0x0006, constant_category::button_style);
  add("BS_AUTOCHECKBOX",      0x0003, constant_category::button_style);
  add("BS_AUTORADIOBUTTON",   0x0009, constant_category::button_style);
  add("BS_BITMAP",            0x0080, constant_category::button_style);
  add("BS_BOTTOM",            0x0800, constant_category::button_style);
  add("BS_CENTER",            0x0300, constant_category::button_style);
  add("BS_CHECKBOX",          0x0002, constant_category::button_style);
  add("BS_DEFPUSHBUTTON",     0x0001, constant_category::button_style);
  add("BS_GROUPBOX",          0x0007, constant_category::button_style);
  add("BS_ICON",              0x0040, constant_category::button_style);
  add("BS_LEFT",              0x0100, constant_category::button_style);
  add("BS_LEFTTEXT",          0x0020, constant_category::button_style);
  add("BS_MULTILINE",         0x2000, constant_category::button_style);
  add("BS_NOTIFY",            0x4000, constant_category::button_style);
  add("BS_OWNERDRAW",         0x000B, constant_category::button_style);
  add("BS_PUSHBUTTON",        0x0000, constant_category::button_style);
  add("BS_PUSHLIKE",          0x1000, constant_category::button_style);
  add("BS_RADIOBUTTON",       0x0004, constant_category::button_style);
  add("BS_RIGHT",             0x0200, constant_category::button_style);
  add("BS_RIGHTBUTTON",       0x0020, constant_category::button_style);
  add("BS_TEXT",              0x0000, constant_category::button_style);
  add("BS_TOP",               0x0400, constant_category::button_style);
  add("BS_VCENTER",           0x0C00, constant_category::button_style);
  add("BS_FLAT",              0x8000, constant_category::button_style);

  /* ── ES_* Edit Control Styles ─────────────────────────────────── */
  add("ES_AUTOHSCROLL",       0x0080, constant_category::edit_style);
  add("ES_AUTOVSCROLL",       0x0040, constant_category::edit_style);
  add("ES_CENTER",            0x0001, constant_category::edit_style);
  add("ES_LEFT",              0x0000, constant_category::edit_style);
  add("ES_LOWERCASE",         0x0010, constant_category::edit_style);
  add("ES_MULTILINE",         0x0004, constant_category::edit_style);
  add("ES_NOHIDESEL",         0x0100, constant_category::edit_style);
  add("ES_NUMBER",            0x2000, constant_category::edit_style);
  add("ES_OEMCONVERT",        0x0400, constant_category::edit_style);
  add("ES_PASSWORD",          0x0020, constant_category::edit_style);
  add("ES_READONLY",          0x0800, constant_category::edit_style);
  add("ES_RIGHT",             0x0002, constant_category::edit_style);
  add("ES_UPPERCASE",         0x0008, constant_category::edit_style);
  add("ES_WANTRETURN",        0x1000, constant_category::edit_style);

  /* ── SS_* Static Control Styles ───────────────────────────────── */
  add("SS_BITMAP",            0x000E, constant_category::static_style);
  add("SS_BLACKFRAME",        0x0007, constant_category::static_style);
  add("SS_BLACKRECT",         0x0004, constant_category::static_style);
  add("SS_CENTER",            0x0001, constant_category::static_style);
  add("SS_CENTERIMAGE",       0x0200, constant_category::static_style);
  add("SS_EDITCONTROL",       0x2000, constant_category::static_style);
  add("SS_ENHMETAFILE",       0x000F, constant_category::static_style);
  add("SS_ETCHEDFRAME",       0x0012, constant_category::static_style);
  add("SS_ETCHEDHORZ",        0x0010, constant_category::static_style);
  add("SS_ETCHEDVERT",        0x0011, constant_category::static_style);
  add("SS_GRAYFRAME",         0x0008, constant_category::static_style);
  add("SS_GRAYRECT",          0x0005, constant_category::static_style);
  add("SS_ICON",              0x0003, constant_category::static_style);
  add("SS_LEFT",              0x0000, constant_category::static_style);
  add("SS_LEFTNOWORDWRAP",    0x000C, constant_category::static_style);
  add("SS_NOPREFIX",          0x0080, constant_category::static_style);
  add("SS_NOTIFY",            0x0100, constant_category::static_style);
  add("SS_OWNERDRAW",         0x000D, constant_category::static_style);
  add("SS_REALSIZECONTROL",   0x0040, constant_category::static_style);
  add("SS_REALSIZEIMAGE",     0x0800, constant_category::static_style);
  add("SS_RIGHT",             0x0002, constant_category::static_style);
  add("SS_RIGHTJUST",         0x0400, constant_category::static_style);
  add("SS_SIMPLE",            0x000B, constant_category::static_style);
  add("SS_SUNKEN",            0x1000, constant_category::static_style);
  add("SS_WHITEFRAME",        0x0009, constant_category::static_style);
  add("SS_WHITERECT",         0x0006, constant_category::static_style);
  add("SS_TYPEMASK",          0x0000001F, constant_category::static_style);
  add("SS_ENDELLIPSIS",       0x00004000, constant_category::static_style);
  add("SS_PATHELLIPSIS",      0x00008000, constant_category::static_style);
  add("SS_WORDELLIPSIS",      0x0000C000, constant_category::static_style);

  /* ── LBS_* List Box Styles ────────────────────────────────────── */
  add("LBS_DISABLENOSCROLL",  0x1000, constant_category::listbox_style);
  add("LBS_EXTENDEDSEL",      0x0800, constant_category::listbox_style);
  add("LBS_HASSTRINGS",       0x0040, constant_category::listbox_style);
  add("LBS_MULTICOLUMN",      0x0200, constant_category::listbox_style);
  add("LBS_MULTIPLESEL",      0x0008, constant_category::listbox_style);
  add("LBS_NODATA",           0x2000, constant_category::listbox_style);
  add("LBS_NOINTEGRALHEIGHT", 0x0100, constant_category::listbox_style);
  add("LBS_NOREDRAW",         0x0004, constant_category::listbox_style);
  add("LBS_NOSEL",            0x4000, constant_category::listbox_style);
  add("LBS_NOTIFY",           0x0001, constant_category::listbox_style);
  add("LBS_OWNERDRAWFIXED",   0x0010, constant_category::listbox_style);
  add("LBS_OWNERDRAWVARIABLE",0x0020, constant_category::listbox_style);
  add("LBS_SORT",             0x0002, constant_category::listbox_style);
  add("LBS_STANDARD",         0xA00003, constant_category::listbox_style);
  add("LBS_USETABSTOPS",      0x0080, constant_category::listbox_style);
  add("LBS_WANTKEYBOARDINPUT",0x0400, constant_category::listbox_style);

  /* ── CBS_* Combo Box Styles ───────────────────────────────────── */
  add("CBS_AUTOHSCROLL",      0x0040, constant_category::combobox_style);
  add("CBS_DISABLENOSCROLL",  0x0800, constant_category::combobox_style);
  add("CBS_DROPDOWN",         0x0002, constant_category::combobox_style);
  add("CBS_DROPDOWNLIST",     0x0003, constant_category::combobox_style);
  add("CBS_HASSTRINGS",       0x0200, constant_category::combobox_style);
  add("CBS_LOWERCASE",        0x4000, constant_category::combobox_style);
  add("CBS_NOINTEGRALHEIGHT", 0x0400, constant_category::combobox_style);
  add("CBS_OEMCONVERT",       0x0080, constant_category::combobox_style);
  add("CBS_OWNERDRAWFIXED",   0x0010, constant_category::combobox_style);
  add("CBS_OWNERDRAWVARIABLE",0x0020, constant_category::combobox_style);
  add("CBS_SIMPLE",           0x0001, constant_category::combobox_style);
  add("CBS_SORT",             0x0100, constant_category::combobox_style);
  add("CBS_UPPERCASE",        0x2000, constant_category::combobox_style);

  /* ── SBS_* Scroll Bar Styles ──────────────────────────────────── */
  add("SBS_BOTTOMALIGN",      0x0004, constant_category::scrollbar_style);
  add("SBS_HORZ",             0x0000, constant_category::scrollbar_style);
  add("SBS_LEFTALIGN",        0x0002, constant_category::scrollbar_style);
  add("SBS_RIGHTALIGN",       0x0004, constant_category::scrollbar_style);
  add("SBS_SIZEBOX",          0x0008, constant_category::scrollbar_style);
  add("SBS_SIZEBOXBOTTOMRIGHTALIGN", 0x0004, constant_category::scrollbar_style);
  add("SBS_SIZEBOXTOPLEFTALIGN",     0x0002, constant_category::scrollbar_style);
  add("SBS_SIZEGRIP",         0x0010, constant_category::scrollbar_style);
  add("SBS_TOPALIGN",         0x0002, constant_category::scrollbar_style);
  add("SBS_VERT",             0x0001, constant_category::scrollbar_style);

  /* ── VK_* Virtual Key Codes ───────────────────────────────────── */
  add("VK_LBUTTON",           0x01, constant_category::virtual_key);
  add("VK_RBUTTON",           0x02, constant_category::virtual_key);
  add("VK_CANCEL",            0x03, constant_category::virtual_key);
  add("VK_MBUTTON",           0x04, constant_category::virtual_key);
  add("VK_XBUTTON1",          0x05, constant_category::virtual_key);
  add("VK_XBUTTON2",          0x06, constant_category::virtual_key);
  add("VK_BACK",              0x08, constant_category::virtual_key);
  add("VK_TAB",               0x09, constant_category::virtual_key);
  add("VK_CLEAR",             0x0C, constant_category::virtual_key);
  add("VK_RETURN",            0x0D, constant_category::virtual_key);
  add("VK_SHIFT",             0x10, constant_category::virtual_key);
  add("VK_CONTROL",           0x11, constant_category::virtual_key);
  add("VK_MENU",              0x12, constant_category::virtual_key);
  add("VK_PAUSE",             0x13, constant_category::virtual_key);
  add("VK_CAPITAL",           0x14, constant_category::virtual_key);
  add("VK_ESCAPE",            0x1B, constant_category::virtual_key);
  add("VK_SPACE",             0x20, constant_category::virtual_key);
  add("VK_PRIOR",             0x21, constant_category::virtual_key);
  add("VK_NEXT",              0x22, constant_category::virtual_key);
  add("VK_END",               0x23, constant_category::virtual_key);
  add("VK_HOME",              0x24, constant_category::virtual_key);
  add("VK_LEFT",              0x25, constant_category::virtual_key);
  add("VK_UP",                0x26, constant_category::virtual_key);
  add("VK_RIGHT",             0x27, constant_category::virtual_key);
  add("VK_DOWN",              0x28, constant_category::virtual_key);
  add("VK_SELECT",            0x29, constant_category::virtual_key);
  add("VK_PRINT",             0x2A, constant_category::virtual_key);
  add("VK_EXECUTE",           0x2B, constant_category::virtual_key);
  add("VK_SNAPSHOT",          0x2C, constant_category::virtual_key);
  add("VK_INSERT",            0x2D, constant_category::virtual_key);
  add("VK_DELETE",            0x2E, constant_category::virtual_key);
  add("VK_HELP",              0x2F, constant_category::virtual_key);
  add("VK_0",                 0x30, constant_category::virtual_key);
  add("VK_1",                 0x31, constant_category::virtual_key);
  add("VK_2",                 0x32, constant_category::virtual_key);
  add("VK_3",                 0x33, constant_category::virtual_key);
  add("VK_4",                 0x34, constant_category::virtual_key);
  add("VK_5",                 0x35, constant_category::virtual_key);
  add("VK_6",                 0x36, constant_category::virtual_key);
  add("VK_7",                 0x37, constant_category::virtual_key);
  add("VK_8",                 0x38, constant_category::virtual_key);
  add("VK_9",                 0x39, constant_category::virtual_key);
  add("VK_A",                 0x41, constant_category::virtual_key);
  add("VK_B",                 0x42, constant_category::virtual_key);
  add("VK_C",                 0x43, constant_category::virtual_key);
  add("VK_D",                 0x44, constant_category::virtual_key);
  add("VK_E",                 0x45, constant_category::virtual_key);
  add("VK_F",                 0x46, constant_category::virtual_key);
  add("VK_G",                 0x47, constant_category::virtual_key);
  add("VK_H",                 0x48, constant_category::virtual_key);
  add("VK_I",                 0x49, constant_category::virtual_key);
  add("VK_J",                 0x4A, constant_category::virtual_key);
  add("VK_K",                 0x4B, constant_category::virtual_key);
  add("VK_L",                 0x4C, constant_category::virtual_key);
  add("VK_M",                 0x4D, constant_category::virtual_key);
  add("VK_N",                 0x4E, constant_category::virtual_key);
  add("VK_O",                 0x4F, constant_category::virtual_key);
  add("VK_P",                 0x50, constant_category::virtual_key);
  add("VK_Q",                 0x51, constant_category::virtual_key);
  add("VK_R",                 0x52, constant_category::virtual_key);
  add("VK_S",                 0x53, constant_category::virtual_key);
  add("VK_T",                 0x54, constant_category::virtual_key);
  add("VK_U",                 0x55, constant_category::virtual_key);
  add("VK_V",                 0x56, constant_category::virtual_key);
  add("VK_W",                 0x57, constant_category::virtual_key);
  add("VK_X",                 0x58, constant_category::virtual_key);
  add("VK_Y",                 0x59, constant_category::virtual_key);
  add("VK_Z",                 0x5A, constant_category::virtual_key);
  add("VK_LWIN",              0x5B, constant_category::virtual_key);
  add("VK_RWIN",              0x5C, constant_category::virtual_key);
  add("VK_APPS",              0x5D, constant_category::virtual_key);
  add("VK_SLEEP",             0x5F, constant_category::virtual_key);
  add("VK_NUMPAD0",           0x60, constant_category::virtual_key);
  add("VK_NUMPAD1",           0x61, constant_category::virtual_key);
  add("VK_NUMPAD2",           0x62, constant_category::virtual_key);
  add("VK_NUMPAD3",           0x63, constant_category::virtual_key);
  add("VK_NUMPAD4",           0x64, constant_category::virtual_key);
  add("VK_NUMPAD5",           0x65, constant_category::virtual_key);
  add("VK_NUMPAD6",           0x66, constant_category::virtual_key);
  add("VK_NUMPAD7",           0x67, constant_category::virtual_key);
  add("VK_NUMPAD8",           0x68, constant_category::virtual_key);
  add("VK_NUMPAD9",           0x69, constant_category::virtual_key);
  add("VK_MULTIPLY",          0x6A, constant_category::virtual_key);
  add("VK_ADD",               0x6B, constant_category::virtual_key);
  add("VK_SEPARATOR",         0x6C, constant_category::virtual_key);
  add("VK_SUBTRACT",          0x6D, constant_category::virtual_key);
  add("VK_DECIMAL",           0x6E, constant_category::virtual_key);
  add("VK_DIVIDE",            0x6F, constant_category::virtual_key);
  add("VK_F1",                0x70, constant_category::virtual_key);
  add("VK_F2",                0x71, constant_category::virtual_key);
  add("VK_F3",                0x72, constant_category::virtual_key);
  add("VK_F4",                0x73, constant_category::virtual_key);
  add("VK_F5",                0x74, constant_category::virtual_key);
  add("VK_F6",                0x75, constant_category::virtual_key);
  add("VK_F7",                0x76, constant_category::virtual_key);
  add("VK_F8",                0x77, constant_category::virtual_key);
  add("VK_F9",                0x78, constant_category::virtual_key);
  add("VK_F10",               0x79, constant_category::virtual_key);
  add("VK_F11",               0x7A, constant_category::virtual_key);
  add("VK_F12",               0x7B, constant_category::virtual_key);
  add("VK_NUMLOCK",           0x90, constant_category::virtual_key);
  add("VK_SCROLL",            0x91, constant_category::virtual_key);
  add("VK_LSHIFT",            0xA0, constant_category::virtual_key);
  add("VK_RSHIFT",            0xA1, constant_category::virtual_key);
  add("VK_LCONTROL",          0xA2, constant_category::virtual_key);
  add("VK_RCONTROL",          0xA3, constant_category::virtual_key);
  add("VK_LMENU",             0xA4, constant_category::virtual_key);
  add("VK_RMENU",             0xA5, constant_category::virtual_key);

  /* ── WM_* Window Messages ─────────────────────────────────────── */
  add("WM_NULL",              0x0000, constant_category::window_message);
  add("WM_CREATE",            0x0001, constant_category::window_message);
  add("WM_DESTROY",           0x0002, constant_category::window_message);
  add("WM_MOVE",              0x0003, constant_category::window_message);
  add("WM_SIZE",              0x0005, constant_category::window_message);
  add("WM_ACTIVATE",          0x0006, constant_category::window_message);
  add("WM_SETFOCUS",          0x0007, constant_category::window_message);
  add("WM_KILLFOCUS",         0x0008, constant_category::window_message);
  add("WM_ENABLE",            0x000A, constant_category::window_message);
  add("WM_SETREDRAW",         0x000B, constant_category::window_message);
  add("WM_SETTEXT",           0x000C, constant_category::window_message);
  add("WM_GETTEXT",           0x000D, constant_category::window_message);
  add("WM_GETTEXTLENGTH",     0x000E, constant_category::window_message);
  add("WM_PAINT",             0x000F, constant_category::window_message);
  add("WM_CLOSE",             0x0010, constant_category::window_message);
  add("WM_QUERYENDSESSION",   0x0011, constant_category::window_message);
  add("WM_QUIT",              0x0012, constant_category::window_message);
  add("WM_ERASEBKGND",        0x0014, constant_category::window_message);
  add("WM_SHOWWINDOW",        0x0018, constant_category::window_message);
  add("WM_ACTIVATEAPP",       0x001C, constant_category::window_message);
  add("WM_CANCELMODE",        0x001F, constant_category::window_message);
  add("WM_SETCURSOR",         0x0020, constant_category::window_message);
  add("WM_MOUSEACTIVATE",     0x0021, constant_category::window_message);
  add("WM_CHILDACTIVATE",     0x0022, constant_category::window_message);
  add("WM_GETMINMAXINFO",     0x0024, constant_category::window_message);
  add("WM_SETFONT",           0x0030, constant_category::window_message);
  add("WM_GETFONT",           0x0031, constant_category::window_message);
  add("WM_SETHOTKEY",         0x0032, constant_category::window_message);
  add("WM_GETHOTKEY",         0x0033, constant_category::window_message);
  add("WM_QUERYDRAGICON",     0x0037, constant_category::window_message);
  add("WM_COMPAREITEM",       0x0039, constant_category::window_message);
  add("WM_WINDOWPOSCHANGING", 0x0046, constant_category::window_message);
  add("WM_WINDOWPOSCHANGED",  0x0047, constant_category::window_message);
  add("WM_COPYDATA",          0x004A, constant_category::window_message);
  add("WM_NOTIFY",            0x004E, constant_category::window_message);
  add("WM_CONTEXTMENU",       0x007B, constant_category::window_message);
  add("WM_STYLECHANGING",     0x007C, constant_category::window_message);
  add("WM_STYLECHANGED",      0x007D, constant_category::window_message);
  add("WM_DISPLAYCHANGE",     0x007E, constant_category::window_message);
  add("WM_GETICON",           0x007F, constant_category::window_message);
  add("WM_SETICON",           0x0080, constant_category::window_message);
  add("WM_NCCREATE",          0x0081, constant_category::window_message);
  add("WM_NCDESTROY",         0x0082, constant_category::window_message);
  add("WM_NCCALCSIZE",        0x0083, constant_category::window_message);
  add("WM_NCHITTEST",         0x0084, constant_category::window_message);
  add("WM_NCPAINT",           0x0085, constant_category::window_message);
  add("WM_NCACTIVATE",        0x0086, constant_category::window_message);
  add("WM_GETDLGCODE",        0x0087, constant_category::window_message);
  add("WM_KEYDOWN",           0x0100, constant_category::window_message);
  add("WM_KEYUP",             0x0101, constant_category::window_message);
  add("WM_CHAR",              0x0102, constant_category::window_message);
  add("WM_DEADCHAR",          0x0103, constant_category::window_message);
  add("WM_SYSKEYDOWN",        0x0104, constant_category::window_message);
  add("WM_SYSKEYUP",          0x0105, constant_category::window_message);
  add("WM_SYSCHAR",           0x0106, constant_category::window_message);
  add("WM_SYSDEADCHAR",       0x0107, constant_category::window_message);
  add("WM_INITDIALOG",        0x0110, constant_category::window_message);
  add("WM_COMMAND",           0x0111, constant_category::window_message);
  add("WM_SYSCOMMAND",        0x0112, constant_category::window_message);
  add("WM_TIMER",             0x0113, constant_category::window_message);
  add("WM_HSCROLL",           0x0114, constant_category::window_message);
  add("WM_VSCROLL",           0x0115, constant_category::window_message);
  add("WM_INITMENU",          0x0116, constant_category::window_message);
  add("WM_INITMENUPOPUP",     0x0117, constant_category::window_message);
  add("WM_MENUSELECT",        0x011F, constant_category::window_message);
  add("WM_MENUCHAR",          0x0120, constant_category::window_message);
  add("WM_ENTERIDLE",         0x0121, constant_category::window_message);
  add("WM_MENURBUTTONUP",     0x0122, constant_category::window_message);
  add("WM_MENUDRAG",          0x0123, constant_category::window_message);
  add("WM_MENUGETOBJECT",     0x0124, constant_category::window_message);
  add("WM_UNINITMENUPOPUP",   0x0125, constant_category::window_message);
  add("WM_MENUCOMMAND",       0x0126, constant_category::window_message);
  add("WM_CTLCOLORMSGBOX",    0x0132, constant_category::window_message);
  add("WM_CTLCOLOREDIT",      0x0133, constant_category::window_message);
  add("WM_CTLCOLORLISTBOX",   0x0134, constant_category::window_message);
  add("WM_CTLCOLORBTN",       0x0135, constant_category::window_message);
  add("WM_CTLCOLORDLG",       0x0136, constant_category::window_message);
  add("WM_CTLCOLORSCROLLBAR", 0x0137, constant_category::window_message);
  add("WM_CTLCOLORSTATIC",    0x0138, constant_category::window_message);
  add("WM_MOUSEMOVE",         0x0200, constant_category::window_message);
  add("WM_LBUTTONDOWN",       0x0201, constant_category::window_message);
  add("WM_LBUTTONUP",         0x0202, constant_category::window_message);
  add("WM_LBUTTONDBLCLK",     0x0203, constant_category::window_message);
  add("WM_RBUTTONDOWN",       0x0204, constant_category::window_message);
  add("WM_RBUTTONUP",         0x0205, constant_category::window_message);
  add("WM_RBUTTONDBLCLK",     0x0206, constant_category::window_message);
  add("WM_MBUTTONDOWN",       0x0207, constant_category::window_message);
  add("WM_MBUTTONUP",         0x0208, constant_category::window_message);
  add("WM_MBUTTONDBLCLK",     0x0209, constant_category::window_message);
  add("WM_MOUSEWHEEL",        0x020A, constant_category::window_message);
  add("WM_PARENTNOTIFY",      0x0210, constant_category::window_message);
  add("WM_ENTERMENULOOP",     0x0211, constant_category::window_message);
  add("WM_EXITMENULOOP",      0x0212, constant_category::window_message);
  add("WM_NEXTMENU",          0x0213, constant_category::window_message);
  add("WM_SIZING",            0x0214, constant_category::window_message);
  add("WM_PRINT",             0x0317, constant_category::window_message);
  add("WM_PRINTCLIENT",       0x0318, constant_category::window_message);
  add("WM_USER",              0x0400, constant_category::window_message);
  add("WM_APP",               0x8000, constant_category::window_message);

  /* ── MB_* Message Box Flags ───────────────────────────────────── */
  add("MB_OK",                0x00000000, constant_category::message_box);
  add("MB_OKCANCEL",          0x00000001, constant_category::message_box);
  add("MB_ABORTRETRYIGNORE",  0x00000002, constant_category::message_box);
  add("MB_YESNOCANCEL",       0x00000003, constant_category::message_box);
  add("MB_YESNO",             0x00000004, constant_category::message_box);
  add("MB_RETRYCANCEL",       0x00000005, constant_category::message_box);
  add("MB_CANCELTRYCONTINUE", 0x00000006, constant_category::message_box);
  add("MB_ICONHAND",          0x00000010, constant_category::message_box);
  add("MB_ICONERROR",         0x00000010, constant_category::message_box);
  add("MB_ICONQUESTION",      0x00000020, constant_category::message_box);
  add("MB_ICONEXCLAMATION",   0x00000030, constant_category::message_box);
  add("MB_ICONWARNING",       0x00000030, constant_category::message_box);
  add("MB_ICONASTERISK",      0x00000040, constant_category::message_box);
  add("MB_ICONINFORMATION",   0x00000040, constant_category::message_box);
  add("MB_DEFBUTTON1",        0x00000000, constant_category::message_box);
  add("MB_DEFBUTTON2",        0x00000100, constant_category::message_box);
  add("MB_DEFBUTTON3",        0x00000200, constant_category::message_box);
  add("MB_DEFBUTTON4",        0x00000300, constant_category::message_box);
  add("MB_APPLMODAL",         0x00000000, constant_category::message_box);
  add("MB_SYSTEMMODAL",       0x00001000, constant_category::message_box);
  add("MB_TASKMODAL",         0x00002000, constant_category::message_box);
  add("MB_HELP",              0x00004000, constant_category::message_box);
  add("MB_SETFOREGROUND",     0x00010000, constant_category::message_box);
  add("MB_DEFAULT_DESKTOP_ONLY", 0x00020000, constant_category::message_box);
  add("MB_TOPMOST",           0x00040000, constant_category::message_box);
  add("MB_RIGHT",             0x00080000, constant_category::message_box);
  add("MB_RTLREADING",        0x00100000, constant_category::message_box);

  /* ── MF_* Menu Flags ──────────────────────────────────────────── */
  add("MF_STRING",            0x0000, constant_category::menu_flag);
  add("MF_GRAYED",            0x0001, constant_category::menu_flag);
  add("MF_DISABLED",          0x0002, constant_category::menu_flag);
  add("MF_CHECKED",           0x0008, constant_category::menu_flag);
  add("MF_POPUP",             0x0010, constant_category::menu_flag);
  add("MF_MENUBARBREAK",      0x0020, constant_category::menu_flag);
  add("MF_MENUBREAK",         0x0040, constant_category::menu_flag);
  add("MF_END",               0x0080, constant_category::menu_flag);
  add("MF_OWNERDRAW",         0x0100, constant_category::menu_flag);
  add("MF_SEPARATOR",         0x0800, constant_category::menu_flag);
  add("MF_BYCOMMAND",         0x0000, constant_category::menu_flag);
  add("MF_BYPOSITION",        0x0400, constant_category::menu_flag);
  add("MF_DEFAULT",           0x1000, constant_category::menu_flag);
  add("MF_SYSMENU",           0x2000, constant_category::menu_flag);
  add("MF_HELP",              0x4000, constant_category::menu_flag);
  add("MF_RIGHTJUSTIFY",      0x4000, constant_category::menu_flag);
  add("MF_MOUSESELECT",       0x8000, constant_category::menu_flag);

  /* ── MFT_* Extended Menu Item Types ───────────────────────────── */
  add("MFT_STRING",           0x0000, constant_category::menu_flag);
  add("MFT_BITMAP",           0x0004, constant_category::menu_flag);
  add("MFT_MENUBARBREAK",     0x0020, constant_category::menu_flag);
  add("MFT_MENUBREAK",        0x0040, constant_category::menu_flag);
  add("MFT_OWNERDRAW",        0x0100, constant_category::menu_flag);
  add("MFT_RADIOCHECK",       0x0200, constant_category::menu_flag);
  add("MFT_SEPARATOR",        0x0800, constant_category::menu_flag);
  add("MFT_RIGHTORDER",       0x2000, constant_category::menu_flag);
  add("MFT_RIGHTJUSTIFY",     0x4000, constant_category::menu_flag);

  /* ── MFS_* Extended Menu Item States ──────────────────────────── */
  add("MFS_ENABLED",          0x0000, constant_category::menu_flag);
  add("MFS_DISABLED",         0x0003, constant_category::menu_flag);
  add("MFS_GRAYED",           0x0003, constant_category::menu_flag);
  add("MFS_CHECKED",          0x0008, constant_category::menu_flag);
  add("MFS_UNCHECKED",        0x0000, constant_category::menu_flag);
  add("MFS_HILITE",           0x0080, constant_category::menu_flag);
  add("MFS_UNHILITE",         0x0000, constant_category::menu_flag);
  add("MFS_DEFAULT",          0x1000, constant_category::menu_flag);

  /* ── Accelerator Flags ────────────────────────────────────────── */
  add("FVIRTKEY",             0x01, constant_category::accelerator_flag);
  add("FNOINVERT",            0x02, constant_category::accelerator_flag);
  add("FSHIFT",               0x04, constant_category::accelerator_flag);
  add("FCONTROL",             0x08, constant_category::accelerator_flag);
  add("FALT",                 0x10, constant_category::accelerator_flag);
  add("NOINVERT",             0x02, constant_category::accelerator_flag);

  /* ── RT_* Predefined Resource Types ───────────────────────────── */
  add("RT_CURSOR",            1,  constant_category::resource_type);
  add("RT_BITMAP",            2,  constant_category::resource_type);
  add("RT_ICON",              3,  constant_category::resource_type);
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
  add("IDC_STATIC",           -1, constant_category::system_id);
}

}
