#include <QAbstractItemView>
#include <QAbstractSlider>
#include <QAction>
#include <QApplication>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFontMetrics>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMargins>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QStyle>
#include <QStyleOption>
#include <QTabBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QScreen>
#include <QWidget>

#include <QDateTime>

#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

static double g_dlu_x_factor = 4.0;
static double g_dlu_y_factor = 8.0;

static QString format_dlu(double value)
{
  QString s = QString::number(value, 'f', 3);
  while(s.endsWith('0'))
    s.chop(1);
  if(s.endsWith('.'))
    s.chop(1);
  return s;
}

struct writer
{
  QTextStream& s;

  void section(const QString& name)
  {
    s << "[" << name << "]\n";
  }

  void kv(const QString& key, const QString& value)
  {
    s << key << "=" << value << "\n";
  }

  void i(const QString& key, int value)
  {
    s << key << "=" << value << "\n";
  }

  // Horizontal pixel measurement expressed in font-relative dialog units.
  void h(const QString& key, int px)
  {
    s << key << "=" << format_dlu(static_cast<double>(px) / g_dlu_x_factor) << "\n";
  }

  // Vertical pixel measurement expressed in font-relative dialog units.
  void v(const QString& key, int px)
  {
    s << key << "=" << format_dlu(static_cast<double>(px) / g_dlu_y_factor) << "\n";
  }

  void sz(const QString& prefix, const QSize& size)
  {
    h(prefix + "W", size.width());
    v(prefix + "H", size.height());
  }

  void rect(const QString& prefix, const QRect& r)
  {
    h(prefix + "X", r.x());
    v(prefix + "Y", r.y());
    h(prefix + "W", r.width());
    v(prefix + "H", r.height());
  }

  void margin(const QString& prefix, const QMargins& m)
  {
    h(prefix + "Left", m.left());
    v(prefix + "Top", m.top());
    h(prefix + "Right", m.right());
    v(prefix + "Bottom", m.bottom());
  }
};

struct widget_entry
{
  const char* section;
  std::function<QWidget*()> create;
  std::function<void(QWidget*)> populate;
  std::function<void(writer&, QWidget*)> measure;
};

static void measure_common(writer& w, QWidget* widget)
{
  widget->ensurePolished();
  w.sz("sizeHint", widget->sizeHint());
  w.sz("minimumSizeHint", widget->minimumSizeHint());
  w.sz("widgetSize", widget->size());
  w.margin("contentsMargin", widget->contentsMargins());

  const QFontMetrics fm(widget->font());
  w.i("fontHeight", fm.height());
  w.i("fontAscent", fm.ascent());
  w.i("fontDescent", fm.descent());

  if(QLayout* layout = widget->layout())
  {
    w.margin("layoutContentsMargin", layout->contentsMargins());
    w.v("layoutSpacing", layout->spacing());
  }

  QFrame* frame = qobject_cast<QFrame*>(widget);
  if(frame != nullptr)
  {
    w.i("frameShape", frame->frameShape());
    w.v("frameWidth", frame->frameWidth());
    w.rect("frameRect", frame->frameRect());
    w.rect("frameContentsRect", frame->contentsRect());
  }
}

static void measure_push_button(writer& w, QWidget* raw)
{
  QPushButton* btn = static_cast<QPushButton*>(raw);
  QStyle* style = btn->style();
  QStyleOptionButton opt;
  opt.initFrom(btn);
  opt.text = btn->text();
  w.rect("contents", style->subElementRect(QStyle::SE_PushButtonContents, &opt, btn));
  w.rect("focusRect", style->subElementRect(QStyle::SE_PushButtonFocusRect, &opt, btn));
  w.rect("bevel", style->subElementRect(QStyle::SE_PushButtonBevel, &opt, btn));
}

static void measure_check_box(writer& w, QWidget* raw)
{
  QCheckBox* cb = static_cast<QCheckBox*>(raw);
  QStyle* style = cb->style();
  QStyleOptionButton opt;
  opt.initFrom(cb);
  opt.text = cb->text();
  w.rect("indicator", style->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, cb));
  w.rect("contents", style->subElementRect(QStyle::SE_CheckBoxContents, &opt, cb));
  w.rect("focusRect", style->subElementRect(QStyle::SE_CheckBoxFocusRect, &opt, cb));
  w.rect("clickRect", style->subElementRect(QStyle::SE_CheckBoxClickRect, &opt, cb));
}

