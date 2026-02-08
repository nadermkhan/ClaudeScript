#define QT_NO_KEYWORDS
#define QT_STATICPLUGIN

#ifdef QT_STATICPLUGIN
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QMetaObject>
#include <QProgressBar>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSlider>
#include <QSpacerItem>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

typedef void (*RtVoidCallback)(void);
typedef void (*RtIntCallback)(long long);
typedef void (*RtStringCallback)(const char *);

static const char *DARK_STYLESHEET = R"(
    * {
        outline: none;
    }
    QWidget#CSMainWindow {
        background-color: #18181C;
    }
    QWidget {
        background-color: transparent;
        color: #F0F0F0;
        font-family: "Segoe UI", "Ubuntu", "Cantarell", sans-serif;
        font-size: 14px;
    }
    QPushButton {
        background-color: #37373E;
        border: 1px solid #3A3A40;
        border-radius: 6px;
        padding: 8px 20px;
        min-height: 20px;
        color: #F0F0F0;
    }
    QPushButton:hover {
        background-color: #44444C;
    }
    QPushButton:pressed {
        background-color: #303036;
    }
    QPushButton:disabled {
        background-color: #2D2D32;
        color: #5A5A60;
        border-color: #2D2D32;
    }
    QPushButton[accent="true"] {
        background-color: #0067C0;
        border: none;
        color: #FFFFFF;
    }
    QPushButton[accent="true"]:hover {
        background-color: #1078D2;
    }
    QPushButton[accent="true"]:pressed {
        background-color: #0056A3;
    }
    QPushButton[accent="true"]:disabled {
        background-color: #003D73;
        color: #80B0D8;
    }
    QLabel {
        color: #A0A0A8;
        background-color: transparent;
        padding: 2px 0px;
    }
    QLabel[heading="true"] {
        color: #F0F0F0;
        font-weight: bold;
    }
    QLabel[title="true"] {
        color: #F0F0F0;
        font-weight: bold;
        font-size: 24px;
    }
    QLabel[h1="true"] {
        color: #F0F0F0;
        font-weight: bold;
        font-size: 32px;
    }
    QLineEdit {
        background-color: #26262A;
        border: 1px solid #3A3A40;
        border-radius: 6px;
        padding: 8px 10px;
        min-height: 20px;
        color: #F0F0F0;
        selection-background-color: #0067C0;
        selection-color: #FFFFFF;
    }
    QLineEdit:focus {
        border: 2px solid #0067C0;
        padding: 7px 9px;
    }
    QLineEdit:disabled {
        background-color: #1E1E22;
        color: #5A5A60;
    }
    QCheckBox {
        spacing: 8px;
        color: #F0F0F0;
    }
    QCheckBox::indicator {
        width: 20px;
        height: 20px;
        border-radius: 4px;
        border: 1px solid #3A3A40;
        background-color: #26262A;
    }
    QCheckBox::indicator:hover {
        border-color: #50505A;
    }
    QCheckBox::indicator:checked {
        background-color: #0067C0;
        border: none;
    }
    QCheckBox::indicator:disabled {
        background-color: #1E1E22;
        border-color: #2D2D32;
    }
    QSlider::groove:horizontal {
        height: 4px;
        background: #3A3A40;
        border-radius: 2px;
    }
    QSlider::sub-page:horizontal {
        background: #0067C0;
        border-radius: 2px;
    }
    QSlider::handle:horizontal {
        width: 20px;
        height: 20px;
        margin: -8px 0;
        background: #0067C0;
        border-radius: 10px;
    }
    QSlider::handle:horizontal:hover {
        background: #1078D2;
    }
    QSlider::handle:horizontal:pressed {
        background: #0056A3;
    }
    QProgressBar {
        background-color: #303036;
        border: none;
        border-radius: 4px;
        max-height: 8px;
        min-height: 8px;
        text-align: center;
    }
    QProgressBar::chunk {
        background-color: #0067C0;
        border-radius: 4px;
    }
    QFrame#CSSeparator {
        background-color: #34343A;
        max-height: 1px;
        min-height: 1px;
    }
    QScrollArea {
        border: none;
        background-color: transparent;
    }
    QScrollBar:vertical {
        background: #1E1E22;
        width: 8px;
        border-radius: 4px;
    }
    QScrollBar::handle:vertical {
        background: #3A3A40;
        border-radius: 4px;
        min-height: 30px;
    }
    QScrollBar::handle:vertical:hover {
        background: #50505A;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
    }
    QScrollBar:horizontal {
        background: #1E1E22;
        height: 8px;
        border-radius: 4px;
    }
    QScrollBar::handle:horizontal {
        background: #3A3A40;
        border-radius: 4px;
        min-width: 30px;
    }
    QScrollBar::handle:horizontal:hover {
        background: #50505A;
    }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
        width: 0px;
    }
)";

enum CSWidgetType {
  CS_NONE = 0,
  CS_BUTTON,
  CS_LABEL,
  CS_INPUT,
  CS_CHECKBOX,
  CS_TOGGLE,
  CS_SLIDER,
  CS_PROGRESS,
  CS_SEPARATOR,
  CS_SPACER
};

struct CSWidget {
  CSWidgetType type = CS_NONE;
  QWidget *widget = nullptr;
  QString name;
  RtVoidCallback onClick = nullptr;
  RtVoidCallback onChange = nullptr;
  RtStringCallback onTextChange = nullptr;
  bool isAccent = false;
};

class CSMainWindow : public QWidget {
  Q_OBJECT
public:
  RtVoidCallback onCreateCb = nullptr;
  RtVoidCallback onDestroyCb = nullptr;
  RtVoidCallback onResumeCb = nullptr;
  RtVoidCallback onPauseCb = nullptr;
  RtVoidCallback onBackCb = nullptr;

  explicit CSMainWindow(QWidget *parent = nullptr) : QWidget(parent) {
    setObjectName("CSMainWindow");
  }

protected:
  void keyPressEvent(QKeyEvent *e) override {
    if (e->key() == Qt::Key_Escape && onBackCb) {
      onBackCb();
      return;
    }
    QWidget::keyPressEvent(e);
  }
  void closeEvent(QCloseEvent *e) override {
    if (onDestroyCb)
      onDestroyCb();
    QWidget::closeEvent(e);
  }
  void showEvent(QShowEvent *e) override {
    QWidget::showEvent(e);
    if (onResumeCb)
      onResumeCb();
  }
  void hideEvent(QHideEvent *e) override {
    if (onPauseCb)
      onPauseCb();
    QWidget::hideEvent(e);
  }
};

struct Activity {
  int id = -1;
  CSMainWindow *window = nullptr;
  QVBoxLayout *rootLayout = nullptr;
  int firstWidget = 0;
  int widgetCount = 0;
};

static QApplication *g_app = nullptr;
static std::deque<CSWidget> g_widgets;
static std::stack<QLayout *> g_layoutStack;
static std::vector<Activity> g_activities;
static int g_currentActivity = -1;
static std::recursive_mutex g_widgetMutex;
static std::atomic<bool> g_shuttingDown{false};

// String pool to keep returned strings alive until window closes
static std::vector<std::string *> g_stringPool;
static std::mutex g_stringPoolMutex;

static const char *poolStr(std::string s) {
  std::lock_guard<std::mutex> lk(g_stringPoolMutex);
  auto *p = new std::string(std::move(s));
  g_stringPool.push_back(p);
  return p->c_str();
}

static void clearStringPool() {
  std::lock_guard<std::mutex> lk(g_stringPoolMutex);
  for (auto *p : g_stringPool)
    delete p;
  g_stringPool.clear();
}

struct TaskInfo {
  std::atomic<bool> completed{false};
  std::recursive_mutex mtx;
  std::condition_variable_any cv;
  std::vector<RtVoidCallback> thenCallbacks;
  std::vector<RtVoidCallback> catchCallbacks;
  bool hadError = false;
  bool erased = false;
};

static std::recursive_mutex g_taskMapMutex;
static std::unordered_map<long long, std::shared_ptr<TaskInfo>> g_taskMap;
static std::atomic<long long> g_taskIdGen{1};
static std::vector<std::thread> g_taskThreads;
static std::mutex g_taskThreadsMutex;

static CSMainWindow *currentWindow() {
  if (g_currentActivity >= 0 && g_currentActivity < (int)g_activities.size())
    return g_activities[g_currentActivity].window;
  return nullptr;
}

static QLayout *currentLayout() {
  if (!g_layoutStack.empty())
    return g_layoutStack.top();
  return nullptr;
}

static void addWidgetToLayout(QWidget *w) {
  QLayout *lay = currentLayout();
  if (lay)
    lay->addWidget(w);
}

static void addLayoutToLayout(QLayout *child) {
  QLayout *lay = currentLayout();
  if (!lay)
    return;
  if (auto *vbox = dynamic_cast<QVBoxLayout *>(lay)) {
    vbox->addLayout(child);
  } else if (auto *hbox = dynamic_cast<QHBoxLayout *>(lay)) {
    hbox->addLayout(child);
  } else {
    lay->addItem(child);
  }
}

static void ensureApp() {
  if (!g_app) {
    static int argc = 1;
    static char arg0[] = "claudescript";
    static char *argv[] = {arg0, nullptr};
    g_app = new QApplication(argc, argv);
    g_app->setStyleSheet(DARK_STYLESHEET);
    QFont defaultFont("Segoe UI", 10);
    defaultFont.setStyleStrategy(QFont::PreferAntialias);
    g_app->setFont(defaultFont);
  }
}

static bool validHandle(long long h) {
  return h >= 0 && h < (long long)g_widgets.size();
}

extern "C" void rt_window_create(const char *title, int w, int h) {
  ensureApp();
  g_shuttingDown = false;
  Activity act;
  act.id = (int)g_activities.size();
  act.window = new CSMainWindow();
  act.window->setWindowTitle(QString::fromUtf8(title));
  int ww = w > 0 ? w : 400;
  int wh = h > 0 ? h : 300;
  act.window->resize(ww, wh);
  QScreen *screen = QApplication::primaryScreen();
  if (screen) {
    QRect screenGeom = screen->availableGeometry();
    int sx = (screenGeom.width() - ww) / 2 + screenGeom.x();
    int sy = (screenGeom.height() - wh) / 2 + screenGeom.y();
    act.window->move(sx, sy);
  }
  act.rootLayout = new QVBoxLayout(act.window);
  act.rootLayout->setContentsMargins(16, 16, 16, 16);
  act.rootLayout->setSpacing(10);
  act.firstWidget = (int)g_widgets.size();
  act.widgetCount = 0;
  g_currentActivity = act.id;
  g_activities.push_back(act);
  while (!g_layoutStack.empty())
    g_layoutStack.pop();
  g_layoutStack.push(act.rootLayout);
}

extern "C" void rt_window_end(void) {
  if (g_currentActivity < 0 || g_currentActivity >= (int)g_activities.size())
    return;
  Activity &act = g_activities[g_currentActivity];
  act.widgetCount = (int)g_widgets.size() - act.firstWidget;
  act.rootLayout->addStretch();
  if (act.window->onCreateCb)
    act.window->onCreateCb();
  act.window->show();
  g_app->exec();

  g_shuttingDown = true;

  {
    std::lock_guard<std::mutex> lk(g_taskThreadsMutex);
    for (auto &t : g_taskThreads) {
      if (t.joinable())
        t.join();
    }
    g_taskThreads.clear();
  }

  {
    std::lock_guard<std::recursive_mutex> lk(g_taskMapMutex);
    g_taskMap.clear();
  }

  g_app->processEvents();

  {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    for (auto &a : g_activities) {
      if (a.window) {
        delete a.window;
        a.window = nullptr;
      }
    }
    g_activities.clear();
    g_widgets.clear();
  }

  while (!g_layoutStack.empty())
    g_layoutStack.pop();
  clearStringPool();
  delete g_app;
  g_app = nullptr;
}

extern "C" void rt_begin_column(void) {
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(10);
  addLayoutToLayout(layout);
  g_layoutStack.push(layout);
}

extern "C" void rt_begin_row(void) {
  auto *layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(10);
  addLayoutToLayout(layout);
  g_layoutStack.push(layout);
}

extern "C" void rt_end_layout(void) {
  if (g_layoutStack.size() > 1)
    g_layoutStack.pop();
}

extern "C" long long rt_create_button(const char *text) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *btn = new QPushButton(QString::fromUtf8(text));
  btn->setCursor(Qt::PointingHandCursor);
  btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_BUTTON;
  cw.widget = btn;
  g_widgets.push_back(cw);
  addWidgetToLayout(btn);
  QObject::connect(btn, &QPushButton::clicked, [idx]() {
    std::lock_guard<std::recursive_mutex> lk2(g_widgetMutex);
    if (g_shuttingDown)
      return;
    if (validHandle(idx) && g_widgets[idx].onClick)
      g_widgets[idx].onClick();
  });
  return idx;
}

extern "C" long long rt_create_label(const char *text) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *lbl = new QLabel(QString::fromUtf8(text));
  lbl->setWordWrap(true);
  lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_LABEL;
  cw.widget = lbl;
  g_widgets.push_back(cw);
  addWidgetToLayout(lbl);
  return idx;
}

extern "C" long long rt_create_input(const char *hint) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *edit = new QLineEdit();
  if (hint && hint[0])
    edit->setPlaceholderText(QString::fromUtf8(hint));
  edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_INPUT;
  cw.widget = edit;
  g_widgets.push_back(cw);
  addWidgetToLayout(edit);
  QObject::connect(edit, &QLineEdit::textChanged, [idx](const QString &text) {
    std::lock_guard<std::recursive_mutex> lk2(g_widgetMutex);
    if (g_shuttingDown)
      return;
    if (validHandle(idx)) {
      if (g_widgets[idx].onTextChange) {
        QByteArray ba = text.toUtf8();
        g_widgets[idx].onTextChange(ba.constData());
      }
      if (g_widgets[idx].onChange)
        g_widgets[idx].onChange();
    }
  });
  return idx;
}

extern "C" long long rt_create_checkbox(const char *text) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *chk = new QCheckBox(QString::fromUtf8(text));
  chk->setCursor(Qt::PointingHandCursor);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_CHECKBOX;
  cw.widget = chk;
  g_widgets.push_back(cw);
  addWidgetToLayout(chk);
  QObject::connect(chk, &QCheckBox::stateChanged, [idx](int) {
    std::lock_guard<std::recursive_mutex> lk2(g_widgetMutex);
    if (g_shuttingDown)
      return;
    if (validHandle(idx) && g_widgets[idx].onChange)
      g_widgets[idx].onChange();
  });
  return idx;
}

extern "C" long long rt_create_toggle(const char *text) {
  return rt_create_checkbox(text);
}

extern "C" long long rt_create_slider(void) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *sl = new QSlider(Qt::Horizontal);
  sl->setRange(0, 1000);
  sl->setValue(0);
  sl->setCursor(Qt::PointingHandCursor);
  sl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_SLIDER;
  cw.widget = sl;
  g_widgets.push_back(cw);
  addWidgetToLayout(sl);
  QObject::connect(sl, &QSlider::valueChanged, [idx](int) {
    std::lock_guard<std::recursive_mutex> lk2(g_widgetMutex);
    if (g_shuttingDown)
      return;
    if (validHandle(idx) && g_widgets[idx].onChange)
      g_widgets[idx].onChange();
  });
  return idx;
}

extern "C" long long rt_create_progress(void) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *pb = new QProgressBar();
  pb->setRange(0, 1000);
  pb->setValue(0);
  pb->setTextVisible(false);
  pb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_PROGRESS;
  cw.widget = pb;
  g_widgets.push_back(cw);
  addWidgetToLayout(pb);
  return idx;
}

