#ifndef RCDEF_H
#define RCDEF_H

#include <string>
#include <vector>
#include <map>
#include <list>

/*
AFX_    generated from afx?

Prefix 	Resource type
IDR_    multiple
IDD_    dialog templates
IDC_    cursor
IDI_    icon
IDB_    bitmap
IDS_    string
IDP_    string
ID_     command
IDC_    control
*/

static std::map<std::string, std::list<std::string>> resource_map;

static const std::list<std::string> builtin_types =
{
  "container",
  "string",
  "char8_t",
  "int16_t",
  "uint16_t",
  "uint32_t",
  "textual_id",
};

static const std::list<std::string> key_types =
{
  "choice",
  "option",
  "multi",
  "literal",
  "type",
  "keyword",
  "container",
  "statements",
  "controls",
};

std::list<std::string> class_list =
{
/*
    "ANIMATE_CLASS",
    "DATETIMEPICK_CLASS",
    "HOTKEY_CLASS",
    "LINK_CLASS",
    "MONTHCAL_CLASS",
    "NATIVEFNTCTL_CLASS",
    "PROGRESS_CLASS",
    "REBARCLASSNAME",
    "STANDARD_CLASSES",
    "STATUSCLASSNAME",
    "TOOLBARCLASSNAME",
    "TOOLTIPS_CLASS",
    "TRACKBAR_CLASS",
    "UPDOWN_CLASS",
    "WC_BUTTON",
    "WC_COMBOBOX",
    "WC_COMBOBOXEX",
    "WC_EDIT",
    "WC_HEADER",
    "WC_LISTBOX",
    "WC_IPADDRESS",
    "WC_LINK",
    "WC_LISTVIEW",
    "WC_NATIVEFONTCTL",
    "WC_PAGESCROLLER",
    "WC_SCROLLBAR",
    "WC_STATIC",
    "WC_TABCONTROL",
    "WC_TREEVIEW",
*/
    // old style (case insensitive)
    // see: https://learn.microsoft.com/en-gb/windows/win32/winauto/uiauto-controlsupport
    "Static",
    "Button",
    "ComboBoxEx32",
    "ComboBox",
    "Edit",
    "SysLink"
    "SysIPAddress32",
    "SysHeader32",
    "SysListView32",
    "ListBox",
    "#32768", // Menu/MenuItem
    "msctls_progress32",
    "RichEdit",
    "RichEdit20A",
    "RichEdit20W",
    "RichEdit50W",
    "ScrollBar",
    "msctls_trackbar32",
    "msctls_updown32",
    "msctls_statusbar32",
    "SysTabControl32",
    "ToolbarWindow32",
    "tooltips_class32",
    "#32774", // ToolTip
    "ReBarWindow32",
    "SysTreeView32",
    "SysAnimate32",
    "SysPager",
    "SysDateTimePick32",
    "SysMonthCal32",
    "MS_WINNOTE",
    "VBBubble",
    "SuperGrid",
    "msctls_hotkey32",
    "NativeFontCtl",
    "commctrl_DragListMsg",
};

std::list<std::string> super_class_list =
{
  "AUTO3STATE"
  "AUTOCHECKBOX"
  "AUTORADIOBUTTON"
  "CHECKBOX"
  "COMBOBOX"
  "CONTROL"
  "CTEXT"
  "DEFPUSHBUTTON"
  "EDITTEXT"
  "GROUPBOX"
  "ICON"
  "LISTBOX"
  "LTEXT"
  "PUSHBOX"
  "PUSHBUTTON"
  "RADIOBUTTON"
  "RTEXT"
  "SCROLLBAR"
  "STATE3"
};

// map improper names and super classes to control class names
std::map<std::string, std::string> class_aliases =
{
    { "static",           "Static"    },
    { "radiobutton",      "Button"    },
    { "pushbutton",       "Button"    },
    { "button",           "Button"    },
    { "button",           "Button"    },
    { "combobox",         "ComboBox"  },
    { "comboboxex32",     "ComboBox"  },
    { "richedit",         "RichEdit"  },
    { "richedit20a",      "RichEdit"  },
    { "richedit20w",      "RichEdit"  },
    { "richedit50w",      "RichEdit"  },
    { "edit",             "Edit"      },
    { "edittext",         "Edit"      },
    { "scrollbar",        "ScrollBar" },
    { "syslink",          "SysLink"           },
    { "sysipaddress32",   "SysIPAddress32"    },
    { "sysheader32",      "SysHeader32"       },
    { "syslistview32",    "SysListView32"     },
    { "listbox",          "ListBox"           },
    { "systabcontrol32",  "SysTabControl32"   },
    { "toolbarwindow32",  "ToolbarWindow32"   },
    { "rebarwindow32",    "ReBarWindow32"     },
    { "systabcontrol32",  "SysTabControl32"   },
    { "systreeview32",    "SysTreeView32"     },
    { "sysanimate32",     "SysAnimate32"      },
    { "syspager",         "SysPager"          },
    { "sysdatetimepick32","SysDateTimePick32" },
    { "sysmonthcal32",    "SysMonthCal32"     },
    { "ms_winnote",       "MS_WINNOTE"        },
    { "vbbubble",         "VBBubble"          },
    { "supergrid",        "SuperGrid"         },
    { "nativefontctl",    "NativeFontCtl"     },
    { "commctrl_draglistmsg", "commctrl_DragListMsg" },
};