static void measure_radio_button(writer& w, QWidget* raw)
{
  QRadioButton* rb = static_cast<QRadioButton*>(raw);
  QStyle* style = rb->style();
  QStyleOptionButton opt;
  opt.initFrom(rb);
  opt.text = rb->text();
  w.rect("indicator", style->subElementRect(QStyle::SE_RadioButtonIndicator, &opt, rb));
  w.rect("contents", style->subElementRect(QStyle::SE_RadioButtonContents, &opt, rb));
  w.rect("focusRect", style->subElementRect(QStyle::SE_RadioButtonFocusRect, &opt, rb));
  w.rect("clickRect", style->subElementRect(QStyle::SE_RadioButtonClickRect, &opt, rb));
}

static void measure_group_box(writer& w, QWidget* raw)
{
  QGroupBox* gb = static_cast<QGroupBox*>(raw);
  QStyle* style = gb->style();
  QStyleOptionGroupBox opt;
  opt.initFrom(gb);
  opt.text = gb->title();
  opt.subControls = QStyle::SC_All;
  w.rect("frame", style->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxFrame, gb));
  w.rect("label", style->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxLabel, gb));
  w.rect("checkBox", style->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxCheckBox, gb));
  w.sz("titleTextSize", QFontMetrics(gb->font()).size(Qt::TextShowMnemonic, gb->title()));
  w.i("flat", gb->isFlat());
}

static void measure_line_edit(writer& w, QWidget* raw)
{
  QLineEdit* le = static_cast<QLineEdit*>(raw);
  QStyle* style = le->style();
  QStyleOptionFrame opt;
  opt.initFrom(le);
  w.rect("contents", style->subElementRect(QStyle::SE_LineEditContents, &opt, le));
  w.margin("textMargins", le->textMargins());
}

static void measure_combo_box(writer& w, QWidget* raw)
{
  QComboBox* cb = static_cast<QComboBox*>(raw);
  QStyle* style = cb->style();
  QStyleOptionComboBox opt;
  opt.initFrom(cb);
  opt.editable = false;
  opt.subControls = QStyle::SC_All;
  w.rect("frame", style->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxFrame, cb));
  w.rect("arrow", style->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, cb));
  w.rect("editField", style->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, cb));
  w.rect("listBoxPopup", style->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxListBoxPopup, cb));
  QAbstractItemView* view = cb->view();
  if(view != nullptr)
    w.v("viewRowHeight", view->sizeHintForRow(0));
  w.i("count", cb->count());
}

static void measure_scroll_bar(writer& w, QWidget* raw)
{
  QScrollBar* sb = static_cast<QScrollBar*>(raw);
  QStyle* style = sb->style();
  QStyleOptionSlider opt;
  opt.initFrom(sb);
  opt.orientation = sb->orientation();
  opt.minimum = sb->minimum();
  opt.maximum = sb->maximum();
  opt.sliderValue = sb->sliderPosition();
  opt.sliderPosition = sb->sliderPosition();
  opt.pageStep = sb->pageStep();
  opt.singleStep = sb->singleStep();
  opt.subControls = QStyle::SC_All;
  opt.activeSubControls = QStyle::SC_None;
  w.rect("groove", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, sb));
  w.rect("handle", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, sb));
  w.rect("subLine", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSubLine, sb));
  w.rect("addLine", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarAddLine, sb));
  w.rect("subPage", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSubPage, sb));
  w.rect("addPage", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarAddPage, sb));
  w.rect("first", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarFirst, sb));
  w.rect("last", style->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarLast, sb));
}

static void measure_tab_widget(writer& w, QWidget* raw)
{
  QTabWidget* tw = static_cast<QTabWidget*>(raw);
  QTabBar* tab = tw->tabBar();
  QStyle* style = tw->style();
  w.rect("tab0Rect", tab->tabRect(0));
  w.sz("tabBarSizeHint", tab->sizeHint());
  QStyleOptionTabWidgetFrame opt;
  opt.initFrom(tw);
  opt.tabBarSize = tab->sizeHint();
  w.rect("pane", style->subElementRect(QStyle::SE_TabWidgetTabPane, &opt, tw));
  w.rect("tabBarRect", style->subElementRect(QStyle::SE_TabWidgetTabBar, &opt, tw));
  w.rect("leftCorner", style->subElementRect(QStyle::SE_TabWidgetLeftCorner, &opt, tw));
  w.rect("rightCorner", style->subElementRect(QStyle::SE_TabWidgetRightCorner, &opt, tw));
  w.i("tabCount", tab->count());
}