extern "C" long long rt_create_separator(void) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *sep = new QFrame();
  sep->setObjectName("CSSeparator");
  sep->setFrameShape(QFrame::HLine);
  sep->setFrameShadow(QFrame::Plain);
  sep->setFixedHeight(1);
  sep->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_SEPARATOR;
  cw.widget = sep;
  g_widgets.push_back(cw);
  addWidgetToLayout(sep);
  return idx;
}

extern "C" long long rt_create_spacer(int w, int h) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  auto *spacer = new QWidget();
  spacer->setFixedSize(w > 0 ? w : 8, h > 0 ? h : 8);
  spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  long long idx = (long long)g_widgets.size();
  CSWidget cw;
  cw.type = CS_SPACER;
  cw.widget = spacer;
  g_widgets.push_back(cw);
  addWidgetToLayout(spacer);
  return idx;
}

extern "C" void rt_widget_set_id(long long h, const char *id) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  g_widgets[h].name = QString::fromUtf8(id);
  g_widgets[h].widget->setObjectName(QString::fromUtf8(id));
}

extern "C" void rt_widget_on_click(long long h, void (*fn)(void)) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  g_widgets[h].onClick = fn;
}

extern "C" void rt_widget_set_text(long long h, const char *text) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QString t = QString::fromUtf8(text);
  QWidget *w = g_widgets[h].widget;
  if (QThread::currentThread() != g_app->thread()) {
    QMetaObject::invokeMethod(
        w,
        [w, t]() {
          if (auto *btn = qobject_cast<QPushButton *>(w))
            btn->setText(t);
          else if (auto *lbl = qobject_cast<QLabel *>(w))
            lbl->setText(t);
          else if (auto *edit = qobject_cast<QLineEdit *>(w))
            edit->setText(t);
        },
        Qt::QueuedConnection);
  } else {
    if (auto *btn = qobject_cast<QPushButton *>(w))
      btn->setText(t);
    else if (auto *lbl = qobject_cast<QLabel *>(w))
      lbl->setText(t);
    else if (auto *edit = qobject_cast<QLineEdit *>(w))
      edit->setText(t);
  }
}

extern "C" void rt_widget_set_accent(long long h, int a) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  g_widgets[h].isAccent = (a != 0);
  QWidget *w = g_widgets[h].widget;
  auto apply = [w, a]() {
    w->setProperty("accent", a ? true : false);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_visible(long long h, int v) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  bool vis = (v != 0);
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(
        w, [w, vis]() { w->setVisible(vis); }, Qt::QueuedConnection);
  else
    w->setVisible(vis);
}

extern "C" void rt_widget_set_enabled(long long h, int e) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  bool en = (e != 0);
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(
        w, [w, en]() { w->setEnabled(en); }, Qt::QueuedConnection);
  else
    w->setEnabled(en);
}

