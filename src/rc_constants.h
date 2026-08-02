#ifndef RC_CONSTANTS_H
#define RC_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

#include "rc_helpers.h"

namespace rc
{

/* Reference: CSV data for these constants was originally sourced from
   Microsoft Windows SDK headers and documentation. See rc_constants.cpp
   for the actual registrations with descriptions.
*/


/* Reference information for MFC/AFX resource ID prefixes:
   AFX_ — Application Framework eXtension resource or symbol identifier
   IDP_ — Prompt resource identifier used for message box strings and error prompts
   IDC_ — Control resource identifier
   IDB_ — Bitmap resource identifier
   IDI_ — Icon resource identifier
   IDD_ — Dialog template identifier
   IDM_ — Menu command identifier
   IDR_ — Generic resource identifier
*/

enum class category_t : uint64_t
{
  bad_category = 0,
  window_style = 1ULL << 0,       // WS_*
  extended_style = 1ULL << 1,     // WS_EX_*
  dialog_style = 1ULL << 2,       // DS_*
  button_style = 1ULL << 3,       // BS_*
  edit_style = 1ULL << 4,         // ES_*
  static_style = 1ULL << 5,       // SS_*
  listbox_style = 1ULL << 6,      // LBS_*
  combobox_style = 1ULL << 7,     // CBS_*
  scrollbar_style = 1ULL << 8,    // SBS_*
  listview_style = 1ULL << 9,     // LVS_*
  treeview_style = 1ULL << 10,    // TVS_*
  trackbar_style = 1ULL << 11,    // TBS_*
  progressbar_style = 1ULL << 12, // PBS_*
  updown_style = 1ULL << 13,      // UDS_*
  datetimepicker_style = 1ULL << 14, // DTS_*
  tabcontrol_style = 1ULL << 15,  // TCS_*
  window_message = 1ULL << 16,    // WM_*
  virtual_key = 1ULL << 17,       // VK_*
  message_box = 1ULL << 18,       // MB_*
  menu_flag = 1ULL << 19,         // MF_*, MFT_*, MFS_*
  accelerator_flag = 1ULL << 20,  // FVIRTKEY, FALT, etc.
  resource_type = 1ULL << 21,     // RT_*
  control_message = 1ULL << 22,   // CB_*, LB_*, EM_*, BM_*, STM_*, SBM_*, DM_*
  notification = 1ULL << 23,      // CBN_*, LBN_*, EN_*, BN_*, STN_*
  dialog_id = 1ULL << 24,         // IDOK, IDCANCEL, etc.
  system_id = 1ULL << 25,         // IDC_STATIC, etc.
  control_id = 1ULL << 26,        // AFX_IDC_* control identifiers
  mfc_dialog_id = 1ULL << 27,     // AFX_IDD_*, IDD_* dialog template IDs
  mfc_string_id = 1ULL << 28,     // AFX_IDS_* string IDs
  mfc_bitmap_id = 1ULL << 29,     // AFX_IDB_*, IDB_* bitmap IDs
  mfc_icon_id = 1ULL << 30,       // AFX_IDI_* icon IDs
  mfc_prompt_id = 1ULL << 31,     // AFX_IDP_* prompt IDs
  oem_bitmap = 1ULL << 32,        // OBM_* OEM bitmap IDs
  system_resource_id = 1ULL << 33, // ID_* system resource IDs
  mfc_cursor_id = 1ULL << 34,     // AFX_IDC_* cursor IDs
  mfc_accel_id = 1ULL << 35,      // AFX_IDR_* accelerator table IDs
};

struct constant_entry
{
  std::string name;
  int64_t value;
  category_t category;
  std::string description;
};

class constant_registry
{
public:
  static constant_registry& instance();

  void add(category_t cat,
           int64_t value,
           const std::string& name,
           const std::string& desc = "");

  bool has_name(const std::string& name) const;

  static rc::category_t resolve_category(const std::string& name);
  int64_t resolve(const std::string& name) const;
  std::string resolve(category_t cat, int64_t value) const;

  std::vector<constant_entry> entries_by_category(category_t cat) const;
  std::vector<constant_entry> all_entries() const;

  size_t size() const;

private:
  constant_registry();
  void register_all();

  struct catval_t
  {
    category_t cat = category_t::bad_category;
    int64_t value = 0;

    bool operator <(const catval_t& other) const
    {
      return cat < other.cat ||
             (cat == other.cat && value < other.value);
    }
  };

  std::unordered_map<std::string, int64_t> m_name_to_value;
  std::map<catval_t, std::string> m_value_to_name;
  std::vector<constant_entry> m_entries;
};

}

#endif
