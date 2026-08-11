#pragma once

#include <concepts>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#if defined(HAVE_PUGIXML)
#include <pugixml.hpp>
#elif defined(HAVE_QT)
#include <QDomDocument>
#else
#error "xml: no XML backend selected -- define HAVE_PUGIXML or HAVE_QT"
#endif

/* XML abstraction layer.
 *
 * Exposes a single pugixml-shaped API over two interchangeable backends:
 *  - HAVE_PUGIXML: pugixml (see xml_pugi.cpp)
 *  - HAVE_QT:      Qt XML DOM, QDomDocument (see xml_qt.cpp)
 *
 * Exactly one backend must be selected at compile time; the #error above
 * fires when neither define is present. When both are defined, pugixml wins.
 *
 * Value semantics: node/attribute/text handles are lightweight, copyable
 * wrappers that reference a backend-owned document, exactly like pugixml
 * handles. String-returning accessors return std::string so the Qt backend
 * (whose values are QString) never exposes dangling pointers.
 *
 * parse_flags mirrors pugixml's flag bit values so the pugixml backend can
 * forward them verbatim. The Qt backend honors the structurally meaningful
 * flags (comments, pi, declaration, doctype, ws_pcdata) by pruning nodes
 * after parsing; the whitespace-normalization flags (escapes, eol,
 * wconv_attribute, wnorm_attribute) are accepted for API compatibility and
 * left to the XML parser's standard normalization.
 */

namespace xml
{

#if defined(HAVE_PUGIXML)
namespace backend
{
  using node = pugi::xml_node;
  using attribute = pugi::xml_attribute;
  using text = pugi::xml_text;
  using document = pugi::xml_document;
} // namespace backend
#elif defined(HAVE_QT)
namespace backend
{
  using node = QDomNode;
  using attribute = QDomAttr;
  using text = QDomNode;
  using document = QDomDocument;
} // namespace backend
#endif

/* Node types, numbered to match pugixml's xml_node_type. */
enum class node_type
{
  null = 0,
  document = 1,
  element = 2,
  pcdata = 3,
  cdata = 4,
  comment = 5,
  pi = 6,
  declaration = 7,
  doctype = 8,
};

/* Parse flags, bit values identical to pugixml's parse_* constants. */
enum class parse_flags : unsigned int
{
  none = 0u,
  pi = 1u << 0,
  comments = 1u << 1,
  cdata = 1u << 2,
  ws_pcdata = 1u << 3,
  escapes = 1u << 4,
  eol = 1u << 5,
  wconv_attribute = 1u << 6,
  wnorm_attribute = 1u << 7,
  declaration = 1u << 8,
  doctype = 1u << 9,
  ws_pcdata_single = 1u << 10,
  trim_pcdata = 1u << 11,
  fragment = 1u << 12,
  embed_pcdata = 1u << 13,
  all = (1u << 14) - 1u,
};

constexpr parse_flags parse_default = static_cast<parse_flags>(
    static_cast<unsigned int>(parse_flags::cdata) |
    static_cast<unsigned int>(parse_flags::escapes) |
    static_cast<unsigned int>(parse_flags::wconv_attribute) |
    static_cast<unsigned int>(parse_flags::eol));

constexpr parse_flags parse_full = static_cast<parse_flags>(
    static_cast<unsigned int>(parse_default) |
    static_cast<unsigned int>(parse_flags::pi) |
    static_cast<unsigned int>(parse_flags::comments) |
    static_cast<unsigned int>(parse_flags::declaration) |
    static_cast<unsigned int>(parse_flags::doctype));

constexpr parse_flags operator|(parse_flags a, parse_flags b)
{
  return static_cast<parse_flags>(static_cast<unsigned int>(a) |
                                  static_cast<unsigned int>(b));
}

constexpr parse_flags operator&(parse_flags a, parse_flags b)
{
  return static_cast<parse_flags>(static_cast<unsigned int>(a) &
                                  static_cast<unsigned int>(b));
}

/* Matches pugixml's default_float_precision so attribute/text formatting of
   floating point values stays identical across backends. */
constexpr int default_float_precision = 9;

enum class parse_status
{
  ok,
  file_not_found,
  io_error,
  out_of_memory,
  internal_error,
  unsupported_encoding,
  parse_error,
};

class document;

class parse_result
{
public:
  parse_result() = default;

  /* True when the document parsed successfully. */
  explicit operator bool() const
  {
    return m_status == parse_status::ok;
  }

