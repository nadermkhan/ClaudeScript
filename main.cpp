#define QT_NO_KEYWORDS
#define QT_STATICPLUGIN

#ifdef QT_STATICPLUGIN
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QMessageBox>
#include <QClipboard>
#include <QTimer>
#include <QThread>
#include <QMetaObject>
#include <QStyle>
#include <QStyleFactory>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QScreen>
#include <QFont>
#include <QSizePolicy>
#include <QScrollArea>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stack>
#include <map>
#include <condition_variable>
#include <deque>

typedef void (*RtVoidCallback)(void);
typedef void (*RtIntCallback)(long long);
typedef void (*RtStringCallback)(const char*);

static const char* DARK_STYLESHEET = R"(
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
    CS_BUTTON, CS_LABEL, CS_INPUT, CS_CHECKBOX, CS_TOGGLE,
    CS_SLIDER, CS_PROGRESS, CS_SEPARATOR, CS_SPACER
};

struct CSWidget {
    CSWidgetType type = CS_NONE;
    QWidget* widget = nullptr;
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

    explicit CSMainWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setObjectName("CSMainWindow");
    }

protected:
    void keyPressEvent(QKeyEvent* e) override {
        if (e->key() == Qt::Key_Escape && onBackCb) {
            onBackCb();
            return;
        }
        QWidget::keyPressEvent(e);
    }
    void closeEvent(QCloseEvent* e) override {
        if (onDestroyCb) onDestroyCb();
        QWidget::closeEvent(e);
    }
    void showEvent(QShowEvent* e) override {
        QWidget::showEvent(e);
        if (onResumeCb) onResumeCb();
    }
    void hideEvent(QHideEvent* e) override {
        if (onPauseCb) onPauseCb();
        QWidget::hideEvent(e);
    }
};

struct Activity {
    int id = -1;
    CSMainWindow* window = nullptr;
    QVBoxLayout* rootLayout = nullptr;
    int firstWidget = 0;
    int widgetCount = 0;
};

static QApplication* g_app = nullptr;
static std::deque<CSWidget> g_widgets;
static std::stack<QLayout*> g_layoutStack;
static std::vector<Activity> g_activities;
static int g_currentActivity = -1;
static std::recursive_mutex g_widgetMutex;
static std::atomic<bool> g_shuttingDown{false};

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

static CSMainWindow* currentWindow() {
    if (g_currentActivity >= 0 && g_currentActivity < (int)g_activities.size())
        return g_activities[g_currentActivity].window;
    return nullptr;
}

static QLayout* currentLayout() {
    if (!g_layoutStack.empty()) return g_layoutStack.top();
    return nullptr;
}

static void addWidgetToLayout(QWidget* w) {
    QLayout* lay = currentLayout();
    if (lay) lay->addWidget(w);
}

static void addLayoutToLayout(QLayout* child) {
    QLayout* lay = currentLayout();
    if (!lay) return;
    if (auto* vbox = dynamic_cast<QVBoxLayout*>(lay)) {
        vbox->addLayout(child);
    } else if (auto* hbox = dynamic_cast<QHBoxLayout*>(lay)) {
        hbox->addLayout(child);
    } else {
        lay->addItem(child);
    }
}

static void ensureApp() {
    if (!g_app) {
        static int argc = 1;
        static char arg0[] = "claudescript";
        static char* argv[] = { arg0, nullptr };
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

extern "C" void rt_window_create(const char* title, int w, int h) {
    ensureApp();
    g_shuttingDown = false;
    Activity act;
    act.id = (int)g_activities.size();
    act.window = new CSMainWindow();
    act.window->setWindowTitle(QString::fromUtf8(title));
    int ww = w > 0 ? w : 400;
    int wh = h > 0 ? h : 300;
    act.window->resize(ww, wh);
    QScreen* screen = QApplication::primaryScreen();
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
    while (!g_layoutStack.empty()) g_layoutStack.pop();
    g_layoutStack.push(act.rootLayout);
}

extern "C" void rt_window_end(void) {
    if (g_currentActivity < 0 || g_currentActivity >= (int)g_activities.size()) return;
    Activity& act = g_activities[g_currentActivity];
    act.widgetCount = (int)g_widgets.size() - act.firstWidget;
    act.rootLayout->addStretch();
    if (act.window->onCreateCb) act.window->onCreateCb();
    act.window->show();
    g_app->exec();

    g_shuttingDown = true;

    {
        std::lock_guard<std::mutex> lk(g_taskThreadsMutex);
        for (auto& t : g_taskThreads) {
            if (t.joinable()) t.join();
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
        for (auto& a : g_activities) {
            if (a.window) {
                delete a.window;
                a.window = nullptr;
            }
        }
        g_activities.clear();
        g_widgets.clear();
    }

    while (!g_layoutStack.empty()) g_layoutStack.pop();
    delete g_app;
    g_app = nullptr;
}

extern "C" void rt_begin_column(void) {
    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    addLayoutToLayout(layout);
    g_layoutStack.push(layout);
}

extern "C" void rt_begin_row(void) {
    auto* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    addLayoutToLayout(layout);
    g_layoutStack.push(layout);
}

extern "C" void rt_end_layout(void) {
    if (g_layoutStack.size() > 1)
        g_layoutStack.pop();
}

extern "C" long long rt_create_button(const char* text) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    auto* btn = new QPushButton(QString::fromUtf8(text));
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
        if (g_shuttingDown) return;
        if (validHandle(idx) && g_widgets[idx].onClick)
            g_widgets[idx].onClick();
    });
    return idx;
}

extern "C" long long rt_create_label(const char* text) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    auto* lbl = new QLabel(QString::fromUtf8(text));
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