std::list<std::string> common_styles =
{
  "WS_BORDER",
  "WS_CAPTION",
  "WS_CHILD",
  "WS_CHILDWINDOW",
  "WS_CLIPCHILDREN",
  "WS_CLIPSIBLINGS",
  "WS_DISABLED",
  "WS_DLGFRAME",
  "WS_GROUP",
  "WS_HSCROLL",
  "WS_ICONIC",
  "WS_MAXIMIZE",
  "WS_MAXIMIZEBOX",
  "WS_MINIMIZE",
  "WS_MINIMIZEBOX",
  "WS_OVERLAPPED",
  "WS_OVERLAPPEDWINDOW",
  "WS_POPUP",
  "WS_POPUPWINDOW",
  "WS_SIZEBOX",
  "WS_SYSMENU",
  "WS_TABSTOP",
  "WS_THICKFRAME",
  "WS_TILED",
  "WS_TILEDWINDOW",
  "WS_VISIBLE",
  "WS_VSCROLL",
  // for "CONTROL"
  "CCS_ADJUSTABLE",
  "CCS_BOTTOM",
  "CCS_LEFT",
  "CCS_NODIVIDER",
  "CCS_NOMOVEX",
  "CCS_NOMOVEY",
  "CCS_NOPARENTALIGN",
  "CCS_NORESIZE",
  "CCS_RIGHT",
  "CCS_TOP",
  "CCS_VERT",
};