static void measure_progress_bar(writer& w, QWidget* raw)
{
  QProgressBar* pb = static_cast<QProgressBar*>(raw);
  QStyle* style = pb->style();
  QStyleOptionProgressBar opt;
  opt.initFrom(pb);
  opt.minimum = pb->minimum();
  opt.maximum = pb->maximum();
  opt.progress = pb->value();
  opt.text = pb->text();
  opt.textVisible = pb->isTextVisible();
  opt.orientation = pb->orientation();
  w.rect("groove", style->subElementRect(QStyle::SE_ProgressBarGroove, &opt, pb));
  w.rect("contents", style->subElementRect(QStyle::SE_ProgressBarContents, &opt, pb));
  w.rect("label", style->subElementRect(QStyle::SE_ProgressBarLabel, &opt, pb));
}

static void measure_slider(writer& w, QWidget* raw)
{
  QSlider* sl = static_cast<QSlider*>(raw);
  QStyle* style = sl->style();
  QStyleOptionSlider opt;
  opt.initFrom(sl);
  opt.orientation = sl->orientation();
  opt.minimum = sl->minimum();
  opt.maximum = sl->maximum();
  opt.sliderValue = sl->value();
  opt.sliderPosition = sl->sliderPosition();
  opt.singleStep = sl->singleStep();
  opt.pageStep = sl->pageStep();
  opt.tickPosition = sl->tickPosition();
  opt.subControls = QStyle::SC_All;
  opt.activeSubControls = QStyle::SC_None;
  w.rect("groove", style->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, sl));
  w.rect("handle", style->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, sl));
  w.rect("tickmarks", style->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderTickmarks, sl));
}

static void measure_abstract_spin(writer& w, QWidget* raw)
{
  QAbstractSpinBox* sp = static_cast<QAbstractSpinBox*>(raw);
  QStyle* style = sp->style();
  QStyleOptionSpinBox opt;
  opt.initFrom(sp);
  opt.frame = sp->hasFrame();
  opt.buttonSymbols = sp->buttonSymbols();
  opt.subControls = QStyle::SC_All;
  opt.activeSubControls = QStyle::SC_None;
  w.rect("frame", style->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxFrame, sp));
  w.rect("up", style->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, sp));
  w.rect("down", style->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxDown, sp));
  w.rect("editField", style->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxEditField, sp));
  QLineEdit* internal_edit = sp->findChild<QLineEdit*>();
  if(internal_edit != nullptr)
    w.rect("lineEditRect", internal_edit->geometry());
}

static void measure_calendar_widget(writer& w, QWidget* raw)
{
  QCalendarWidget* cw = static_cast<QCalendarWidget*>(raw);
  const QList<QHeaderView*> headers = cw->findChildren<QHeaderView*>();
  for(const QHeaderView* header : headers)
  {
    if(header->orientation() == Qt::Horizontal)
    {
      w.h("headerSectionSizeHint", header->sectionSizeHint(0));
      w.h("headerDefaultSectionSize", header->defaultSectionSize());
      w.sz("headerSizeHint", header->sizeHint());
    }
    else
    {
      w.v("verticalHeaderDefaultSectionSize", header->defaultSectionSize());
      w.sz("verticalHeaderSizeHint", header->sizeHint());
    }
  }
  w.sz("dateTextSize", QFontMetrics(cw->font()).size(Qt::TextSingleLine, QStringLiteral("31")));
  w.i("gridVisible", cw->isGridVisible());
}

static void measure_tool_bar(writer& w, QWidget* raw)
{
  QToolBar* tb = static_cast<QToolBar*>(raw);
  QStyle* style = tb->style();
  w.sz("iconSize", tb->iconSize());
  w.i("toolButtonStyle", tb->toolButtonStyle());
  w.i("movable", tb->isMovable());
  w.i("floatable", tb->isFloatable());
  w.i("actionCount", tb->actions().size());
  QStyleOptionToolBar opt;
  opt.initFrom(tb);
  opt.features = tb->isMovable() ? QStyleOptionToolBar::Movable : QStyleOptionToolBar::None;
  opt.positionWithinLine = QStyleOptionToolBar::OnlyOne;
  w.rect("handle", style->subElementRect(QStyle::SE_ToolBarHandle, &opt, tb));
  int idx = 0;
  for(QAction* action : tb->actions())
  {
    w.rect(QString("action%1Rect").arg(idx), tb->actionGeometry(action));
    ++idx;
  }
}

static void measure_header_view(writer& w, QWidget* raw)
{
  QHeaderView* hv = static_cast<QHeaderView*>(raw);
  w.h("sectionSizeHint0", hv->sectionSizeHint(0));
  w.h("defaultSectionSize", hv->defaultSectionSize());
  w.h("offset", hv->offset());
  w.i("count", hv->count());
  w.rect("viewportRect", hv->viewport()->rect());
}