extern "C" long long rt_create_input(const char* hint) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    auto* edit = new QLineEdit();
    if (hint && hint[0]) edit->setPlaceholderText(QString::fromUtf8(hint));
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    long long idx = (long long)g_widgets.size();
    CSWidget cw;
    cw.type = CS_INPUT;
    cw.widget = edit;
    g_widgets.push_back(cw);
    addWidgetToLayout(edit);
    QObject::connect(edit, &QLineEdit::textChanged, [idx](const QString& text) {
        std::lock_guard<std::recursive_mutex> lk2(g_widgetMutex);
        if (g_shuttingDown) return;
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

extern "C" long long rt_create_checkbox(const char* text) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    auto* chk = new QCheckBox(QString::fromUtf8(text));
    chk->setCursor(Qt::PointingHandCursor);
    long long idx = (long long)g_widgets.size();
    CSWidget cw;
    cw.type = CS_CHECKBOX;
    cw.widget = chk;
    g_widgets.push_back(cw);
    addWidgetToLayout(chk);
    QObject::connect(chk, &QCheckBox::stateChanged, [idx](int) {
        std::lock_guard<std::recursive_mutex> lk2(g_widgetMutex);
        if (g_shuttingDown) return;
        if (validHandle(idx) && g_widgets[idx].onChange)
            g_widgets[idx].onChange();
    });
    return idx;
}

extern "C" long long rt_create_toggle(const char* text) {
    return rt_create_checkbox(text);
}

extern "C" long long rt_create_slider(void) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    auto* sl = new QSlider(Qt::Horizontal);
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
        if (g_shuttingDown) return;
        if (validHandle(idx) && g_widgets[idx].onChange)
            g_widgets[idx].onChange();
    });
    return idx;
}

extern "C" long long rt_create_progress(void) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    auto* pb = new QProgressBar();
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
    auto* sep = new QFrame();
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
    auto* spacer = new QWidget();
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

extern "C" void rt_widget_set_id(long long h, const char* id) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    g_widgets[h].name = QString::fromUtf8(id);
    g_widgets[h].widget->setObjectName(QString::fromUtf8(id));
}

extern "C" void rt_widget_on_click(long long h, void (*fn)(void)) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    g_widgets[h].onClick = fn;
}

extern "C" void rt_widget_set_text(long long h, const char* text) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    QString t = QString::fromUtf8(text);
    QWidget* w = g_widgets[h].widget;
    if (QThread::currentThread() != g_app->thread()) {
        QMetaObject::invokeMethod(w, [w, t]() {
            if (auto* btn = qobject_cast<QPushButton*>(w)) btn->setText(t);
            else if (auto* lbl = qobject_cast<QLabel*>(w)) lbl->setText(t);
            else if (auto* edit = qobject_cast<QLineEdit*>(w)) edit->setText(t);
        }, Qt::QueuedConnection);
    } else {
        if (auto* btn = qobject_cast<QPushButton*>(w)) btn->setText(t);
        else if (auto* lbl = qobject_cast<QLabel*>(w)) lbl->setText(t);
        else if (auto* edit = qobject_cast<QLineEdit*>(w)) edit->setText(t);
    }
}

extern "C" void rt_widget_set_accent(long long h, int a) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    g_widgets[h].isAccent = (a != 0);
    QWidget* w = g_widgets[h].widget;
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
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    bool vis = (v != 0);
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, [w, vis]() { w->setVisible(vis); }, Qt::QueuedConnection);
    else
        w->setVisible(vis);
}

extern "C" void rt_widget_set_enabled(long long h, int e) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    bool en = (e != 0);
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, [w, en]() { w->setEnabled(en); }, Qt::QueuedConnection);
    else
        w->setEnabled(en);
}

extern "C" void rt_widget_set_bold(long long h, int b) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    bool bold = (b != 0);
    auto apply = [w, bold]() {
        QFont f = w->font();
        f.setBold(bold);
        w->setFont(f);
        if (auto* lbl = qobject_cast<QLabel*>(w)) {
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
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    auto apply = [w, sz]() {
        QFont f = w->font();
        f.setPointSize(sz > 0 ? sz : 10);
        w->setFont(f);
        if (auto* lbl = qobject_cast<QLabel*>(w)) {
            if (sz >= 32) lbl->setProperty("h1", true);
            else if (sz >= 24) lbl->setProperty("title", true);
            else if (sz >= 18) lbl->setProperty("heading", true);
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
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
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
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    int stretch = (int)(weight * 10);
    if (stretch < 1) stretch = 1;
    auto apply = [w, stretch]() {
        QLayout* parent = w->parentWidget() ? w->parentWidget()->layout() : nullptr;
        if (parent) {
            if (auto* vbox = qobject_cast<QVBoxLayout*>(parent))
                vbox->setStretchFactor(w, stretch);
            else if (auto* hbox = qobject_cast<QHBoxLayout*>(parent))
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
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    bool checked = (c != 0);
    auto apply = [w, checked]() {
        if (auto* chk = qobject_cast<QCheckBox*>(w))
            chk->setChecked(checked);
    };
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
    else
        apply();
}

extern "C" int rt_widget_get_checked(long long h) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return 0;
    QWidget* w = g_widgets[h].widget;
    if (QThread::currentThread() != g_app->thread()) {
        int result = 0;
        QMetaObject::invokeMethod(w, [w, &result]() {
            if (auto* chk = qobject_cast<QCheckBox*>(w))
                result = chk->isChecked() ? 1 : 0;
        }, Qt::BlockingQueuedConnection);
        return result;
    }
    if (auto* chk = qobject_cast<QCheckBox*>(w))
        return chk->isChecked() ? 1 : 0;
    return 0;
}

extern "C" void rt_widget_set_progress(long long h, float v) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    QWidget* w = g_widgets[h].widget;
    int val = (int)(v * 1000);
    auto apply = [w, val]() {
        if (auto* pb = qobject_cast<QProgressBar*>(w))
            pb->setValue(val);
    };
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
    else
        apply();
}

extern "C" void rt_widget_set_slider(long long h, float v) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    QWidget* w = g_widgets[h].widget;
    int val = (int)(v * 1000);
    auto apply = [w, val]() {
        if (auto* sl = qobject_cast<QSlider*>(w))
            sl->setValue(val);
    };
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
    else
        apply();
}

extern "C" float rt_widget_get_slider(long long h) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return 0.0f;
    QWidget* w = g_widgets[h].widget;
    if (QThread::currentThread() != g_app->thread()) {
        float result = 0.0f;
        QMetaObject::invokeMethod(w, [w, &result]() {
            if (auto* sl = qobject_cast<QSlider*>(w))
                result = (float)sl->value() / 1000.0f;
        }, Qt::BlockingQueuedConnection);
        return result;
    }
    if (auto* sl = qobject_cast<QSlider*>(w))
        return (float)sl->value() / 1000.0f;
    return 0.0f;
}

extern "C" void rt_widget_on_change(long long h, void (*fn)(void)) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    g_widgets[h].onChange = fn;
}

