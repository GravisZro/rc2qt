#include "rc_render.h"

#include <QAbstractButton>
#include <QApplication>
#include <QBoxLayout>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QMargins>
#include <QPlainTextEdit>
#include <QPoint>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QRect>
#include <QScrollArea>
#include <QScrollBar>
#include <QSize>
#include <QSlider>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QStyleFactory>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeView>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QPixmap>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace rc
{
namespace render
{

namespace
{

int int_child(const xml::node& node, const char* tag)
{
  xml::node c = node.child(tag);
  return c ? c.text().as_int(0) : 0;
}

std::string text_child(const xml::node& node, const char* tag)
{
  xml::node c = node.child(tag);
  return c ? c.text().get() : "";
}

void apply_size(QWidget* w, const xml::node& prop)
{
  xml::node s = prop.child("size");
  if(!s)
    return;
  QSize size(int_child(s, "width"), int_child(s, "height"));
  std::string name = prop.attribute("name").value();
  if(name == "minimumSize")
    w->setMinimumSize(size);
  else if(name == "maximumSize")
    w->setMaximumSize(size);
  else if(name == "sizeHint")
    w->setMinimumSize(size);
}

QSizePolicy::Policy policy_from_name(const std::string& name)
{
  if(name == "Fixed")
    return QSizePolicy::Fixed;
  if(name == "Minimum")
    return QSizePolicy::Minimum;
  if(name == "Maximum")
    return QSizePolicy::Maximum;
  if(name == "Preferred")
    return QSizePolicy::Preferred;
  if(name == "Expanding")
    return QSizePolicy::Expanding;
  if(name == "MinimumExpanding")
    return QSizePolicy::MinimumExpanding;
  if(name == "Ignored")
    return QSizePolicy::Ignored;
  return QSizePolicy::Preferred;
}

void apply_sizepolicy(QWidget* w, const xml::node& prop)
{
  xml::node sp = prop.child("sizepolicy");
  if(!sp)
    return;
  QSizePolicy policy(
    policy_from_name(sp.attribute("hsizetype").value()),
    policy_from_name(sp.attribute("vsizetype").value()));
  policy.setHorizontalStretch(int_child(sp, "horstretch"));
  policy.setVerticalStretch(int_child(sp, "verstretch"));
  w->setSizePolicy(policy);
}

Qt::Alignment alignment_from_set(const std::string& value)
{
  Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter;
  size_t start = 0;
  while(start <= value.size())
  {
    size_t end = value.find('|', start);
    std::string token = value.substr(start, end == std::string::npos ? std::string::npos : end - start);
    if(token == "Qt::AlignLeft")
      align = (align & ~Qt::AlignHorizontal_Mask) | Qt::AlignLeft;
    else if(token == "Qt::AlignRight")
      align = (align & ~Qt::AlignHorizontal_Mask) | Qt::AlignRight;
    else if(token == "Qt::AlignHCenter")
      align = (align & ~Qt::AlignHorizontal_Mask) | Qt::AlignHCenter;
    else if(token == "Qt::AlignJustify")
      align = (align & ~Qt::AlignHorizontal_Mask) | Qt::AlignJustify;
    else if(token == "Qt::AlignTop")
      align = (align & ~Qt::AlignVertical_Mask) | Qt::AlignTop;
    else if(token == "Qt::AlignBottom")
      align = (align & ~Qt::AlignVertical_Mask) | Qt::AlignBottom;
    else if(token == "Qt::AlignVCenter")
      align = (align & ~Qt::AlignVertical_Mask) | Qt::AlignVCenter;
    else if(token == "Qt::AlignCenter")
      align = Qt::AlignCenter;
    if(end == std::string::npos)
      break;
    start = end + 1;
  }
  return align;
}

Qt::WindowFlags flags_from_set(const std::string& value)
{
  Qt::WindowFlags flags;
  size_t start = 0;
  while(start <= value.size())
  {
    size_t end = value.find('|', start);
    std::string token = value.substr(start, end == std::string::npos ? std::string::npos : end - start);
    if(token == "Qt::Dialog")
      flags |= Qt::Dialog;
    else if(token == "Qt::Window")
      flags |= Qt::Window;
    else if(token == "Qt::WindowCloseButtonHint")
      flags |= Qt::WindowCloseButtonHint;
    if(end == std::string::npos)
      break;
    start = end + 1;
  }
  return flags;
}

void apply_font(QWidget* w, const xml::node& font)
{
  QFont f = w->font();
  std::string family = text_child(font, "family");
  if(!family.empty())
    f.setFamily(QString::fromStdString(family));
  if(int pointsize = int_child(font, "pointsize"); pointsize > 0)
    f.setPointSize(pointsize);
  if(xml::node b = font.child("bold"))
    f.setBold(b.text().as_bool(false));
  if(xml::node i = font.child("italic"))
    f.setItalic(i.text().as_bool(false));
  w->setFont(f);
}

void apply_widget_properties(QWidget* w, const xml::node& node)
{
  for(xml::node prop : node.children("property"))
  {
    std::string name = prop.attribute("name").value();
    xml::node value = prop.first_child();
    if(!value)
      continue;
    std::string tag = value.name();

    if(tag == "rect")
    {
      xml::node r = prop.child("rect");
      if(r)
      {
        w->setGeometry(
          int_child(r, "x"), int_child(r, "y"),
          int_child(r, "width"), int_child(r, "height"));
      }
    }
    else if(tag == "size")
    {
      apply_size(w, prop);
    }
    else if(tag == "sizepolicy")
    {
      apply_sizepolicy(w, prop);
    }
    else if(tag == "string")
    {
      std::string text = value.text().get();
      if(name == "text")
      {
        if(auto b = qobject_cast<QAbstractButton*>(w))
          b->setText(QString::fromStdString(text));
        else if(auto le = qobject_cast<QLineEdit*>(w))
          le->setText(QString::fromStdString(text));
        else if(auto lb = qobject_cast<QLabel*>(w))
          lb->setText(QString::fromStdString(text));
        else if(auto te = qobject_cast<QTextEdit*>(w))
          te->setPlainText(QString::fromStdString(text));
        else if(auto te = qobject_cast<QPlainTextEdit*>(w))
          te->setPlainText(QString::fromStdString(text));
        else if(auto cb = qobject_cast<QComboBox*>(w))
        {
          cb->setEditable(false);
          cb->addItem(QString::fromStdString(text));
        }
      }
      else if(name == "title")
      {
        if(auto gb = qobject_cast<QGroupBox*>(w))
          gb->setTitle(QString::fromStdString(text));
      }
      else if(name == "windowTitle")
      {
        w->setWindowTitle(QString::fromStdString(text));
      }
    }
    else if(tag == "set")
    {
      std::string set = value.text().get();
      if(name == "alignment")
      {
        if(auto lb = qobject_cast<QLabel*>(w))
          lb->setAlignment(alignment_from_set(set));
      }
      else if(name == "windowFlags")
      {
        w->setWindowFlags(flags_from_set(set));
      }
    }
    else if(tag == "bool")
    {
      bool bval = value.text().as_bool(false);
      if(name == "visible")
        w->setVisible(bval);
      else if(name == "enabled")
        w->setEnabled(bval);
      else if(name == "wordWrap")
      {
        if(auto lb = qobject_cast<QLabel*>(w))
          lb->setWordWrap(bval);
      }
      else if(name == "default")
      {
        if(auto btn = qobject_cast<QPushButton*>(w))
          btn->setDefault(bval);
      }
      else if(name == "autoDefault")
      {
        if(auto btn = qobject_cast<QPushButton*>(w))
          btn->setAutoDefault(bval);
      }
    }
    else if(tag == "enum")
    {
      std::string enum_value = value.text().get();
      if(name == "tabPosition" && enum_value == "QTabWidget::North")
      {
        if(auto tw = qobject_cast<QTabWidget*>(w))
          tw->setTabPosition(QTabWidget::North);
      }
      else if(name == "frameShape")
      {
        if(auto f = qobject_cast<QFrame*>(w))
          f->setFrameShape(QFrame::StyledPanel);
      }
      else if(name == "frameShadow")
      {
        if(auto f = qobject_cast<QFrame*>(w))
          f->setFrameShadow(QFrame::Sunken);
      }
    }
    else if(tag == "number")
    {
      int num = value.text().as_int(0);
      if(name == "currentIndex")
      {
        if(auto sw = qobject_cast<QStackedWidget*>(w))
          sw->setCurrentIndex(num);
        else if(auto tw = qobject_cast<QTabWidget*>(w))
          tw->setCurrentIndex(num);
      }
    }
    else if(tag == "font")
    {
      apply_font(w, value);
    }
  }
}

void apply_layout_properties(QLayout* layout, const xml::node& node)
{
  for(xml::node prop : node.children("property"))
  {
    std::string name = prop.attribute("name").value();
    xml::node value = prop.first_child();
    if(!value)
      continue;
    if(name == "spacing")
    {
      int s = value.text().as_int(-1);
      if(s >= 0)
        layout->setSpacing(s);
    }
    else if(name == "margin")
    {
      int m = value.text().as_int(0);
      layout->setContentsMargins(m, m, m, m);
    }
    else if(name == "leftMargin")
    {
      QMargins mg = layout->contentsMargins();
      mg.setLeft(value.text().as_int(0));
      layout->setContentsMargins(mg);
    }
    else if(name == "topMargin")
    {
      QMargins mg = layout->contentsMargins();
      mg.setTop(value.text().as_int(0));
      layout->setContentsMargins(mg);
    }
    else if(name == "rightMargin")
    {
      QMargins mg = layout->contentsMargins();
      mg.setRight(value.text().as_int(0));
      layout->setContentsMargins(mg);
    }
    else if(name == "bottomMargin")
    {
      QMargins mg = layout->contentsMargins();
      mg.setBottom(value.text().as_int(0));
      layout->setContentsMargins(mg);
    }
    else if(name == "horizontalSpacing")
    {
      if(auto g = qobject_cast<QGridLayout*>(layout))
        g->setHorizontalSpacing(value.text().as_int(-1));
    }
    else if(name == "verticalSpacing")
    {
      if(auto g = qobject_cast<QGridLayout*>(layout))
        g->setVerticalSpacing(value.text().as_int(-1));
    }
  }
}

QWidget* create_widget(const std::string& cls, QWidget* parent)
{
  if(cls == "QDialog")
    return new QDialog(parent);
  if(cls == "QPushButton")
    return new QPushButton(parent);
  if(cls == "QLabel")
    return new QLabel(parent);
  if(cls == "QLineEdit")
    return new QLineEdit(parent);
  if(cls == "QTextEdit")
    return new QTextEdit(parent);
  if(cls == "QPlainTextEdit")
    return new QPlainTextEdit(parent);
  if(cls == "QComboBox")
    return new QComboBox(parent);
  if(cls == "QCheckBox")
    return new QCheckBox(parent);
  if(cls == "QRadioButton")
    return new QRadioButton(parent);
  if(cls == "QGroupBox")
    return new QGroupBox(parent);
  if(cls == "QListWidget")
    return new QListWidget(parent);
  if(cls == "QListView")
    return new QListView(parent);
  if(cls == "QTableWidget")
    return new QTableWidget(parent);
  if(cls == "QTableView")
    return new QTableView(parent);
  if(cls == "QTreeWidget")
    return new QTreeWidget(parent);
  if(cls == "QTreeView")
    return new QTreeView(parent);
  if(cls == "QScrollArea")
    return new QScrollArea(parent);
  if(cls == "QProgressBar")
    return new QProgressBar(parent);
  if(cls == "QSlider")
    return new QSlider(parent);
  if(cls == "QSpinBox")
    return new QSpinBox(parent);
  if(cls == "QDoubleSpinBox")
    return new QDoubleSpinBox(parent);
  if(cls == "QDateTimeEdit")
    return new QDateTimeEdit(parent);
  if(cls == "QCalendarWidget")
    return new QCalendarWidget(parent);
  if(cls == "QScrollBar")
    return new QScrollBar(parent);
  if(cls == "QFrame")
    return new QFrame(parent);
  if(cls == "QStackedWidget")
    return new QStackedWidget(parent);
  if(cls == "QTabWidget")
    return new QTabWidget(parent);
  if(cls == "QStatusBar")
    return new QStatusBar(parent);
  if(cls == "QToolBar")
    return new QToolBar(parent);
  if(cls == "QHeaderView")
    return new QHeaderView(Qt::Horizontal, parent);
  return new QWidget(parent);
}

QLayout* create_layout(const std::string& cls, QWidget* owner)
{
  if(cls == "QGridLayout")
    return new QGridLayout(owner);
  if(cls == "QHBoxLayout")
    return new QHBoxLayout(owner);
  if(cls == "QVBoxLayout")
    return new QVBoxLayout(owner);
  return new QGridLayout(owner);
}

struct builder
{
  std::map<std::string, QWidget*> by_name;
  std::map<QWidget*, QWidget*> widget_container;
};

QWidget* build_widget(const xml::node& node, QWidget* parent, builder& b);

bool is_container_class(const std::string& cls)
{
  return cls == "QDialog" || cls == "QWidget" || cls == "QGroupBox" ||
         cls == "QFrame";
}

void build_layout(const xml::node& node, QWidget* owner, builder& b,
                  QLayout* parent_layout, int row, int col, int rowspan, int colspan,
                  Qt::Alignment item_align = Qt::Alignment())
{
  std::string cls = node.attribute("class").value();
  QLayout* layout = parent_layout ? create_layout(cls, nullptr) : create_layout(cls, owner);
  if(!layout)
    return;

  apply_layout_properties(layout, node);

  /* uic maps the QGridLayout columnminimumwidth/rowminimumheight attributes to
     setColumnMinimumWidth/setRowMinimumHeight(0, ...) and the stretch
     attributes to comma-separated per-index stretch factors. */
  if(auto g = qobject_cast<QGridLayout*>(layout))
  {
    std::string cmw = node.attribute("columnminimumwidth").value();
    if(!cmw.empty())
      g->setColumnMinimumWidth(0, std::atoi(cmw.c_str()));
    std::string rmh = node.attribute("rowminimumheight").value();
    if(!rmh.empty())
      g->setRowMinimumHeight(0, std::atoi(rmh.c_str()));

    std::string cst = node.attribute("columnstretch").value();
    int ci = 0;
    size_t pos = 0;
    while(pos <= cst.size())
    {
      size_t comma = cst.find(',', pos);
      std::string tok = cst.substr(pos, comma == std::string::npos ? std::string::npos : comma - pos);
      if(!tok.empty())
        g->setColumnStretch(ci++, std::atoi(tok.c_str()));
      if(comma == std::string::npos)
        break;
      pos = comma + 1;
    }
    std::string rst = node.attribute("rowstretch").value();
    int ri = 0;
    pos = 0;
    while(pos <= rst.size())
    {
      size_t comma = rst.find(',', pos);
      std::string tok = rst.substr(pos, comma == std::string::npos ? std::string::npos : comma - pos);
      if(!tok.empty())
        g->setRowStretch(ri++, std::atoi(tok.c_str()));
      if(comma == std::string::npos)
        break;
      pos = comma + 1;
    }
  }

  if(parent_layout)
  {
    if(auto g = qobject_cast<QGridLayout*>(parent_layout))
      g->addLayout(layout, row, col, rowspan, colspan);
    else if(auto box = qobject_cast<QBoxLayout*>(parent_layout))
      box->addLayout(layout);
    if(item_align)
      parent_layout->setAlignment(layout, item_align);
  }

  QWidget* owner_effective = owner;

  for(xml::node item : node.children("item"))
  {
    xml::node child = item.first_child();
    if(!child)
      continue;
    std::string child_tag = child.name();
    int irow = item.attribute("row").as_int(0);
    int icol = item.attribute("column").as_int(0);
    int irs = item.attribute("rowspan").as_int(1);
    int ics = item.attribute("colspan").as_int(1);
    Qt::Alignment item_align = Qt::Alignment();
    std::string align_str = item.attribute("alignment").value();
    if(!align_str.empty())
      item_align = alignment_from_set(align_str);

    if(child_tag == "widget")
    {
      QWidget* w = build_widget(child, owner_effective, b);
      b.widget_container[w] = owner_effective;
      if(auto g = qobject_cast<QGridLayout*>(layout))
      {
        g->addWidget(w, irow, icol, irs, ics);
      }
      else if(auto box = qobject_cast<QBoxLayout*>(layout))
      {
        box->addWidget(w);
      }
      if(item_align)
        layout->setAlignment(w, item_align);
    }
    else if(child_tag == "layout")
    {
      build_layout(child, owner, b, layout, irow, icol, irs, ics, item_align);
    }
    else if(child_tag == "spacer")
    {
      QSizePolicy::Policy policy = QSizePolicy::Expanding;
      int sw = 20;
      int sh = 20;
      xml::node prop = child.find_child_by_attribute("property", "name", "sizeHint");
      if(xml::node s = prop.child("size"))
      {
        sw = int_child(s, "width");
        sh = int_child(s, "height");
      }
      if(auto g = qobject_cast<QGridLayout*>(layout))
        g->addItem(new QSpacerItem(sw, sh, policy, policy), irow, icol);
      else if(auto box = qobject_cast<QBoxLayout*>(layout))
      {
        box->addItem(new QSpacerItem(sw, sh, policy, policy));
        box->setStretch(box->count() - 1, 1);
      }
    }
  }
}

QWidget* build_widget(const xml::node& node, QWidget* parent, builder& b)
{
  std::string cls = node.attribute("class").value();
  std::string name = node.attribute("name").value();
  QWidget* w = create_widget(cls, parent);
  w->setObjectName(QString::fromStdString(name));
  apply_widget_properties(w, node);
  if(!name.empty())
    b.by_name[name] = w;

  if(xml::node layout = node.child("layout"))
    build_layout(layout, w, b, nullptr, 0, 0, 1, 1);

  if(auto tw = qobject_cast<QTabWidget*>(w))
  {
    for(xml::node page : node.children("widget"))
    {
      QString title;
      if(xml::node attr = page.child("attribute");
         attr && std::string(attr.attribute("name").value()) == "title")
        title = QString::fromStdString(attr.child("string").text().get());
      QWidget* pw = build_widget(page, tw, b);
      b.widget_container[pw] = tw;
      tw->addTab(pw, title);
    }
    return w;
  }

  if(is_container_class(cls))
  {
    for(xml::node child : node.children("widget"))
    {
      QWidget* cw = build_widget(child, w, b);
      b.widget_container[cw] = w;
    }
  }

  return w;
}

bool rects_overlap(const QRect& a, const QRect& b, int tolerance)
{
  QRect isect = a.intersected(b);
  return isect.width() > tolerance && isect.height() > tolerance;
}

std::string sanitize_name(const std::string& name)
{
  std::string out;
  for(char c : name)
    out += (std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_';
  return out;
}

} // namespace

result verify_layout(const verify_input& input, const std::string& dump_dir)
{
  result res;

  static bool style_set = false;
  if(!style_set)
  {
    if(QStyle* fusion = QStyleFactory::create("Fusion"))
      QApplication::setStyle(fusion);
    style_set = true;
  }

  xml::node ui = input.doc.child("ui");
  xml::node root_node = ui.child("widget");
  if(!root_node)
    return res;

  builder b;
  QWidget* dialog = build_widget(root_node, nullptr, b);
  dialog->resize(input.dialog_width, input.dialog_height);
  dialog->show();
  QApplication::processEvents();
  QApplication::processEvents();

  /* Collect the rendered rect of every widget that has a target. */
  std::map<std::string, QRect> rendered;
  for(const auto& [name, target] : input.targets)
  {
    auto it = b.by_name.find(name);
    if(it == b.by_name.end())
    {
      ++res.missing_widgets;
      continue;
    }
    QWidget* w = it->second;
    auto cit = b.widget_container.find(w);
    QWidget* container = cit != b.widget_container.end() ? cit->second : dialog;
    QPoint tl = w->mapTo(container, QPoint(0, 0));
    rendered[name] = QRect(tl, w->size());
  }

  /* Deviation and IoU per widget. */
  double iou_sum = 0.0;
  double dx_sum = 0.0;
  double dy_sum = 0.0;
  double dw_sum = 0.0;
  double dh_sum = 0.0;
  res.target_widgets = static_cast<int>(input.targets.size());
  for(const auto& [name, target] : input.targets)
  {
    auto it = rendered.find(name);
    if(it == rendered.end())
      continue;
    ++res.rendered_widgets;
    const QRect& r = it->second;
    double dx = r.x() - static_cast<double>(target.x);
    double dy = r.y() - static_cast<double>(target.y);
    double dw = r.width() - static_cast<double>(target.w);
    double dh = r.height() - static_cast<double>(target.h);
    dx_sum += std::fabs(dx);
    dy_sum += std::fabs(dy);
    dw_sum += std::fabs(dw);
    dh_sum += std::fabs(dh);
    res.max_dx = std::max(res.max_dx, std::fabs(dx));
    res.max_dy = std::max(res.max_dy, std::fabs(dy));
    if(std::getenv("RC2QT_VERIFY_DEBUG"))
      printf("  VW %-24s target(%4d,%4d,%4d,%4d) render(%4d,%4d,%4d,%4d) dx=%4.0f dy=%4.0f\n",
             name.c_str(), target.x, target.y, target.w, target.h,
             r.x(), r.y(), r.width(), r.height(), dx, dy);

    QRect target_rect(target.x, target.y, target.w, target.h);
    QRect isect = r.intersected(target_rect);
    double union_area = static_cast<double>(r.width() * r.height() +
                                           target_rect.width() * target_rect.height() -
                                           isect.width() * isect.height());
    double iou = (union_area > 0.0) ? (isect.width() * isect.height()) / union_area : 0.0;
    iou_sum += iou;
  }
  if(res.rendered_widgets > 0)
  {
    res.mean_iou = iou_sum / static_cast<double>(res.rendered_widgets);
    res.mean_dx = dx_sum / static_cast<double>(res.rendered_widgets);
    res.mean_dy = dy_sum / static_cast<double>(res.rendered_widgets);
    res.mean_dw = dw_sum / static_cast<double>(res.rendered_widgets);
    res.mean_dh = dh_sum / static_cast<double>(res.rendered_widgets);
  }

  /* Overlap violations: within one container, a rendered overlap between two
     widgets whose source rects do not overlap is an induced overlap. */
  std::vector<std::string> ordered;
  ordered.reserve(input.targets.size());
  for(const auto& [name, target] : input.targets)
  {
    if(rendered.count(name) == 0)
      continue;
    ordered.push_back(name);
  }
  for(size_t i = 0; i < ordered.size(); ++i)
  {
    for(size_t j = i + 1; j < ordered.size(); ++j)
    {
      const target& ti = input.targets.at(ordered[i]);
      const target& tj = input.targets.at(ordered[j]);
      if(ti.container != tj.container)
        continue;
      QRect src_a(ti.x, ti.y, ti.w, ti.h);
      QRect src_b(tj.x, tj.y, tj.w, tj.h);
      if(rects_overlap(src_a, src_b, 0))
        continue;
      const QRect& ra = rendered.at(ordered[i]);
      const QRect& rb = rendered.at(ordered[j]);
      if(rects_overlap(ra, rb, 2))
        ++res.overlap_violations;
    }
  }

  if(!dump_dir.empty())
  {
    QDir().mkpath(QString::fromStdString(dump_dir));
    QString path = QString::fromStdString(dump_dir + "/" + sanitize_name(input.name) + ".png");
    QPixmap pix = dialog->grab();
    pix.save(path);
  }

  delete dialog;
  return res;
}

} // namespace render
} // namespace rc
