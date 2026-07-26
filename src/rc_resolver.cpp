#include "rc_resolver.h"
#include "rc_ast.h"
#include "rc_constants.h"

#include <algorithm>
#include <sstream>

namespace rc
{

int64_t resolver::resolve_style(const style_expr& expr) const
{
  const auto& reg = constant_registry::instance();

  if(expr.first.empty() && expr.ops.empty())
    return -1;

  int64_t result = 0;
  if(!expr.first.empty())
  {
    int64_t val = reg.resolve(expr.first);
    if(val >= 0)
      result = val;
    else
      return -1;
  }

  for(const auto& [op, name] : expr.ops)
  {
    int64_t val = reg.resolve(name);
    if(val < 0)
      return -1;

    if(op == "|")
      result |= val;
    else if(op == "+")
      result += val;
  }

  return result;
}

std::string resolver::format_value(int64_t value) const
{
  const auto& reg = constant_registry::instance();
  std::string name = reg.resolve(value);
  if(!name.empty())
    return name;

  std::ostringstream oss;
  oss << "0x" << std::hex << value;
  return oss.str();
}

void resolver::resolve_file(rc_file& file) const
{
  for(auto& res : file.resources)
  {
    if(std::holds_alternative<dialog_data>(res.data))
      resolve_dialog(std::get<dialog_data>(res.data));
  }
}

void resolver::resolve_dialog(dialog_data& dd) const
{
  for(auto& s : dd.statements)
  {
    if(s.keyword == "STYLE" || s.keyword == "EXSTYLE" || s.keyword == "VERSION" || s.keyword == "CHARACTERISTICS")
      s.value.resolved_value = resolve_style(s.value);
  }

  for(auto& c : dd.controls)
    resolve_control(c);
}

void resolver::resolve_control(control& c) const
{
  c.style.resolved_value = resolve_style(c.style);
  c.ext_style.resolved_value = resolve_style(c.ext_style);
}

}
