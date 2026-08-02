#ifndef RC_CONSTANTS_H
#define RC_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

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

enum class category_t : uint32_t
{
  bad_category = 0,
  window_style,       // WS_*
  extended_style,     // WS_EX_*
  dialog_style,       // DS_*
  button_style,       // BS_*
  edit_style,         // ES_*
  static_style,       // SS_*
  listbox_style,      // LBS_*
  combobox_style,     // CBS_*
  scrollbar_style,    // SBS_*
  listview_style,     // LVS_*
  treeview_style,     // TVS_*
  trackbar_style,     // TBS_*
  progressbar_style,  // PBS_*
  updown_style,       // UDS_*
  datetimepicker_style, // DTS_*
  tabcontrol_style,   // TCS_*
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
  control_id,         // AFX_IDC_* control identifiers
  mfc_dialog_id,      // AFX_IDD_*, IDD_* dialog template IDs
  mfc_string_id,      // AFX_IDS_* string IDs
  mfc_bitmap_id,      // AFX_IDB_*, IDB_* bitmap IDs
  mfc_icon_id,        // AFX_IDI_* icon IDs
  mfc_prompt_id,      // AFX_IDP_* prompt IDs
  oem_bitmap,         // OBM_* OEM bitmap IDs
  system_resource_id, // ID_* system resource IDs
  mfc_cursor_id,      // AFX_IDC_* cursor IDs
  mfc_accel_id,       // AFX_IDR_* accelerator table IDs
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
    category_t cat = category_t::window_style;
    int64_t value = 0;
    bool operator ==(const catval_t& other) const
      { return int(cat) == int(other.cat) && value == other.value; }

    bool operator <(const catval_t& other) const
    {
      return int(cat) < int(other.cat) ||
             value < other.value;
    }
  };

  std::unordered_map<std::string, int64_t> m_name_to_value;
  std::map<catval_t, std::string> m_value_to_name;
  std::vector<constant_entry> m_entries;
};

}

#endif