extern "C" void rt_widget_set_bold(long long h, int b) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  bool bold = (b != 0);
  auto apply = [w, bold]() {
    QFont f = w->font();
    f.setBold(bold);
    w->setFont(f);
    if (auto *lbl = qobject_cast<QLabel *>(w)) {
      lbl->setProperty("heading", bold);
      lbl->style()->unpolish(lbl);
      lbl->style()->polish(lbl);
    }
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_font_size(long long h, int sz) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  auto apply = [w, sz]() {
    QFont f = w->font();
    f.setPointSize(sz > 0 ? sz : 10);
    w->setFont(f);
    if (auto *lbl = qobject_cast<QLabel *>(w)) {
      if (sz >= 32)
        lbl->setProperty("h1", true);
      else if (sz >= 24)
        lbl->setProperty("title", true);
      else if (sz >= 18)
        lbl->setProperty("heading", true);
      lbl->style()->unpolish(lbl);
      lbl->style()->polish(lbl);
    }
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_expand(long long h) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  auto apply = [w]() {
    QSizePolicy sp = w->sizePolicy();
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    w->setSizePolicy(sp);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_weight(long long h, float weight) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  int stretch = (int)(weight * 10);
  if (stretch < 1)
    stretch = 1;
  auto apply = [w, stretch]() {
    QLayout *parent = w->parentWidget() ? w->parentWidget()->layout() : nullptr;
    if (parent) {
      if (auto *vbox = qobject_cast<QVBoxLayout *>(parent))
        vbox->setStretchFactor(w, stretch);
      else if (auto *hbox = qobject_cast<QHBoxLayout *>(parent))
        hbox->setStretchFactor(w, stretch);
    }
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_checked(long long h, int c) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  bool checked = (c != 0);
  auto apply = [w, checked]() {
    if (auto *chk = qobject_cast<QCheckBox *>(w))
      chk->setChecked(checked);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" int rt_widget_get_checked(long long h) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return 0;
  QWidget *w = g_widgets[h].widget;
  if (QThread::currentThread() != g_app->thread()) {
    int result = 0;
    QMetaObject::invokeMethod(
        w,
        [w, &result]() {
          if (auto *chk = qobject_cast<QCheckBox *>(w))
            result = chk->isChecked() ? 1 : 0;
        },
        Qt::BlockingQueuedConnection);
    return result;
  }
  if (auto *chk = qobject_cast<QCheckBox *>(w))
    return chk->isChecked() ? 1 : 0;
  return 0;
}

extern "C" void rt_widget_set_progress(long long h, float v) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  if (v < 0.0f)
    v = 0.0f;
  if (v > 1.0f)
    v = 1.0f;
  QWidget *w = g_widgets[h].widget;
  int val = (int)(v * 1000);
  auto apply = [w, val]() {
    if (auto *pb = qobject_cast<QProgressBar *>(w))
      pb->setValue(val);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_slider(long long h, float v) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  if (v < 0.0f)
    v = 0.0f;
  if (v > 1.0f)
    v = 1.0f;
  QWidget *w = g_widgets[h].widget;
  int val = (int)(v * 1000);
  auto apply = [w, val]() {
    if (auto *sl = qobject_cast<QSlider *>(w))
      sl->setValue(val);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" float rt_widget_get_slider(long long h) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return 0.0f;
  QWidget *w = g_widgets[h].widget;
  if (QThread::currentThread() != g_app->thread()) {
    float result = 0.0f;
    QMetaObject::invokeMethod(
        w,
        [w, &result]() {
          if (auto *sl = qobject_cast<QSlider *>(w))
            result = (float)sl->value() / 1000.0f;
        },
        Qt::BlockingQueuedConnection);
    return result;
  }
  if (auto *sl = qobject_cast<QSlider *>(w))
    return (float)sl->value() / 1000.0f;
  return 0.0f;
}

extern "C" void rt_widget_on_change(long long h, void (*fn)(void)) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  g_widgets[h].onChange = fn;
}

extern "C" const char *rt_widget_get_text(long long h) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return "";
  std::string result;
  QWidget *w = g_widgets[h].widget;
  if (QThread::currentThread() != g_app->thread()) {
    QMetaObject::invokeMethod(
        w,
        [w, &result]() {
          if (auto *edit = qobject_cast<QLineEdit *>(w))
            result = edit->text().toUtf8().constData();
          else if (auto *lbl = qobject_cast<QLabel *>(w))
            result = lbl->text().toUtf8().constData();
          else if (auto *btn = qobject_cast<QPushButton *>(w))
            result = btn->text().toUtf8().constData();
        },
        Qt::BlockingQueuedConnection);
  } else {
    if (auto *edit = qobject_cast<QLineEdit *>(w))
      result = edit->text().toUtf8().constData();
    else if (auto *lbl = qobject_cast<QLabel *>(w))
      result = lbl->text().toUtf8().constData();
    else if (auto *btn = qobject_cast<QPushButton *>(w))
      result = btn->text().toUtf8().constData();
  }
  return poolStr(std::move(result));
}

extern "C" void rt_widget_set_hint(long long h, const char *hint) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  QString hintStr = QString::fromUtf8(hint);
  auto apply = [w, hintStr]() {
    if (auto *edit = qobject_cast<QLineEdit *>(w))
      edit->setPlaceholderText(hintStr);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_color(long long h, int r, int g, int b) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  QString style = QString("color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b);
  auto apply = [w, style]() {
    QString existing = w->styleSheet();
    w->setStyleSheet(existing + style);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" void rt_widget_set_bg_color(long long h, int r, int g, int b) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  if (!validHandle(h))
    return;
  QWidget *w = g_widgets[h].widget;
  QString style =
      QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b);
  auto apply = [w, style]() {
    QString existing = w->styleSheet();
    w->setStyleSheet(existing + style);
  };
  if (QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" long long rt_find_widget(const char *id) {
  std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
  QString qid = QString::fromUtf8(id);
  for (int i = 0; i < (int)g_widgets.size(); i++) {
    if (g_widgets[i].name == qid)
      return (long long)i;
  }
  return -1;
}

extern "C" void rt_print(const char *text) {
  printf("%s\n", text);
  fflush(stdout);
}

extern "C" void rt_message_box(const char *title, const char *msg) {
  if (g_shuttingDown)
    return;
  CSMainWindow *win = currentWindow();
  QString qtitle = QString::fromUtf8(title);
  QString qmsg = QString::fromUtf8(msg);
  auto show = [win, qtitle, qmsg]() {
    QMessageBox::information(win, qtitle, qmsg);
  };
  if (g_app && QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(win ? (QObject *)win : (QObject *)g_app, show,
                              Qt::BlockingQueuedConnection);
  else
    show();
}

extern "C" int rt_confirm_box(const char *title, const char *msg) {
  if (g_shuttingDown)
    return 0;
  CSMainWindow *win = currentWindow();
  QString qtitle = QString::fromUtf8(title);
  QString qmsg = QString::fromUtf8(msg);
  int result = 0;
  auto show = [win, qtitle, qmsg, &result]() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        win, qtitle, qmsg, QMessageBox::Yes | QMessageBox::No);
    result = (reply == QMessageBox::Yes) ? 1 : 0;
  };
  if (g_app && QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(win ? (QObject *)win : (QObject *)g_app, show,
                              Qt::BlockingQueuedConnection);
  else
    show();
  return result;
}

extern "C" void rt_set_clipboard(const char *text) {
  QString t = QString::fromUtf8(text);
  auto apply = [t]() {
    QClipboard *cb = QApplication::clipboard();
    if (cb)
      cb->setText(t);
  };
  if (g_app && QThread::currentThread() != g_app->thread())
    QMetaObject::invokeMethod(g_app, apply, Qt::QueuedConnection);
  else
    apply();
}

extern "C" const char *rt_get_clipboard(void) {
  std::string result;
  if (g_app && QThread::currentThread() != g_app->thread()) {
    QMetaObject::invokeMethod(
        g_app,
        [&result]() {
          QClipboard *cb = QApplication::clipboard();
          if (cb)
            result = cb->text().toUtf8().constData();
        },
        Qt::BlockingQueuedConnection);
  } else {
    QClipboard *cb = QApplication::clipboard();
    if (cb)
      result = cb->text().toUtf8().constData();
  }
  return poolStr(std::move(result));
}

extern "C" void rt_set_on_init(void (*fn)(void)) {
  CSMainWindow *w = currentWindow();
  if (w)
    w->onCreateCb = fn;
}

extern "C" void rt_set_on_destroy(void (*fn)(void)) {
  CSMainWindow *w = currentWindow();
  if (w)
    w->onDestroyCb = fn;
}

extern "C" void rt_set_on_resume(void (*fn)(void)) {
  CSMainWindow *w = currentWindow();
  if (w)
    w->onResumeCb = fn;
}

extern "C" void rt_set_on_pause(void (*fn)(void)) {
  CSMainWindow *w = currentWindow();
  if (w)
    w->onPauseCb = fn;
}

extern "C" void rt_set_on_back(void (*fn)(void)) {
  CSMainWindow *w = currentWindow();
  if (w)
    w->onBackCb = fn;
}

extern "C" void rt_finish_activity(void) {
  CSMainWindow *w = currentWindow();
  if (w) {
    if (QThread::currentThread() != g_app->thread())
      QMetaObject::invokeMethod(w, [w]() { w->close(); }, Qt::QueuedConnection);
    else
      w->close();
  }
}

extern "C" long long rt_task_run(void (*fn)(void)) {
  long long id = g_taskIdGen++;
  auto info = std::make_shared<TaskInfo>();
  {
    std::lock_guard<std::recursive_mutex> lk(g_taskMapMutex);
    g_taskMap[id] = info;
  }
  std::thread taskThread([fn, info, id]() {
    bool hadError = false;
    try {
      if (fn)
        fn();
    } catch (...) {
      hadError = true;
    }
    if (g_shuttingDown)
      return;

    std::vector<RtVoidCallback> thens;
    std::vector<RtVoidCallback> catches;
    {
      std::lock_guard<std::recursive_mutex> lk(info->mtx);
      info->hadError = hadError;
      info->completed = true;
      thens = info->thenCallbacks;
      catches = info->catchCallbacks;
    }
    info->cv.notify_all();
    if (!hadError) {
      for (auto &tcb : thens) {
        if (tcb && g_app && !g_shuttingDown) {
          QMetaObject::invokeMethod(
              g_app,
              [tcb]() {
                if (!g_shuttingDown)
                  tcb();
              },
              Qt::QueuedConnection);
        }
      }
    } else {
      for (auto &ccb : catches) {
        if (ccb && g_app && !g_shuttingDown) {
          QMetaObject::invokeMethod(
              g_app,
              [ccb]() {
                if (!g_shuttingDown)
                  ccb();
              },
              Qt::QueuedConnection);
        }
      }
    }
  });

  {
    std::lock_guard<std::mutex> lk(g_taskThreadsMutex);
    g_taskThreads.push_back(std::move(taskThread));
  }

  return id;
}

extern "C" long long rt_task_then(long long task, void (*fn)(void)) {
  if (!fn)
    return task;
  std::shared_ptr<TaskInfo> info;
  {
    std::lock_guard<std::recursive_mutex> lk(g_taskMapMutex);
    auto it = g_taskMap.find(task);
    if (it != g_taskMap.end())
      info = it->second;
  }
  if (!info) {
    return task;
  }
  std::lock_guard<std::recursive_mutex> lk(info->mtx);
  if (info->completed) {
    if (!info->hadError && g_app && !g_shuttingDown) {
      QMetaObject::invokeMethod(
          g_app,
          [fn]() {
            if (!g_shuttingDown)
              fn();
          },
          Qt::QueuedConnection);
    }
  } else {
    info->thenCallbacks.push_back(fn);
  }
  return task;
}

extern "C" long long rt_task_catch(long long task, void (*fn)(void)) {
  if (!fn)
    return task;
  std::shared_ptr<TaskInfo> info;
  {
    std::lock_guard<std::recursive_mutex> lk(g_taskMapMutex);
    auto it = g_taskMap.find(task);
    if (it != g_taskMap.end())
      info = it->second;
  }
  if (!info)
    return task;
  std::lock_guard<std::recursive_mutex> lk(info->mtx);
  if (info->completed) {
    if (info->hadError && g_app && !g_shuttingDown) {
      QMetaObject::invokeMethod(
          g_app,
          [fn]() {
            if (!g_shuttingDown)
              fn();
          },
          Qt::QueuedConnection);
    }
  } else {
    info->catchCallbacks.push_back(fn);
  }
  return task;
}

extern "C" void rt_post_to_ui(void (*fn)(void)) {
  if (!fn)
    return;
  if (g_app && !g_shuttingDown) {
    QMetaObject::invokeMethod(
        g_app,
        [fn]() {
          if (!g_shuttingDown)
            fn();
        },
        Qt::QueuedConnection);
  } else if (!g_shuttingDown) {
    fn();
  }
}

extern "C" void rt_sleep_ms(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

extern "C" long long rt_get_tick_ms(void) {
  auto now = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      now.time_since_epoch());
  return (long long)ms.count();
}

struct SourceLoc {
  int line = 1;
  int col = 1;
};
struct Error {
  std::string message;
  SourceLoc loc;
  std::string format() const {
    return "Error at line " + std::to_string(loc.line) + ", col " +
           std::to_string(loc.col) + ": " + message;
  }
};

enum class TK {
  Str,
  Int,
  Float,
  Id,
  LP,
  RP,
  LB,
  RB,
  Comma,
  Semi,
  Dot,
  Arrow,
  Eq,
  EqEq,
  BangEq,
  Lt,
  Gt,
  LtEq,
  GtEq,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  AmpAmp,
  PipePipe,
  Bang,
  Colon,
  Question,
  Eof,
  Bad
};
static const char *tkName(TK k) {
  switch (k) {
  case TK::Str:
    return "string";
  case TK::Int:
    return "integer";
  case TK::Float:
    return "float";
  case TK::Id:
    return "identifier";
  case TK::LP:
    return "'('";
  case TK::RP:
    return "')'";
  case TK::LB:
    return "'{'";
  case TK::RB:
    return "'}'";
  case TK::Comma:
    return "','";
  case TK::Semi:
    return "';'";
  case TK::Dot:
    return "'.'";
  case TK::Arrow:
    return "'=>'";
  case TK::Eq:
    return "'='";
  case TK::EqEq:
    return "'=='";
  case TK::BangEq:
    return "'!='";
  case TK::Lt:
    return "'<'";
  case TK::Gt:
    return "'>'";
  case TK::LtEq:
    return "'<='";
  case TK::GtEq:
    return "'>='";
  case TK::Plus:
    return "'+'";
  case TK::Minus:
    return "'-'";
  case TK::Star:
    return "'*'";
  case TK::Slash:
    return "'/'";
  case TK::Percent:
    return "'%'";
  case TK::AmpAmp:
    return "'&&'";
  case TK::PipePipe:
    return "'||'";
  case TK::Bang:
    return "'!'";
  case TK::Colon:
    return "':'";
  case TK::Question:
    return "'?'";
  case TK::Eof:
    return "EOF";
  case TK::Bad:
    return "invalid";
  }
  return "?";
}

struct Token {
  TK kind = TK::Bad;
  std::string text;
  SourceLoc loc;
  long long intVal = 0;
  double floatVal = 0.0;
};

class Lexer {
public:
  explicit Lexer(const std::string &s) : src_(s), pos_(0) {}
  Token next() {
    Token t = nextImpl();
    prevKind_ = t.kind;
    return t;
  }
  Token nextImpl() {
    skipWS();
    if (pos_ >= src_.size())
      return mk(TK::Eof, "");
    SourceLoc sl = loc_;
    char c = src_[pos_];
    if (c == '"')
      return lexStr();
    if (c >= '0' && c <= '9')
      return lexNum(false);
    if (c == '-' && pos_ + 1 < src_.size() && src_[pos_ + 1] >= '0' &&
        src_[pos_ + 1] <= '9' && prevKind_ != TK::Int &&
        prevKind_ != TK::Float && prevKind_ != TK::Str && prevKind_ != TK::Id &&
        prevKind_ != TK::RP)
      return lexNum(true);
    if (isIS(c))
      return lexId();
    switch (c) {
    case '(':
      adv();
      return mkAt(TK::LP, "(", sl);
    case ')':
      adv();
      return mkAt(TK::RP, ")", sl);
    case '{':
      adv();
      return mkAt(TK::LB, "{", sl);
    case '}':
      adv();
      return mkAt(TK::RB, "}", sl);
    case ',':
      adv();
      return mkAt(TK::Comma, ",", sl);
    case ';':
      adv();
      return mkAt(TK::Semi, ";", sl);
    case '.':
      adv();
      return mkAt(TK::Dot, ".", sl);
    case ':':
      adv();
      return mkAt(TK::Colon, ":", sl);
    case '?':
      adv();
      return mkAt(TK::Question, "?", sl);
    case '+':
      adv();
      return mkAt(TK::Plus, "+", sl);
    case '*':
      adv();
      return mkAt(TK::Star, "*", sl);
    case '%':
      adv();
      return mkAt(TK::Percent, "%", sl);
    case '/':
      adv();
      return mkAt(TK::Slash, "/", sl);
    case '-':
      adv();
      return mkAt(TK::Minus, "-", sl);
    case '=':
      adv();
      if (pos_ < src_.size() && src_[pos_] == '>') {
        adv();
        return mkAt(TK::Arrow, "=>", sl);
      }
      if (pos_ < src_.size() && src_[pos_] == '=') {
        adv();
        return mkAt(TK::EqEq, "==", sl);
      }
      return mkAt(TK::Eq, "=", sl);
    case '!':
      adv();
      if (pos_ < src_.size() && src_[pos_] == '=') {
        adv();
        return mkAt(TK::BangEq, "!=", sl);
      }
      return mkAt(TK::Bang, "!", sl);
    case '<':
      adv();
      if (pos_ < src_.size() && src_[pos_] == '=') {
        adv();
        return mkAt(TK::LtEq, "<=", sl);
      }
      return mkAt(TK::Lt, "<", sl);
    case '>':
      adv();
      if (pos_ < src_.size() && src_[pos_] == '=') {
        adv();
        return mkAt(TK::GtEq, ">=", sl);
      }
      return mkAt(TK::Gt, ">", sl);
    case '&':
      adv();
      if (pos_ < src_.size() && src_[pos_] == '&') {
        adv();
        return mkAt(TK::AmpAmp, "&&", sl);
      }
      return err("Expected '&&'", sl);
    case '|':
      adv();
      if (pos_ < src_.size() && src_[pos_] == '|') {
        adv();
        return mkAt(TK::PipePipe, "||", sl);
      }
      return err("Expected '||'", sl);
    default:
      adv();
      return err(std::string("Unexpected '") + c + "'", sl);
    }
  }
  Token peek() {
    size_t sp = pos_;
    SourceLoc sl = loc_;
    Token t = nextImpl();
    pos_ = sp;
    loc_ = sl;
    return t;
  }

private:
  TK prevKind_ = TK::Bad;
  std::string src_;
  size_t pos_;
  SourceLoc loc_;
  char adv() {
    char c = src_[pos_++];
    if (c == '\n') {
      loc_.line++;
      loc_.col = 1;
    } else
      loc_.col++;
    return c;
  }
  bool isIS(char c) const {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
  }
  bool isIC(char c) const { return isIS(c) || (c >= '0' && c <= '9'); }
  void skipWS() {
    while (pos_ < src_.size()) {
      char c = src_[pos_];
      if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
        adv();
        continue;
      }
      if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '/') {
        while (pos_ < src_.size() && src_[pos_] != '\n')
          adv();
        continue;
      }
      if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '*') {
        adv();
        adv();
        while (pos_ + 1 < src_.size()) {
          if (src_[pos_] == '*' && src_[pos_ + 1] == '/') {
            adv();
            adv();
            break;
          }
          adv();
        }
        continue;
      }
      break;
    }
  }
  Token lexStr() {
    SourceLoc sl = loc_;
    adv();
    std::string v;
    while (pos_ < src_.size() && src_[pos_] != '"') {
      if (src_[pos_] == '\\') {
        adv();
        if (pos_ >= src_.size())
          return err("Unterminated escape", sl);
        char e = src_[pos_];
        switch (e) {
        case 'n':
          v += '\n';
          break;
        case 't':
          v += '\t';
          break;
        case '\\':
          v += '\\';
          break;
        case '"':
          v += '"';
          break;
        default:
          v += e;
        }
        adv();
      } else {
        v += src_[pos_];
        adv();
      }
    }
    if (pos_ >= src_.size())
      return err("Unterminated string", sl);
    adv();
    Token t;
    t.kind = TK::Str;
    t.text = v;
    t.loc = sl;
    return t;
  }
  Token lexNum(bool negative) {
    SourceLoc sl = loc_;
    std::string v;
    if (negative) {
      v += '-';
      adv();
    }
    while (pos_ < src_.size() && src_[pos_] >= '0' && src_[pos_] <= '9') {
      v += src_[pos_];
      adv();
    }
    if (pos_ < src_.size() && src_[pos_] == '.' && pos_ + 1 < src_.size() &&
        src_[pos_ + 1] >= '0' && src_[pos_ + 1] <= '9') {
      v += '.';
      adv();
      while (pos_ < src_.size() && src_[pos_] >= '0' && src_[pos_] <= '9') {
        v += src_[pos_];
        adv();
      }
      Token t;
      t.kind = TK::Float;
      t.text = v;
      t.loc = sl;
      try {
        t.floatVal = std::stod(v);
      } catch (...) {
        return err("Float out of range: " + v, sl);
      }
      return t;
    }
    Token t;
    t.kind = TK::Int;
    t.text = v;
    t.loc = sl;
    try {
      t.intVal = std::stoll(v);
    } catch (...) {
      return err("Integer out of range: " + v, sl);
    }
    return t;
  }
  Token lexId() {
    SourceLoc sl = loc_;
    std::string v;
    while (pos_ < src_.size() && isIC(src_[pos_])) {
      v += src_[pos_];
      adv();
    }
    Token t;
    t.kind = TK::Id;
    t.text = v;
    t.loc = sl;
    return t;
  }
  Token mk(TK k, const std::string &t) {
    Token tk;
    tk.kind = k;
    tk.text = t;
    tk.loc = loc_;
    return tk;
  }
  Token mkAt(TK k, const std::string &t, SourceLoc l) {
    Token tk;
    tk.kind = k;
    tk.text = t;
    tk.loc = l;
    return tk;
  }
  Token err(const std::string &m, SourceLoc l) {
    Token tk;
    tk.kind = TK::Bad;
    tk.text = m;
    tk.loc = l;
    return tk;
  }
};

struct ASTNode;
using ASTPtr = std::unique_ptr<ASTNode>;

enum class AK {
  Print,
  Arrow,
  Button,
  Label,
  Column,
  Row,
  Task,
  Window,
  VarDecl,
  VarAssign,
  VarRef,
  If,
  BinOp,
  UnaryOp,
  IntLit,
  FloatLit,
  StringLit,
  WidgetSetText,
  WidgetGetText,
  WidgetSetVisible,
  WidgetSetEnabled,
  WidgetSetProgress,
  WidgetSetSlider,
  WidgetGetSlider,
  WidgetSetChecked,
  WidgetGetChecked,
  MessageBox,
  ConfirmBox,
  FindWidget,
  Input,
  Checkbox,
  Toggle,
  Slider,
  Progress,
  Separator,
  Spacer,
  SleepMs,
  GetTickMs,
  FinishActivity,
  PostToUI,
  SetOnInit,
  SetOnDestroy,
  SetOnResume,
  SetOnPause,
  SetOnBack,
  Concat,
  Ternary
};

struct ASTNode {
  AK kind;
  SourceLoc loc;
  virtual ~ASTNode() = default;

protected:
  ASTNode(AK k, SourceLoc l) : kind(k), loc(l) {}
};

// --- Expression Nodes ---

struct IntLitExpr : ASTNode {
  long long value;
  IntLitExpr(SourceLoc l, long long v) : ASTNode(AK::IntLit, l), value(v) {}
};

struct FloatLitExpr : ASTNode {
  double value;
  FloatLitExpr(SourceLoc l, double v) : ASTNode(AK::FloatLit, l), value(v) {}
};

struct StringLitExpr : ASTNode {
  std::string value;
  StringLitExpr(SourceLoc l, std::string v)
      : ASTNode(AK::StringLit, l), value(std::move(v)) {}
};

struct VarRefExpr : ASTNode {
  std::string name;
  VarRefExpr(SourceLoc l, std::string n)
      : ASTNode(AK::VarRef, l), name(std::move(n)) {}
};

enum class BinOpKind {
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  EqEq,
  NotEq,
  Lt,
  Gt,
  LtEq,
  GtEq,
  And,
  Or
};

struct BinOpExpr : ASTNode {
  BinOpKind op;
  ASTPtr left, right;
  BinOpExpr(SourceLoc l, BinOpKind o, ASTPtr lhs, ASTPtr rhs)
      : ASTNode(AK::BinOp, l), op(o), left(std::move(lhs)),
        right(std::move(rhs)) {}
};

enum class UnaryOpKind { Negate, Not };

struct UnaryOpExpr : ASTNode {
  UnaryOpKind op;
  ASTPtr operand;
  UnaryOpExpr(SourceLoc l, UnaryOpKind o, ASTPtr e)
      : ASTNode(AK::UnaryOp, l), op(o), operand(std::move(e)) {}
};

struct ConcatExpr : ASTNode {
  ASTPtr left, right;
  ConcatExpr(SourceLoc l, ASTPtr lhs, ASTPtr rhs)
      : ASTNode(AK::Concat, l), left(std::move(lhs)), right(std::move(rhs)) {}
};

struct TernaryExpr : ASTNode {
  ASTPtr condition, thenExpr, elseExpr;
  TernaryExpr(SourceLoc l, ASTPtr cond, ASTPtr t, ASTPtr e)
      : ASTNode(AK::Ternary, l), condition(std::move(cond)),
        thenExpr(std::move(t)), elseExpr(std::move(e)) {}
};

struct FindWidgetExpr : ASTNode {
  ASTPtr idExpr;
  FindWidgetExpr(SourceLoc l, ASTPtr e)
      : ASTNode(AK::FindWidget, l), idExpr(std::move(e)) {}
};

struct WidgetGetTextExpr : ASTNode {
  ASTPtr handle;
  WidgetGetTextExpr(SourceLoc l, ASTPtr h)
      : ASTNode(AK::WidgetGetText, l), handle(std::move(h)) {}
};

struct WidgetGetSliderExpr : ASTNode {
  ASTPtr handle;
  WidgetGetSliderExpr(SourceLoc l, ASTPtr h)
      : ASTNode(AK::WidgetGetSlider, l), handle(std::move(h)) {}
};

struct WidgetGetCheckedExpr : ASTNode {
  ASTPtr handle;
  WidgetGetCheckedExpr(SourceLoc l, ASTPtr h)
      : ASTNode(AK::WidgetGetChecked, l), handle(std::move(h)) {}
};

struct ConfirmBoxExpr : ASTNode {
  ASTPtr title, message;
  ConfirmBoxExpr(SourceLoc l, ASTPtr t, ASTPtr m)
      : ASTNode(AK::ConfirmBox, l), title(std::move(t)), message(std::move(m)) {
  }
};

struct GetTickMsExpr : ASTNode {
  GetTickMsExpr(SourceLoc l) : ASTNode(AK::GetTickMs, l) {}
};

// --- Statement Nodes ---

struct ArrowFunc : ASTNode {
  std::vector<ASTPtr> body;
  ArrowFunc(SourceLoc l) : ASTNode(AK::Arrow, l) {}
};

struct PrintStmt : ASTNode {
  ASTPtr expr;
  PrintStmt(SourceLoc l, ASTPtr e)
      : ASTNode(AK::Print, l), expr(std::move(e)) {}
};

struct VarDeclStmt : ASTNode {
  std::string name;
  ASTPtr init; // can be null for uninitialized
  VarDeclStmt(SourceLoc l, std::string n, ASTPtr i)
      : ASTNode(AK::VarDecl, l), name(std::move(n)), init(std::move(i)) {}
};

struct VarAssignStmt : ASTNode {
  std::string name;
  ASTPtr value;
  VarAssignStmt(SourceLoc l, std::string n, ASTPtr v)
      : ASTNode(AK::VarAssign, l), name(std::move(n)), value(std::move(v)) {}
};

struct IfBranch {
  ASTPtr condition; // null for else
  std::vector<ASTPtr> body;
};

struct IfStmt : ASTNode {
  std::vector<IfBranch> branches; // if, else if..., else
  IfStmt(SourceLoc l) : ASTNode(AK::If, l) {}
};

struct MCall {
  std::string name;
  SourceLoc loc;
  std::string sArg;
  ASTPtr exprArg;
  std::unique_ptr<ArrowFunc> cbArg;
  std::vector<ASTPtr> extraArgs;
};

struct ButtonStmt : ASTNode {
  ASTPtr textExpr;
  std::vector<MCall> methods;
  ButtonStmt(SourceLoc l, ASTPtr t)
      : ASTNode(AK::Button, l), textExpr(std::move(t)) {}
};

struct LabelStmt : ASTNode {
  ASTPtr textExpr;
  std::vector<MCall> methods;
  LabelStmt(SourceLoc l, ASTPtr t)
      : ASTNode(AK::Label, l), textExpr(std::move(t)) {}
};

struct InputStmt : ASTNode {
  ASTPtr hintExpr;
  std::vector<MCall> methods;
  InputStmt(SourceLoc l, ASTPtr h)
      : ASTNode(AK::Input, l), hintExpr(std::move(h)) {}
};

struct CheckboxStmt : ASTNode {
  ASTPtr textExpr;
  std::vector<MCall> methods;
  CheckboxStmt(SourceLoc l, ASTPtr t)
      : ASTNode(AK::Checkbox, l), textExpr(std::move(t)) {}
};

struct SliderStmt : ASTNode {
  std::vector<MCall> methods;
  SliderStmt(SourceLoc l) : ASTNode(AK::Slider, l) {}
};

struct ProgressStmt : ASTNode {
  std::vector<MCall> methods;
  ProgressStmt(SourceLoc l) : ASTNode(AK::Progress, l) {}
};

struct SeparatorStmt : ASTNode {
  SeparatorStmt(SourceLoc l) : ASTNode(AK::Separator, l) {}
};

struct SpacerStmt : ASTNode {
  ASTPtr widthExpr, heightExpr;
  SpacerStmt(SourceLoc l, ASTPtr w, ASTPtr h)
      : ASTNode(AK::Spacer, l), widthExpr(std::move(w)),
        heightExpr(std::move(h)) {}
};

struct ColumnStmt : ASTNode {
  std::vector<ASTPtr> children;
  ColumnStmt(SourceLoc l) : ASTNode(AK::Column, l) {}
};

struct RowStmt : ASTNode {
  std::vector<ASTPtr> children;
  RowStmt(SourceLoc l) : ASTNode(AK::Row, l) {}
};

struct TaskStmt : ASTNode {
  std::unique_ptr<ArrowFunc> body;
  std::vector<MCall> methods;
  TaskStmt(SourceLoc l) : ASTNode(AK::Task, l) {}
};

struct WidgetSetTextStmt : ASTNode {
  ASTPtr handle, text;
  WidgetSetTextStmt(SourceLoc l, ASTPtr h, ASTPtr t)
      : ASTNode(AK::WidgetSetText, l), handle(std::move(h)),
        text(std::move(t)) {}
};

struct WidgetSetVisibleStmt : ASTNode {
  ASTPtr handle, value;
  WidgetSetVisibleStmt(SourceLoc l, ASTPtr h, ASTPtr v)
      : ASTNode(AK::WidgetSetVisible, l), handle(std::move(h)),
        value(std::move(v)) {}
};

struct WidgetSetEnabledStmt : ASTNode {
  ASTPtr handle, value;
  WidgetSetEnabledStmt(SourceLoc l, ASTPtr h, ASTPtr v)
      : ASTNode(AK::WidgetSetEnabled, l), handle(std::move(h)),
        value(std::move(v)) {}
};

struct WidgetSetProgressStmt : ASTNode {
  ASTPtr handle, value;
  WidgetSetProgressStmt(SourceLoc l, ASTPtr h, ASTPtr v)
      : ASTNode(AK::WidgetSetProgress, l), handle(std::move(h)),
        value(std::move(v)) {}
};

struct WidgetSetSliderStmt : ASTNode {
  ASTPtr handle, value;
  WidgetSetSliderStmt(SourceLoc l, ASTPtr h, ASTPtr v)
      : ASTNode(AK::WidgetSetSlider, l), handle(std::move(h)),
        value(std::move(v)) {}
};

struct WidgetSetCheckedStmt : ASTNode {
  ASTPtr handle, value;
  WidgetSetCheckedStmt(SourceLoc l, ASTPtr h, ASTPtr v)
      : ASTNode(AK::WidgetSetChecked, l), handle(std::move(h)),
        value(std::move(v)) {}
};

struct MessageBoxStmt : ASTNode {
  ASTPtr title, message;
  MessageBoxStmt(SourceLoc l, ASTPtr t, ASTPtr m)
      : ASTNode(AK::MessageBox, l), title(std::move(t)), message(std::move(m)) {
  }
};

struct SleepMsStmt : ASTNode {
  ASTPtr ms;
  SleepMsStmt(SourceLoc l, ASTPtr m)
      : ASTNode(AK::SleepMs, l), ms(std::move(m)) {}
};

struct FinishActivityStmt : ASTNode {
  FinishActivityStmt(SourceLoc l) : ASTNode(AK::FinishActivity, l) {}
};

struct PostToUIStmt : ASTNode {
  std::unique_ptr<ArrowFunc> callback;
  PostToUIStmt(SourceLoc l, std::unique_ptr<ArrowFunc> cb)
      : ASTNode(AK::PostToUI, l), callback(std::move(cb)) {}
};

struct SetOnInitStmt : ASTNode {
  std::unique_ptr<ArrowFunc> callback;
  SetOnInitStmt(SourceLoc l, std::unique_ptr<ArrowFunc> cb)
      : ASTNode(AK::SetOnInit, l), callback(std::move(cb)) {}
};

struct SetOnDestroyStmt : ASTNode {
  std::unique_ptr<ArrowFunc> callback;
  SetOnDestroyStmt(SourceLoc l, std::unique_ptr<ArrowFunc> cb)
      : ASTNode(AK::SetOnDestroy, l), callback(std::move(cb)) {}
};

struct WindowStmt : ASTNode {
  std::string title;
  int width, height;
  std::vector<ASTPtr> body;
  WindowStmt(SourceLoc l, std::string t, int w, int h)
      : ASTNode(AK::Window, l), title(std::move(t)), width(w), height(h) {}
};

class Parser {
public:
  explicit Parser(Lexer &l) : lex_(l), bad_(false) {}
  std::unique_ptr<WindowStmt> parseProgram() {
    auto w = parseWindow();
    if (!w)
      return nullptr;
    Token e = next();
    if (e.kind != TK::Eof) {
      er(e.loc, "Expected EOF, got " + std::string(tkName(e.kind)));
      return nullptr;
    }
    return w;
  }
  bool hasError() const { return bad_; }
  const std::vector<Error> &errors() const { return errs_; }

private:
  Lexer &lex_;
  bool bad_;
  std::vector<Error> errs_;
  std::vector<Token> buf_;

  Token next() {
    if (!buf_.empty()) {
      Token t = std::move(buf_.front());
      buf_.erase(buf_.begin());
      return t;
    }
    return lex_.next();
  }
  Token peek() {
    if (buf_.empty())
      buf_.push_back(lex_.next());
    return buf_.front();
  }
  Token peek2() {
    while (buf_.size() < 2)
      buf_.push_back(lex_.next());
    return buf_[1];
  }
  bool ex(TK k, Token &o) {
    o = next();
    if (o.kind == TK::Bad) {
      er(o.loc, o.text);
      return false;
    }
    if (o.kind != k) {
      er(o.loc, std::string("Expected ") + tkName(k) + ", got " +
                    tkName(o.kind) + " '" + o.text + "'");
      return false;
    }
    return true;
  }
  bool exK(TK k) {
    Token t;
    return ex(k, t);
  }
  void er(SourceLoc l, const std::string &m) {
    bad_ = true;
    errs_.push_back({m, l});
  }

  bool isKeyword(const std::string &s) {
    static const std::vector<std::string> kw = {
        "window",      "button",
        "label",       "input",
        "checkbox",    "toggle",
        "slider",      "progress",
        "separator",   "spacer",
        "column",      "row",
        "print",       "task",
        "var",         "if",
        "else",        "true",
        "false",       "set_text",
        "get_text",    "set_visible",
        "set_enabled", "set_progress",
        "set_slider",  "get_slider",
        "set_checked", "get_checked",
        "find_widget", "message_box",
        "confirm_box", "sleep_ms",
        "get_tick_ms", "finish_activity",
        "post_to_ui",  "on_init",
        "on_destroy",  "on_resume",
        "on_pause",    "on_back"};
    for (auto &k : kw)
      if (s == k)
        return true;
    return false;
  }

  // ---- Expression parsing (precedence climbing) ----

  ASTPtr parseExpr() {
    auto left = parseOr();
    if (!left)
      return nullptr;
    // Ternary: expr ? then_expr : else_expr
    if (peek().kind == TK::Question) {
      Token qTok = next();      // consume '?'
      auto thenE = parseExpr(); // right-associative: allow nested ternaries
      if (!thenE)
        return nullptr;
      if (!exK(TK::Colon))
        return nullptr;
      auto elseE = parseExpr(); // right-associative
      if (!elseE)
        return nullptr;
      return std::make_unique<TernaryExpr>(qTok.loc, std::move(left),
                                           std::move(thenE), std::move(elseE));
    }
    return left;
  }
  ASTPtr parseOr() {
    auto left = parseAnd();
    if (!left)
      return nullptr;
    while (peek().kind == TK::PipePipe) {
      Token op = next();
      auto right = parseAnd();
      if (!right)
        return nullptr;
      left = std::make_unique<BinOpExpr>(op.loc, BinOpKind::Or, std::move(left),
                                         std::move(right));
    }
    return left;
  }

  ASTPtr parseAnd() {
    auto left = parseEquality();
    if (!left)
      return nullptr;
    while (peek().kind == TK::AmpAmp) {
      Token op = next();
      auto right = parseEquality();
      if (!right)
        return nullptr;
      left = std::make_unique<BinOpExpr>(op.loc, BinOpKind::And,
                                         std::move(left), std::move(right));
    }
    return left;
  }

  ASTPtr parseEquality() {
    auto left = parseComparison();
    if (!left)
      return nullptr;
    while (peek().kind == TK::EqEq || peek().kind == TK::BangEq) {
      Token op = next();
      BinOpKind bk = (op.kind == TK::EqEq) ? BinOpKind::EqEq : BinOpKind::NotEq;
      auto right = parseComparison();
      if (!right)
        return nullptr;
      left = std::make_unique<BinOpExpr>(op.loc, bk, std::move(left),
                                         std::move(right));
    }
    return left;
  }

  ASTPtr parseComparison() {
    auto left = parseConcat();
    if (!left)
      return nullptr;
    while (peek().kind == TK::Lt || peek().kind == TK::Gt ||
           peek().kind == TK::LtEq || peek().kind == TK::GtEq) {
      Token op = next();
      BinOpKind bk;
      switch (op.kind) {
      case TK::Lt:
        bk = BinOpKind::Lt;
        break;
      case TK::Gt:
        bk = BinOpKind::Gt;
        break;
      case TK::LtEq:
        bk = BinOpKind::LtEq;
        break;
      default:
        bk = BinOpKind::GtEq;
        break;
      }
      auto right = parseConcat();
      if (!right)
        return nullptr;
      left = std::make_unique<BinOpExpr>(op.loc, bk, std::move(left),
                                         std::move(right));
    }
    return left;
  }

  ASTPtr parseConcat() {
    auto left = parseAddSub();
    if (!left)
      return nullptr;
    // Use .. or just detect string + string at codegen; here we treat + as both
    // add and concat
    return left;
  }

  ASTPtr parseAddSub() {
    auto left = parseMulDiv();
    if (!left)
      return nullptr;
    while (peek().kind == TK::Plus || peek().kind == TK::Minus) {
      Token op = next();
      BinOpKind bk = (op.kind == TK::Plus) ? BinOpKind::Add : BinOpKind::Sub;
      auto right = parseMulDiv();
      if (!right)
        return nullptr;
      left = std::make_unique<BinOpExpr>(op.loc, bk, std::move(left),
                                         std::move(right));
    }
    return left;
  }

  ASTPtr parseMulDiv() {
    auto left = parseUnary();
    if (!left)
      return nullptr;
    while (peek().kind == TK::Star || peek().kind == TK::Slash ||
           peek().kind == TK::Percent) {
      Token op = next();
      BinOpKind bk;
      switch (op.kind) {
      case TK::Star:
        bk = BinOpKind::Mul;
        break;
      case TK::Slash:
        bk = BinOpKind::Div;
        break;
      default:
        bk = BinOpKind::Mod;
        break;
      }
      auto right = parseUnary();
      if (!right)
        return nullptr;
      left = std::make_unique<BinOpExpr>(op.loc, bk, std::move(left),
                                         std::move(right));
    }
    return left;
  }

  ASTPtr parseUnary() {
    if (peek().kind == TK::Bang) {
      Token op = next();
      auto e = parseUnary();
      if (!e)
        return nullptr;
      return std::make_unique<UnaryOpExpr>(op.loc, UnaryOpKind::Not,
                                           std::move(e));
    }
    if (peek().kind == TK::Minus) {
      // Check if it's a unary minus (not a negative literal)
      Token op = next();
      auto e = parseUnary();
      if (!e)
        return nullptr;
      return std::make_unique<UnaryOpExpr>(op.loc, UnaryOpKind::Negate,
                                           std::move(e));
    }
    return parsePrimary();
  }

  ASTPtr parsePrimary() {
    Token p = peek();

    if (p.kind == TK::Int) {
      next();
      return std::make_unique<IntLitExpr>(p.loc, p.intVal);
    }
    if (p.kind == TK::Float) {
      next();
      return std::make_unique<FloatLitExpr>(p.loc, p.floatVal);
    }
    if (p.kind == TK::Str) {
      next();
      return std::make_unique<StringLitExpr>(p.loc, p.text);
    }
    if (p.kind == TK::LP) {
      next();
      auto e = parseExpr();
      if (!e)
        return nullptr;
      if (!exK(TK::RP))
        return nullptr;
      return e;
    }
    if (p.kind == TK::Id) {
      if (p.text == "true") {
        next();
        return std::make_unique<IntLitExpr>(p.loc, 1);
      }
      if (p.text == "false") {
        next();
        return std::make_unique<IntLitExpr>(p.loc, 0);
      }
      if (p.text == "find_widget") {
        next();
        if (!exK(TK::LP))
          return nullptr;
        auto arg = parseExpr();
        if (!arg)
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;
        return std::make_unique<FindWidgetExpr>(p.loc, std::move(arg));
      }
      if (p.text == "get_text") {
        next();
        if (!exK(TK::LP))
          return nullptr;
        auto arg = parseExpr();
        if (!arg)
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;
        return std::make_unique<WidgetGetTextExpr>(p.loc, std::move(arg));
      }
      if (p.text == "get_slider") {
        next();
        if (!exK(TK::LP))
          return nullptr;
        auto arg = parseExpr();
        if (!arg)
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;
        return std::make_unique<WidgetGetSliderExpr>(p.loc, std::move(arg));
      }
      if (p.text == "get_checked") {
        next();
        if (!exK(TK::LP))
          return nullptr;
        auto arg = parseExpr();
        if (!arg)
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;
        return std::make_unique<WidgetGetCheckedExpr>(p.loc, std::move(arg));
      }
      if (p.text == "confirm_box") {
        next();
        if (!exK(TK::LP))
          return nullptr;
        auto t = parseExpr();
        if (!t)
          return nullptr;
        if (!exK(TK::Comma))
          return nullptr;
        auto m = parseExpr();
        if (!m)
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;
        return std::make_unique<ConfirmBoxExpr>(p.loc, std::move(t),
                                                std::move(m));
      }
      if (p.text == "get_tick_ms") {
        next();
        if (!exK(TK::LP))
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;
        return std::make_unique<GetTickMsExpr>(p.loc);
      }
      // Variable reference
      if (!isKeyword(p.text)) {
        next();
        return std::make_unique<VarRefExpr>(p.loc, p.text);
      }
      er(p.loc, "Unexpected keyword '" + p.text + "' in expression");
      return nullptr;
    }

    er(p.loc, "Expected expression, got " + std::string(tkName(p.kind)));
    return nullptr;
  }

  // ---- Statement parsing ----

  ASTPtr parseStmt() {
    Token p = peek();
    if (p.kind == TK::Bad) {
      er(p.loc, p.text);
      return nullptr;
    }
    if (p.kind != TK::Id) {
      er(p.loc, "Expected statement, got " + std::string(tkName(p.kind)));
      return nullptr;
    }
    if (p.text == "column")
      return parseCol();
    if (p.text == "row")
      return parseRow();
    if (p.text == "button")
      return parseBtn();
    if (p.text == "label")
      return parseLbl();
    if (p.text == "input")
      return parseInput();
    if (p.text == "checkbox")
      return parseCheckbox();
    if (p.text == "slider")
      return parseSlider();
    if (p.text == "progress")
      return parseProgress();
    if (p.text == "separator")
      return parseSeparator();
    if (p.text == "spacer")
      return parseSpacer();
    if (p.text == "print")
      return parsePrintS();
    if (p.text == "task")
      return parseTask();
    if (p.text == "var")
      return parseVarDecl();
    if (p.text == "if")
      return parseIf();
    if (p.text == "set_text")
      return parseSetText();
    if (p.text == "set_visible")
      return parseSetVisible();
    if (p.text == "set_enabled")
      return parseSetEnabled();
    if (p.text == "set_progress")
      return parseSetProgress();
    if (p.text == "set_slider")
      return parseSetSlider();
    if (p.text == "set_checked")
      return parseSetChecked();
    if (p.text == "message_box")
      return parseMessageBox();
    if (p.text == "sleep_ms")
      return parseSleepMs();
    if (p.text == "finish_activity")
      return parseFinishActivity();
    if (p.text == "post_to_ui")
      return parsePostToUI();
    if (p.text == "on_init")
      return parseOnInit();
    if (p.text == "on_destroy")
      return parseOnDestroy();

    // Check if it's a variable assignment: identifier = expr;
    Token p2 = peek2();
    if (p2.kind == TK::Eq) {
      return parseVarAssign();
    }

    er(p.loc, "Unknown statement '" + p.text + "'");
    return nullptr;
  }

  // --- var declaration ---
  ASTPtr parseVarDecl() {
    Token kw = next(); // consume 'var'
    Token name;
    if (!ex(TK::Id, name))
      return nullptr;
    ASTPtr init = nullptr;
    if (peek().kind == TK::Eq) {
      next(); // consume '='
      init = parseExpr();
      if (!init)
        return nullptr;
    }
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<VarDeclStmt>(kw.loc, name.text, std::move(init));
  }

  // --- var assignment ---
  ASTPtr parseVarAssign() {
    Token name = next(); // consume identifier
    if (!exK(TK::Eq))
      return nullptr;
    auto val = parseExpr();
    if (!val)
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<VarAssignStmt>(name.loc, name.text, std::move(val));
  }

  // --- if / else if / else ---
  ASTPtr parseIf() {
    Token kw = next(); // consume 'if'
    auto ifNode = std::make_unique<IfStmt>(kw.loc);

    // Parse 'if' condition
    if (!exK(TK::LP))
      return nullptr;
    auto cond = parseExpr();
    if (!cond)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;

    IfBranch branch;
    branch.condition = std::move(cond);
    if (!exK(TK::LB))
      return nullptr;
    while (peek().kind != TK::RB && peek().kind != TK::Eof) {
      auto s = parseStmt();
      if (!s) {
        if (bad_)
          return nullptr;
        continue;
      }
      branch.body.push_back(std::move(s));
    }
    if (!exK(TK::RB))
      return nullptr;
    ifNode->branches.push_back(std::move(branch));

    // Parse 'else if' and 'else' chains
    while (peek().kind == TK::Id && peek().text == "else") {
      next(); // consume 'else'
      if (peek().kind == TK::Id && peek().text == "if") {
        next(); // consume 'if'
        if (!exK(TK::LP))
          return nullptr;
        auto elseIfCond = parseExpr();
        if (!elseIfCond)
          return nullptr;
        if (!exK(TK::RP))
          return nullptr;

        IfBranch eib;
        eib.condition = std::move(elseIfCond);
        if (!exK(TK::LB))
          return nullptr;
        while (peek().kind != TK::RB && peek().kind != TK::Eof) {
          auto s = parseStmt();
          if (!s) {
            if (bad_)
              return nullptr;
            continue;
          }
          eib.body.push_back(std::move(s));
        }
        if (!exK(TK::RB))
          return nullptr;
        ifNode->branches.push_back(std::move(eib));
      } else {
        // plain else
        IfBranch eb;
        eb.condition = nullptr; // null = else
        if (!exK(TK::LB))
          return nullptr;
        while (peek().kind != TK::RB && peek().kind != TK::Eof) {
          auto s = parseStmt();
          if (!s) {
            if (bad_)
              return nullptr;
            continue;
          }
          eb.body.push_back(std::move(s));
        }
        if (!exK(TK::RB))
          return nullptr;
        ifNode->branches.push_back(std::move(eb));
        break; // else is always last
      }
    }

    return ifNode;
  }

  // --- arrow func ---
  std::unique_ptr<ArrowFunc> parseArrow() {
    SourceLoc sl = peek().loc;
    if (!exK(TK::LP))
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Arrow))
      return nullptr;
    auto f = std::make_unique<ArrowFunc>(sl);
    if (peek().kind == TK::LB) {
      next();
      while (peek().kind != TK::RB && peek().kind != TK::Eof) {
        auto s = parseStmt();
        if (!s) {
          if (bad_)
            return nullptr;
          continue;
        }
        f->body.push_back(std::move(s));
      }
      if (peek().kind == TK::Eof) {
        er(peek().loc, "Unexpected EOF, expected '}'");
        return nullptr;
      }
      if (!exK(TK::RB))
        return nullptr;
    } else {
      // single expression statement
      auto s = parseSingleExprStmt();
      if (!s)
        return nullptr;
      f->body.push_back(std::move(s));
    }
    return f;
  }

  ASTPtr parseSingleExprStmt() {
    Token p = peek();
    if (p.kind == TK::Id && p.text == "print")
      return parsePrintE();
    er(p.loc, "Expected expression");
    return nullptr;
  }

  // --- window ---
  std::unique_ptr<WindowStmt> parseWindow() {
    Token id;
    if (!ex(TK::Id, id))
      return nullptr;
    if (id.text != "window") {
      er(id.loc, "Expected 'window'");
      return nullptr;
    }
    if (!exK(TK::LP))
      return nullptr;
    Token t;
    if (!ex(TK::Str, t))
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    Token w;
    if (!ex(TK::Int, w))
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    Token h;
    if (!ex(TK::Int, h))
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto ar = parseArrow();
    if (!ar)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    auto wn = std::make_unique<WindowStmt>(id.loc, t.text, (int)w.intVal,
                                           (int)h.intVal);
    wn->body = std::move(ar->body);
    return wn;
  }

  // --- print ---
  ASTPtr parsePrintS() {
    auto n = parsePrintE();
    if (!n)
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return n;
  }
  ASTPtr parsePrintE() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto expr = parseExpr();
    if (!expr)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    return std::make_unique<PrintStmt>(id.loc, std::move(expr));
  }

  // --- column / row ---
  ASTPtr parseCol() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto a = parseArrow();
    if (!a)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    auto c = std::make_unique<ColumnStmt>(id.loc);
    c->children = std::move(a->body);
    return c;
  }
  ASTPtr parseRow() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto a = parseArrow();
    if (!a)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    auto r = std::make_unique<RowStmt>(id.loc);
    r->children = std::move(a->body);
    return r;
  }

  // ---- Widget method chain parsing helpers ----

  void parseWidgetMethods(std::vector<MCall> &methods,
                          const std::vector<std::string> &allowed) {
    while (peek().kind == TK::Dot) {
      next();
      MCall mc;
      Token mn;
      if (!ex(TK::Id, mn))
        return;
      mc.name = mn.text;
      mc.loc = mn.loc;
      if (!exK(TK::LP))
        return;

      if (mc.name == "id") {
        Token is;
        if (!ex(TK::Str, is))
          return;
        mc.sArg = is.text;
      } else if (mc.name == "onClick" || mc.name == "onChange") {
        mc.cbArg = parseArrow();
        if (!mc.cbArg)
          return;
      } else if (mc.name == "accent" || mc.name == "expand" ||
                 mc.name == "bold") {
        // no args
      } else if (mc.name == "fontSize" || mc.name == "weight") {
        mc.exprArg = parseExpr();
        if (!mc.exprArg)
          return;
      } else if (mc.name == "color" || mc.name == "bgColor") {
        mc.exprArg = parseExpr(); // r
        if (!mc.exprArg)
          return;
        if (!exK(TK::Comma))
          return;
        auto g = parseExpr();
        if (!g)
          return;
        if (!exK(TK::Comma))
          return;
        auto b = parseExpr();
        if (!b)
          return;
        mc.extraArgs.push_back(std::move(g));
        mc.extraArgs.push_back(std::move(b));
      } else if (mc.name == "hint") {
        mc.exprArg = parseExpr();
        if (!mc.exprArg)
          return;
      } else {
        bool found = false;
        for (auto &a : allowed)
          if (a == mc.name) {
            found = true;
            break;
          }
        if (!found) {
          er(mc.loc, "Unknown method '" + mc.name + "'");
          return;
        }
      }
      if (!exK(TK::RP))
        return;
      methods.push_back(std::move(mc));
    }
  }

  // --- button ---
  ASTPtr parseBtn() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto textExpr = parseExpr();
    if (!textExpr)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto b = std::make_unique<ButtonStmt>(id.loc, std::move(textExpr));
    parseWidgetMethods(b->methods, {"id", "onClick", "accent", "expand", "bold",
                                    "fontSize", "color", "bgColor", "weight"});
    if (!exK(TK::Semi))
      return nullptr;
    return b;
  }

  // --- label ---
  ASTPtr parseLbl() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto textExpr = parseExpr();
    if (!textExpr)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto l = std::make_unique<LabelStmt>(id.loc, std::move(textExpr));
    parseWidgetMethods(
        l->methods, {"id", "bold", "fontSize", "color", "bgColor", "expand"});
    if (!exK(TK::Semi))
      return nullptr;
    return l;
  }

  // --- input ---
  ASTPtr parseInput() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto hintExpr = parseExpr();
    if (!hintExpr)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto n = std::make_unique<InputStmt>(id.loc, std::move(hintExpr));
    parseWidgetMethods(
        n->methods, {"id", "onChange", "hint", "expand", "color", "bgColor"});
    if (!exK(TK::Semi))
      return nullptr;
    return n;
  }

  // --- checkbox ---
  ASTPtr parseCheckbox() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto textExpr = parseExpr();
    if (!textExpr)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto n = std::make_unique<CheckboxStmt>(id.loc, std::move(textExpr));
    parseWidgetMethods(n->methods, {"id", "onChange"});
    if (!exK(TK::Semi))
      return nullptr;
    return n;
  }

  // --- slider ---
  ASTPtr parseSlider() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto n = std::make_unique<SliderStmt>(id.loc);
    parseWidgetMethods(n->methods, {"id", "onChange"});
    if (!exK(TK::Semi))
      return nullptr;
    return n;
  }

  // --- progress ---
  ASTPtr parseProgress() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto n = std::make_unique<ProgressStmt>(id.loc);
    parseWidgetMethods(n->methods, {"id"});
    if (!exK(TK::Semi))
      return nullptr;
    return n;
  }

  // --- separator ---
  ASTPtr parseSeparator() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<SeparatorStmt>(id.loc);
  }

  // --- spacer ---
  ASTPtr parseSpacer() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto w = parseExpr();
    if (!w)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<SpacerStmt>(id.loc, std::move(w), std::move(h));
  }

  // --- task ---
  ASTPtr parseTask() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto tb = parseArrow();
    if (!tb)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    auto t = std::make_unique<TaskStmt>(id.loc);
    t->body = std::move(tb);
    while (peek().kind == TK::Dot) {
      next();
      MCall mc;
      Token mn;
      if (!ex(TK::Id, mn))
        return nullptr;
      mc.name = mn.text;
      mc.loc = mn.loc;
      if (!exK(TK::LP))
        return nullptr;
      if (mc.name == "then" || mc.name == "catch") {
        mc.cbArg = parseArrow();
        if (!mc.cbArg)
          return nullptr;
      } else {
        er(mc.loc, "Unknown task method '" + mc.name + "'");
        return nullptr;
      }
      if (!exK(TK::RP))
        return nullptr;
      t->methods.push_back(std::move(mc));
    }
    if (!exK(TK::Semi))
      return nullptr;
    return t;
  }

  // --- set_text(handle, value) ---
  ASTPtr parseSetText() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto v = parseExpr();
    if (!v)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<WidgetSetTextStmt>(id.loc, std::move(h),
                                               std::move(v));
  }

  ASTPtr parseSetVisible() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto v = parseExpr();
    if (!v)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<WidgetSetVisibleStmt>(id.loc, std::move(h),
                                                  std::move(v));
  }

  ASTPtr parseSetEnabled() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto v = parseExpr();
    if (!v)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<WidgetSetEnabledStmt>(id.loc, std::move(h),
                                                  std::move(v));
  }

  ASTPtr parseSetProgress() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto v = parseExpr();
    if (!v)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<WidgetSetProgressStmt>(id.loc, std::move(h),
                                                   std::move(v));
  }

  ASTPtr parseSetSlider() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto v = parseExpr();
    if (!v)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<WidgetSetSliderStmt>(id.loc, std::move(h),
                                                 std::move(v));
  }

  ASTPtr parseSetChecked() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto h = parseExpr();
    if (!h)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto v = parseExpr();
    if (!v)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<WidgetSetCheckedStmt>(id.loc, std::move(h),
                                                  std::move(v));
  }

  ASTPtr parseMessageBox() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto t = parseExpr();
    if (!t)
      return nullptr;
    if (!exK(TK::Comma))
      return nullptr;
    auto m = parseExpr();
    if (!m)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<MessageBoxStmt>(id.loc, std::move(t), std::move(m));
  }

  ASTPtr parseSleepMs() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto ms = parseExpr();
    if (!ms)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<SleepMsStmt>(id.loc, std::move(ms));
  }

  ASTPtr parseFinishActivity() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<FinishActivityStmt>(id.loc);
  }

  ASTPtr parsePostToUI() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto cb = parseArrow();
    if (!cb)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<PostToUIStmt>(id.loc, std::move(cb));
  }

  ASTPtr parseOnInit() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto cb = parseArrow();
    if (!cb)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<SetOnInitStmt>(id.loc, std::move(cb));
  }

  ASTPtr parseOnDestroy() {
    Token id = next();
    if (!exK(TK::LP))
      return nullptr;
    auto cb = parseArrow();
    if (!cb)
      return nullptr;
    if (!exK(TK::RP))
      return nullptr;
    if (!exK(TK::Semi))
      return nullptr;
    return std::make_unique<SetOnDestroyStmt>(id.loc, std::move(cb));
  }
};

