#include "xml.h"

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QTextStream>

#include <cstring>
#include <sstream>

namespace rc
{
namespace xml
{

namespace
{

QDomDocument owner_document(const QDomNode& n)
{
  if(n.isNull())
    return QDomDocument();
  if(n.isDocument())
    return n.toDocument();
  return n.ownerDocument();
}

int indent_width(const char* indent)
{
  int width = 0;
  while(indent && *indent)
  {
    ++width;
    ++indent;
  }
  return width;
}

bool has_flag(parse_flags flags, parse_flags bit)
{
  return (flags & bit) != parse_flags::none;
}

QDomCharacterData first_text_child(const QDomNode& n)
{
  for(QDomNode c = n.firstChild(); !c.isNull(); c = c.nextSibling())
  {
    if(c.nodeType() == QDomNode::TextNode ||
       c.nodeType() == QDomNode::CDATASectionNode)
      return c.toCharacterData();
  }
  return QDomCharacterData();
}

std::string first_text_value(const QDomNode& n)
{
  QDomCharacterData d = first_text_child(n);
  if(d.isNull())
    return "";
  return d.data().toStdString();
}

bool set_text_value(QDomNode& n, const QString& value)
{
  QDomCharacterData d = first_text_child(n);
  if(!d.isNull())
  {
    d.setData(value);
    return true;
  }
  QDomDocument doc = owner_document(n);
  if(doc.isNull())
    return false;
  n.appendChild(doc.createTextNode(value));
  return true;
}

int parse_int(const std::string& s, int def)
{
  if(s.empty())
    return def;
  const char* start = s.c_str();
  char* end = nullptr;
  long v = std::strtol(start, &end, 10);
  if(end == start)
    return def;
  return static_cast<int>(v);
}

unsigned int parse_uint(const std::string& s, unsigned int def)
{
  if(s.empty())
    return def;
  const char* start = s.c_str();
  char* end = nullptr;
  unsigned long v = std::strtoul(start, &end, 10);
  if(end == start)
    return def;
  return static_cast<unsigned int>(v);
}

bool parse_bool(const std::string& s, bool def)
{
  if(s.empty())
    return def;
  const char c = s[0];
  return c == '1' || c == 't' || c == 'T' || c == 'y' || c == 'Y';
}

double parse_double(const std::string& s, double def)
{
  if(s.empty())
    return def;
  const char* start = s.c_str();
  char* end = nullptr;
  double v = std::strtod(start, &end);
  if(end == start)
    return def;
  return v;
}

long long parse_llong(const std::string& s, long long def)
{
  if(s.empty())
    return def;
  const char* start = s.c_str();
  char* end = nullptr;
  long long v = std::strtoll(start, &end, 10);
  if(end == start)
    return def;
  return v;
}

QDomNode make_child(QDomDocument doc, node_type type)
{
  switch(type)
  {
    case node_type::comment: return doc.createComment(QString());
    case node_type::pcdata: return doc.createTextNode(QString());
    case node_type::cdata: return doc.createCDATASection(QString());
    case node_type::pi: return doc.createProcessingInstruction(QString(), QString());
    case node_type::declaration: return doc.createProcessingInstruction("xml", QString());
    case node_type::doctype: return QDomImplementation().createDocumentType(QString(), QString(), QString());
    default: return QDomNode();
  }
}

/* Prune nodes that pugixml would not have added for the given parse flags:
   comments, processing instructions, the XML declaration and whitespace-only
   text nodes. Traverses the whole tree. */
void filter_nodes(QDomNode parent, parse_flags flags)
{
  QDomNode n = parent.firstChild();
  while(!n.isNull())
  {
    QDomNode next = n.nextSibling();
    if(n.isComment())
    {
      if(!has_flag(flags, parse_flags::comments))
        parent.removeChild(n);
    }
    else if(n.isProcessingInstruction())
    {
      const bool is_declaration =
          n.toProcessingInstruction().target() == "xml";
      if(is_declaration ? !has_flag(flags, parse_flags::declaration)
                        : !has_flag(flags, parse_flags::pi))
        parent.removeChild(n);
    }
    else if(n.isElement())
    {
      filter_nodes(n, flags);
    }
    else if(n.nodeType() == QDomNode::TextNode)
    {
      if(!has_flag(flags, parse_flags::ws_pcdata))
      {
        QString data = n.toText().data();
        if(data.trimmed().isEmpty())
          parent.removeChild(n);
      }
    }
    n = next;
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
  return m_impl.isNull();
}

bool attribute::operator==(const attribute& o) const
{
  return m_impl == o.m_impl;
}

std::string attribute::name() const
{
  return m_impl.name().toStdString();
}

std::string attribute::value() const
{
  return m_impl.value().toStdString();
}

bool attribute::set_value(const char* value)
{
  if(m_impl.isNull())
    return false;
  m_impl.setValue(QString::fromUtf8(value));
  return true;
}

bool attribute::set_value(int value)
{
  if(m_impl.isNull())
    return false;
  m_impl.setValue(QString::number(value));
  return true;
}

bool attribute::set_value(bool value)
{
  if(m_impl.isNull())
    return false;
  m_impl.setValue(value ? QStringLiteral("true") : QStringLiteral("false"));
  return true;
}

bool attribute::set_value(double value, int precision)
{
  if(m_impl.isNull())
    return false;
  m_impl.setValue(QString::number(value, 'g', precision));
  return true;
}

std::string attribute::as_string() const
{
  return m_impl.value().toStdString();
}

int attribute::as_int(int def) const
{
  return parse_int(m_impl.value().toStdString(), def);
}

unsigned int attribute::as_uint(unsigned int def) const
{
  return parse_uint(m_impl.value().toStdString(), def);
}

bool attribute::as_bool(bool def) const
{
  return parse_bool(m_impl.value().toStdString(), def);
}

double attribute::as_double(double def) const
{
  return parse_double(m_impl.value().toStdString(), def);
}

long long attribute::as_llong(long long def) const
{
  return parse_llong(m_impl.value().toStdString(), def);
}

// ---------------------------------------------------------------------------
// text
// ---------------------------------------------------------------------------

bool text::empty() const
{
  return m_impl.isNull();
}

bool text::set(const char* value)
{
  if(m_impl.isNull())
    return false;
  return set_text_value(m_impl, QString::fromUtf8(value));
}

bool text::set(int value)
{
  if(m_impl.isNull())
    return false;
  return set_text_value(m_impl, QString::number(value));
}

bool text::set(bool value)
{
  if(m_impl.isNull())
    return false;
  return set_text_value(m_impl,
                        value ? QStringLiteral("true") : QStringLiteral("false"));
}

bool text::set(double value, int precision)
{
  if(m_impl.isNull())
    return false;
  return set_text_value(m_impl, QString::number(value, 'g', precision));
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
  return first_text_value(m_impl);
}

int text::as_int(int def) const
{
  return parse_int(first_text_value(m_impl), def);
}

bool text::as_bool(bool def) const
{
  return parse_bool(first_text_value(m_impl), def);
}

double text::as_double(double def) const
{
  return parse_double(first_text_value(m_impl), def);
}

std::string text::get() const
{
  return first_text_value(m_impl);
}

// ---------------------------------------------------------------------------
// node
// ---------------------------------------------------------------------------

bool node::empty() const
{
  return m_impl.isNull();
}

bool node::operator==(const node& o) const
{
  return m_impl == o.m_impl;
}

node_type node::type() const
{
  switch(m_impl.nodeType())
  {
    case QDomNode::DocumentNode: return node_type::document;
    case QDomNode::ElementNode: return node_type::element;
    case QDomNode::TextNode: return node_type::pcdata;
    case QDomNode::CDATASectionNode: return node_type::cdata;
    case QDomNode::CommentNode: return node_type::comment;
    case QDomNode::ProcessingInstructionNode:
    {
      if(m_impl.toProcessingInstruction().target() == "xml")
        return node_type::declaration;
      return node_type::pi;
    }
    case QDomNode::DocumentTypeNode: return node_type::doctype;
    default: return node_type::null;
  }
}

std::string node::name() const
{
  if(m_impl.isNull())
    return "";
  if(m_impl.isElement())
    return m_impl.toElement().tagName().toStdString();
  if(m_impl.isProcessingInstruction())
    return m_impl.toProcessingInstruction().target().toStdString();
  if(m_impl.isDocumentType())
    return m_impl.toDocumentType().name().toStdString();
  return "";
}

std::string node::value() const
{
  if(m_impl.isNull())
    return "";
  QDomCharacterData cd = m_impl.toCharacterData();
  if(!cd.isNull())
    return cd.data().toStdString();
  if(m_impl.isProcessingInstruction())
    return m_impl.toProcessingInstruction().data().toStdString();
  return "";
}

bool node::set_name(const char* name)
{
  if(m_impl.isNull())
    return false;
  if(m_impl.isElement())
  {
    m_impl.toElement().setTagName(QString::fromUtf8(name));
    return true;
  }
  return false;
}

node node::child(const char* name) const
{
  if(m_impl.isNull())
    return node();
  const QString qname = QString::fromUtf8(name);
  for(QDomNode c = m_impl.firstChild(); !c.isNull(); c = c.nextSibling())
  {
    if(c.isElement() && c.toElement().tagName() == qname)
      return node(c);
  }
  return node();
}

std::vector<node> node::children() const
{
  std::vector<node> out;
  if(m_impl.isNull())
    return out;
  for(QDomNode c = m_impl.firstChild(); !c.isNull(); c = c.nextSibling())
    out.push_back(node(c));
  return out;
}

std::vector<node> node::children(const char* name) const
{
  std::vector<node> out;
  if(m_impl.isNull())
    return out;
  const QString qname = QString::fromUtf8(name);
  for(QDomNode c = m_impl.firstChild(); !c.isNull(); c = c.nextSibling())
  {
    if(c.isElement() && c.toElement().tagName() == qname)
      out.push_back(node(c));
  }
  return out;
}

node node::first_child() const
{
  return node(m_impl.firstChild());
}

node node::last_child() const
{
  return node(m_impl.lastChild());
}

node node::next_sibling() const
{
  return node(m_impl.nextSibling());
}

node node::next_sibling(const char* name) const
{
  if(m_impl.isNull())
    return node();
  const QString qname = QString::fromUtf8(name);
  for(QDomNode c = m_impl.nextSibling(); !c.isNull(); c = c.nextSibling())
  {
    if(c.isElement() && c.toElement().tagName() == qname)
      return node(c);
  }
  return node();
}

node node::previous_sibling() const
{
  return node(m_impl.previousSibling());
}

node node::parent() const
{
  return node(m_impl.parentNode());
}

node node::root() const
{
  QDomNode cur = m_impl;
  if(cur.isNull())
    return node();
  while(!cur.parentNode().isNull())
    cur = cur.parentNode();
  return node(cur);
}

node node::append_child(const char* name)
{
  if(m_impl.isNull())
    return node();
  QDomDocument doc = owner_document(m_impl);
  if(doc.isNull())
    return node();
  QDomNode added =
      m_impl.appendChild(doc.createElement(QString::fromUtf8(name)));
  return node(added);
}

node node::append_child(node_type type)
{
  if(m_impl.isNull())
    return node();
  QDomDocument doc = owner_document(m_impl);
  if(doc.isNull())
    return node();
  QDomNode child = make_child(doc, type);
  if(child.isNull())
    return node();
  QDomNode added = m_impl.appendChild(child);
  return node(added);
}

node node::prepend_child(const char* name)
{
  if(m_impl.isNull())
    return node();
  QDomDocument doc = owner_document(m_impl);
  if(doc.isNull())
    return node();
  QDomNode added = m_impl.insertBefore(
      doc.createElement(QString::fromUtf8(name)), m_impl.firstChild());
  return node(added);
}

node node::insert_child_before(const char* name, const node& ref)
{
  if(m_impl.isNull())
    return node();
  QDomDocument doc = owner_document(m_impl);
  if(doc.isNull())
    return node();
  QDomNode added = m_impl.insertBefore(
      doc.createElement(QString::fromUtf8(name)), ref.m_impl);
  return node(added);
}

node node::insert_child_before(node_type type, const node& ref)
{
  if(m_impl.isNull())
    return node();
  QDomDocument doc = owner_document(m_impl);
  if(doc.isNull())
    return node();
  QDomNode child = make_child(doc, type);
  if(child.isNull())
    return node();
  QDomNode added = m_impl.insertBefore(child, ref.m_impl);
  return node(added);
}

bool node::remove_child(const node& n)
{
  if(m_impl.isNull())
    return false;
  return !m_impl.removeChild(n.m_impl).isNull();
}

bool node::remove_child(const char* name)
{
  if(m_impl.isNull())
    return false;
  const QString qname = QString::fromUtf8(name);
  for(QDomNode c = m_impl.firstChild(); !c.isNull(); c = c.nextSibling())
  {
    if(c.isElement() && c.toElement().tagName() == qname)
    {
      m_impl.removeChild(c);
      return true;
    }
  }
  return false;
}

node node::append_copy(const node& n)
{
  if(m_impl.isNull() || n.m_impl.isNull())
    return node();
  QDomNode added = m_impl.appendChild(n.m_impl.cloneNode(true));
  return node(added);
}

rc::xml::attribute node::attribute(const char* name) const
{
  if(m_impl.isNull())
    return rc::xml::attribute();
  QDomElement e = m_impl.toElement();
  if(e.isNull())
    return rc::xml::attribute();
  return rc::xml::attribute(e.attributeNode(QString::fromUtf8(name)));
}

rc::xml::attribute node::append_attribute(const char* name)
{
  if(m_impl.isNull())
    return rc::xml::attribute();
  QDomElement e = m_impl.toElement();
  if(e.isNull())
    return rc::xml::attribute();
  QDomDocument doc = owner_document(e);
  if(doc.isNull())
    return rc::xml::attribute();
  QDomAttr attr = doc.createAttribute(QString::fromUtf8(name));
  e.setAttributeNode(attr);
  return rc::xml::attribute(attr);
}

bool node::remove_attribute(const char* name)
{
  if(m_impl.isNull())
    return false;
  QDomElement e = m_impl.toElement();
  if(e.isNull())
    return false;
  const QString qname = QString::fromUtf8(name);
  const bool existed = !e.attributeNode(qname).isNull();
  e.removeAttribute(qname);
  return existed;
}

std::vector<rc::xml::attribute> node::attributes() const
{
  std::vector<rc::xml::attribute> out;
  if(m_impl.isNull())
    return out;
  QDomElement e = m_impl.toElement();
  if(e.isNull())
    return out;
  QDomNamedNodeMap map = e.attributes();
  for(int i = 0; i < map.count(); ++i)
    out.push_back(rc::xml::attribute(map.item(i).toAttr()));
  return out;
}

rc::xml::text node::text() const
{
  return rc::xml::text(m_impl);
}

bool node::set_value(const char* value)
{
  if(m_impl.isNull())
    return false;
  QDomCharacterData cd = m_impl.toCharacterData();
  if(!cd.isNull())
  {
    cd.setData(QString::fromUtf8(value));
    return true;
  }
  if(m_impl.isProcessingInstruction())
  {
    m_impl.toProcessingInstruction().setData(QString::fromUtf8(value));
    return true;
  }
  return false;
}

node node::find_child_by_attribute(const char* name, const char* attr,
                                   const char* value) const
{
  if(m_impl.isNull())
    return node();
  const QString qname = QString::fromUtf8(name);
  const QString qattr = QString::fromUtf8(attr);
  const QString qvalue = QString::fromUtf8(value);
  for(QDomNode c = m_impl.firstChild(); !c.isNull(); c = c.nextSibling())
  {
    if(!c.isElement())
      continue;
    QDomElement e = c.toElement();
    if(e.tagName() == qname && e.attribute(qattr) == qvalue)
      return node(c);
  }
  return node();
}

std::string node::path() const
{
  std::vector<std::string> parts;
  QDomNode cur = m_impl;
  while(!cur.isNull() && !cur.isDocument())
  {
    if(cur.isElement())
    {
      const QString name = cur.toElement().tagName();
      int index = 1;
      int total = 1;
      for(QDomNode s = cur.previousSibling(); !s.isNull(); s = s.previousSibling())
      {
        if(s.isElement() && s.toElement().tagName() == name)
        {
          ++index;
          ++total;
        }
      }
      for(QDomNode s = cur.nextSibling(); !s.isNull(); s = s.nextSibling())
      {
        if(s.isElement() && s.toElement().tagName() == name)
          ++total;
      }
      std::string part = name.toStdString();
      if(total > 1)
        part += "[" + std::to_string(index) + "]";
      parts.push_back(part);
    }
    cur = cur.parentNode();
  }
  std::string out;
  for(auto it = parts.rbegin(); it != parts.rend(); ++it)
    out += "/" + *it;
  if(out.empty())
    return "/";
  return out;
}

void node::print(std::ostream& out, const char* indent) const
{
  if(m_impl.isNull())
    return;
  QString buffer;
  QTextStream ts(&buffer);
  ts.setCodec("UTF-8");
  m_impl.save(ts, indent_width(indent), QDomNode::EncodingFromTextStream);
  QByteArray bytes = buffer.toUtf8();
  out.write(bytes.constData(), bytes.size());
}

// ---------------------------------------------------------------------------
// document
// ---------------------------------------------------------------------------

parse_result document::load_file(const char* path, parse_flags flags)
{
  QFile f(QString::fromUtf8(path));
  if(!f.open(QIODevice::ReadOnly))
    return parse_result(parse_status::file_not_found, "file not found");
  const QByteArray data = f.readAll();
  QString error;
  const bool ok = m_impl.setContent(data, &error, nullptr, nullptr);
  if(!ok)
    return parse_result(parse_status::parse_error, error.toStdString());
  if(!has_flag(flags, parse_flags::doctype))
  {
    QDomDocumentType dt = m_impl.doctype();
    if(!dt.isNull())
      m_impl.removeChild(dt);
  }
  filter_nodes(m_impl, flags);
  return parse_result(parse_status::ok, "no error");
}

parse_result document::load_string(const char* data, parse_flags flags)
{
  QString error;
  const bool ok = m_impl.setContent(QString::fromUtf8(data), &error,
                                    nullptr, nullptr);
  if(!ok)
    return parse_result(parse_status::parse_error, error.toStdString());
  if(!has_flag(flags, parse_flags::doctype))
  {
    QDomDocumentType dt = m_impl.doctype();
    if(!dt.isNull())
      m_impl.removeChild(dt);
  }
  filter_nodes(m_impl, flags);
  return parse_result(parse_status::ok, "no error");
}

bool document::save_file(const char* path, const char* indent) const
{
  QFile f(QString::fromUtf8(path));
  if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return false;
  QTextStream ts(&f);
  ts.setCodec("UTF-8");
  m_impl.save(ts, indent_width(indent), QDomNode::EncodingFromTextStream);
  ts << "\n";
  f.close();
  return true;
}

void document::save(std::ostream& out, const char* indent) const
{
  QString buffer;
  QTextStream ts(&buffer);
  ts.setCodec("UTF-8");
  m_impl.save(ts, indent_width(indent), QDomNode::EncodingFromTextStream);
  QByteArray bytes = buffer.toUtf8();
  out.write(bytes.constData(), bytes.size());
}

std::string document::to_string(const char* indent) const
{
  std::ostringstream out;
  save(out, indent);
  return out.str();
}

node document::append_child(const char* name)
{
  QDomNode added =
      m_impl.appendChild(m_impl.createElement(QString::fromUtf8(name)));
  return node(added);
}

node document::child(const char* name) const
{
  return node(m_impl).child(name);
}

node document::first_child() const
{
  return node(m_impl.firstChild());
}

node document::last_child() const
{
  return node(m_impl.lastChild());
}

node document::root() const
{
  return node(m_impl);
}

node document::document_element() const
{
  return node(m_impl.documentElement());
}

} // namespace xml
} // namespace rc