static const std::map<std::string, std::list<std::string>> style_map = {
    {"sysanimate32",
     {
         "ACS_AUTOPLAY",
         "ACS_CENTER",
         "ACS_TIMER",
         "ACS_TRANSPARENT",
     }},
    {"button",
     {
         "BS_3STATE",      "BS_AUTO3STATE",     "BS_AUTOCHECKBOX",  "BS_AUTORADIOBUTTON",
         "BS_BITMAP",      "BS_BOTTOM",         "BS_CENTER",        "BS_CHECKBOX",
         "BS_COMMANDLINK", "BS_DEFCOMMANDLINK", "BS_DEFPUSHBUTTON", "BS_DEFSPLITBUTTON",
         "BS_GROUPBOX",    "BS_ICON",           "BS_FLAT",          "BS_LEFT",
         "BS_LEFTTEXT",    "BS_MULTILINE",      "BS_NOTIFY",        "BS_OWNERDRAW",
         "BS_PUSHBUTTON",  "BS_PUSHLIKE",       "BS_RADIOBUTTON",   "BS_RIGHT",
         "BS_RIGHTBUTTON", "BS_SPLITBUTTON",    "BS_TEXT",          "BS_TOP",
         "BS_TYPEMASK",    "BS_USERBUTTON",     "BS_VCENTER",
     }},
    {"combobox",
     {
         "CBS_AUTOHSCROLL",
         "CBS_DISABLENOSCROLL",
         "CBS_DROPDOWN",
         "CBS_DROPDOWNLIST",
         "CBS_HASSTRINGS",
         "CBS_LOWERCASE",
         "CBS_NOINTEGRALHEIGHT",
         "CBS_OEMCONVERT",
         "CBS_OWNERDRAWFIXED",
         "CBS_OWNERDRAWVARIABLE",
         "CBS_SIMPLE",
         "CBS_SORT",
         "CBS_UPPERCASE",
         // extended
         "CBES_EX_CASESENSITIVE",
         "CBES_EX_NOEDITIMAGE",
         "CBES_EX_NOEDITIMAGEINDENT",
         "CBES_EX_NOSIZELIMIT",
         "CBES_EX_PATHWORDBREAKPROC",
         "CBES_EX_TEXTENDELLIPSIS",
     }},
    {"sysdatetimepick32",
     {
         "DTS_APPCANPARSE",
         "DTS_LONGDATEFORMAT",
         "DTS_RIGHTALIGN",
         "DTS_SHOWNONE",
         "DTS_SHORTDATEFORMAT",
         "DTS_SHORTDATECENTURYFORMAT",
         "DTS_TIMEFORMAT",
         "DTS_UPDOWN",
     }},
    {"edit", // EDITTEXT supertype
     {
         "ES_AUTOHSCROLL",
         "ES_AUTOVSCROLL",
         "ES_CENTER",
         "ES_LEFT",
         "ES_LOWERCASE",
         "ES_MULTILINE",
         "ES_NOHIDESEL",
         "ES_NUMBER",
         "ES_OEMCONVERT",
         "ES_READONLY",
         "ES_RIGHT",
         "ES_UPPERCASE",
         "ES_WANTRETURN",
     }},
    {"sysheader32",
     {
         "HDS_BUTTONS",
         "HDS_DRAGDROP",
         "HDS_FILTERBAR",
         "HDS_FLAT",
         "HDS_FULLDRAG",
         "HDS_HIDDEN",
         "HDS_HORZ",
         "HDS_HOTTRACK",
         "HDS_CHECKBOXES",
         "HDS_NOSIZING",
         "HDS_OVERFLOW",
     }},
    {"listbox", // LISTBOX super type
     {
         "LBS_COMBOBOX",
         "LBS_DISABLENOSCROLL",
         "LBS_EXTENDEDSEL",
         "LBS_HASSTRINGS",
         "LBS_MULTICOLUMN",
         "LBS_MULTIPLESEL",
         "LBS_NODATA",
         "LBS_NOINTEGRALHEIGHT",
         "LBS_NOREDRAW",
         "LBS_NOSEL",
         "LBS_NOTIFY",
         "LBS_OWNERDRAWFIXED",
         "LBS_OWNERDRAWVARIABLE",
         "LBS_SORT",
         "LBS_STANDARD",
         "LBS_USETABSTOPS",
         "LBS_WANTKEYBOARDINPUT",
     }},
    {"syslistview32",
     {
         "LVS_ALIGNLEFT",     "LVS_ALIGNMASK",     "LVS_ALIGNTOP",       "LVS_AUTOARRANGE",   "LVS_EDITLABELS",
         "LVS_ICON",          "LVS_LIST",          "LVS_NOCOLUMNHEADER", "LVS_NOLABELWRAP",   "LVS_NOSCROLL",
         "LVS_NOSORTHEADER",  "LVS_OWNERDATA",     "LVS_OWNERDRAWFIXED", "LVS_REPORT",        "LVS_SHAREIMAGELISTS",
         "LVS_SHOWSELALWAYS", "LVS_SINGLESEL",     "LVS_SMALLICON",      "LVS_SORTASCENDING", "LVS_SORTDESCENDING",
         "LVS_TYPEMASK",      "LVS_TYPESTYLEMASK",
     }},
    {"sysmonthcal32",
     {
         "MCS_DAYSTATE",
         "MCS_MULTISELECT",
         "MCS_WEEKNUMBERS",
         "MCS_NOTODAYCIRCLE",
         "MCS_NOTODAY",
         "MCS_NOTRAILINGDATES",
         "MCS_SHORTDAYSOFWEEK",
         "MCS_NOSELCHANGEONNAV",
     }},
    {
        "syspager",
        {
            "PGS_AUTOSCROLL",
            "PGS_DRAGNDROP",
            "PGS_HORZ",
            "PGS_VERT",
        },
    },
    {"msctls_progress32",
     {
         "PBS_MARQUEE",
         "PBS_SMOOTH",
         "PBS_SMOOTHREVERSE",
         "PBS_VERTICAL",
     }},
    {"rebarwindow32",
     {
         "RBS_AUTOSIZE",
         "RBS_BANDBORDERS",
         "RBS_DBLCLKTOGGLE",
         "RBS_FIXEDORDER",
         "RBS_REGISTERDROP",
         "RBS_TOOLTIPS",
         "RBS_VARHEIGHT",
         "RBS_VERTICALGRIPPER",
     }},
    {"richedit",
     {
         "ES_DISABLENOSCROLL", "ES_EX_NOCALLOLEINIT", "ES_NOIME",    "ES_NOOLEDRAGDROP", "ES_SAVESEL",
         "ES_SELECTIONBAR",    "ES_SELFIME",          "ES_SUNKEN",   "ES_VERTICAL",      "ES_AUTOHSCROLL",
         "ES_AUTOVSCROLL",     "ES_CENTER",           "ES_LEFT",     "ES_MULTILINE",     "ES_NOHIDESEL",
         "ES_NUMBER",          "ES_PASSWORD",         "ES_READONLY", "ES_RIGHT",         "ES_WANTRETURN",
     }},
    {"scrollbar",
     {
         "SBS_BOTTOMALIGN",
         "SBS_HORZ",
         "SBS_LEFTALIGN",
         "SBS_RIGHTALIGN",
         "SBS_SIZEBOX",
         "SBS_SIZEBOXBOTTOMRIGHTALIGN",
         "SBS_SIZEBOXTOPLEFTALIGN",
         "SBS_SIZEGRIP",
         "SBS_TOPALIGN",
         "SBS_VERT",
     }},
    {"static",
     {
         "SS_BITMAP",          "SS_BLACKFRAME",    "SS_BLACKRECT",   "SS_CENTER",      "SS_CENTERIMAGE",
         "SS_EDITCONTROL",     "SS_ENDELLIPSIS",   "SS_ENHMETAFILE", "SS_ETCHEDFRAME", "SS_ETCHEDHORZ",
         "SS_ETCHEDVERT",      "SS_GRAYFRAME",     "SS_GRAYRECT",    "SS_ICON",        "SS_LEFT",
         "SS_LEFTNOWORDWRAP",  "SS_NOPREFIX",      "SS_NOTIFY",      "SS_OWNERDRAW",   "SS_PATHELLIPSIS",
         "SS_REALSIZECONTROL", "SS_REALSIZEIMAGE", "SS_RIGHT",       "SS_RIGHTJUST",   "SS_SIMPLE",
         "SS_SUNKEN",          "SS_TYPEMASK",      "SS_WHITEFRAME",  "SS_WHITERECT",   "SS_WORDELLIPSIS",
     }},
    {"msctls_statusbar32",
     {
         "SBARS_SIZEGRIP",
         "SBT_TOOLTIPS",
         "SBARS_TOOLTIPS",
     }},
    {"syslink",
     {
         "LWS_TRANSPARENT",
         "LWS_IGNORERETURN",
         "LWS_NOPREFIX",
         "LWS_USEVISUALSTYLE",
         "LWS_USECUSTOMTEXT",
         "LWS_RIGHT",
     }},
    {"systabcontrol32",
     {
         "TCS_BOTTOM",         "TCS_BUTTONS",        "TCS_FIXEDWIDTH",
         "TCS_FLATBUTTONS",    "TCS_FOCUSNEVER",     "TCS_FOCUSONBUTTONDOWN",
         "TCS_FORCEICONLEFT",  "TCS_FORCELABELLEFT", "TCS_HOTTRACK",
         "TCS_MULTILINE",      "TCS_MULTISELECT",    "TCS_OWNERDRAWFIXED",
         "TCS_RAGGEDRIGHT",    "TCS_RIGHT",          "TCS_RIGHTJUSTIFY",
         "TCS_SCROLLOPPOSITE", "TCS_SINGLELINE",     "TCS_TABS",
         "TCS_TOOLTIPS",       "TCS_VERTICAL",
     }},
    {"toolbarwindow32",
     {
         "TBSTYLE_ALTDRAG",
         "TBSTYLE_CUSTOMERASE",
         "TBSTYLE_FLAT",
         "TBSTYLE_LIST",
         "TBSTYLE_REGISTERDROP",
         "TBSTYLE_TOOLTIPS",
         "TBSTYLE_TRANSPARENT",
         "TBSTYLE_WRAPABLE",
         // extended
         "TBSTYLE_EX_DRAWDDARROWS",
         "TBSTYLE_EX_HIDECLIPPEDBUTTONS",
         "TBSTYLE_EX_DOUBLEBUFFER",
         "TBSTYLE_EX_MIXEDBUTTONS",
         "TBSTYLE_EX_MULTICOLUMN",
         "TBSTYLE_EX_VERTICAL",
     }},
    {"TBBUTTON", // toolbar button
     {
         "BTNS_AUTOSIZE",
         "BTNS_BUTTON",
         "BTNS_CHECK",
         "BTNS_CHECKGROUP",
         "BTNS_DROPDOWN",
         "BTNS_GROUP",
         "BTNS_NOPREFIX",
         "BTNS_SEP",
         "BTNS_SHOWTEXT",
         "BTNS_WHOLEDROPDOWN",
         "TBSTYLE_AUTOSIZE",
         "TBSTYLE_BUTTON",
         "TBSTYLE_CHECK",
         "TBSTYLE_CHECKGROUP",
         "TBSTYLE_DROPDOWN",
         "TBSTYLE_GROUP",
         "TBSTYLE_NOPREFIX",
         "TBSTYLE_SEP",
     }},
    {"tooltips_class32",
     {
         "TTS_ALWAYSTIP",
         "TTS_BALLOON",
         "TTS_CLOSE",
         "TTS_NOANIMATE",
         "TTS_NOFADE",
         "TTS_NOPREFIX",
         "TTS_USEVISUALSTYLE",
     }},
    {"systreeview32",
     {
         "TVS_CHECKBOXES",
         "TVS_DISABLEDRAGDROP",
         "TVS_EDITLABELS",
         "TVS_FULLROWSELECT",
         "TVS_HASBUTTONS",
         "TVS_HASLINES",
         "TVS_INFOTIP",
         "TVS_LINESATROOT",
         "TVS_NOHSCROLL",
         "TVS_NONEVENHEIGHT",
         "TVS_NOSCROLL",
         "TVS_NOTOOLTIPS",
         "TVS_RTLREADING",
         "TVS_SHOWSELALWAYS",
         "TVS_SINGLEEXPAND",
         "TVS_TRACKSELECT",
         // extended
         "TVS_EX_AUTOHSCROLL",
         "TVS_EX_DIMMEDCHECKBOXES",
         "TVS_EX_DOUBLEBUFFER",
         "TVS_EX_DRAWIMAGEASYNC",
         "TVS_EX_EXCLUSIONCHECKBOXES",
         "TVS_EX_FADEINOUTEXPANDOS",
         "TVS_EX_MULTISELECT",
         "TVS_EX_NOINDENTSTATE",
         "TVS_EX_NOSINGLECOLLAPSE",
         "TVS_EX_PARTIALCHECKBOXES",
         "TVS_EX_RICHTOOLTIP",
         // treeview items
         "TVIS_BOLD",
         "TVIS_CUT",
         "TVIS_DROPHILITED",
         "TVIS_EXPANDED",
         "TVIS_EXPANDEDONCE",
         "TVIS_EXPANDPARTIAL",
         "TVIS_SELECTED",
         "TVIS_OVERLAYMASK",
         "TVIS_STATEIMAGEMASK",
         "TVIS_USERMASK",
     }},
};

