#ifndef RC_CONSTANTS_H
#define RC_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace rc
{

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
