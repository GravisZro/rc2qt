#include "xml.h"

#include <sstream>

namespace xml
{

namespace
{

parse_status map_status(pugi::xml_parse_status status)
{
  switch(status)
  {
    case pugi::status_ok: return parse_status::ok;
    case pugi::status_file_not_found: return parse_status::file_not_found;
    case pugi::status_io_error: return parse_status::io_error;
    case pugi::status_out_of_memory: return parse_status::out_of_memory;
    case pugi::status_internal_error: return parse_status::internal_error;
    default: return parse_status::parse_error;
  }
}

} // namespace

// ---------------------------------------------------------------------------
// parse_result
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// attribute
// ---------------------------------------------------------------------------

bool attribute::empty() const
{
  return m_impl.empty();
}

bool attribute::operator==(const attribute& o) const
{
  return m_impl == o.m_impl;
}

std::string attribute::name() const
{
  const char* n = m_impl.name();
  return n ? n : "";
}

std::string attribute::value() const
{
  const char* v = m_impl.value();
  return v ? v : "";
}

bool attribute::set_value(const char* value)
{
  return m_impl.set_value(value);
}

bool attribute::set_value(int value)
{
  return m_impl.set_value(value);
}

bool attribute::set_value(bool value)
{
  return m_impl.set_value(value);
}

bool attribute::set_value(double value, int precision)
{
  return m_impl.set_value(value, precision);
}

std::string attribute::as_string() const
{
  const char* v = m_impl.as_string();
  return v ? v : "";
}

int attribute::as_int(int def) const
{
  return m_impl.as_int(def);
}

unsigned int attribute::as_uint(unsigned int def) const
{
  return m_impl.as_uint(def);
}

bool attribute::as_bool(bool def) const
{
  return m_impl.as_bool(def);
}

double attribute::as_double(double def) const
{
  return m_impl.as_double(def);
}

long long attribute::as_llong(long long def) const
{
  return m_impl.as_llong(def);
}

// ---------------------------------------------------------------------------
// text
// ---------------------------------------------------------------------------

bool text::empty() const
{
  return m_impl.empty();
}

bool text::set(const char* value)
{
  return m_impl.set(value);
}

bool text::set(int value)
{
  return m_impl.set(value);
}

bool text::set(bool value)
{
  return m_impl.set(value);
}

bool text::set(double value, int precision)
{
  return m_impl.set(value, precision);
}

text& text::operator=(const char* value)
{
  set(value);
  return *this;
}

text& text::operator=(int value)
{
  set(value);
  return *this;
}

text& text::operator=(bool value)
{
  set(value);
  return *this;
}

text& text::operator=(double value)
{
  set(value);
  return *this;
}

std::string text::as_string() const
{
  const char* v = m_impl.as_string();
  return v ? v : "";
}

int text::as_int(int def) const
{
  return m_impl.as_int(def);
}

bool text::as_bool(bool def) const
{
  return m_impl.as_bool(def);
}

double text::as_double(double def) const
{
  return m_impl.as_double(def);
}

std::string text::get() const
{
  const char* v = m_impl.get();
  return v ? v : "";
}

// ---------------------------------------------------------------------------
// node
// ---------------------------------------------------------------------------

bool node::empty() const
{
  return m_impl.empty();
}

bool node::operator==(const node& o) const
{
  return m_impl == o.m_impl;
}

node_type node::type() const
{
  switch(m_impl.type())
  {
    case pugi::node_document: return node_type::document;
    case pugi::node_element: return node_type::element;
    case pugi::node_pcdata: return node_type::pcdata;
    case pugi::node_cdata: return node_type::cdata;
    case pugi::node_comment: return node_type::comment;
    case pugi::node_pi: return node_type::pi;
    case pugi::node_declaration: return node_type::declaration;
    case pugi::node_doctype: return node_type::doctype;
    default: return node_type::null;
  }
}

std::string node::name() const
{
  const char* n = m_impl.name();
  return n ? n : "";
}

std::string node::value() const
{
  const char* v = m_impl.value();
  return v ? v : "";
}

bool node::set_name(const char* name)
{
  return m_impl.set_name(name);
}

node node::child(const char* name) const
{
  return node(m_impl.child(name));
}

std::vector<node> node::children() const
{
  std::vector<node> out;
  for(pugi::xml_node c : m_impl.children())
    out.push_back(node(c));
  return out;
}

std::vector<node> node::children(const char* name) const
{
  std::vector<node> out;
  for(pugi::xml_node c : m_impl.children(name))
    out.push_back(node(c));
  return out;
}

node node::first_child() const
{
  return node(m_impl.first_child());
}

node node::last_child() const
{
  return node(m_impl.last_child());
}

node node::next_sibling() const
{
  return node(m_impl.next_sibling());
}

node node::next_sibling(const char* name) const
{
  return node(m_impl.next_sibling(name));
}

node node::previous_sibling() const
{
  return node(m_impl.previous_sibling());
}

node node::parent() const
{
  return node(m_impl.parent());
}

node node::root() const
{
  return node(m_impl.root());
}

node node::append_child(const char* name)
{
  return node(m_impl.append_child(name));
}

node node::append_child(node_type type)
{
  return node(m_impl.append_child(static_cast<pugi::xml_node_type>(type)));
}

node node::prepend_child(const char* name)
{
  return node(m_impl.prepend_child(name));
}

node node::insert_child_before(const char* name, const node& ref)
{
  return node(m_impl.insert_child_before(name, ref.m_impl));
}

node node::insert_child_before(node_type type, const node& ref)
{
  return node(m_impl.insert_child_before(
      static_cast<pugi::xml_node_type>(type), ref.m_impl));
}

bool node::remove_child(const node& n)
{
  return m_impl.remove_child(n.m_impl);
}

bool node::remove_child(const char* name)
{
  return m_impl.remove_child(name);
}

node node::append_copy(const node& n)
{
  return node(m_impl.append_copy(n.m_impl));
}

xml::attribute node::attribute(const char* name) const
{
  return xml::attribute(m_impl.attribute(name));
}

xml::attribute node::append_attribute(const char* name)
{
  return xml::attribute(m_impl.append_attribute(name));
}

bool node::remove_attribute(const char* name)
{
  return m_impl.remove_attribute(name);
}

std::vector<xml::attribute> node::attributes() const
{
  std::vector<xml::attribute> out;
  for(pugi::xml_attribute a : m_impl.attributes())
    out.push_back(xml::attribute(a));
  return out;
}

xml::text node::text() const
{
  return xml::text(m_impl.text());
}

bool node::set_value(const char* value)
{
  return m_impl.set_value(value);
}

node node::find_child_by_attribute(const char* name, const char* attr,
                                   const char* value) const
{
  return node(m_impl.find_child_by_attribute(name, attr, value));
}

std::string node::path() const
{
  return m_impl.path();
}

void node::print(std::ostream& out, const char* indent) const
{
  m_impl.print(out, indent);
}

// ---------------------------------------------------------------------------
// document
// ---------------------------------------------------------------------------

parse_result document::load_file(const char* path, parse_flags flags)
{
  pugi::xml_parse_result r =
      m_impl.load_file(path, static_cast<unsigned int>(flags));
  return parse_result(map_status(r.status), r.description());
}

parse_result document::load_string(const char* data, parse_flags flags)
{
  pugi::xml_parse_result r =
      m_impl.load_string(data, static_cast<unsigned int>(flags));
  return parse_result(map_status(r.status), r.description());
}

bool document::save_file(const char* path, const char* indent) const
{
  return m_impl.save_file(path, indent);
}

void document::save(std::ostream& out, const char* indent) const
{
  m_impl.save(out, indent);
}

std::string document::to_string(const char* indent) const
{
  std::ostringstream out;
  m_impl.save(out, indent);
  return out.str();
}

node document::append_child(const char* name)
{
  return node(m_impl.append_child(name));
}

node document::child(const char* name) const
{
  return node(m_impl.child(name));
}

node document::first_child() const
{
  return node(m_impl.first_child());
}

node document::last_child() const
{
  return node(m_impl.last_child());
}

node document::root() const
{
  return node(m_impl.root());
}

node document::document_element() const
{
  return node(m_impl.document_element());
}

} // namespace xml