  parse_status status() const
  {
    return m_status;
  }

  std::string description() const
  {
    return m_description;
  }

private:
  parse_result(parse_status status, std::string description)
    : m_status(status), m_description(std::move(description))
  {
  }

  parse_status m_status = parse_status::internal_error;
  std::string m_description;

  friend class document;
};

class node;

class attribute
{
public:
  attribute() = default;

  bool empty() const;
  explicit operator bool() const
  {
    return !empty();
  }
  bool operator==(const attribute& o) const;

  std::string name() const;
  std::string value() const;

  bool set_value(const char* value);
  bool set_value(int value);
  bool set_value(bool value);
  bool set_value(double value, int precision = default_float_precision);

  std::string as_string() const;
  int as_int(int def = 0) const;
  unsigned int as_uint(unsigned int def = 0) const;
  bool as_bool(bool def = false) const;
  double as_double(double def = 0) const;
  long long as_llong(long long def = 0) const;

private:
  explicit attribute(backend::attribute impl)
    : m_impl(impl)
  {
  }

  backend::attribute m_impl;

  friend class node;
};

class text
{
public:
  text() = default;

  bool empty() const;

  bool set(const char* value);
  bool set(int value);
  bool set(bool value);
  bool set(double value, int precision = default_float_precision);

  text& operator=(const char* value);
  text& operator=(int value);
  text& operator=(bool value);
  text& operator=(double value);

  std::string as_string() const;
  int as_int(int def = 0) const;
  bool as_bool(bool def = false) const;
  double as_double(double def = 0) const;
  std::string get() const;

private:
  explicit text(backend::text impl)
    : m_impl(impl)
  {
  }

  backend::text m_impl;

  friend class node;
};

class node
{
public:
  node() = default;

  bool empty() const;
  explicit operator bool() const
  {
    return !empty();
  }
  bool operator==(const node& o) const;

  node_type type() const;
  std::string name() const;
  std::string value() const;
  bool set_name(const char* name);

  node child(const char* name) const;
  std::vector<node> children() const;
  std::vector<node> children(const char* name) const;
  node first_child() const;
  node last_child() const;
  node next_sibling() const;
  node next_sibling(const char* name) const;
  node previous_sibling() const;
  node parent() const;
  node root() const;

  node append_child(const char* name);
  node append_child(node_type type);
  node prepend_child(const char* name);
  node insert_child_before(const char* name, const node& ref);
  node insert_child_before(node_type type, const node& ref);
  bool remove_child(const node& n);
  bool remove_child(const char* name);
  node append_copy(const node& n);

  xml::attribute attribute(const char* name) const;
  xml::attribute append_attribute(const char* name);
  bool remove_attribute(const char* name);
  std::vector<xml::attribute> attributes() const;

  /* Convenience writers: append an attribute, skipping empty/null strings. */
  void add_attr(const char* name, const char* value)
  {
    if (value && *value)
      append_attribute(name).set_value(value);
  }

  void add_attr(const char* name, const std::string& value)
  {
    if (!value.empty())
      append_attribute(name).set_value(value.c_str());
  }

  /* Prevent string literals from binding to the bool overload. */
  void add_attr(const char* name, bool value)
  {
    append_attribute(name).set_value(value ? "true" : "false");
  }

  template<std::integral T>
  void add_attr(const char* name, T value)
  {
    append_attribute(name).set_value(value);
  }

  void add_attr(const char* name, float value, int precision = default_float_precision)
  {
    append_attribute(name).set_value(value, precision);
  }

  xml::text text() const;
  bool set_value(const char* value);

  node find_child_by_attribute(const char* name, const char* attr,
                               const char* value) const;
  std::string path() const;
  void print(std::ostream& out, const char* indent) const;

private:
  explicit node(backend::node impl)
    : m_impl(impl)
  {
  }

  backend::node m_impl;

  friend class document;
};

class document
{
public:
  document() = default;

  parse_result load_file(const char* path,
                         parse_flags flags = parse_default);
  parse_result load_string(const char* data,
                           parse_flags flags = parse_default);
  bool save_file(const char* path, const char* indent) const;
  void save(std::ostream& out, const char* indent) const;
  std::string to_string(const char* indent) const;

  node append_child(const char* name);
  node child(const char* name) const;
  node first_child() const;
  node last_child() const;
  node root() const;
  node document_element() const;

private:
  backend::document m_impl;
};

} // namespace xml