static void measure_list_widget(writer& w, QWidget* raw)
{
  QListWidget* lw = static_cast<QListWidget*>(raw);
  w.v("rowHeight0", lw->sizeHintForRow(0));
  w.i("count", lw->count());
}

static void measure_tree_widget(writer& w, QWidget* raw)
{
  QTreeWidget* tw = static_cast<QTreeWidget*>(raw);
  QHeaderView* header = tw->header();
  if(header != nullptr)
  {
    w.h("headerSectionSizeHint", header->sectionSizeHint(0));
    w.h("headerDefaultSectionSize", header->defaultSectionSize());
  }
  QTreeWidgetItem* item = tw->topLevelItem(0);
  if(item != nullptr)
    w.v("rowHeight0", tw->visualItemRect(item).height());
}

static void measure_table_widget(writer& w, QWidget* raw)
{
  QTableWidget* tw = static_cast<QTableWidget*>(raw);
  QHeaderView* header = tw->horizontalHeader();
  if(header != nullptr)
  {
    w.h("headerSectionSizeHint", header->sectionSizeHint(0));
    w.h("headerDefaultSectionSize", header->defaultSectionSize());
  }
  w.v("rowHeight0", tw->verticalHeader()->defaultSectionSize());
  w.h("columnWidth0", tw->columnWidth(0));
}

static void measure_menu_bar(writer& w, QWidget* raw)
{
  QMenuBar* mb = static_cast<QMenuBar*>(raw);
  w.sz("sizeHint", mb->sizeHint());
  int idx = 0;
  for(QAction* action : mb->actions())
  {
    w.rect(QString("action%1Rect").arg(idx), mb->actionGeometry(action));
    ++idx;
  }
  w.i("actionCount", mb->actions().size());
}

static void measure_menu(writer& w, QWidget* raw)
{
  QMenu* menu = static_cast<QMenu*>(raw);
  menu->adjustSize();
  w.sz("sizeHint", menu->sizeHint());
  int idx = 0;
  for(QAction* action : menu->actions())
  {
    QRect r = menu->actionGeometry(action);
    w.rect(QString("action%1Rect").arg(idx), r);
    w.v(QString("action%1Height").arg(idx), r.height());
    ++idx;
  }
  w.i("actionCount", menu->actions().size());
}

static void measure_dialog(writer& w, QWidget* raw)
{
  QDialog* dlg = static_cast<QDialog*>(raw);
  QLayout* layout = dlg->layout();
  if(layout != nullptr)
  {
    w.margin("layoutContentsMargin", layout->contentsMargins());
    w.i("layoutSpacing", layout->spacing());
  }
}

static void measure_status_bar(writer& w, QWidget* raw)
{
  QStatusBar* sb = static_cast<QStatusBar*>(raw);
  w.i("sizeGripEnabled", sb->isSizeGripEnabled());
}

static QSize minimum_test_size(QWidget* widget)
{
  QSize min_hint = widget->minimumSizeHint();
  if(min_hint.isValid() && min_hint.width() > 0 && min_hint.height() > 0)
    return min_hint;
  QSize hint = widget->sizeHint();
  if(hint.isValid() && hint.width() > 0 && hint.height() > 0)
    return hint;
  return QSize(64, 24);
}

struct pm_entry
{
  const char* name;
  QStyle::PixelMetric value;
};