std::list<std::string> common_signal = {
    "NM_CHAR",
    "NM_CUSTOMTEXT",
    "NM_FONTCHANGED",
    "NM_GETCUSTOMSPLITRECT",
    "NM_HOVER",
    "NM_KEYDOWN",
    "NM_KILLFOCUS",
    "NM_LDOWN",
    "NM_NCHITTEST",
    "NM_OUTOFMEMORY",
    "NM_RDOWN",
    "NM_RELEASEDCAPTURE",
    "NM_RETURN",
    "NM_SETCURSOR",
    "NM_SETFOCUS",
    "NM_THEMECHANGED",
    "NM_TOOLTIPSCREATED",
    "NM_TVSTATEIMAGECHANGING",
};

static const std::map<std::string, std::list<std::string>> signal_map = {
    {"sysanimate32",
     {
         "ACN_START",
         "ACN_STOP",
     }},
    {"button",
     {
         "BCN_DROPDOWN",
         "BCN_HOTITEMCHANGE",
         "BN_CLICKED",
         "BN_DBLCLK",
         "BN_DISABLE",
         "BN_DOUBLECLICKED",
         "BN_HILITE",
         "BN_KILLFOCUS",
         "BN_PAINT",
         "BN_PUSHED",
         "BN_SETFOCUS",
         "BN_UNHILITE",
         "BN_UNPUSHED",
         "NM_CUSTOMDRAW",
         "WM_CTLCOLORBTN",
     }},
    {"combobox",
     {
         "CBN_CLOSEUP",
         "CBN_DBLCLK",
         "CBN_DROPDOWN",
         "CBN_EDITCHANGE",
         "CBN_EDITUPDATE",
         "CBN_ERRSPACE",
         "CBN_KILLFOCUS",
         "CBN_SELCHANGE",
         "CBN_SELENDCANCEL",
         "CBN_SELENDOK",
         "CBN_SETFOCUS",
         "WM_COMPAREITEM",
         "WM_DRAWITEM",
         "WM_MEASUREITEM",
         // extended

         "CBEN_BEGINEDIT",
         "CBEN_DELETEITEM",
         "CBEN_DRAGBEGIN",
         "CBEN_ENDEDIT",
         "CBEN_GETDISPINFO",
         "CBEN_INSERTITEM",
         "NM_SETCURSOR",
     }},
    {"sysdatetimepick32",
     {
         "DTN_CLOSEUP",
         "DTN_DATETIMECHANGE",
         "DTN_DROPDOWN",
         "DTN_FORMAT",
         "DTN_FORMATQUERY",
         "DTN_USERSTRING",
         "DTN_WMKEYDOWN",
         "NM_KILLFOCUS",
         "NM_SETFOCUS",
     }},
    {"edit",
     {
         "EN_ALIGN_LTR_EC",
         "EN_ALIGN_RTL_EC",
         "EN_CHANGE",
         "EN_ERRSPACE",
         "EN_HSCROLL",
         "EN_KILLFOCUS",
         "EN_MAXTEXT",
         "EN_SETFOCUS",
         "EN_UPDATE",
         "EN_VSCROLL",
         "WM_CTLCOLOREDIT",
     }},
    {"sysheader32",
     {
         "HDN_BEGINDRAG",      "HDN_BEGINFILTEREDIT", "HDN_BEGINTRACK",    "HDN_DIVIDERDBLCLICK",
         "HDN_DROPDOWN",       "HDN_ENDDRAG",         "HDN_ENDFILTEREDIT", "HDN_FILTERBTNCLICK",
         "HDN_FILTERCHANGE",   "HDN_GETDISPINFO",     "HDN_ITEMCHANGED",   "HDN_ITEMCHANGING",
         "HDN_ITEMCLICK",      "HDN_ITEMDBLCLICK",    "HDN_ITEMKEYDOWN",   "HDN_ITEMSTATEICONCLICK",
         "HDN_OVERFLOWCLICK",  "HDN_TRACK",           "NM_CUSTOMDRAW",     "NM_RCLICK",
         "NM_RELEASEDCAPTURE",
     }},
    {"listbox",
     {
         "DL_BEGINDRAG",
         "DL_CANCELDRAG",
         "DL_DRAGGING",
         "DL_DROPPED",
         "LBN_DBLCLK",
         "LBN_ERRSPACE",
         "LBN_KILLFOCUS",
         "LBN_SELCANCEL",
         "LBN_SELCHANGE",
         "LBN_SETFOCUS",
         "WM_CHARTOITEM",
         "WM_CTLCOLORLISTBOX",
         "WM_DELETEITEM",
         "WM_VKEYTOITEM",
     }},
    {"syslistview32",
     {
         "LVN_BEGINDRAG",
         "LVN_BEGINLABELEDIT",
         "LVN_BEGINRDRAG",
         "LVN_BEGINSCROLL",
         "LVN_COLUMNCLICK",
         "LVN_COLUMNDROPDOWN",
         "LVN_COLUMNOVERFLOWCLICK",
         "LVN_DELETEALLITEMS",
         "LVN_DELETEITEM",
         "LVN_ENDLABELEDIT",
         "LVN_ENDSCROLL",
         "LVN_GETDISPINFO",
         "LVN_GETEMPTYMARKUP",
         "LVN_GETINFOTIP",
         "LVN_HOTTRACK",
         "LVN_INCREMENTALSEARCH",
         "LVN_INSERTITEM",
         "LVN_ITEMACTIVATE",
         "LVN_ITEMCHANGED",
         "LVN_ITEMCHANGING",
         "LVN_KEYDOWN",
         "LVN_LINKCLICK",
         "LVN_MARQUEEBEGIN",
         "LVN_ODCACHEHINT",
         "LVN_ODFINDITEM",
         "LVN_ODSTATECHANGED",
         "LVN_SETDISPINFO",
         "NM_CLICK",
         "NM_CUSTOMDRAW",
         "NM_DBLCLK",
         "NM_HOVER",
         "NM_KILLFOCUS",
         "NM_RCLICK",
         "NM_RDBLCLK",
         "NM_RELEASEDCAPTURE",
         "NM_RETURN",
         "NM_SETFOCUS",
     }},
    {"sysmonthcal32",
     {
         "MCN_GETDAYSTATE",
         "MCN_SELCHANGE",
         "MCN_SELECT",
         "MCN_VIEWCHANGE",
         "NM_RELEASEDCAPTURE",
     }},
    {"syspager",
     {
         "NM_RELEASEDCAPTURE",
         "PGN_CALCSIZE",
         "PGN_HOTITEMCHANGE",
         "PGN_SCROLL",
     }},
    {"rebarwindow32",
     {
         "NM_CUSTOMDRAW",
         "NM_NCHITTEST",
         "NM_RELEASEDCAPTURE",
         "RBN_AUTOBREAK",
         "RBN_AUTOSIZE",
         "RBN_BEGINDRAG",
         "RBN_CHEVRONPUSHED",
         "RBN_CHILDSIZE",
         "RBN_DELETEDBAND",
         "RBN_DELETINGBAND",
         "RBN_ENDDRAG",
         "RBN_GETOBJECT",
         "RBN_HEIGHTCHANGE",
         "RBN_LAYOUTCHANGED",
         "RBN_MINMAX",
         "RBN_SPLITTERDRAG",
     }},
    {"richedit",
     {
         "EN_CHANGE",       "EN_ALIGNLTR",      "EN_ALIGNRTL",        "EN_CLIPFORMAT",  "EN_CORRECTTEXT",
         "EN_DRAGDROPDONE", "EN_DROPFILES",     "EN_ENDCOMPOSITION",  "EN_IMECHANGE",   "EN_LINK",
         "EN_LOWFIRTF",     "EN_MSGFILTER",     "EN_OBJECTPOSITIONS", "EN_OLEOPFAILED", "EN_PARAGRAPHEXPANDED",
         "EN_PROTECTED",    "EN_REQUESTRESIZE", "EN_SAVECLIPBOARD",   "EN_SELCHANGE",   "EN_STARTCOMPOSITION",
         "EN_STOPNOUNDO",
     }},
    {"scrollbar",
     {
         "WM_CTLCOLORSCROLLBAR",
         "WM_HSCROLL",
         "WM_VSCROLL",
     }},
    {"static",
     {
         "STN_CLICKED",
         "STN_DBLCLK",
         "STN_DISABLE",
         "STN_ENABLE",
         "WM_CTLCOLORSTATIC",
     }},
    {"msctls_statusbar32",
     {
         "NM_CLICK",
         "NM_DBLCLK",
         "NM_RCLICK",
         "NM_RDBLCLK",
         "SBN_SIMPLEMODECHANGE",
     }},
    {"syslink", {"NM_CLICK"}},
    {"systabcontrol32",
     {
         "NM_CLICK",
         "NM_DBLCLK",
         "NM_RCLICK",
         "NM_RDBLCLK",
         "NM_RELEASEDCAPTURE",
         "TCN_FOCUSCHANGE",
         "TCN_GETOBJECT",
         "TCN_KEYDOWN",
         "TCN_SELCHANGE",
         "TCN_SELCHANGING",
     }},
    {"toolbarwindow32",
     {
         "NM_CHAR",           "NM_CLICK",           "NM_CUSTOMDRAW",      "NM_DBLCLK",           "NM_KEYDOWN",
         "NM_LDOWN",          "NM_RCLICK",          "NM_RDBLCLK",         "NM_RELEASEDCAPTURE",  "NM_TOOLTIPSCREATED",
         "TBN_BEGINADJUST",   "TBN_BEGINDRAG",      "TBN_CUSTHELP",       "TBN_DELETINGBUTTON",  "TBN_DRAGOUT",
         "TBN_DRAGOVER",      "TBN_DROPDOWN",       "TBN_DUPACCELERATOR", "TBN_ENDADJUST",       "TBN_ENDDRAG",
         "TBN_GETBUTTONINFO", "TBN_GETDISPINFO",    "TBN_GETINFOTIP",     "TBN_GETOBJECT",       "TBN_HOTITEMCHANGE",
         "TBN_INITCUSTOMIZE", "TBN_MAPACCELERATOR", "TBN_QUERYDELETE",    "TBN_QUERYINSERT",     "TBN_RESET",
         "TBN_RESTORE",       "TBN_SAVE",           "TBN_TOOLBARCHANGE",  "TBN_WRAPACCELERATOR", "TBN_WRAPHOTITEM",
     }},
    {"tooltips_class32",
     {
         "NM_CUSTOMDRAW",
         "TTN_GETDISPINFO",
         "TTN_LINKCLICK",
         "TTN_NEEDTEXT",
         "TTN_POP",
         "TTN_SHOW",
     }},
};

