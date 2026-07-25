#include "rc_constants.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace rc;

struct category_info
{
  constant_category cat;
  const char* heading;
  const char* prefix_filter;
};

static const std::vector<category_info> ordered_categories =
{
  { constant_category::window_style,      "WS_* Window Styles",              "WS_" },
  { constant_category::extended_style,    "WS_EX_* Extended Window Styles",  "WS_EX_" },
  { constant_category::dialog_style,      "DS_* Dialog Styles",              "DS_" },
  { constant_category::button_style,      "BS_* Button Styles",              "BS_" },
  { constant_category::edit_style,        "ES_* Edit Control Styles",        "ES_" },
  { constant_category::static_style,      "SS_* Static Control Styles",      "SS_" },
  { constant_category::listbox_style,     "LBS_* List Box Styles",           "LBS_" },
  { constant_category::combobox_style,    "CBS_* Combo Box Styles",          "CBS_" },
  { constant_category::scrollbar_style,   "SBS_* Scroll Bar Styles",         "SBS_" },
  { constant_category::window_message,    "WM_* Window Messages",            "WM_" },
  { constant_category::virtual_key,       "VK_* Virtual Key Codes",          "VK_" },
  { constant_category::message_box,       "MB_* Message Box Flags",          "MB_" },
  { constant_category::menu_flag,         "MF_*/MFT_*/MFS_* Menu Flags",    "MF" },
  { constant_category::accelerator_flag,  "F_* Accelerator Key Flags",       "F_" },
  { constant_category::resource_type,     "RT_* Resource Types",             "RT_" },
  { constant_category::dialog_id,         "Dialog Button Identifiers",       "ID" },
  { constant_category::system_id,         "System Control Identifiers",      "IDC_" },
  { constant_category::control_id,        "AFX_IDC_* Control Identifiers",   "AFX_IDC_" },
  { constant_category::mfc_dialog_id,     "AFX_IDD_* Dialog Template IDs",   "AFX_IDD_" },
  { constant_category::mfc_string_id,     "AFX_IDS_* String IDs",           "AFX_IDS_" },
  { constant_category::mfc_bitmap_id,     "AFX_IDB_*/IDB_* Bitmap IDs",     "IDB_" },
  { constant_category::mfc_icon_id,       "AFX_IDI_*/IDI_* Icon IDs",       "IDI_" },
  { constant_category::mfc_prompt_id,     "AFX_IDP_* Prompt IDs",           "AFX_IDP_" },
  { constant_category::oem_bitmap,        "OBM_* OEM Bitmap IDs",           "OBM_" },
  { constant_category::system_resource_id,"ID_* System Resource IDs",        "ID_" },
  { constant_category::mfc_cursor_id,     "AFX_IDC_* Cursor IDs",           "AFX_IDC_" },
  { constant_category::mfc_accel_id,      "AFX_IDR_* Accelerator Table IDs", "AFX_IDR_" },
};

static std::string format_value(int64_t v)
{
  if(v < 0)
  {
    if(v >= -0x7FFFFFFF)
      return "(-" + format_value(-v) + ")";
    return "(" + std::to_string(v) + ")";
  }
  if(v >= 16)
  {
    std::ostringstream oss;
    oss << "0x" << std::hex << v;
    return oss.str();
  }
  return std::to_string(v);
}

static std::string clean_description(const std::string& desc)
{
  if(desc.empty())
    return "";
  std::string s = desc;
  while(!s.empty() && s.back() == '"')
    s.pop_back();
  return s;
}

int main()
{
  auto& reg = constant_registry::instance();
  auto all = reg.all_entries();

  std::map<constant_category, std::vector<constant_entry>> grouped;
  for(const auto& e : all)
    grouped[e.category].push_back(e);

  std::ofstream out("docs/menurc/predefined-constants.md");

  out << "# Predefined Constants (WinUser.h / CommCtrl.h)\n"
      << "\n"
      << "Numeric constants defined in the Windows SDK headers `WinUser.h` and `CommCtrl.h` can be used interchangeably with their integer values anywhere in a resource script (.rc) file. For example, `VK_RETURN` and `13` are equivalent in an accelerator table, and `WS_CHILD` and `0x40000000` are equivalent in a STYLE statement.\n"
      << "\n"
      << "The resource compiler resolves these symbols at compile time via `#include <windows.h>` (which includes both headers).\n"
      << "\n"
      << "## Naming Conventions\n"
      << "\n"
      << "Constants follow a prefix-based naming pattern organized by subsystem:\n"
      << "\n"
      << "| Prefix | Subsystem | Purpose |\n"
      << "|--------|-----------|---------|\n"
      << "| CB\\_ | Combo Box | Window styles |\n"
      << "| LBS\\_ | List Box | Window styles |\n"
      << "| ES\\_ | Edit Control | Window styles |\n"
      << "| BS\\_ | Button | Window styles |\n"
      << "| SS\\_ | Static Control | Window styles |\n"
      << "| SBS\\_ | Scroll Bar | Window styles |\n"
      << "| DS\\_ | Dialog | Window styles |\n"
      << "| WS\\_ | Window | General window styles |\n"
      << "| WS\\_EX\\_ | Window | Extended window styles |\n"
      << "| WM\\_ | Window | Messages |\n"
      << "| VK\\_ | Keyboard | Virtual key codes |\n"
      << "| MB\\_ | Message Box | Flags |\n"
      << "| ID | Dialog | Button identifiers |\n"
      << "\n";

  size_t total_rows = 0;

  for(const auto& ci : ordered_categories)
  {
    auto it = grouped.find(ci.cat);
    if(it == grouped.end())
      continue;
    const auto& entries = it->second;
    if(entries.empty())
      continue;

    out << "## " << ci.heading << "\n"
        << "\n"
        << "| Constant | Value | Description |\n"
        << "|----------|-------|-------------|\n";

    for(const auto& e : entries)
    {
      out << "| " << e.name
          << " | " << format_value(e.value)
          << " | " << clean_description(e.description)
          << " |\n";
      ++total_rows;
    }
    out << "\n";
  }

  out.close();
  std::cout << "Generated docs/menurc/predefined-constants.md\n";
  std::cout << "Total constant rows: " << total_rows << "\n";
  return 0;
}