// Runtime helpers for string concatenation and int-to-string conversion
extern "C" const char *rt_concat(const char *a, const char *b) {
  return poolStr(std::string(a ? a : "") + std::string(b ? b : ""));
}

extern "C" const char *rt_int_to_str(long long v) {
  return poolStr(std::to_string(v));
}

extern "C" const char *rt_float_to_str(double v) {
  std::ostringstream oss;
  oss << v;
  return poolStr(oss.str());
}

extern "C" long long rt_str_eq(const char *a, const char *b) {
  if (a == b)
    return 1;
  if (!a || !b)
    return 0;
  return strcmp(a, b) == 0 ? 1 : 0;
}

class Codegen {
public:
  Codegen()
      : ctx_(std::make_unique<llvm::LLVMContext>()), B(*ctx_), cc(0), sc(0),
        gc(0) {
    M = std::make_unique<llvm::Module>("ClaudeScriptModule", *ctx_);
  }
  bool generate(const WindowStmt &win) {
    decl();
    auto *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*ctx_), false);
    mainFn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage,
                                    "cs_main", M.get());
    auto *bb = llvm::BasicBlock::Create(*ctx_, "entry", mainFn);
    B.SetInsertPoint(bb);
    currentFn = mainFn;
    pushScope();
    B.CreateCall(fWindowCreate,
                 {str(win.title), i32(win.width), i32(win.height)});
    for (auto &s : win.body)
      if (!emitNode(*s))
        return false;
    B.CreateCall(fWindowEnd);
    popScope();
    B.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*ctx_), 0));
    return true;
  }
  bool verify(std::string &e) {
    llvm::raw_string_ostream o(e);
    return !llvm::verifyModule(*M, &o);
  }
  void printIR(llvm::raw_ostream &o) { M->print(o, nullptr); }
  std::unique_ptr<llvm::LLVMContext> takeCtx() { return std::move(ctx_); }
  std::unique_ptr<llvm::Module> takeMod() { return std::move(M); }