extern "C" const char* rt_widget_get_text(long long h) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return "";
    static thread_local std::string buf;
    QWidget* w = g_widgets[h].widget;
    if (QThread::currentThread() != g_app->thread()) {
        QMetaObject::invokeMethod(w, [w, &buf]() {
            if (auto* edit = qobject_cast<QLineEdit*>(w)) buf = edit->text().toUtf8().constData();
            else if (auto* lbl = qobject_cast<QLabel*>(w)) buf = lbl->text().toUtf8().constData();
            else if (auto* btn = qobject_cast<QPushButton*>(w)) buf = btn->text().toUtf8().constData();
            else buf.clear();
        }, Qt::BlockingQueuedConnection);
    } else {
        if (auto* edit = qobject_cast<QLineEdit*>(w)) buf = edit->text().toUtf8().constData();
        else if (auto* lbl = qobject_cast<QLabel*>(w)) buf = lbl->text().toUtf8().constData();
        else if (auto* btn = qobject_cast<QPushButton*>(w)) buf = btn->text().toUtf8().constData();
        else buf.clear();
    }
    return buf.c_str();
}

extern "C" void rt_widget_set_hint(long long h, const char* hint) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    QString hintStr = QString::fromUtf8(hint);
    auto apply = [w, hintStr]() {
        if (auto* edit = qobject_cast<QLineEdit*>(w))
            edit->setPlaceholderText(hintStr);
    };
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
    else
        apply();
}

extern "C" void rt_widget_set_color(long long h, int r, int g, int b) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
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
    if (!validHandle(h)) return;
    QWidget* w = g_widgets[h].widget;
    QString style = QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b);
    auto apply = [w, style]() {
        QString existing = w->styleSheet();
        w->setStyleSheet(existing + style);
    };
    if (QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(w, apply, Qt::QueuedConnection);
    else
        apply();
}

extern "C" long long rt_find_widget(const char* id) {
    std::lock_guard<std::recursive_mutex> lk(g_widgetMutex);
    QString qid = QString::fromUtf8(id);
    for (int i = 0; i < (int)g_widgets.size(); i++) {
        if (g_widgets[i].name == qid) return (long long)i;
    }
    return -1;
}

extern "C" void rt_print(const char* text) {
    printf("%s\n", text);
    fflush(stdout);
}

extern "C" void rt_message_box(const char* title, const char* msg) {
    if (g_shuttingDown) return;
    CSMainWindow* win = currentWindow();
    QString qtitle = QString::fromUtf8(title);
    QString qmsg = QString::fromUtf8(msg);
    auto show = [win, qtitle, qmsg]() {
        QMessageBox::information(win, qtitle, qmsg);
    };
    if (g_app && QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(win ? (QObject*)win : (QObject*)g_app, show, Qt::BlockingQueuedConnection);
    else
        show();
}

extern "C" int rt_confirm_box(const char* title, const char* msg) {
    if (g_shuttingDown) return 0;
    CSMainWindow* win = currentWindow();
    QString qtitle = QString::fromUtf8(title);
    QString qmsg = QString::fromUtf8(msg);
    int result = 0;
    auto show = [win, qtitle, qmsg, &result]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            win, qtitle, qmsg,
            QMessageBox::Yes | QMessageBox::No);
        result = (reply == QMessageBox::Yes) ? 1 : 0;
    };
    if (g_app && QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(win ? (QObject*)win : (QObject*)g_app, show, Qt::BlockingQueuedConnection);
    else
        show();
    return result;
}

extern "C" void rt_set_clipboard(const char* text) {
    QString t = QString::fromUtf8(text);
    auto apply = [t]() {
        QClipboard* cb = QApplication::clipboard();
        if (cb) cb->setText(t);
    };
    if (g_app && QThread::currentThread() != g_app->thread())
        QMetaObject::invokeMethod(g_app, apply, Qt::QueuedConnection);
    else
        apply();
}

extern "C" const char* rt_get_clipboard(void) {
    static thread_local std::string buf;
    if (g_app && QThread::currentThread() != g_app->thread()) {
        QMetaObject::invokeMethod(g_app, [&buf]() {
            QClipboard* cb = QApplication::clipboard();
            if (cb) buf = cb->text().toUtf8().constData();
            else buf.clear();
        }, Qt::BlockingQueuedConnection);
    } else {
        QClipboard* cb = QApplication::clipboard();
        if (cb) buf = cb->text().toUtf8().constData();
        else buf.clear();
    }
    return buf.c_str();
}

extern "C" void rt_set_on_init(void (*fn)(void)) {
    CSMainWindow* w = currentWindow();
    if (w) w->onCreateCb = fn;
}

extern "C" void rt_set_on_destroy(void (*fn)(void)) {
    CSMainWindow* w = currentWindow();
    if (w) w->onDestroyCb = fn;
}

extern "C" void rt_set_on_resume(void (*fn)(void)) {
    CSMainWindow* w = currentWindow();
    if (w) w->onResumeCb = fn;
}

extern "C" void rt_set_on_pause(void (*fn)(void)) {
    CSMainWindow* w = currentWindow();
    if (w) w->onPauseCb = fn;
}

extern "C" void rt_set_on_back(void (*fn)(void)) {
    CSMainWindow* w = currentWindow();
    if (w) w->onBackCb = fn;
}