static const std::vector<pm_entry> kPixelMetrics =
{
  { "PM_ButtonMargin", QStyle::PM_ButtonMargin },
  { "PM_ButtonDefaultIndicator", QStyle::PM_ButtonDefaultIndicator },
  { "PM_ButtonShiftHorizontal", QStyle::PM_ButtonShiftHorizontal },
  { "PM_ButtonShiftVertical", QStyle::PM_ButtonShiftVertical },
  { "PM_DefaultFrameWidth", QStyle::PM_DefaultFrameWidth },
  { "PM_ComboBoxFrameWidth", QStyle::PM_ComboBoxFrameWidth },
  { "PM_SpinBoxFrameWidth", QStyle::PM_SpinBoxFrameWidth },
  { "PM_LayoutLeftMargin", QStyle::PM_LayoutLeftMargin },
  { "PM_LayoutTopMargin", QStyle::PM_LayoutTopMargin },
  { "PM_LayoutRightMargin", QStyle::PM_LayoutRightMargin },
  { "PM_LayoutBottomMargin", QStyle::PM_LayoutBottomMargin },
  { "PM_LayoutHorizontalSpacing", QStyle::PM_LayoutHorizontalSpacing },
  { "PM_LayoutVerticalSpacing", QStyle::PM_LayoutVerticalSpacing },
  { "PM_FocusFrameHMargin", QStyle::PM_FocusFrameHMargin },
  { "PM_FocusFrameVMargin", QStyle::PM_FocusFrameVMargin },
  { "PM_ScrollBarExtent", QStyle::PM_ScrollBarExtent },
  { "PM_ScrollBarSliderMin", QStyle::PM_ScrollBarSliderMin },
  { "PM_IndicatorWidth", QStyle::PM_IndicatorWidth },
  { "PM_IndicatorHeight", QStyle::PM_IndicatorHeight },
  { "PM_ExclusiveIndicatorWidth", QStyle::PM_ExclusiveIndicatorWidth },
  { "PM_ExclusiveIndicatorHeight", QStyle::PM_ExclusiveIndicatorHeight },
  { "PM_ListViewIconSize", QStyle::PM_ListViewIconSize },
  { "PM_SliderThickness", QStyle::PM_SliderThickness },
  { "PM_SliderLength", QStyle::PM_SliderLength },
  { "PM_SliderTickmarkOffset", QStyle::PM_SliderTickmarkOffset },
  { "PM_SliderSpaceAvailable", QStyle::PM_SliderSpaceAvailable },
  { "PM_SliderControlThickness", QStyle::PM_SliderControlThickness },
  { "PM_ProgressBarChunkWidth", QStyle::PM_ProgressBarChunkWidth },
  { "PM_ToolBarHandleExtent", QStyle::PM_ToolBarHandleExtent },
  { "PM_ToolBarItemMargin", QStyle::PM_ToolBarItemMargin },
  { "PM_ToolBarItemSpacing", QStyle::PM_ToolBarItemSpacing },
  { "PM_ToolBarSeparatorExtent", QStyle::PM_ToolBarSeparatorExtent },
  { "PM_ToolBarIconSize", QStyle::PM_ToolBarIconSize },
  { "PM_ToolBarFrameWidth", QStyle::PM_ToolBarFrameWidth },
  { "PM_MenuBarPanelWidth", QStyle::PM_MenuBarPanelWidth },
  { "PM_MenuBarHMargin", QStyle::PM_MenuBarHMargin },
  { "PM_MenuBarVMargin", QStyle::PM_MenuBarVMargin },
  { "PM_MenuBarItemSpacing", QStyle::PM_MenuBarItemSpacing },
  { "PM_MenuHMargin", QStyle::PM_MenuHMargin },
  { "PM_MenuVMargin", QStyle::PM_MenuVMargin },
  { "PM_MenuPanelWidth", QStyle::PM_MenuPanelWidth },
  { "PM_MenuTearoffHeight", QStyle::PM_MenuTearoffHeight },
  { "PM_MenuDesktopFrameWidth", QStyle::PM_MenuDesktopFrameWidth },
  { "PM_SubMenuOverlap", QStyle::PM_SubMenuOverlap },
  { "PM_TabBarTabOverlap", QStyle::PM_TabBarTabOverlap },
  { "PM_TabBarBaseHeight", QStyle::PM_TabBarBaseHeight },
  { "PM_TabBarBaseOverlap", QStyle::PM_TabBarBaseOverlap },
  { "PM_TabBarTabVSpace", QStyle::PM_TabBarTabVSpace },
  { "PM_TabBarTabHSpace", QStyle::PM_TabBarTabHSpace },
  { "PM_TabBarScrollButtonWidth", QStyle::PM_TabBarScrollButtonWidth },
  { "PM_HeaderMargin", QStyle::PM_HeaderMargin },
  { "PM_HeaderMarkSize", QStyle::PM_HeaderMarkSize },
  { "PM_HeaderGripMargin", QStyle::PM_HeaderGripMargin },
  { "PM_TitleBarHeight", QStyle::PM_TitleBarHeight },
  { "PM_DockWidgetTitleBarButtonMargin", QStyle::PM_DockWidgetTitleBarButtonMargin },
};

static void write_pixel_metrics(writer& w, QStyle* style, QWidget* base)
{
  for(const pm_entry& e : kPixelMetrics)
    w.i(QString("pixelMetric.%1").arg(e.name), style->pixelMetric(e.value, nullptr, base));
}