private:
  std::unique_ptr<llvm::LLVMContext> ctx_;
  llvm::IRBuilder<> B;
  std::unique_ptr<llvm::Module> M;
  llvm::Function *mainFn = nullptr;
  llvm::Function *currentFn = nullptr;
  int cc, sc, gc;

  // Variable storage: maps variable name -> global variable
  // We use globals so that callbacks (separate LLVM functions) can access them.
  struct VarInfo {
    llvm::GlobalVariable *gv;
    llvm::Type *type;
  };
  struct VarScope {
    std::unordered_map<std::string, VarInfo> vars;
  };
  std::vector<VarScope> varScopes;

  void pushScope() { varScopes.push_back({}); }
  void popScope() {
    if (!varScopes.empty())
      varScopes.pop_back();
  }

  VarInfo *lookupVar(const std::string &name) {
    for (int i = (int)varScopes.size() - 1; i >= 0; i--) {
      auto it = varScopes[i].vars.find(name);
      if (it != varScopes[i].vars.end())
        return &it->second;
    }
    return nullptr;
  }

  VarInfo &declareVar(const std::string &name, llvm::Type *type,
                      llvm::Constant *init) {
    std::string gname = ".var." + name + "." + std::to_string(gc++);
    auto *gv = new llvm::GlobalVariable(
        *M, type, false, llvm::GlobalValue::InternalLinkage, init, gname);
    VarInfo vi{gv, type};
    if (!varScopes.empty()) {
      varScopes.back().vars[name] = vi;
    }
    return varScopes.back().vars[name];
  }

  // All runtime function callees
  llvm::FunctionCallee fWindowCreate, fWindowEnd, fBeginCol, fBeginRow,
      fEndLayout;
  llvm::FunctionCallee fCreateBtn, fCreateLbl, fCreateInput, fCreateCheckbox,
      fCreateToggle;
  llvm::FunctionCallee fCreateSlider, fCreateProgress, fCreateSeparator,
      fCreateSpacer;
  llvm::FunctionCallee fSetId, fOnClick, fSetAccent, fPrint;
  llvm::FunctionCallee fTaskRun, fTaskThen, fTaskCatch;
  llvm::FunctionCallee fSetText, fGetText, fSetVisible, fSetEnabled;
  llvm::FunctionCallee fSetBold, fSetFontSize, fSetExpand, fSetWeight;
  llvm::FunctionCallee fSetChecked, fGetChecked, fSetProgress, fSetSlider,
      fGetSlider;
  llvm::FunctionCallee fOnChange, fSetHint;
  llvm::FunctionCallee fSetColor, fSetBgColor;
  llvm::FunctionCallee fFindWidget, fMessageBox, fConfirmBox;
  llvm::FunctionCallee fSleepMs, fGetTickMs, fFinishActivity, fPostToUI;
  llvm::FunctionCallee fSetOnInit, fSetOnDestroy, fSetOnResume, fSetOnPause,
      fSetOnBack;
  llvm::FunctionCallee fConcat, fIntToStr, fFloatToStr;
  llvm::FunctionCallee fStrEq;

  llvm::Type *Void() { return llvm::Type::getVoidTy(*ctx_); }
  llvm::Type *Ptr() { return llvm::PointerType::getUnqual(*ctx_); }
  llvm::Type *I32() { return llvm::Type::getInt32Ty(*ctx_); }
  llvm::Type *I64() { return llvm::Type::getInt64Ty(*ctx_); }
  llvm::Type *F32() { return llvm::Type::getFloatTy(*ctx_); }
  llvm::Type *F64() { return llvm::Type::getDoubleTy(*ctx_); }
  llvm::Value *i32(int v) { return llvm::ConstantInt::get(I32(), v); }
  llvm::Value *i64(long long v) { return llvm::ConstantInt::get(I64(), v); }

  void decl() {
    fWindowCreate = M->getOrInsertFunction(
        "rt_window_create",
        llvm::FunctionType::get(Void(), {Ptr(), I32(), I32()}, false));
    fWindowEnd = M->getOrInsertFunction("rt_window_end",
                                        llvm::FunctionType::get(Void(), false));
    fBeginCol = M->getOrInsertFunction("rt_begin_column",
                                       llvm::FunctionType::get(Void(), false));
    fBeginRow = M->getOrInsertFunction("rt_begin_row",
                                       llvm::FunctionType::get(Void(), false));
    fEndLayout = M->getOrInsertFunction("rt_end_layout",
                                        llvm::FunctionType::get(Void(), false));
    fCreateBtn = M->getOrInsertFunction(
        "rt_create_button", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fCreateLbl = M->getOrInsertFunction(
        "rt_create_label", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fCreateInput = M->getOrInsertFunction(
        "rt_create_input", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fCreateCheckbox = M->getOrInsertFunction(
        "rt_create_checkbox", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fCreateToggle = M->getOrInsertFunction(
        "rt_create_toggle", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fCreateSlider = M->getOrInsertFunction(
        "rt_create_slider", llvm::FunctionType::get(I64(), false));
    fCreateProgress = M->getOrInsertFunction(
        "rt_create_progress", llvm::FunctionType::get(I64(), false));
    fCreateSeparator = M->getOrInsertFunction(
        "rt_create_separator", llvm::FunctionType::get(I64(), false));
    fCreateSpacer = M->getOrInsertFunction(
        "rt_create_spacer",
        llvm::FunctionType::get(I64(), {I32(), I32()}, false));
    fSetId = M->getOrInsertFunction(
        "rt_widget_set_id",
        llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
    fOnClick = M->getOrInsertFunction(
        "rt_widget_on_click",
        llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
    fSetAccent = M->getOrInsertFunction(
        "rt_widget_set_accent",
        llvm::FunctionType::get(Void(), {I64(), I32()}, false));
    fPrint = M->getOrInsertFunction(
        "rt_print", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fTaskRun = M->getOrInsertFunction(
        "rt_task_run", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fTaskThen = M->getOrInsertFunction(
        "rt_task_then", llvm::FunctionType::get(I64(), {I64(), Ptr()}, false));
    fTaskCatch = M->getOrInsertFunction(
        "rt_task_catch", llvm::FunctionType::get(I64(), {I64(), Ptr()}, false));
    fSetText = M->getOrInsertFunction(
        "rt_widget_set_text",
        llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
    fGetText = M->getOrInsertFunction(
        "rt_widget_get_text", llvm::FunctionType::get(Ptr(), {I64()}, false));
    fSetVisible = M->getOrInsertFunction(
        "rt_widget_set_visible",
        llvm::FunctionType::get(Void(), {I64(), I32()}, false));
    fSetEnabled = M->getOrInsertFunction(
        "rt_widget_set_enabled",
        llvm::FunctionType::get(Void(), {I64(), I32()}, false));
    fSetBold = M->getOrInsertFunction(
        "rt_widget_set_bold",
        llvm::FunctionType::get(Void(), {I64(), I32()}, false));
    fSetFontSize = M->getOrInsertFunction(
        "rt_widget_set_font_size",
        llvm::FunctionType::get(Void(), {I64(), I32()}, false));
    fSetExpand =
        M->getOrInsertFunction("rt_widget_set_expand",
                               llvm::FunctionType::get(Void(), {I64()}, false));
    fSetWeight = M->getOrInsertFunction(
        "rt_widget_set_weight",
        llvm::FunctionType::get(Void(), {I64(), F32()}, false));
    fSetChecked = M->getOrInsertFunction(
        "rt_widget_set_checked",
        llvm::FunctionType::get(Void(), {I64(), I32()}, false));
    fGetChecked =
        M->getOrInsertFunction("rt_widget_get_checked",
                               llvm::FunctionType::get(I32(), {I64()}, false));
    fSetProgress = M->getOrInsertFunction(
        "rt_widget_set_progress",
        llvm::FunctionType::get(Void(), {I64(), F32()}, false));
    fSetSlider = M->getOrInsertFunction(
        "rt_widget_set_slider",
        llvm::FunctionType::get(Void(), {I64(), F32()}, false));
    fGetSlider = M->getOrInsertFunction(
        "rt_widget_get_slider", llvm::FunctionType::get(F32(), {I64()}, false));
    fOnChange = M->getOrInsertFunction(
        "rt_widget_on_change",
        llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
    fSetHint = M->getOrInsertFunction(
        "rt_widget_set_hint",
        llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
    fSetColor = M->getOrInsertFunction(
        "rt_widget_set_color",
        llvm::FunctionType::get(Void(), {I64(), I32(), I32(), I32()}, false));
    fSetBgColor = M->getOrInsertFunction(
        "rt_widget_set_bg_color",
        llvm::FunctionType::get(Void(), {I64(), I32(), I32(), I32()}, false));
    fFindWidget = M->getOrInsertFunction(
        "rt_find_widget", llvm::FunctionType::get(I64(), {Ptr()}, false));
    fMessageBox = M->getOrInsertFunction(
        "rt_message_box",
        llvm::FunctionType::get(Void(), {Ptr(), Ptr()}, false));
    fConfirmBox = M->getOrInsertFunction(
        "rt_confirm_box",
        llvm::FunctionType::get(I32(), {Ptr(), Ptr()}, false));
    fSleepMs = M->getOrInsertFunction(
        "rt_sleep_ms", llvm::FunctionType::get(Void(), {I32()}, false));
    fGetTickMs = M->getOrInsertFunction("rt_get_tick_ms",
                                        llvm::FunctionType::get(I64(), false));
    fFinishActivity = M->getOrInsertFunction(
        "rt_finish_activity", llvm::FunctionType::get(Void(), false));
    fPostToUI = M->getOrInsertFunction(
        "rt_post_to_ui", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fSetOnInit = M->getOrInsertFunction(
        "rt_set_on_init", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fSetOnDestroy = M->getOrInsertFunction(
        "rt_set_on_destroy", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fSetOnResume = M->getOrInsertFunction(
        "rt_set_on_resume", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fSetOnPause = M->getOrInsertFunction(
        "rt_set_on_pause", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fSetOnBack = M->getOrInsertFunction(
        "rt_set_on_back", llvm::FunctionType::get(Void(), {Ptr()}, false));
    fConcat = M->getOrInsertFunction(
        "rt_concat", llvm::FunctionType::get(Ptr(), {Ptr(), Ptr()}, false));
    fIntToStr = M->getOrInsertFunction(
        "rt_int_to_str", llvm::FunctionType::get(Ptr(), {I64()}, false));
    fFloatToStr = M->getOrInsertFunction(
        "rt_float_to_str", llvm::FunctionType::get(Ptr(), {F64()}, false));
    fStrEq = M->getOrInsertFunction(
        "rt_str_eq", llvm::FunctionType::get(I64(), {Ptr(), Ptr()}, false));
  }

  llvm::Value *str(const std::string &s) {
    auto *c = llvm::ConstantDataArray::getString(*ctx_, s, true);
    auto *gv = new llvm::GlobalVariable(*M, c->getType(), true,
                                        llvm::GlobalValue::PrivateLinkage, c,
                                        ".s." + std::to_string(sc++));
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    llvm::Value *z = llvm::ConstantInt::get(I64(), 0);
    return B.CreateInBoundsGEP(c->getType(), gv, {z, z});
  }

  llvm::Function *makeCb(const ArrowFunc &af) {
    auto *ft = llvm::FunctionType::get(Void(), false);
    auto *fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage,
                                      "__cb_" + std::to_string(cc++), M.get());
    auto *sb = B.GetInsertBlock();
    auto si = B.GetInsertPoint();
    auto *savedFn = currentFn;
    currentFn = fn;
    auto *entry = llvm::BasicBlock::Create(*ctx_, "e", fn);
    B.SetInsertPoint(entry);
    // Don't push/pop scope here - callbacks share the parent scope's globals
    for (auto &s : af.body)
      emitNode(*s);
    B.CreateRetVoid();
    currentFn = savedFn;
    B.SetInsertPoint(sb, si);
    return fn;
  }

  llvm::Value *emitExpr(const ASTNode &n) {
    switch (n.kind) {
    case AK::IntLit:
      return i64(static_cast<const IntLitExpr &>(n).value);
    case AK::FloatLit:
      return llvm::ConstantFP::get(F64(),
                                   static_cast<const FloatLitExpr &>(n).value);
    case AK::StringLit:
      return str(static_cast<const StringLitExpr &>(n).value);
    case AK::VarRef: {
      auto &vr = static_cast<const VarRefExpr &>(n);
      VarInfo *vi = lookupVar(vr.name);
      if (!vi) {
        return i64(0);
      }
      return B.CreateLoad(vi->type, vi->gv, vr.name);
    }
    case AK::BinOp: {
      auto &bo = static_cast<const BinOpExpr &>(n);
      auto *lhs = emitExpr(*bo.left);
      auto *rhs = emitExpr(*bo.right);
      if (!lhs || !rhs)
        return i64(0);

      bool lIsPtr = lhs->getType()->isPointerTy();
      bool rIsPtr = rhs->getType()->isPointerTy();
      bool lIsFloat = lhs->getType()->isDoubleTy();
      bool rIsFloat = rhs->getType()->isDoubleTy();

      if (bo.op == BinOpKind::Add && (lIsPtr || rIsPtr)) {
        if (!lIsPtr)
          lhs = valToStr(lhs);
        if (!rIsPtr)
          rhs = valToStr(rhs);
        return B.CreateCall(fConcat, {lhs, rhs});
      }

      // String equality comparison
      if ((bo.op == BinOpKind::EqEq || bo.op == BinOpKind::NotEq) &&
          (lIsPtr || rIsPtr)) {
        if (!lIsPtr)
          lhs = valToStr(lhs);
        if (!rIsPtr)
          rhs = valToStr(rhs);
        auto *eq = B.CreateCall(fStrEq, {lhs, rhs});
        if (bo.op == BinOpKind::NotEq) {
          return B.CreateZExt(B.CreateICmpEQ(eq, i64(0)), I64());
        }
        return eq;
      }

      if (lIsFloat || rIsFloat) {
        if (!lIsFloat)
          lhs = B.CreateSIToFP(lhs, F64());
        if (!rIsFloat)
          rhs = B.CreateSIToFP(rhs, F64());
        switch (bo.op) {
        case BinOpKind::Add:
          return B.CreateFAdd(lhs, rhs);
        case BinOpKind::Sub:
          return B.CreateFSub(lhs, rhs);
        case BinOpKind::Mul:
          return B.CreateFMul(lhs, rhs);
        case BinOpKind::Div:
          return B.CreateFDiv(lhs, rhs);
        case BinOpKind::EqEq:
          return B.CreateZExt(B.CreateFCmpOEQ(lhs, rhs), I64());
        case BinOpKind::NotEq:
          return B.CreateZExt(B.CreateFCmpONE(lhs, rhs), I64());
        case BinOpKind::Lt:
          return B.CreateZExt(B.CreateFCmpOLT(lhs, rhs), I64());
        case BinOpKind::Gt:
          return B.CreateZExt(B.CreateFCmpOGT(lhs, rhs), I64());
        case BinOpKind::LtEq:
          return B.CreateZExt(B.CreateFCmpOLE(lhs, rhs), I64());
        case BinOpKind::GtEq:
          return B.CreateZExt(B.CreateFCmpOGE(lhs, rhs), I64());
        default:
          return llvm::ConstantFP::get(F64(), 0.0);
        }
      }

      switch (bo.op) {
      case BinOpKind::Add:
        return B.CreateAdd(lhs, rhs);
      case BinOpKind::Sub:
        return B.CreateSub(lhs, rhs);
      case BinOpKind::Mul:
        return B.CreateMul(lhs, rhs);
      case BinOpKind::Div:
        return B.CreateSDiv(lhs, rhs);
      case BinOpKind::Mod:
        return B.CreateSRem(lhs, rhs);
      case BinOpKind::EqEq:
        return B.CreateZExt(B.CreateICmpEQ(lhs, rhs), I64());
      case BinOpKind::NotEq:
        return B.CreateZExt(B.CreateICmpNE(lhs, rhs), I64());
      case BinOpKind::Lt:
        return B.CreateZExt(B.CreateICmpSLT(lhs, rhs), I64());
      case BinOpKind::Gt:
        return B.CreateZExt(B.CreateICmpSGT(lhs, rhs), I64());
      case BinOpKind::LtEq:
        return B.CreateZExt(B.CreateICmpSLE(lhs, rhs), I64());
      case BinOpKind::GtEq:
        return B.CreateZExt(B.CreateICmpSGE(lhs, rhs), I64());
      case BinOpKind::And: {
        auto *lb = B.CreateICmpNE(lhs, i64(0));
        auto *rb = B.CreateICmpNE(rhs, i64(0));
        return B.CreateZExt(B.CreateAnd(lb, rb), I64());
      }
      case BinOpKind::Or: {
        auto *lb = B.CreateICmpNE(lhs, i64(0));
        auto *rb = B.CreateICmpNE(rhs, i64(0));
        return B.CreateZExt(B.CreateOr(lb, rb), I64());
      }
      }
      return i64(0);
    }
    case AK::UnaryOp: {
      auto &uo = static_cast<const UnaryOpExpr &>(n);
      auto *val = emitExpr(*uo.operand);
      if (!val)
        return i64(0);
      if (uo.op == UnaryOpKind::Not) {
        if (val->getType()->isDoubleTy()) {
          auto *cmp = B.CreateFCmpOEQ(val, llvm::ConstantFP::get(F64(), 0.0));
          return B.CreateZExt(cmp, I64());
        }
        if (val->getType()->isIntegerTy(64)) {
          auto *cmp = B.CreateICmpEQ(val, i64(0));
          return B.CreateZExt(cmp, I64());
        }
        if (val->getType()->isIntegerTy(32)) {
          auto *cmp = B.CreateICmpEQ(val, i32(0));
          return B.CreateZExt(cmp, I64());
        }
        return i64(0);
      }
      if (uo.op == UnaryOpKind::Negate) {
        if (val->getType()->isDoubleTy())
          return B.CreateFNeg(val);
        if (val->getType()->isIntegerTy(64))
          return B.CreateNeg(val);
        if (val->getType()->isIntegerTy(32)) {
          auto *ext = B.CreateSExt(val, I64());
          return B.CreateNeg(ext);
        }
        return i64(0);
      }
      return i64(0);
    }
    case AK::FindWidget: {
      auto &fw = static_cast<const FindWidgetExpr &>(n);
      auto *id = emitExpr(*fw.idExpr);
      if (!id)
        return i64(-1);
      return B.CreateCall(fFindWidget, {valToStr(id)});
    }
    case AK::WidgetGetText: {
      auto &gt = static_cast<const WidgetGetTextExpr &>(n);
      auto *h = emitExpr(*gt.handle);
      if (!h)
        return str("");
      return B.CreateCall(fGetText, {valToI64(h)});
    }
    case AK::WidgetGetSlider: {
      auto &gs = static_cast<const WidgetGetSliderExpr &>(n);
      auto *h = emitExpr(*gs.handle);
      if (!h)
        return llvm::ConstantFP::get(F32(), 0.0);
      auto *val = B.CreateCall(fGetSlider, {valToI64(h)});
      return B.CreateFPExt(val, F64());
    }
    case AK::WidgetGetChecked: {
      auto &gc2 = static_cast<const WidgetGetCheckedExpr &>(n);
      auto *h = emitExpr(*gc2.handle);
      if (!h)
        return i64(0);
      auto *val = B.CreateCall(fGetChecked, {valToI64(h)});
      return B.CreateSExt(val, I64());
    }
    case AK::ConfirmBox: {
      auto &cb = static_cast<const ConfirmBoxExpr &>(n);
      auto *t = emitExpr(*cb.title);
      auto *m = emitExpr(*cb.message);
      auto *val = B.CreateCall(fConfirmBox, {valToStr(t), valToStr(m)});
      return B.CreateSExt(val, I64());
    }
    case AK::GetTickMs: {
      return B.CreateCall(fGetTickMs);
    }
    case AK::Ternary: {
      auto &te = static_cast<const TernaryExpr &>(n);
      // Evaluate condition
      auto *condVal = emitExpr(*te.condition);
      auto *condBool = valToBool(condVal);

      llvm::Function *fn = B.GetInsertBlock()->getParent();
      auto *thenBB = llvm::BasicBlock::Create(*ctx_, "tern.then", fn);
      auto *elseBB = llvm::BasicBlock::Create(*ctx_, "tern.else", fn);
      auto *mergeBB = llvm::BasicBlock::Create(*ctx_, "tern.merge", fn);

      B.CreateCondBr(condBool, thenBB, elseBB);

      // Then branch
      B.SetInsertPoint(thenBB);
      auto *thenVal = emitExpr(*te.thenExpr);
      thenBB = B.GetInsertBlock(); // update in case emitExpr added blocks
      B.CreateBr(mergeBB);

      // Else branch
      B.SetInsertPoint(elseBB);
      auto *elseVal = emitExpr(*te.elseExpr);
      elseBB = B.GetInsertBlock(); // update in case emitExpr added blocks
      B.CreateBr(mergeBB);

      // Merge: unify types and create PHI
      B.SetInsertPoint(mergeBB);

      // Type unification: both sides must have the same LLVM type
      llvm::Type *thenTy = thenVal->getType();
      llvm::Type *elseTy = elseVal->getType();

      if (thenTy != elseTy) {
        // If either is a pointer (string), coerce both to pointer
        if (thenTy->isPointerTy() || elseTy->isPointerTy()) {
          if (!thenTy->isPointerTy()) {
            B.SetInsertPoint(thenBB->getTerminator());
            thenVal = valToStr(thenVal);
            B.SetInsertPoint(mergeBB);
          }
          if (!elseTy->isPointerTy()) {
            B.SetInsertPoint(elseBB->getTerminator());
            elseVal = valToStr(elseVal);
            B.SetInsertPoint(mergeBB);
          }
        }
        // If either is double, coerce both to double
        else if (thenTy->isDoubleTy() || elseTy->isDoubleTy()) {
          if (!thenTy->isDoubleTy()) {
            B.SetInsertPoint(thenBB->getTerminator());
            thenVal = B.CreateSIToFP(thenVal, F64());
            B.SetInsertPoint(mergeBB);
          }
          if (!elseTy->isDoubleTy()) {
            B.SetInsertPoint(elseBB->getTerminator());
            elseVal = B.CreateSIToFP(elseVal, F64());
            B.SetInsertPoint(mergeBB);
          }
        }
        // If one is i32 and the other i64, coerce to i64
        else if (thenTy->isIntegerTy() && elseTy->isIntegerTy()) {
          if (!thenTy->isIntegerTy(64)) {
            B.SetInsertPoint(thenBB->getTerminator());
            thenVal = B.CreateSExt(thenVal, I64());
            B.SetInsertPoint(mergeBB);
          }
          if (!elseTy->isIntegerTy(64)) {
            B.SetInsertPoint(elseBB->getTerminator());
            elseVal = B.CreateSExt(elseVal, I64());
            B.SetInsertPoint(mergeBB);
          }
        }
      }

      llvm::PHINode *phi = B.CreatePHI(thenVal->getType(), 2, "tern.result");
      phi->addIncoming(thenVal, thenBB);
      phi->addIncoming(elseVal, elseBB);
      return phi;
    }
    default:
      return i64(0);
    }
  }

  llvm::Value *valToStr(llvm::Value *v) {
    if (v->getType()->isPointerTy())
      return v;
    if (v->getType()->isDoubleTy())
      return B.CreateCall(fFloatToStr, {v});
    if (v->getType()->isIntegerTy(32))
      v = B.CreateSExt(v, I64());
    if (v->getType()->isIntegerTy(64))
      return B.CreateCall(fIntToStr, {v});
    return str("");
  }

  llvm::Value *valToI64(llvm::Value *v) {
    if (v->getType()->isIntegerTy(64))
      return v;
    if (v->getType()->isIntegerTy(32))
      return B.CreateSExt(v, I64());
    if (v->getType()->isDoubleTy())
      return B.CreateFPToSI(v, I64());
    if (v->getType()->isPointerTy()) {
      return B.CreateZExt(B.CreateICmpNE(B.CreatePtrToInt(v, I64()), i64(0)),
                          I64());
    }
    return i64(0);
  }

  llvm::Value *valToI32(llvm::Value *v) {
    if (v->getType()->isIntegerTy(32))
      return v;
    if (v->getType()->isIntegerTy(64))
      return B.CreateTrunc(v, I32());
    if (v->getType()->isDoubleTy())
      return B.CreateFPToSI(v, I32());
    return i32(0);
  }

  llvm::Value *valToF32(llvm::Value *v) {
    if (v->getType()->isFloatTy())
      return v;
    if (v->getType()->isDoubleTy())
      return B.CreateFPTrunc(v, F32());
    if (v->getType()->isIntegerTy(64))
      return B.CreateSIToFP(v, F32());
    if (v->getType()->isIntegerTy(32))
      return B.CreateSIToFP(v, F32());
    return llvm::ConstantFP::get(F32(), 0.0);
  }

  llvm::Value *valToBool(llvm::Value *v) {
    if (v->getType()->isIntegerTy(1))
      return v;
    if (v->getType()->isIntegerTy(64))
      return B.CreateICmpNE(v, i64(0));
    if (v->getType()->isIntegerTy(32))
      return B.CreateICmpNE(v, i32(0));
    if (v->getType()->isDoubleTy())
      return B.CreateFCmpONE(v, llvm::ConstantFP::get(F64(), 0.0));
    if (v->getType()->isPointerTy())
      return B.CreateICmpNE(B.CreatePtrToInt(v, I64()), i64(0));
    return llvm::ConstantInt::getFalse(*ctx_);
  }

  void emitWidgetMethods(llvm::Value *h, const std::vector<MCall> &methods) {
    for (auto &m : methods) {
      if (m.name == "id")
        B.CreateCall(fSetId, {h, str(m.sArg)});
      else if (m.name == "onClick")
        B.CreateCall(fOnClick, {h, makeCb(*m.cbArg)});
      else if (m.name == "onChange")
        B.CreateCall(fOnChange, {h, makeCb(*m.cbArg)});
      else if (m.name == "accent")
        B.CreateCall(fSetAccent, {h, i32(1)});
      else if (m.name == "bold")
        B.CreateCall(fSetBold, {h, i32(1)});
      else if (m.name == "expand")
        B.CreateCall(fSetExpand, {h});
      else if (m.name == "fontSize") {
        auto *v = emitExpr(*m.exprArg);
        B.CreateCall(fSetFontSize, {h, valToI32(v)});
      } else if (m.name == "weight") {
        auto *v = emitExpr(*m.exprArg);
        B.CreateCall(fSetWeight, {h, valToF32(v)});
      } else if (m.name == "hint") {
        auto *v = emitExpr(*m.exprArg);
        B.CreateCall(fSetHint, {h, valToStr(v)});
      } else if (m.name == "color") {
        auto *r = emitExpr(*m.exprArg);
        auto *g = emitExpr(*m.extraArgs[0]);
        auto *b = emitExpr(*m.extraArgs[1]);
        B.CreateCall(fSetColor, {h, valToI32(r), valToI32(g), valToI32(b)});
      } else if (m.name == "bgColor") {
        auto *r = emitExpr(*m.exprArg);
        auto *g = emitExpr(*m.extraArgs[0]);
        auto *b = emitExpr(*m.extraArgs[1]);
        B.CreateCall(fSetBgColor, {h, valToI32(r), valToI32(g), valToI32(b)});
      }
    }
  }

  bool emitNode(const ASTNode &n) {
    switch (n.kind) {
    case AK::Print: {
      auto &p = static_cast<const PrintStmt &>(n);
      auto *v = emitExpr(*p.expr);
      B.CreateCall(fPrint, {valToStr(v)});
      return true;
    }
    case AK::Column: {
      auto &c = static_cast<const ColumnStmt &>(n);
      B.CreateCall(fBeginCol);
      for (auto &ch : c.children)
        if (!emitNode(*ch))
          return false;
      B.CreateCall(fEndLayout);
      return true;
    }
    case AK::Row: {
      auto &r = static_cast<const RowStmt &>(n);
      B.CreateCall(fBeginRow);
      for (auto &ch : r.children)
        if (!emitNode(*ch))
          return false;
      B.CreateCall(fEndLayout);
      return true;
    }
    case AK::Button: {
      auto &b = static_cast<const ButtonStmt &>(n);
      auto *textVal = emitExpr(*b.textExpr);
      auto *h = B.CreateCall(fCreateBtn, {valToStr(textVal)});
      emitWidgetMethods(h, b.methods);
      return true;
    }
    case AK::Label: {
      auto &l = static_cast<const LabelStmt &>(n);
      auto *textVal = emitExpr(*l.textExpr);
      auto *h = B.CreateCall(fCreateLbl, {valToStr(textVal)});
      emitWidgetMethods(h, l.methods);
      return true;
    }
    case AK::Input: {
      auto &inp = static_cast<const InputStmt &>(n);
      auto *hintVal = emitExpr(*inp.hintExpr);
      auto *h = B.CreateCall(fCreateInput, {valToStr(hintVal)});
      emitWidgetMethods(h, inp.methods);
      return true;
    }
    case AK::Checkbox: {
      auto &cb = static_cast<const CheckboxStmt &>(n);
      auto *textVal = emitExpr(*cb.textExpr);
      auto *h = B.CreateCall(fCreateCheckbox, {valToStr(textVal)});
      emitWidgetMethods(h, cb.methods);
      return true;
    }
    case AK::Slider: {
      auto &sl = static_cast<const SliderStmt &>(n);
      auto *h = B.CreateCall(fCreateSlider);
      emitWidgetMethods(h, sl.methods);
      return true;
    }
    case AK::Progress: {
      auto &pr = static_cast<const ProgressStmt &>(n);
      auto *h = B.CreateCall(fCreateProgress);
      emitWidgetMethods(h, pr.methods);
      return true;
    }
    case AK::Separator: {
      B.CreateCall(fCreateSeparator);
      return true;
    }
    case AK::Spacer: {
      auto &sp = static_cast<const SpacerStmt &>(n);
      auto *w = emitExpr(*sp.widthExpr);
      auto *h = emitExpr(*sp.heightExpr);
      B.CreateCall(fCreateSpacer, {valToI32(w), valToI32(h)});
      return true;
    }
    case AK::Task: {
      auto &t = static_cast<const TaskStmt &>(n);
      llvm::Value *th = B.CreateCall(fTaskRun, {makeCb(*t.body)});
      for (auto &m : t.methods) {
        if (m.name == "then")
          th = B.CreateCall(fTaskThen, {th, makeCb(*m.cbArg)});
        else if (m.name == "catch")
          th = B.CreateCall(fTaskCatch, {th, makeCb(*m.cbArg)});
      }
      return true;
    }
    case AK::VarDecl: {
      auto &vd = static_cast<const VarDeclStmt &>(n);
      llvm::Type *varType = I64();
      llvm::Value *initVal = nullptr;
      if (vd.init) {
        initVal = emitExpr(*vd.init);
        varType = initVal->getType();
      }
      // Create a global variable with appropriate zero initializer
      llvm::Constant *zeroInit = llvm::Constant::getNullValue(varType);
      auto &vi = declareVar(vd.name, varType, zeroInit);
      // Store initial value if present
      if (initVal)
        B.CreateStore(initVal, vi.gv);
      return true;
    }
    case AK::VarAssign: {
      auto &va = static_cast<const VarAssignStmt &>(n);
      VarInfo *vi = lookupVar(va.name);
      if (!vi) {
        // Auto-declare as global
        auto *val = emitExpr(*va.value);
        llvm::Constant *zeroInit = llvm::Constant::getNullValue(val->getType());
        auto &newVi = declareVar(va.name, val->getType(), zeroInit);
        B.CreateStore(val, newVi.gv);
      } else {
        auto *val = emitExpr(*va.value);
        // Type coercion if needed
        if (val->getType() != vi->type) {
          if (vi->type->isIntegerTy(64))
            val = valToI64(val);
          else if (vi->type->isDoubleTy()) {
            if (val->getType()->isIntegerTy())
              val = B.CreateSIToFP(val, F64());
          } else if (vi->type->isPointerTy()) {
            val = valToStr(val);
          }
        }
        B.CreateStore(val, vi->gv);
      }
      return true;
    }
    case AK::If: {
      auto &ifn = static_cast<const IfStmt &>(n);
      llvm::Function *fn = B.GetInsertBlock()->getParent();
      llvm::BasicBlock *mergeBB =
          llvm::BasicBlock::Create(*ctx_, "ifmerge", fn);

      for (size_t i = 0; i < ifn.branches.size(); i++) {
        auto &branch = ifn.branches[i];
        if (!branch.condition) {
          // else branch
          for (auto &s : branch.body)
            emitNode(*s);
          B.CreateBr(mergeBB);
        } else {
          auto *condVal = emitExpr(*branch.condition);
          auto *condBool = valToBool(condVal);
          auto *thenBB = llvm::BasicBlock::Create(*ctx_, "then", fn);
          llvm::BasicBlock *elseBB;
          if (i + 1 < ifn.branches.size()) {
            elseBB = llvm::BasicBlock::Create(*ctx_, "elif", fn);
          } else {
            elseBB = mergeBB;
          }
          B.CreateCondBr(condBool, thenBB, elseBB);

          B.SetInsertPoint(thenBB);
          for (auto &s : branch.body)
            emitNode(*s);
          B.CreateBr(mergeBB);

          if (elseBB != mergeBB) {
            B.SetInsertPoint(elseBB);
          }
        }
      }
      B.SetInsertPoint(mergeBB);
      return true;
    }
    case AK::WidgetSetText: {
      auto &st = static_cast<const WidgetSetTextStmt &>(n);
      auto *h = emitExpr(*st.handle);
      auto *v = emitExpr(*st.text);
      B.CreateCall(fSetText, {valToI64(h), valToStr(v)});
      return true;
    }
    case AK::WidgetSetVisible: {
      auto &sv = static_cast<const WidgetSetVisibleStmt &>(n);
      auto *h = emitExpr(*sv.handle);
      auto *v = emitExpr(*sv.value);
      B.CreateCall(fSetVisible, {valToI64(h), valToI32(v)});
      return true;
    }
    case AK::WidgetSetEnabled: {
      auto &se = static_cast<const WidgetSetEnabledStmt &>(n);
      auto *h = emitExpr(*se.handle);
      auto *v = emitExpr(*se.value);
      B.CreateCall(fSetEnabled, {valToI64(h), valToI32(v)});
      return true;
    }
    case AK::WidgetSetProgress: {
      auto &sp = static_cast<const WidgetSetProgressStmt &>(n);
      auto *h = emitExpr(*sp.handle);
      auto *v = emitExpr(*sp.value);
      B.CreateCall(fSetProgress, {valToI64(h), valToF32(v)});
      return true;
    }
    case AK::WidgetSetSlider: {
      auto &ss = static_cast<const WidgetSetSliderStmt &>(n);
      auto *h = emitExpr(*ss.handle);
      auto *v = emitExpr(*ss.value);
      B.CreateCall(fSetSlider, {valToI64(h), valToF32(v)});
      return true;
    }
    case AK::WidgetSetChecked: {
      auto &sc2 = static_cast<const WidgetSetCheckedStmt &>(n);
      auto *h = emitExpr(*sc2.handle);
      auto *v = emitExpr(*sc2.value);
      B.CreateCall(fSetChecked, {valToI64(h), valToI32(v)});
      return true;
    }
    case AK::MessageBox: {
      auto &mb = static_cast<const MessageBoxStmt &>(n);
      auto *t = emitExpr(*mb.title);
      auto *m = emitExpr(*mb.message);
      B.CreateCall(fMessageBox, {valToStr(t), valToStr(m)});
      return true;
    }
    case AK::SleepMs: {
      auto &sm = static_cast<const SleepMsStmt &>(n);
      auto *v = emitExpr(*sm.ms);
      B.CreateCall(fSleepMs, {valToI32(v)});
      return true;
    }
    case AK::FinishActivity: {
      B.CreateCall(fFinishActivity);
      return true;
    }
    case AK::PostToUI: {
      auto &pu = static_cast<const PostToUIStmt &>(n);
      B.CreateCall(fPostToUI, {makeCb(*pu.callback)});
      return true;
    }
    case AK::SetOnInit: {
      auto &si2 = static_cast<const SetOnInitStmt &>(n);
      B.CreateCall(fSetOnInit, {makeCb(*si2.callback)});
      return true;
    }
    case AK::SetOnDestroy: {
      auto &sd = static_cast<const SetOnDestroyStmt &>(n);
      B.CreateCall(fSetOnDestroy, {makeCb(*sd.callback)});
      return true;
    }
    default:
      return false;
    }
  }
};

struct Sym {
  const char *n;
  void *a;
};
static const Sym g_syms[] = {
    {"rt_window_create", (void *)(intptr_t)rt_window_create},
    {"rt_window_end", (void *)(intptr_t)rt_window_end},
    {"rt_begin_column", (void *)(intptr_t)rt_begin_column},
    {"rt_begin_row", (void *)(intptr_t)rt_begin_row},
    {"rt_end_layout", (void *)(intptr_t)rt_end_layout},
    {"rt_create_button", (void *)(intptr_t)rt_create_button},
    {"rt_create_label", (void *)(intptr_t)rt_create_label},
    {"rt_create_input", (void *)(intptr_t)rt_create_input},
    {"rt_create_checkbox", (void *)(intptr_t)rt_create_checkbox},
    {"rt_create_toggle", (void *)(intptr_t)rt_create_toggle},
    {"rt_create_slider", (void *)(intptr_t)rt_create_slider},
    {"rt_create_progress", (void *)(intptr_t)rt_create_progress},
    {"rt_create_separator", (void *)(intptr_t)rt_create_separator},
    {"rt_create_spacer", (void *)(intptr_t)rt_create_spacer},
    {"rt_widget_set_id", (void *)(intptr_t)rt_widget_set_id},
    {"rt_widget_on_click", (void *)(intptr_t)rt_widget_on_click},
    {"rt_widget_set_text", (void *)(intptr_t)rt_widget_set_text},
    {"rt_widget_set_accent", (void *)(intptr_t)rt_widget_set_accent},
    {"rt_widget_set_visible", (void *)(intptr_t)rt_widget_set_visible},
    {"rt_widget_set_enabled", (void *)(intptr_t)rt_widget_set_enabled},
    {"rt_widget_set_bold", (void *)(intptr_t)rt_widget_set_bold},
    {"rt_widget_set_font_size", (void *)(intptr_t)rt_widget_set_font_size},
    {"rt_widget_set_expand", (void *)(intptr_t)rt_widget_set_expand},
    {"rt_widget_set_weight", (void *)(intptr_t)rt_widget_set_weight},
    {"rt_widget_set_checked", (void *)(intptr_t)rt_widget_set_checked},
    {"rt_widget_get_checked", (void *)(intptr_t)rt_widget_get_checked},
    {"rt_widget_set_progress", (void *)(intptr_t)rt_widget_set_progress},
    {"rt_widget_set_slider", (void *)(intptr_t)rt_widget_set_slider},
    {"rt_widget_get_slider", (void *)(intptr_t)rt_widget_get_slider},
    {"rt_widget_on_change", (void *)(intptr_t)rt_widget_on_change},
    {"rt_widget_get_text", (void *)(intptr_t)rt_widget_get_text},
    {"rt_widget_set_hint", (void *)(intptr_t)rt_widget_set_hint},
    {"rt_widget_set_color", (void *)(intptr_t)rt_widget_set_color},
    {"rt_widget_set_bg_color", (void *)(intptr_t)rt_widget_set_bg_color},
    {"rt_find_widget", (void *)(intptr_t)rt_find_widget},
    {"rt_print", (void *)(intptr_t)rt_print},
    {"rt_message_box", (void *)(intptr_t)rt_message_box},
    {"rt_confirm_box", (void *)(intptr_t)rt_confirm_box},
    {"rt_set_clipboard", (void *)(intptr_t)rt_set_clipboard},
    {"rt_get_clipboard", (void *)(intptr_t)rt_get_clipboard},
    {"rt_set_on_init", (void *)(intptr_t)rt_set_on_init},
    {"rt_set_on_destroy", (void *)(intptr_t)rt_set_on_destroy},
    {"rt_set_on_resume", (void *)(intptr_t)rt_set_on_resume},
    {"rt_set_on_pause", (void *)(intptr_t)rt_set_on_pause},
    {"rt_set_on_back", (void *)(intptr_t)rt_set_on_back},
    {"rt_finish_activity", (void *)(intptr_t)rt_finish_activity},
    {"rt_task_run", (void *)(intptr_t)rt_task_run},
    {"rt_task_then", (void *)(intptr_t)rt_task_then},
    {"rt_task_catch", (void *)(intptr_t)rt_task_catch},
    {"rt_post_to_ui", (void *)(intptr_t)rt_post_to_ui},
    {"rt_sleep_ms", (void *)(intptr_t)rt_sleep_ms},
    {"rt_get_tick_ms", (void *)(intptr_t)rt_get_tick_ms},
    {"rt_concat", (void *)(intptr_t)rt_concat},
    {"rt_int_to_str", (void *)(intptr_t)rt_int_to_str},
    {"rt_float_to_str", (void *)(intptr_t)rt_float_to_str},
    {"rt_str_eq", (void *)(intptr_t)rt_str_eq},
    {nullptr, nullptr}};

static std::string readFile(const std::string &p) {
  std::ifstream f(p);
  if (!f.is_open())
    return "";
  std::ostringstream s;
  s << f.rdbuf();
  return s.str();
}

int main(int argc, char **argv) {
  std::string inp, outp;
  bool ir = false;
  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];
    if (a == "-o" && i + 1 < argc)
      outp = argv[++i];
    else if (a == "--emit-ir")
      ir = true;
    else if (a[0] != '-')
      inp = a;
    else {
      std::cerr << "Unknown option: " << a << "\n";
      return 1;
    }
  }
  if (inp.empty()) {
    std::cerr << "Usage: claudescript <file.cs> [--emit-ir] [-o out.ll]\n";
    return 1;
  }
  std::string src = readFile(inp);
  if (src.empty()) {
    std::ifstream t(inp);
    if (!t.is_open()) {
      std::cerr << "Cannot open '" << inp << "'\n";
      return 1;
    }
    std::cerr << "File '" << inp << "' is empty\n";
    return 1;
  }
  Lexer lex(src);
  Parser par(lex);
  auto ast = par.parseProgram();
  if (par.hasError() || !ast) {
    for (auto &e : par.errors())
      std::cerr << e.format() << "\n";
    return 1;
  }
  Codegen cg;
  if (!cg.generate(*ast)) {
    std::cerr << "Codegen failed\n";
    return 1;
  }
  std::string ve;
  if (!cg.verify(ve)) {
    std::cerr << "Verify failed:\n" << ve << "\n";
    return 1;
  }
  if (ir || !outp.empty()) {
    if (!outp.empty()) {
      std::error_code ec;
      llvm::raw_fd_ostream o(outp, ec);
      if (ec) {
        std::cerr << "Cannot write '" << outp << "'\n";
        return 1;
      }
      cg.printIR(o);
    } else {
      cg.printIR(llvm::outs());
    }
    return 0;
  }
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  auto jE = llvm::orc::LLJITBuilder().create();
  if (!jE) {
    std::cerr << "JIT create failed: ";
    llvm::logAllUnhandledErrors(jE.takeError(), llvm::errs());
    return 1;
  }
  auto &jit = *jE;
  auto &dl = jit->getMainJITDylib();
  llvm::orc::SymbolMap sm;
  for (int i = 0; g_syms[i].n; i++)
    sm[jit->mangleAndIntern(g_syms[i].n)] = llvm::orc::ExecutorSymbolDef(
        llvm::orc::ExecutorAddr::fromPtr(g_syms[i].a),
        llvm::JITSymbolFlags::Exported | llvm::JITSymbolFlags::Callable);
  if (auto e = dl.define(llvm::orc::absoluteSymbols(std::move(sm)))) {
    std::cerr << "Symbol reg failed: ";
    llvm::logAllUnhandledErrors(std::move(e), llvm::errs());
    return 1;
  }
  if (auto e = jit->addIRModule(
          llvm::orc::ThreadSafeModule(cg.takeMod(), cg.takeCtx()))) {
    std::cerr << "IR add failed: ";
    llvm::logAllUnhandledErrors(std::move(e), llvm::errs());
    return 1;
  }
  auto ms = jit->lookup("cs_main");
  if (!ms) {
    std::cerr << "Lookup failed: ";
    llvm::logAllUnhandledErrors(ms.takeError(), llvm::errs());
    return 1;
  }
  int result = ms->toPtr<int (*)()>()();

  {
    std::lock_guard<std::mutex> lk(g_taskThreadsMutex);
    for (auto &t : g_taskThreads) {
      if (t.joinable())
        t.join();
    }
    g_taskThreads.clear();
  }

  return result;
}

#include "main.moc"