extern "C" void rt_finish_activity(void) {
    CSMainWindow* w = currentWindow();
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
            if (fn) fn();
        } catch (...) {
            hadError = true;
        }
        if (g_shuttingDown) return;

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
            for (auto& tcb : thens) {
                if (tcb && g_app && !g_shuttingDown) {
                    QMetaObject::invokeMethod(g_app, [tcb]() {
                        if (!g_shuttingDown) tcb();
                    }, Qt::QueuedConnection);
                }
            }
        } else {
            for (auto& ccb : catches) {
                if (ccb && g_app && !g_shuttingDown) {
                    QMetaObject::invokeMethod(g_app, [ccb]() {
                        if (!g_shuttingDown) ccb();
                    }, Qt::QueuedConnection);
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
    if (!fn) return task;
    std::shared_ptr<TaskInfo> info;
    {
        std::lock_guard<std::recursive_mutex> lk(g_taskMapMutex);
        auto it = g_taskMap.find(task);
        if (it != g_taskMap.end()) info = it->second;
    }
    if (!info) {
        return task;
    }
    std::lock_guard<std::recursive_mutex> lk(info->mtx);
    if (info->completed) {
        if (!info->hadError && g_app && !g_shuttingDown) {
            QMetaObject::invokeMethod(g_app, [fn]() {
                if (!g_shuttingDown) fn();
            }, Qt::QueuedConnection);
        }
    } else {
        info->thenCallbacks.push_back(fn);
    }
    return task;
}

extern "C" long long rt_task_catch(long long task, void (*fn)(void)) {
    if (!fn) return task;
    std::shared_ptr<TaskInfo> info;
    {
        std::lock_guard<std::recursive_mutex> lk(g_taskMapMutex);
        auto it = g_taskMap.find(task);
        if (it != g_taskMap.end()) info = it->second;
    }
    if (!info) return task;
    std::lock_guard<std::recursive_mutex> lk(info->mtx);
    if (info->completed) {
        if (info->hadError && g_app && !g_shuttingDown) {
            QMetaObject::invokeMethod(g_app, [fn]() {
                if (!g_shuttingDown) fn();
            }, Qt::QueuedConnection);
        }
    } else {
        info->catchCallbacks.push_back(fn);
    }
    return task;
}

extern "C" void rt_post_to_ui(void (*fn)(void)) {
    if (!fn) return;
    if (g_app && !g_shuttingDown) {
        QMetaObject::invokeMethod(g_app, [fn]() {
            if (!g_shuttingDown) fn();
        }, Qt::QueuedConnection);
    } else if (!g_shuttingDown) {
        fn();
    }
}

extern "C" void rt_sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

extern "C" long long rt_get_tick_ms(void) {
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return (long long)ms.count();
}

struct SourceLoc { int line = 1; int col = 1; };
struct Error {
    std::string message; SourceLoc loc;
    std::string format() const {
        return "Error at line " + std::to_string(loc.line) + ", col " + std::to_string(loc.col) + ": " + message;
    }
};

enum class TK { Str, Int, Id, LP, RP, LB, RB, Comma, Semi, Dot, Arrow, Eof, Bad };
static const char* tkName(TK k) {
    switch (k) {
    case TK::Str: return "string"; case TK::Int: return "integer"; case TK::Id: return "identifier";
    case TK::LP: return "'('"; case TK::RP: return "')'"; case TK::LB: return "'{'"; case TK::RB: return "'}'";
    case TK::Comma: return "','"; case TK::Semi: return "';'"; case TK::Dot: return "'.'"; case TK::Arrow: return "'=>'";
    case TK::Eof: return "EOF"; case TK::Bad: return "invalid";
    } return "?";
}

struct Token { TK kind = TK::Bad; std::string text; SourceLoc loc; long long intVal = 0; };

class Lexer {
public:
    explicit Lexer(const std::string& s) : src_(s), pos_(0) {}
    Token next() {
        skipWS();
        if (pos_ >= src_.size()) return mk(TK::Eof, "");
        SourceLoc sl = loc_; char c = src_[pos_];
        if (c == '"') return lexStr();
        if (c == '-' && pos_ + 1 < src_.size() && src_[pos_ + 1] >= '0' && src_[pos_ + 1] <= '9') return lexNum();
        if (c >= '0' && c <= '9') return lexNum();
        if (isIS(c)) return lexId();
        switch (c) {
        case '(': adv(); return mkAt(TK::LP, "(", sl);
        case ')': adv(); return mkAt(TK::RP, ")", sl);
        case '{': adv(); return mkAt(TK::LB, "{", sl);
        case '}': adv(); return mkAt(TK::RB, "}", sl);
        case ',': adv(); return mkAt(TK::Comma, ",", sl);
        case ';': adv(); return mkAt(TK::Semi, ";", sl);
        case '.': adv(); return mkAt(TK::Dot, ".", sl);
        case '=': adv();
            if (pos_ < src_.size() && src_[pos_] == '>') { adv(); return mkAt(TK::Arrow, "=>", sl); }
            return err("Unexpected '='", sl);
        default: adv(); return err(std::string("Unexpected '") + c + "'", sl);
        }
    }
    Token peek() {
        size_t sp = pos_; SourceLoc sl = loc_;
        Token t = next();
        pos_ = sp; loc_ = sl;
        return t;
    }
private:
    std::string src_; size_t pos_; SourceLoc loc_;
    char adv() {
        char c = src_[pos_++];
        if (c == '\n') { loc_.line++; loc_.col = 1; }
        else loc_.col++;
        return c;
    }
    bool isIS(char c) const { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_'; }
    bool isIC(char c) const { return isIS(c) || (c >= '0' && c <= '9'); }
    void skipWS() {
        while (pos_ < src_.size()) {
            char c = src_[pos_];
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { adv(); continue; }
            if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '/') {
                while (pos_ < src_.size() && src_[pos_] != '\n') adv();
                continue;
            }
            if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '*') {
                adv(); adv();
                while (pos_ + 1 < src_.size()) {
                    if (src_[pos_] == '*' && src_[pos_ + 1] == '/') { adv(); adv(); break; }
                    adv();
                }
                continue;
            }
            break;
        }
    }
    Token lexStr() {
        SourceLoc sl = loc_; adv(); std::string v;
        while (pos_ < src_.size() && src_[pos_] != '"') {
            if (src_[pos_] == '\\') {
                adv();
                if (pos_ >= src_.size()) return err("Unterminated escape", sl);
                char e = src_[pos_];
                switch (e) {
                case 'n': v += '\n'; break; case 't': v += '\t'; break;
                case '\\': v += '\\'; break; case '"': v += '"'; break;
                default: v += e;
                }
                adv();
            }
            else { v += src_[pos_]; adv(); }
        }
        if (pos_ >= src_.size()) return err("Unterminated string", sl);
        adv();
        Token t; t.kind = TK::Str; t.text = v; t.loc = sl; return t;
    }
    Token lexNum() {
        SourceLoc sl = loc_; std::string v;
        bool neg = false;
        if (pos_ < src_.size() && src_[pos_] == '-') {
            neg = true;
            v += '-';
            adv();
        }
        while (pos_ < src_.size() && src_[pos_] >= '0' && src_[pos_] <= '9') { v += src_[pos_]; adv(); }
        Token t; t.kind = TK::Int; t.text = v; t.loc = sl;
        try {
            t.intVal = std::stoll(v);
        } catch (...) {
            return err("Integer out of range: " + v, sl);
        }
        return t;
    }
    Token lexId() {
        SourceLoc sl = loc_; std::string v;
        while (pos_ < src_.size() && isIC(src_[pos_])) { v += src_[pos_]; adv(); }
        Token t; t.kind = TK::Id; t.text = v; t.loc = sl; return t;
    }
    Token mk(TK k, const std::string& t) { Token tk; tk.kind = k; tk.text = t; tk.loc = loc_; return tk; }
    Token mkAt(TK k, const std::string& t, SourceLoc l) { Token tk; tk.kind = k; tk.text = t; tk.loc = l; return tk; }
    Token err(const std::string& m, SourceLoc l) { Token tk; tk.kind = TK::Bad; tk.text = m; tk.loc = l; return tk; }
};

struct ASTNode; using ASTPtr = std::unique_ptr<ASTNode>;
enum class AK { Print, Arrow, Button, Label, Column, Row, Task, Window };

struct ASTNode {
    AK kind; SourceLoc loc; virtual ~ASTNode() = default;
protected:
    ASTNode(AK k, SourceLoc l) : kind(k), loc(l) {}
};

struct ArrowFunc : ASTNode {
    std::vector<ASTPtr> body;
    ArrowFunc(SourceLoc l) : ASTNode(AK::Arrow, l) {}
};

struct PrintStmt : ASTNode {
    std::string text;
    PrintStmt(SourceLoc l, std::string t) : ASTNode(AK::Print, l), text(std::move(t)) {}
};

struct MCall {
    std::string name; SourceLoc loc;
    std::string sArg;
    std::unique_ptr<ArrowFunc> cbArg;
};

struct ButtonStmt : ASTNode {
    std::string text; std::vector<MCall> methods;
    ButtonStmt(SourceLoc l, std::string t) : ASTNode(AK::Button, l), text(std::move(t)) {}
};

struct LabelStmt : ASTNode {
    std::string text; std::vector<MCall> methods;
    LabelStmt(SourceLoc l, std::string t) : ASTNode(AK::Label, l), text(std::move(t)) {}
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
    std::unique_ptr<ArrowFunc> body; std::vector<MCall> methods;
    TaskStmt(SourceLoc l) : ASTNode(AK::Task, l) {}
};

struct WindowStmt : ASTNode {
    std::string title; int width, height; std::vector<ASTPtr> body;
    WindowStmt(SourceLoc l, std::string t, int w, int h)
        : ASTNode(AK::Window, l), title(std::move(t)), width(w), height(h) {}
};

class Parser {
public:
    explicit Parser(Lexer& l) : lex_(l), bad_(false) {}
    std::unique_ptr<WindowStmt> parseProgram() {
        auto w = parseWindow();
        if (!w) return nullptr;
        Token e = next();
        if (e.kind != TK::Eof) {
            er(e.loc, "Expected EOF, got " + std::string(tkName(e.kind)));
            return nullptr;
        }
        return w;
    }
    bool hasError() const { return bad_; }
    const std::vector<Error>& errors() const { return errs_; }
private:
    Lexer& lex_; bool bad_; std::vector<Error> errs_; std::vector<Token> buf_;
    Token next() {
        if (!buf_.empty()) { Token t = std::move(buf_.front()); buf_.erase(buf_.begin()); return t; }
        return lex_.next();
    }
    Token peek() {
        if (buf_.empty()) buf_.push_back(lex_.next());
        return buf_.front();
    }
    bool ex(TK k, Token& o) {
        o = next();
        if (o.kind == TK::Bad) { er(o.loc, o.text); return false; }
        if (o.kind != k) {
            er(o.loc, std::string("Expected ") + tkName(k) + ", got " + tkName(o.kind) + " '" + o.text + "'");
            return false;
        }
        return true;
    }
    bool exK(TK k) { Token t; return ex(k, t); }
    void er(SourceLoc l, const std::string& m) { bad_ = true; errs_.push_back({m, l}); }
    std::unique_ptr<WindowStmt> parseWindow() {
        Token id; if (!ex(TK::Id, id)) return nullptr;
        if (id.text != "window") { er(id.loc, "Expected 'window'"); return nullptr; }
        if (!exK(TK::LP)) return nullptr;
        Token t; if (!ex(TK::Str, t)) return nullptr; if (!exK(TK::Comma)) return nullptr;
        Token w; if (!ex(TK::Int, w)) return nullptr; if (!exK(TK::Comma)) return nullptr;
        Token h; if (!ex(TK::Int, h)) return nullptr; if (!exK(TK::Comma)) return nullptr;
        auto ar = parseArrow(); if (!ar) return nullptr;
        if (!exK(TK::RP)) return nullptr; if (!exK(TK::Semi)) return nullptr;
        auto wn = std::make_unique<WindowStmt>(id.loc, t.text, (int)w.intVal, (int)h.intVal);
        wn->body = std::move(ar->body);
        return wn;
    }
    std::unique_ptr<ArrowFunc> parseArrow() {
        SourceLoc sl = peek().loc;
        if (!exK(TK::LP)) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        if (!exK(TK::Arrow)) return nullptr;
        auto f = std::make_unique<ArrowFunc>(sl);
        if (peek().kind == TK::LB) {
            next();
            while (peek().kind != TK::RB && peek().kind != TK::Eof) {
                auto s = parseStmt();
                if (!s) {
                    if (bad_) return nullptr;
                    continue;
                }
                f->body.push_back(std::move(s));
            }
            if (peek().kind == TK::Eof) { er(peek().loc, "Unexpected EOF, expected '}'"); return nullptr; }
            if (!exK(TK::RB)) return nullptr;
        } else {
            auto s = parseExpr();
            if (!s) return nullptr;
            f->body.push_back(std::move(s));
        }
        return f;
    }
    ASTPtr parseStmt() {
        Token p = peek();
        if (p.kind == TK::Bad) { er(p.loc, p.text); return nullptr; }
        if (p.kind != TK::Id) { er(p.loc, "Expected statement, got " + std::string(tkName(p.kind))); return nullptr; }
        if (p.text == "column") return parseCol();
        if (p.text == "row") return parseRow();
        if (p.text == "button") return parseBtn();
        if (p.text == "label") return parseLbl();
        if (p.text == "print") return parsePrintS();
        if (p.text == "task") return parseTask();
        er(p.loc, "Unknown statement '" + p.text + "'");
        return nullptr;
    }
    ASTPtr parseExpr() {
        Token p = peek();
        if (p.kind == TK::Id && p.text == "print") return parsePrintE();
        er(p.loc, "Expected expression");
        return nullptr;
    }
    ASTPtr parsePrintS() {
        auto n = parsePrintE(); if (!n) return nullptr;
        if (!exK(TK::Semi)) return nullptr;
        return n;
    }
    ASTPtr parsePrintE() {
        Token id = next();
        if (!exK(TK::LP)) return nullptr;
        Token s; if (!ex(TK::Str, s)) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        return std::make_unique<PrintStmt>(id.loc, s.text);
    }
    ASTPtr parseCol() {
        Token id = next();
        if (!exK(TK::LP)) return nullptr;
        auto a = parseArrow(); if (!a) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        if (!exK(TK::Semi)) return nullptr;
        auto c = std::make_unique<ColumnStmt>(id.loc);
        c->children = std::move(a->body);
        return c;
    }
    ASTPtr parseRow() {
        Token id = next();
        if (!exK(TK::LP)) return nullptr;
        auto a = parseArrow(); if (!a) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        if (!exK(TK::Semi)) return nullptr;
        auto r = std::make_unique<RowStmt>(id.loc);
        r->children = std::move(a->body);
        return r;
    }
    ASTPtr parseBtn() {
        Token id = next();
        if (!exK(TK::LP)) return nullptr;
        Token s; if (!ex(TK::Str, s)) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        auto b = std::make_unique<ButtonStmt>(id.loc, s.text);
        while (peek().kind == TK::Dot) {
            next();
            MCall mc; Token mn; if (!ex(TK::Id, mn)) return nullptr;
            mc.name = mn.text; mc.loc = mn.loc;
            if (!exK(TK::LP)) return nullptr;
            if (mc.name == "id") {
                Token is; if (!ex(TK::Str, is)) return nullptr;
                mc.sArg = is.text;
            } else if (mc.name == "onClick") {
                mc.cbArg = parseArrow(); if (!mc.cbArg) return nullptr;
            } else if (mc.name == "accent") {
            } else {
                er(mc.loc, "Unknown button method '" + mc.name + "'");
                return nullptr;
            }
            if (!exK(TK::RP)) return nullptr;
            b->methods.push_back(std::move(mc));
        }
        if (!exK(TK::Semi)) return nullptr;
        return b;
    }
    ASTPtr parseLbl() {
        Token id = next();
        if (!exK(TK::LP)) return nullptr;
        Token s; if (!ex(TK::Str, s)) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        auto l = std::make_unique<LabelStmt>(id.loc, s.text);
        while (peek().kind == TK::Dot) {
            next();
            MCall mc; Token mn; if (!ex(TK::Id, mn)) return nullptr;
            mc.name = mn.text; mc.loc = mn.loc;
            if (!exK(TK::LP)) return nullptr;
            if (mc.name == "id") {
                Token is; if (!ex(TK::Str, is)) return nullptr;
                mc.sArg = is.text;
            } else {
                er(mc.loc, "Unknown label method '" + mc.name + "'");
                return nullptr;
            }
            if (!exK(TK::RP)) return nullptr;
            l->methods.push_back(std::move(mc));
        }
        if (!exK(TK::Semi)) return nullptr;
        return l;
    }
    ASTPtr parseTask() {
        Token id = next();
        if (!exK(TK::LP)) return nullptr;
        auto tb = parseArrow(); if (!tb) return nullptr;
        if (!exK(TK::RP)) return nullptr;
        auto t = std::make_unique<TaskStmt>(id.loc);
        t->body = std::move(tb);
        while (peek().kind == TK::Dot) {
            next();
            MCall mc; Token mn; if (!ex(TK::Id, mn)) return nullptr;
            mc.name = mn.text; mc.loc = mn.loc;
            if (!exK(TK::LP)) return nullptr;
            if (mc.name == "then" || mc.name == "catch") {
                mc.cbArg = parseArrow(); if (!mc.cbArg) return nullptr;
            } else {
                er(mc.loc, "Unknown task method '" + mc.name + "'");
                return nullptr;
            }
            if (!exK(TK::RP)) return nullptr;
            t->methods.push_back(std::move(mc));
        }
        if (!exK(TK::Semi)) return nullptr;
        return t;
    }
};

class Codegen {
public:
    Codegen() : ctx_(std::make_unique<llvm::LLVMContext>()), B(*ctx_), cc(0), sc(0) {
        M = std::make_unique<llvm::Module>("ClaudeScriptModule", *ctx_);
    }
    bool generate(const WindowStmt& win) {
        decl();
        auto* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*ctx_), false);
        mainFn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "cs_main", M.get());
        auto* bb = llvm::BasicBlock::Create(*ctx_, "entry", mainFn);
        B.SetInsertPoint(bb);
        B.CreateCall(fWindowCreate, {str(win.title), i32(win.width), i32(win.height)});
        for (auto& s : win.body) if (!emitNode(*s)) return false;
        B.CreateCall(fWindowEnd);
        B.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*ctx_), 0));
        return true;
    }
    bool verify(std::string& e) {
        llvm::raw_string_ostream o(e);
        return !llvm::verifyModule(*M, &o);
    }
    void printIR(llvm::raw_ostream& o) { M->print(o, nullptr); }
    std::unique_ptr<llvm::LLVMContext> takeCtx() { return std::move(ctx_); }
    std::unique_ptr<llvm::Module> takeMod() { return std::move(M); }
