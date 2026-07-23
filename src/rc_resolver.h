#ifndef RC_RESOLVER_H
#define RC_RESOLVER_H

#include <cstdint>
#include <string>
#include <vector>

namespace rc
{

struct style_expr;
struct rc_file;
struct dialog_data;
struct control;

class resolver
{
public:
  int64_t resolve_style(const style_expr& expr) const;
  std::string format_style(int64_t value) const;

  std::string resolve_name(const std::string& name) const;
  std::string format_value(int64_t value) const;

  void resolve_file(rc_file& file) const;

private:
  void resolve_dialog(dialog_data& dd) const;
  void resolve_control(control& c) const;
};

}

#endif
