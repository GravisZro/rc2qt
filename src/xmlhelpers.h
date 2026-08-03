#pragma once
#include <type_traits>
#include <limits>
#include <string>
#include <cstdint>
#include <pugixml.hpp>
#include <exception>
#include <format>

#include "utils.h"

namespace xml
{
  inline void set_attr(pugi::xml_node node, const char* const name, float value, int precision = pugi::default_float_precision)
    { node.append_attribute(name).set_value(value, precision); }

  template<std::integral T>
  inline void set_attr(pugi::xml_node node, const char* const name, T value)
    { node.append_attribute(name).set_value(value); }

  inline void set_attr(pugi::xml_node node, const char* const name, bool value)
    { node.append_attribute(name).set_value(value ? "true" : "false"); }

  inline void set_attr(pugi::xml_node node, const char* const name, const std::string& value)
  {
    if(!value.empty())
      node.append_attribute(name).set_value(value.c_str());
  }

  template<typename T, std::enable_if_t<std::is_enum_v<T>, bool> = false>
  static inline void set_attr(pugi::xml_node node, const char* const name, T value)
    { set_attr(node, name, to_string(value)); }

  template<std::integral T>
  inline void set_attr_hex(pugi::xml_node node, const char* const name, T value)
    { set_attr(node, name, std::format("0x{:08x}", static_cast<uint32_t>(value))); }

  template<typename T>
  static inline typename std::enable_if<std::is_enum<T>::value, T>::type
  get_attr(const pugi::xml_node& node, const char* name)
    { return from_string<T>(node.attribute(name).as_string()); }

  template<typename T>
  static inline typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
  get_attr(const pugi::xml_node& node, const char* name)
    { return node.attribute(name).as_string(); }

  template<typename T>
  static inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
  get_attr(const pugi::xml_node& node, const char* name, T default_value = 0)
  {
    auto attr = node.attribute(name);
    T out = default_value;
    if(!sscanf(attr.as_string(), "%f", &out))
      throw std::runtime_error(std::format("failed to convert \"{}\" to a 32-bit floating point", attr.as_string()));
    return out;
  }

         // NOTE: pugixml will truncate the highest bit hex values stored in signed types
  template<typename T>
  static inline typename std::enable_if<!std::is_enum<T>::value && !std::is_same<T, std::string>::value && !std::is_floating_point<T>::value, T>::type
  get_attr(const pugi::xml_node& node, const char* name, T default_value = 0)
  {
    auto attr = node.attribute(name);
    const char* attrstr = attr.as_string();
    T out = default_value;

    if constexpr ((sizeof(T) == 2))
    {
      if(!sscanf(attrstr, "%hi", &out))
        throw std::runtime_error(std::format("failed to convert \"{}\" to a 16-bit integer", attrstr));
    }
    else if constexpr ((sizeof(T) == 4))
    {
      if(!sscanf(attrstr, "%i", &out))
        throw std::runtime_error(std::format("failed to convert \"{}\" to a 32-bit integer", attrstr));
    }
    else if constexpr (sizeof(T) == 8)
    {
      if(!sscanf(attrstr, "%li", &out))
        throw std::runtime_error(std::format("failed to convert \"{}\" to a 64-bit integer", attrstr));
    }
    else
      throw std::runtime_error(std::format("unsupported type size: {} bytes", sizeof(T)));
    return out;
  }
}