private:
    std::unique_ptr<llvm::LLVMContext> ctx_;
    llvm::IRBuilder<> B;
    std::unique_ptr<llvm::Module> M;
    llvm::Function* mainFn = nullptr;
    int cc, sc;
    llvm::FunctionCallee fWindowCreate, fWindowEnd, fBeginCol, fBeginRow, fEndLayout;
    llvm::FunctionCallee fCreateBtn, fCreateLbl, fSetId, fOnClick, fSetAccent, fPrint;
    llvm::FunctionCallee fTaskRun, fTaskThen, fTaskCatch;
    llvm::Type* Void() { return llvm::Type::getVoidTy(*ctx_); }
    llvm::Type* Ptr() { return llvm::PointerType::getUnqual(*ctx_); }
    llvm::Type* I32() { return llvm::Type::getInt32Ty(*ctx_); }
    llvm::Type* I64() { return llvm::Type::getInt64Ty(*ctx_); }
    llvm::Value* i32(int v) { return llvm::ConstantInt::get(I32(), v); }
    void decl() {
        fWindowCreate = M->getOrInsertFunction("rt_window_create",
            llvm::FunctionType::get(Void(), {Ptr(), I32(), I32()}, false));
        fWindowEnd = M->getOrInsertFunction("rt_window_end",
            llvm::FunctionType::get(Void(), false));
        fBeginCol = M->getOrInsertFunction("rt_begin_column",
            llvm::FunctionType::get(Void(), false));
        fBeginRow = M->getOrInsertFunction("rt_begin_row",
            llvm::FunctionType::get(Void(), false));
        fEndLayout = M->getOrInsertFunction("rt_end_layout",
            llvm::FunctionType::get(Void(), false));
        fCreateBtn = M->getOrInsertFunction("rt_create_button",
            llvm::FunctionType::get(I64(), {Ptr()}, false));
        fCreateLbl = M->getOrInsertFunction("rt_create_label",
            llvm::FunctionType::get(I64(), {Ptr()}, false));
        fSetId = M->getOrInsertFunction("rt_widget_set_id",
            llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
        fOnClick = M->getOrInsertFunction("rt_widget_on_click",
            llvm::FunctionType::get(Void(), {I64(), Ptr()}, false));
        fSetAccent = M->getOrInsertFunction("rt_widget_set_accent",
            llvm::FunctionType::get(Void(), {I64(), I32()}, false));
        fPrint = M->getOrInsertFunction("rt_print",
            llvm::FunctionType::get(Void(), {Ptr()}, false));
        fTaskRun = M->getOrInsertFunction("rt_task_run",
            llvm::FunctionType::get(I64(), {Ptr()}, false));
        fTaskThen = M->getOrInsertFunction("rt_task_then",
            llvm::FunctionType::get(I64(), {I64(), Ptr()}, false));
        fTaskCatch = M->getOrInsertFunction("rt_task_catch",
            llvm::FunctionType::get(I64(), {I64(), Ptr()}, false));
    }
    llvm::Value* str(const std::string& s) {
        auto* c = llvm::ConstantDataArray::getString(*ctx_, s, true);
        auto* gv = new llvm::GlobalVariable(*M, c->getType(), true,
            llvm::GlobalValue::PrivateLinkage, c, ".s." + std::to_string(sc++));
        gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        llvm::Value* z = llvm::ConstantInt::get(I64(), 0);
        return B.CreateInBoundsGEP(c->getType(), gv, {z, z});
    }
    llvm::Function* makeCb(const ArrowFunc& af) {
        auto* ft = llvm::FunctionType::get(Void(), false);
        auto* fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage,
            "__cb_" + std::to_string(cc++), M.get());
        auto* sb = B.GetInsertBlock();
        auto si = B.GetInsertPoint();
        auto* entry = llvm::BasicBlock::Create(*ctx_, "e", fn);
        B.SetInsertPoint(entry);
        for (auto& s : af.body) emitNode(*s);
        B.CreateRetVoid();
        B.SetInsertPoint(sb, si);
        return fn;
    }
    bool emitNode(const ASTNode& n) {
        switch (n.kind) {
        case AK::Print:
            B.CreateCall(fPrint, {str(static_cast<const PrintStmt&>(n).text)});
            return true;
        case AK::Column: {
            auto& c = static_cast<const ColumnStmt&>(n);
            B.CreateCall(fBeginCol);
            for (auto& ch : c.children) if (!emitNode(*ch)) return false;
            B.CreateCall(fEndLayout);
            return true;
        }
        case AK::Row: {
            auto& r = static_cast<const RowStmt&>(n);
            B.CreateCall(fBeginRow);
            for (auto& ch : r.children) if (!emitNode(*ch)) return false;
            B.CreateCall(fEndLayout);
            return true;
        }
        case AK::Button: {
            auto& b = static_cast<const ButtonStmt&>(n);
            auto* h = B.CreateCall(fCreateBtn, {str(b.text)});
            for (auto& m : b.methods) {
                if (m.name == "id") B.CreateCall(fSetId, {h, str(m.sArg)});
                else if (m.name == "onClick") B.CreateCall(fOnClick, {h, makeCb(*m.cbArg)});
                else if (m.name == "accent") B.CreateCall(fSetAccent, {h, i32(1)});
            }
            return true;
        }
        case AK::Label: {
            auto& l = static_cast<const LabelStmt&>(n);
            auto* h = B.CreateCall(fCreateLbl, {str(l.text)});
            for (auto& m : l.methods) {
                if (m.name == "id") B.CreateCall(fSetId, {h, str(m.sArg)});
            }
            return true;
        }
        case AK::Task: {
            auto& t = static_cast<const TaskStmt&>(n);
            llvm::Value* th = B.CreateCall(fTaskRun, {makeCb(*t.body)});
            for (auto& m : t.methods) {
                if (m.name == "then") th = B.CreateCall(fTaskThen, {th, makeCb(*m.cbArg)});
                else if (m.name == "catch") th = B.CreateCall(fTaskCatch, {th, makeCb(*m.cbArg)});
            }
            return true;
        }
        default: return false;
        }
    }
};