static void write_font_section(writer& w, const QFont& font)
{
  QFontMetrics fm(font);
  QString alphabet = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  double alphabet_advance = static_cast<double>(fm.horizontalAdvance(alphabet));
  double avg_char_width = alphabet_advance / 52.0;
  g_dlu_x_factor = avg_char_width / 4.0;
  g_dlu_y_factor = static_cast<double>(fm.height()) / 8.0;

  w.section(QStringLiteral("font"));
  w.i("dpi", qRound(QGuiApplication::primaryScreen() ? QGuiApplication::primaryScreen()->logicalDotsPerInch() : 100.0));
  w.kv("family", font.family());
  w.i("pointSize", font.pointSize());
  w.i("pixelSize", font.pixelSize());
  w.i("weight", font.weight());
  w.i("italic", font.italic());
  w.i("height", fm.height());
  w.i("ascent", fm.ascent());
  w.i("descent", fm.descent());
  w.i("leading", fm.leading());
  w.i("lineSpacing", fm.lineSpacing());
  w.i("xHeight", fm.xHeight());
  w.i("capHeight", fm.capHeight());
  w.i("averageCharWidth", fm.averageCharWidth());
  w.i("maxWidth", fm.maxWidth());
  w.i("underlinePos", fm.underlinePos());
  w.i("strikeOutPos", fm.strikeOutPos());
  w.i("overlinePos", fm.overlinePos());
  w.i("alphabetAdvance", static_cast<int>(alphabet_advance));
  w.i("avgCharWidth", static_cast<int>(avg_char_width + 0.5));
  w.kv("dluXFactor", QString::number(g_dlu_x_factor, 'g', 10));
  w.kv("dluYFactor", QString::number(g_dlu_y_factor, 'g', 10));
}

static void print_usage(const char* argv0)
{
  std::printf("Usage: %s [options] [output.txt]\n"
              "Renders rc2qt widget classes offscreen and writes their component\n"
              "metrics to a text file.\n"
              "\n"
              "Options:\n"
              "  --font-family NAME   font family to measure (default: Liberation Sans)\n"
              "  --point-size N       font point size to measure (default: 8)\n"
              "  --style NAME         Qt style to measure (default: platform default)\n"
              "  --image-dir DIR      also dump a PNG render of every widget to DIR\n"
              "  -h, --help           show this help\n",
              argv0);
}