static const std::string rules_json =
R"json(
{
  "definition_types" : [ "resources", "controls", "statements", "attributes", "type_definition" ],
  "attributes" : [ "FIXED", "MOVEABLE", "DISCARDABLE", "PURE", "IMPURE", "SHARED", "NONSHARED" ],

  "type_definition" :
  {
    "BOOLEAN" :     { "choice" : { "literal" : "TRUE", "literal" : "FALSE" } },
    "text" :        { "type" : "string",    },
    "x" :           { "type" : "int16_t",   },
    "y" :           { "type" : "int16_t",   },
    "width" :       { "type" : "uint16_t",  },
    "height" :      { "type" : "uint16_t",  },
    "filename" :    { "type" : "string",    },
    "idvalue" :     { "type" : "uint16_t"   },
    "pointsize" :   { "type" : "uint16_t"   },
    "typeface" :    { "type" : "string"     },
    "weight" :      { "type" : "uint16_t"   },
    "italic" :      { "type" : "BOOLEAN"    },
    "charset" :     { "type" : "textual_id" },
    "WORD" :        { "type" : "uint16_t"   },
    "DWORD" :
    {
      "type" : "uint32_t",
      "option" : { "literal" : "L" }
    },
    "version" :
    {
      "type" : "WORD",
      "type" : "WORD",
      "type" : "WORD",
      "type" : "WORD"
    },
    "widestring" :
    {
      "literal" : "L",
      "type" : "string"
    },
    "variant" :
    {
      "choice" :
      {
        "type" : "WORD",
        "type" : "DWORD",
        "type" : "string",
        "type" : "widestring"
      }
    },
    "resource_id" :
    {
      "choice" :
      {
        "type" : "uint16_t",
        "type" : "textual_id"
      }
    },
    "language" :    { "type" : "resource_id" },
    "sublanguage" : { "type" : "resource_id" },
    "class_id" :
    {
      "choice" :
      {
        "type" : "textual_id",
        "literal" : "BUTTON",
        "literal" : "STATIC",
        "literal" : "EDIT",
        "literal" : "LISTBOX",
        "literal" : "SCROLLBAR",
        "literal" : "COMBOBOX"
      }
    },
    "idvalue_type" :
    {
      "choice" :
      {
        "literal" : "ASCII",
        "literal" : "VIRTKEY"
      }
    },
    "event" :
    {
      "choice" :
      {
        "type" : "escaped_quoted_char",
        "type" : "uint8_t",
        "type" : "quoted_char"
      }
    },
    "quoted_char" :
    {
      "literal" : "\"",
      "type" : "char8_t",
      "literal" : "\""
    },
    "escaped_quoted_char" :
    {
      "literal" : "^\"",
      "type" : "char8_t",
      "literal" : "\""
    },
    "accelerator_type" :
    {
      "type" : "event",
      "type" : "idvalue",
      "option" : { "type" : "idvalue_type" },
      "option" : { "literal" : "NOINVERT" },
      "option" : { "literal" : "ALT" },
      "option" : { "literal" : "SHIFT" },
      "option" : { "literal" : "CONTROL" }
    },
    "standard_options" :
    {
      "option" : { "statements" : [ "CHARACTERISTICS", "LANGUAGE", "VERSION" ] }
    },
    "string_table_entry" :
    {
      "type" : "resource_id",
      "choice" :
      {
        "type" : "string",
        "type" : "widestring"
      }
    },
    "bitwise_value" :
    {
      "multi" :
      {
        "type" : "resource_id",
        "option" : { "literal" : "|" },
        "option" : { "literal" : "&" },
        "option" : { "literal" : "+" },
        "option" : { "literal" : "-" },
        "option" : { "literal" : "*" },
        "option" : { "literal" : "/" }
      }
    },
    "style" : { "type" : "bitwise_value" },
    "extended_style" : { "option" : { "type" : "bitwise_value" } },
    "style_options" :
    {
      "option" :
      {
        "type" : "style",
        "type" : "extended_style"
      }
    },
    "control_options" :
    {
      "type" : "resource_id",
      "type" : "x",
      "type" : "y",
      "type" : "width",
      "type" : "height",
      "type" : "style_options"
    },
    "toolbar_entry" :
    {
      "choice" :
      [
        { "keyword" : "SEPARATOR" },
        { "keyword" : "BUTTON", "type" : "resource_id" }
      ],
    }

  },
  "resources" :
  {
    "ACCELERATORS" :
    {
      "type" : "resource_id",
      "keyword" : "ACCELERATORS",
      "type" : "standard_options",
      "container" : { "type" : "accelerator_type" }
    },
    "BITMAP" :
    {
      "type" : "resource_id" ,
      "keyword" : "BITMAP",
      "type" : "filename"
    },
    "CURSOR" :
    {
      "type" : "resource_id" ,
      "keyword" : "CURSOR",
      "type" : "filename"
    },
    "DIALOG" :
    {
      "type" : "resource_id",
      "keyword" : "DIALOG",
      "type" : "x",
      "type" : "y",
      "type" : "width",
      "type" : "height",
      "option" : { "statements" : [ "CAPTION", "CHARACTERISTICS", "CLASS", "EXSTYLE", "FONT", "LANGUAGE", "MENU", "STYLE", "VERSION" ] },
      "container" : { "controls" : [ "AUTO3STATE", "AUTOCHECKBOX", "AUTORADIOBUTTON", "CHECKBOX", "COMBOBOX", "CONTROL", "CTEXT",
                                     "DEFPUSHBUTTON", "EDITTEXT", "GROUPBOX", "ICON", "LISTBOX", "LTEXT", "PUSHBOX", "PUSHBUTTON",
                                     "RADIOBUTTON", "RTEXT", "SCROLLBAR", "STATE3" ] }
    },
    "DIALOGEX" :
    {
      "type" : "resource_id",
      "keyword" : "DIALOGEX",
      "type" : "x",
      "type" : "y",
      "type" : "width",
      "type" : "height",
      "option" : { "type" : "resource_id" },
      "option" : { "statements" : [ "CAPTION", "CHARACTERISTICS", "CLASS", "EXSTYLE", "FONT", "LANGUAGE", "MENU", "STYLE", "VERSION" ] },
      "container" : { "controls" : [ "AUTO3STATE", "AUTOCHECKBOX", "AUTORADIOBUTTON", "CHECKBOX", "COMBOBOX", "CONTROL", "CTEXT",
                                     "DEFPUSHBUTTON", "EDITTEXT", "GROUPBOX", "ICON", "LISTBOX", "LTEXT", "PUSHBOX", "PUSHBUTTON",
                                     "RADIOBUTTON", "RTEXT", "SCROLLBAR", "STATE3" ] }
    },
    "FONT" :
    {
      "type" : "resource_id" ,
      "keyword" : "FONT",
      "type" : "filename"
    },
    "HTML" :
    {
      "type" : "resource_id" ,
      "keyword" : "HTML",
      "type" : "filename"
    },
    "ICON" :
    {
      "type" : "resource_id" ,
      "keyword" : "ICON",
      "type" : "filename"
    },
    "MENU" :
    {
      "type" : "resource_id" ,
      "keyword" : "MENU",
      "type" : "standard_options",
      "container" : { "statements" : [ "MENUITEM", "POPUP" ] }
    },
    "MESSAGETABLE" :
    {
      "type" : "resource_id" ,
      "keyword" : "MESSAGETABLE",
      "type" : "filename"
    },
    "POPUP" :
    {
      "keyword" : "POPUP",
      "type" : "text",
      "option" : { "literal" : "CHECKED" },
      "option" : { "literal" : "GRAYED" },
      "option" : { "literal" : "HELP" },
      "option" : { "literal" : "INACTIVE" },
      "option" : { "literal" : "MENUBARBREAK" },
      "option" : { "literal" : "MENUBREAK" },
      "container" : { "statements" : [ "MENUITEM", "POPUP" ] }
    },
    "RCDATA" :
    {
      "type" : "resource_id" ,
      "keyword" : "RCDATA",
      "type" : "standard_options",
      "container" : { "type" : "variant" }
    },
    "STRINGTABLE" :
    {
      "type" : "resource_id" ,
      "keyword" : "STRINGTABLE",
      "type" : "standard_options",
      "container" : { "type" : "string_table_entry" }
    },
    "TOOLBAR" :
    {
      "type" : "resource_id" ,
      "keyword" : "TOOLBAR",
      "type" : "x",
      "type" : "y",
      "container" : { "type" : "toolbar_entry" }
    },
    "VERSIONINFO" :
    {
      "type" : "resource_id" ,
      "keyword" : "VERSIONINFO",
      "option" : { "keyword" : "FILEVERSION",     "type" : "version" },
      "option" : { "keyword" : "PRODUCTVERSION",  "type" : "version" },
      "option" : { "keyword" : "FILEFLAGSMASK",   "type" : "DWORD" },
      "option" : { "keyword" : "FILEFLAGS",       "type" : "DWORD" },
      "option" : { "keyword" : "FILEOS",          "type" : "DWORD" },
      "option" : { "keyword" : "FILETYPE",        "type" : "DWORD" },
      "option" : { "keyword" : "FILESUBTYPE",     "type" : "DWORD" }
    }
  },

  "controls" :
  {
    "AUTO3STATE" :
    {
      "keyword" : "AUTO3STATE",
      "type" : "text",
      "type" : "control_options"
    },
    "AUTOCHECKBOX" :
    {
      "keyword" : "AUTORADIOBUTTON",
      "type" : "text",
      "type" : "control_options"
    },
    "AUTORADIOBUTTON" :
    {
      "keyword": "AUTORADIOBUTTON",
      "type" : "text",
      "type" : "control_options"
    },
    "CHECKBOX" :
    {
      "keyword": "CHECKBOX",
      "type" : "text",
      "type" : "control_options"
    },
    "COMBOBOX" :
    {
      "keyword": "COMBOBOX",
      "type" : "control_options"
    },
    "CONTROL" :
    {
      "keyword": "CONTROL",
      "type" : "text",
      "type" : "resource_id",
      "type" : "class",
      "type" : "style",
      "type" : "x",
      "type" : "y",
      "type" : "width",
      "type" : "height",
      "type" : "extended_style"
    },
    "CTEXT" :
    {
      "keyword": "CTEXT",
      "type" : "text",
      "type" : "control_options"
    },
    "DEFPUSHBUTTON" :
    {
      "keyword": "DEFPUSHBUTTON",
      "type" : "text",
      "type" : "control_options"
    },
    "EDITTEXT" :
    {
      "keyword": "EDITTEXT",
      "type" : "control_options"
    },
    "GROUPBOX" :
    {
      "keyword": "GROUPBOX",
      "type" : "text",
      "type" : "control_options"
    },
    "ICON" :
    {
      "keyword": "ICON",
      "type" : "text",
      "type" : "control_options"
    },
    "LISTBOX" :
    {
      "keyword": "LISTBOX",
      "type" : "control_options"
    },
    "LTEXT" :
    {
      "keyword": "LTEXT",
      "type" : "text",
      "type" : "control_options"
    },
    "PUSHBOX" :
    {
      "keyword": "PUSHBOX",
      "type" : "text",
      "type" : "control_options"
    },
    "PUSHBUTTON" :
    {
      "keyword": "PUSHBUTTON",
      "type" : "text",
      "type" : "control_options"
    },
    "RADIOBUTTON" :
    {
      "keyword": "RADIOBUTTON",
      "type" : "text",
      "type" : "control_options"
    },
    "RTEXT" :
    {
      "keyword": "RTEXT",
      "type" : "text",
      "type" : "control_options"
    },
    "SCROLLBAR" :
    {
      "keyword": "SCROLLBAR",
      "type" : "control_options"
    },
    "STATE3" :
    {
      "keyword": "STATE3",
      "type" : "text",
      "type" : "control_options"
    }
  },

  "statements" :
  {
    "CAPTION" :
    {
      "keyword" : "CAPTION",
      "type" : "text"
    },
    "CHARACTERISTICS" :
    {
      "keyword" : "CHARACTERISTICS",
      "type" : "DWORD"
    },
    "CLASS" :
    {
      "keyword" : "CLASS",
      "type" : "class_id"
    },
    "EXSTYLE" :
    {
      "keyword" : "EXSTYLE",
      "type" : "bitwise_value"
    },
    "FONT" :
    {
      "keyword" : "FONT",
      "type" : "pointsize",
      "type" : "typeface",
      "option" :
      {
        "type" : "weight",
        "option" :
        {
          "type" : "italic",
          "option" : { "type" : "charset" }
        }
      }
    },
    "LANGUAGE" :
    {
      "keyword" : "LANGUAGE",
      "type" : "language",
      "type" : "sublanguage"
    },
    "MENU" :
    {
      "keyword" : "MENU",
      "type" : "resource_id"
    },
    "MENUITEM" :
    {
      "keyword" : "MENUITEM",
      "literal" : "SEPARATOR"
    },
    "MENUITEM" :
    {
      "keyword" : "MENUITEM",
      "type" : "text",
      "type" : "resource_id",
      "option" : { "literal" : "CHECKED" },
      "option" : { "literal" : "GRAYED" },
      "option" : { "literal" : "HELP" },
      "option" : { "literal" : "INACTIVE" },
      "option" : { "literal" : "MENUBARBREAK" },
      "option" : { "literal" : "MENUBREAK" }
    },
    "STYLE" :
    {
      "keyword" : "STYLE",
      "type" : "bitwise_value"
    },
    "VERSION" :
    {
      "keyword" : "VERSION",
      "type" : "DWORD"
    }
  }
}
)json";

#endif // RCDEF_H