struct Sym { const char* n; void* a; };
static const Sym g_syms[] = {
    {"rt_window_create",  (void*)(intptr_t)rt_window_create},
    {"rt_window_end",     (void*)(intptr_t)rt_window_end},
    {"rt_begin_column",   (void*)(intptr_t)rt_begin_column},
    {"rt_begin_row",      (void*)(intptr_t)rt_begin_row},
    {"rt_end_layout",     (void*)(intptr_t)rt_end_layout},
    {"rt_create_button",  (void*)(intptr_t)rt_create_button},
    {"rt_create_label",   (void*)(intptr_t)rt_create_label},
    {"rt_create_input",   (void*)(intptr_t)rt_create_input},
    {"rt_create_checkbox",(void*)(intptr_t)rt_create_checkbox},
    {"rt_create_toggle",  (void*)(intptr_t)rt_create_toggle},
    {"rt_create_slider",  (void*)(intptr_t)rt_create_slider},
    {"rt_create_progress",(void*)(intptr_t)rt_create_progress},
    {"rt_create_separator",(void*)(intptr_t)rt_create_separator},
    {"rt_create_spacer",  (void*)(intptr_t)rt_create_spacer},
    {"rt_widget_set_id",  (void*)(intptr_t)rt_widget_set_id},
    {"rt_widget_on_click",(void*)(intptr_t)rt_widget_on_click},
    {"rt_widget_set_text",(void*)(intptr_t)rt_widget_set_text},
    {"rt_widget_set_accent",(void*)(intptr_t)rt_widget_set_accent},
    {"rt_widget_set_visible",(void*)(intptr_t)rt_widget_set_visible},
    {"rt_widget_set_enabled",(void*)(intptr_t)rt_widget_set_enabled},
    {"rt_widget_set_bold",(void*)(intptr_t)rt_widget_set_bold},
    {"rt_widget_set_font_size",(void*)(intptr_t)rt_widget_set_font_size},
    {"rt_widget_set_expand",(void*)(intptr_t)rt_widget_set_expand},
    {"rt_widget_set_weight",(void*)(intptr_t)rt_widget_set_weight},
    {"rt_widget_set_checked",(void*)(intptr_t)rt_widget_set_checked},
    {"rt_widget_get_checked",(void*)(intptr_t)rt_widget_get_checked},
    {"rt_widget_set_progress",(void*)(intptr_t)rt_widget_set_progress},
    {"rt_widget_set_slider",(void*)(intptr_t)rt_widget_set_slider},
    {"rt_widget_get_slider",(void*)(intptr_t)rt_widget_get_slider},
    {"rt_widget_on_change",(void*)(intptr_t)rt_widget_on_change},
    {"rt_widget_get_text",(void*)(intptr_t)rt_widget_get_text},
    {"rt_widget_set_hint",(void*)(intptr_t)rt_widget_set_hint},
    {"rt_widget_set_color",(void*)(intptr_t)rt_widget_set_color},
    {"rt_widget_set_bg_color",(void*)(intptr_t)rt_widget_set_bg_color},
    {"rt_find_widget",    (void*)(intptr_t)rt_find_widget},
    {"rt_print",          (void*)(intptr_t)rt_print},
    {"rt_message_box",    (void*)(intptr_t)rt_message_box},
    {"rt_confirm_box",    (void*)(intptr_t)rt_confirm_box},
    {"rt_set_clipboard",  (void*)(intptr_t)rt_set_clipboard},
    {"rt_get_clipboard",  (void*)(intptr_t)rt_get_clipboard},
    {"rt_set_on_init",    (void*)(intptr_t)rt_set_on_init},
    {"rt_set_on_destroy", (void*)(intptr_t)rt_set_on_destroy},
    {"rt_set_on_resume",  (void*)(intptr_t)rt_set_on_resume},
    {"rt_set_on_pause",   (void*)(intptr_t)rt_set_on_pause},
    {"rt_set_on_back",    (void*)(intptr_t)rt_set_on_back},
    {"rt_finish_activity",(void*)(intptr_t)rt_finish_activity},
    {"rt_task_run",       (void*)(intptr_t)rt_task_run},
    {"rt_task_then",      (void*)(intptr_t)rt_task_then},
    {"rt_task_catch",     (void*)(intptr_t)rt_task_catch},
    {"rt_post_to_ui",     (void*)(intptr_t)rt_post_to_ui},
    {"rt_sleep_ms",       (void*)(intptr_t)rt_sleep_ms},
    {"rt_get_tick_ms",    (void*)(intptr_t)rt_get_tick_ms},
    {nullptr, nullptr}
};