int main(int argc, char** argv)
{
  if(qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
    qputenv("QT_QPA_PLATFORM", "offscreen");

  // Measure fonts at 96 DPI, the value Qt uses when rendering to a screen.
  // This must be set before QApplication is constructed.
  qputenv("QT_FONT_DPI", "96");

  QApplication app(argc, argv);

  QString output_path = QStringLiteral("uimetrics.txt");
  QString font_family = QStringLiteral("Liberation Sans");
  int point_size = 8;
  QString image_dir;

  QStringList args = app.arguments();
  for(int i = 1; i < args.size(); ++i)
  {
    if(args[i] == QStringLiteral("--font-family") && i + 1 < args.size())
    {
      ++i;
      font_family = args[i];
    }
    else if(args[i] == QStringLiteral("--point-size") && i + 1 < args.size())
    {
      ++i;
      point_size = args[i].toInt();
    }
    else if(args[i] == QStringLiteral("--style") && i + 1 < args.size())
    {
      ++i;
      QApplication::setStyle(args[i]);
    }
    else if(args[i] == QStringLiteral("--image-dir") && i + 1 < args.size())
    {
      ++i;
      image_dir = args[i];
    }
    else if(args[i] == QStringLiteral("-h") || args[i] == QStringLiteral("--help"))
    {
      print_usage(argv[0]);
      return 0;
    }
    else if(!args[i].startsWith('-'))
    {
      output_path = args[i];
    }
    else
    {
      print_usage(argv[0]);
      return 2;
    }
  }

  QFont app_font(font_family, point_size);
  app.setFont(app_font);

  if(!image_dir.isEmpty())
    QDir().mkpath(image_dir);

  QFile file(output_path);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    std::fprintf(stderr, "getuimetrics: cannot open '%s' for writing\n",
                 output_path.toUtf8().constData());
    return 1;
  }

  QTextStream out(&file);
  out.setCodec("UTF-8");
  writer w{out};

  out << "# Qt widget UI metrics for the rc2qt generator\n"
      << "# Section-based key=value format. Lengths in [widget:*] sections are\n"
      << "# expressed relative to the measurement font as dialog units (DLU):\n"
      << "# horizontal lengths divide by dluXFactor, vertical lengths by\n"
      << "# dluYFactor from the [font] section. The generator scales them to\n"
      << "# pixels using the actual dialog font so that widget sizes and\n"
      << "# minimum margins are preserved for any font family, size, or style.\n"
      << "# Counts, flags, and font metrics remain in absolute units.\n";

  w.section(QStringLiteral("meta"));
  w.kv("tool", QStringLiteral("getuimetrics"));
  w.kv("toolVersion", QStringLiteral("2.0"));
  w.kv("qtVersion", QStringLiteral(QT_VERSION_STR));
  w.kv("style", app.style()->objectName());
  w.kv("platform", QApplication::platformName());
  w.kv("fontFamily", font_family);
  w.i("pointSize", point_size);
  w.kv("generatedAt", QDateTime::currentDateTime().toString(Qt::ISODate));

  write_font_section(w, app_font);

  w.section(QStringLiteral("style"));
  w.kv("name", app.style()->objectName());
  write_pixel_metrics(w, app.style(), nullptr);

  std::unique_ptr<QPushButton> base_button(new QPushButton(QStringLiteral("Base")));
  base_button->setFont(app_font);
  base_button->show();
  app.processEvents();

  const std::vector<widget_entry> kWidgets =
  {
    { "QWidget", []() -> QWidget* { return new QWidget; }, nullptr, nullptr },
    {
      "QDialog",
      []() -> QWidget*
      {
        QDialog* dlg = new QDialog;
        QVBoxLayout* layout = new QVBoxLayout(dlg);
        layout->addWidget(new QPushButton(QStringLiteral("Button")));
        layout->addWidget(new QLineEdit(QStringLiteral("Edit")));
        return dlg;
      },
      nullptr,
      measure_dialog
    },
    { "QPushButton", []() -> QWidget* { return new QPushButton(QStringLiteral("&OK")); }, nullptr, measure_push_button },
    { "QCheckBox", []() -> QWidget* { return new QCheckBox(QStringLiteral("Check Box")); }, nullptr, measure_check_box },
    { "QRadioButton", []() -> QWidget* { return new QRadioButton(QStringLiteral("Radio Button")); }, nullptr, measure_radio_button },
    { "QGroupBox", []() -> QWidget* { return new QGroupBox(QStringLiteral("Group Box")); }, nullptr, measure_group_box },
    { "QLabel", []() -> QWidget* { return new QLabel(QStringLiteral("Text Label")); }, nullptr, nullptr },
    { "QLineEdit", []() -> QWidget* { return new QLineEdit(QStringLiteral("Edit")); }, nullptr, measure_line_edit },
    { "QTextEdit", []() -> QWidget* { return new QTextEdit(QStringLiteral("Multi-line\ntext")); }, nullptr, nullptr },
    {
      "QListWidget",
      []() -> QWidget* { return new QListWidget; },
      [](QWidget* raw)
      {
        static_cast<QListWidget*>(raw)->addItems({ QStringLiteral("Item 1"), QStringLiteral("Item 2"), QStringLiteral("Item 3") });
      },
      measure_list_widget
    },
    {
      "QComboBox",
      []() -> QWidget* { return new QComboBox; },
      [](QWidget* raw)
      {
        static_cast<QComboBox*>(raw)->addItems({ QStringLiteral("Item 1"), QStringLiteral("Item 2") });
      },
      measure_combo_box
    },
    {
      "QScrollBar",
      []() -> QWidget*
      {
        QScrollBar* sb = new QScrollBar(Qt::Vertical);
        sb->setRange(0, 100);
        sb->setValue(50);
        return sb;
      },
      nullptr,
      measure_scroll_bar
    },
    {
      "QTabWidget",
      []() -> QWidget*
      {
        QTabWidget* tw = new QTabWidget;
        tw->addTab(new QLabel(QStringLiteral("Tab 1")), QStringLiteral("Tab 1"));
        tw->addTab(new QLabel(QStringLiteral("Tab 2")), QStringLiteral("Tab 2"));
        return tw;
      },
      nullptr,
      measure_tab_widget
    },
    {
      "QTreeWidget",
      []() -> QWidget* { return new QTreeWidget; },
      [](QWidget* raw)
      {
        QTreeWidget* tw = static_cast<QTreeWidget*>(raw);
        tw->setColumnCount(2);
        tw->setHeaderLabels({ QStringLiteral("Column 1"), QStringLiteral("Column 2") });
        new QTreeWidgetItem(tw, { QStringLiteral("Item"), QStringLiteral("Value") });
      },
      measure_tree_widget
    },
    {
      "QTableWidget",
      []() -> QWidget* { return new QTableWidget(2, 2); },
      [](QWidget* raw)
      {
        QTableWidget* tw = static_cast<QTableWidget*>(raw);
        tw->setHorizontalHeaderLabels({ QStringLiteral("A"), QStringLiteral("B") });
        tw->setItem(0, 0, new QTableWidgetItem(QStringLiteral("x")));
        tw->setItem(1, 0, new QTableWidgetItem(QStringLiteral("y")));
      },
      measure_table_widget
    },
    {
      "QProgressBar",
      []() -> QWidget*
      {
        QProgressBar* pb = new QProgressBar;
        pb->setRange(0, 100);
        pb->setValue(50);
        return pb;
      },
      nullptr,
      measure_progress_bar
    },
    {
      "QSlider",
      []() -> QWidget*
      {
        QSlider* sl = new QSlider(Qt::Horizontal);
        sl->setRange(0, 100);
        sl->setValue(50);
        sl->setTickPosition(QSlider::TicksBelow);
        return sl;
      },
      nullptr,
      measure_slider
    },
    { "QSpinBox", []() -> QWidget* { return new QSpinBox; }, nullptr, measure_abstract_spin },
    {
      "QDateTimeEdit",
      []() -> QWidget*
      {
        QDateTimeEdit* de = new QDateTimeEdit(QDateTime::currentDateTime());
        return de;
      },
      nullptr,
      measure_abstract_spin
    },
    { "QCalendarWidget", []() -> QWidget* { return new QCalendarWidget; }, nullptr, measure_calendar_widget },
    {
      "QToolBar",
      []() -> QWidget*
      {
        QToolBar* tb = new QToolBar;
        tb->setMovable(true);
        tb->addAction(QStringLiteral("&New"));
        tb->addAction(QStringLiteral("&Open"));
        return tb;
      },
      nullptr,
      measure_tool_bar
    },
    {
      "QStackedWidget",
      []() -> QWidget*
      {
        QStackedWidget* sw = new QStackedWidget;
        sw->addWidget(new QLabel(QStringLiteral("Page 1")));
        sw->addWidget(new QLabel(QStringLiteral("Page 2")));
        return sw;
      },
      nullptr,
      nullptr
    },
    {
      "QStatusBar",
      []() -> QWidget*
      {
        QStatusBar* sb = new QStatusBar;
        sb->showMessage(QStringLiteral("Status message"));
        return sb;
      },
      nullptr,
      measure_status_bar
    },
    {
      "QHeaderView",
      []() -> QWidget* { return new QHeaderView(Qt::Horizontal); },
      [](QWidget* raw)
      {
        QHeaderView* hv = static_cast<QHeaderView*>(raw);
        QStandardItemModel* model = new QStandardItemModel(1, 3, hv);
        model->setHorizontalHeaderLabels({ QStringLiteral("A"), QStringLiteral("B"), QStringLiteral("C") });
        hv->setModel(model);
      },
      measure_header_view
    },
    {
      "QMenuBar",
      []() -> QWidget*
      {
        QMenuBar* mb = new QMenuBar;
        QMenu* file = mb->addMenu(QStringLiteral("&File"));
        file->addAction(QStringLiteral("&New"));
        file->addAction(QStringLiteral("&Open"));
        mb->addAction(QStringLiteral("&Help"));
        return mb;
      },
      nullptr,
      measure_menu_bar
    },
    {
      "QMenu",
      []() -> QWidget*
      {
        QMenu* menu = new QMenu(QStringLiteral("&File"));
        menu->addAction(QStringLiteral("&New"));
        menu->addAction(QStringLiteral("&Open"));
        menu->addSeparator();
        menu->addAction(QStringLiteral("E&xit"));
        return menu;
      },
      nullptr,
      measure_menu
    },
  };

  for(const widget_entry& entry : kWidgets)
  {
    std::unique_ptr<QWidget> widget(entry.create());
    widget->setFont(app_font);
    if(entry.populate != nullptr)
      entry.populate(widget.get());
    widget->show();
    app.processEvents();
    widget->resize(minimum_test_size(widget.get()));
    app.processEvents();

    w.section(QString("widget:%1").arg(entry.section));
    measure_common(w, widget.get());
    if(entry.measure != nullptr)
      entry.measure(w, widget.get());

    if(!image_dir.isEmpty())
    {
      QPixmap pixmap(widget->size());
      widget->render(&pixmap);
      pixmap.save(QDir(image_dir).filePath(QString("%1.png").arg(entry.section)));
    }

    std::cout << "measured " << entry.section << " (" << widget->size().width()
              << "x" << widget->size().height() << ")\n";
  }

  file.close();
  std::cout << "wrote " << output_path.toStdString() << "\n";
  return 0;
}
