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

std::string resolver::format_style(int64_t value) const
{
  const auto& reg = constant_registry::instance();
  return reg.resolve(value);
}

std::string resolver::resolve_name(const std::string& name) const
{
  const auto& reg = constant_registry::instance();
  int64_t val = reg.resolve(name);
  if(val >= 0)
    return format_value(val);
  return name;
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

}