static std::string readFile(const std::string& p) {
    std::ifstream f(p);
    if (!f.is_open()) return "";
    std::ostringstream s;
    s << f.rdbuf();
    return s.str();
}

int main(int argc, char** argv) {
    std::string inp, outp;
    bool ir = false;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "-o" && i + 1 < argc) outp = argv[++i];
        else if (a == "--emit-ir") ir = true;
        else if (a[0] != '-') inp = a;
        else { std::cerr << "Unknown option: " << a << "\n"; return 1; }
    }
    if (inp.empty()) {
        std::cerr << "Usage: claudescript <file.cs> [--emit-ir] [-o out.ll]\n";
        return 1;
    }
    std::string src = readFile(inp);
    if (src.empty()) {
        std::ifstream t(inp);
        if (!t.is_open()) { std::cerr << "Cannot open '" << inp << "'\n"; return 1; }
        std::cerr << "File '" << inp << "' is empty\n"; return 1;
    }
    Lexer lex(src);
    Parser par(lex);
    auto ast = par.parseProgram();
    if (par.hasError() || !ast) {
        for (auto& e : par.errors()) std::cerr << e.format() << "\n";
        return 1;
    }
    Codegen cg;
    if (!cg.generate(*ast)) { std::cerr << "Codegen failed\n"; return 1; }
    std::string ve;
    if (!cg.verify(ve)) { std::cerr << "Verify failed:\n" << ve << "\n"; return 1; }
    if (ir || !outp.empty()) {
        if (!outp.empty()) {
            std::error_code ec;
            llvm::raw_fd_ostream o(outp, ec);
            if (ec) { std::cerr << "Cannot write '" << outp << "'\n"; return 1; }
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
    auto& jit = *jE;
    auto& dl = jit->getMainJITDylib();
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
    if (auto e = jit->addIRModule(llvm::orc::ThreadSafeModule(cg.takeMod(), cg.takeCtx()))) {
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
    int result = ms->toPtr<int(*)()>()();

    {
        std::lock_guard<std::mutex> lk(g_taskThreadsMutex);
        for (auto& t : g_taskThreads) {
            if (t.joinable()) t.join();
        }
        g_taskThreads.clear();
    }

    return result;
}

#include "main.moc"
