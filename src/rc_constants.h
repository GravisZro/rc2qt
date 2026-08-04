#ifndef RC_CONSTANTS_H
#define RC_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

#include "utils.h"

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
  bad_category          = 0,
  window_style          = 0x00000001, // WS_*
  extended_style        = 0x00000002, // WS_EX_*
  dialog_style          = 0x00000004, // DS_*
  button_style          = 0x00000008, // BS_*
  edit_style            = 0x00000010, // ES_*
  static_style          = 0x00000020, // SS_*
  listbox_style         = 0x00000040, // LBS_*
  combobox_style        = 0x00000080, // CBS_*
  scrollbar_style       = 0x00000100, // SBS_*
  listview_style        = 0x00000200, // LVS_*
  treeview_style        = 0x00000400, // TVS_*
  trackbar_style        = 0x00000800, // TBS_*
  progressbar_style     = 0x00001000, // PBS_*
  updown_style          = 0x00002000, // UDS_*
  datetime_style        = 0x00004000, // DTS_*
  tabcontrol_style      = 0x00008000, // TCS_*
  window_message        = 0x00010000, // WM_*
  virtual_key           = 0x00020000, // VK_*
  message_box           = 0x00040000, // MB_*
  menu_flag             = 0x00080000, // MF_*, MFT_*, MFS_*
  accelerator_flag      = 0x00100000, // FVIRTKEY, FALT, etc.
  resource_type         = 0x00200000, // RT_*
  control_message       = 0x00400000, // CB_*, LB_*, EM_*, BM_*, STM_*, SBM_*, DM_*
  notification          = 0x00800000, // CBN_*, LBN_*, EN_*, BN_*, STN_*
  dialog_id             = 0x01000000, // IDOK, IDCANCEL, etc.
  system_id             = 0x02000000, // IDC_STATIC, etc.
  control_id            = 0x04000000, // AFX_IDC_* control identifiers
  mfc_dialog_id         = 0x08000000, // AFX_IDD_*, IDD_* dialog template IDs
  mfc_string_id         = 0x10000000, // AFX_IDS_* string IDs
  mfc_bitmap_id         = 0x20000000, // AFX_IDB_*, IDB_* bitmap IDs
  mfc_icon_id           = 0x40000000, // AFX_IDI_* icon IDs
  mfc_prompt_id         = 0x80000000, // AFX_IDP_* prompt IDs
  oem_bitmap            = 0x00000001ULL << 32, // OBM_* OEM bitmap IDs
  system_resource_id    = 0x00000002ULL << 32, // ID_* system resource IDs
  mfc_cursor_id         = 0x00000004ULL << 32, // AFX_IDC_* cursor IDs
  mfc_accel_id          = 0x00000008ULL << 32, // AFX_IDR_* accelerator table IDs
  header_style          = 0x00000010ULL << 32, // HDS_* header control styles
  common_control_style  = 0x00000020ULL << 32, // CCS_* common control styles
  listview_ex_style     = 0x00000040ULL << 32, // LVS_EX_* extended list-view styles
  edit_ex_style         = 0x00000080ULL << 32, // ES_EX_* extended edit control styles
  treeview_ex_style     = 0x00000100ULL << 32, // TVS_EX_* extended tree-view styles
  month_calendar_style  = 0x00000200ULL << 32, // MCS_* month calendar styles
  pager_style           = 0x00000400ULL << 32, // PGS_* pager control styles
  rebar_style           = 0x00000800ULL << 32, // RBS_* rebar control styles
  tooltip_style         = 0x00001000ULL << 32, // TTS_* tooltip control styles
  toolbar_style         = 0x00002000ULL << 32, // TBSTYLE_*, BTNS_* toolbar styles
  animate_style         = 0x00004000ULL << 32, // ACS_* animation control styles
  statusbar_style       = 0x00008000ULL << 32, // SBARS_*, SBT_* status bar styles
  tabcontrol_ex_style   = 0x00010000ULL << 32, // TCS_EX_* extended tab control styles
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
