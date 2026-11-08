#include "jstmainwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QActionGroup>

JstMainWindow::JstMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QString::fromUtf8("图形交互技术-上机 · Linux Qt 版"));

    m_view = new CanvasView(this);
    setCentralWidget(m_view);

    buildMenus();
    buildStatusBar();

    connect(m_view, &CanvasView::statusChanged, this, &JstMainWindow::refreshStatus);
    connect(m_view, &CanvasView::sizeChanged, this, &JstMainWindow::refreshStatus);
    m_view->setView(VIEW_MOUSELINE);
    refreshStatus();
    resize(1100, 760);
}

QAction *JstMainWindow::addViewAction(QMenu *m, int vid)
{
    QAction *a = m->addAction(QString::fromUtf8(kViewNames[vid]));
    a->setCheckable(true);
    connect(a, &QAction::triggered, this, [this, vid]() { onViewAction(vid); });
    return a;
}

void JstMainWindow::buildMenus()
{
    /* 关键：不使用全局/原生菜单栏。否则在无桌面集成（如 Xvfb、纯 xcb）时
     * 菜单会被交给外部 appmenu 服务而完全不显示。 */
    menuBar()->setNativeMenuBar(false);

    QActionGroup *g = new QActionGroup(this);
    g->setExclusive(true);

    QMenu *mv = menuBar()->addMenu(QString::fromUtf8("视图"));
    for (int i = 0; i < VIEW_COUNT; ++i) {
        QAction *a = addViewAction(mv, i);
        a->setData(i + 1);
        g->addAction(a);
        if (i == 0) a->setChecked(true);
    }

    QMenu *md = menuBar()->addMenu(QString::fromUtf8("对话框"));
    md->addAction(QString::fromUtf8("有模式对话框（exec）"), this, &JstMainWindow::onModalDialog);
    md->addAction(QString::fromUtf8("无模式对话框（show）"), this, &JstMainWindow::onModelessDialog);
    md->addSeparator();
    md->addAction(QString::fromUtf8("坐标输入（定线）"), this, &JstMainWindow::onCoordDialog);
    md->addAction(QString::fromUtf8("三角形顶点（p1/Fill）"), this, &JstMainWindow::onTriDialog);

    QMenu *mh = menuBar()->addMenu(QString::fromUtf8("帮助"));
    mh->addAction(QString::fromUtf8("关于"), this, &JstMainWindow::onAbout);
    mh->addAction(QString::fromUtf8("快捷键"), this, &JstMainWindow::onKeys);
}

void JstMainWindow::buildStatusBar()
{
    m_lblView = new QLabel(this);
    m_lblPos  = new QLabel(this);
    statusBar()->addWidget(m_lblView, 1);
    statusBar()->addPermanentWidget(m_lblPos);
}

void JstMainWindow::setViewId(int id)
{
    if (id < 0 || id >= VIEW_COUNT) return;
    m_view->setView((ViewId)id);
    /* 同步菜单勾选状态 */
    for (QAction *a : menuBar()->actions()) {
        QMenu *m = a->menu();
        if (!m) continue;
        for (QAction *ia : m->actions()) {
            if (ia->isCheckable() && ia->data().toInt() == id + 1) ia->setChecked(true);
        }
    }
    refreshStatus();
}

void JstMainWindow::onViewAction(int id)
{
    m_view->setView((ViewId)id);
    refreshStatus();
}

void JstMainWindow::refreshStatus()
{
    if (!m_view) return;
    QString s = m_view->viewName() + "  —  " + m_view->viewHint();
    QString st = m_view->statusText();
    if (!st.isEmpty()) s += "   [" + st + "]";
    m_lblView->setText(s);
    m_lblPos->setText(QString::fromUtf8("绘图区 %1x%2")
                          .arg(m_view->width()).arg(m_view->height()));
}

/* 有模式：exec() 阻塞，等价 CDialog::DoModal */
void JstMainWindow::onModalDialog()
{
    QDialog d(this);
    d.setWindowTitle(QString::fromUtf8("对话框示例（CDlgExam）"));
    QFormLayout *fl = new QFormLayout(&d);
    QSpinBox *sb = new QSpinBox(&d);
    sb->setRange(-99999, 99999);
    sb->setValue(42);
    fl->addRow(QString::fromUtf8("请输入一个整数 m_exam："), sb);
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    fl->addRow(bb);
    connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted)
        QMessageBox::information(this, QString::fromUtf8("返回值"),
                                 QString::fromUtf8("DoModal 返回 IDOK，m_exam = %1").arg(sb->value()));
}

/* 无模式：show() 不阻塞，主窗口照常可交互，等价 Create + ShowWindow */
void JstMainWindow::onModelessDialog()
{
    if (m_modeless) { m_modeless->raise(); m_modeless->activateWindow(); return; }

    QDialog *d = new QDialog(this);
    d->setWindowTitle(QString::fromUtf8("无模式对话框（Create + ShowWindow）"));
    d->setAttribute(Qt::WA_DeleteOnClose);
    m_modeless = d;

    QFormLayout *fl = new QFormLayout(d);
    QSpinBox *sb = new QSpinBox(d);
    sb->setRange(-99999, 99999);
    sb->setValue(42);
    fl->addRow(QString::fromUtf8("m_exam："), sb);
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, d);
    fl->addRow(bb);
    connect(bb, &QDialogButtonBox::accepted, d, [this, sb, d]() {
        QMessageBox::information(d, QString::fromUtf8("无模式对话框"),
                                 QString::fromUtf8("主窗口仍然可以操作，当前 m_exam = %1").arg(sb->value()));
    });
    connect(bb, &QDialogButtonBox::rejected, d, &QDialog::close);
    d->show();
}

/* 坐标输入（p1/SingleDraw2 的 CDiaLineWith2P） */
void JstMainWindow::onCoordDialog()
{
    QDialog d(this);
    d.setWindowTitle(QString::fromUtf8("输入直线端点（CDiaLineWith2P）"));
    QFormLayout *fl = new QFormLayout(&d);
    QSpinBox *sx = new QSpinBox(&d), *sy = new QSpinBox(&d);
    QSpinBox *ex = new QSpinBox(&d), *ey = new QSpinBox(&d);
    for (QSpinBox *s : {sx, sy, ex, ey}) s->setRange(-4000, 4000);
    QPointF a = m_view->startPoint(), b = m_view->endPoint();
    sx->setValue((int)a.x()); sy->setValue((int)a.y());
    ex->setValue((int)b.x()); ey->setValue((int)b.y());
    fl->addRow("起点 X：", sx);
    fl->addRow("起点 Y：", sy);
    fl->addRow("终点 X：", ex);
    fl->addRow("终点 Y：", ey);

    QHBoxLayout *hl = new QHBoxLayout;
    QPushButton *pickS = new QPushButton(QString::fromUtf8("取起点"), &d);
    QPushButton *pickE = new QPushButton(QString::fromUtf8("取终点"), &d);
    hl->addWidget(pickS); hl->addWidget(pickE);
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    hl->addWidget(bb);
    fl->addRow(hl);

    connect(pickS, &QPushButton::clicked, &d, [this, &d]() { d.done(10); });
    connect(pickE, &QPushButton::clicked, &d, [this, &d]() { d.done(11); });
    connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    int r = d.exec();
    if (r == QDialog::Accepted) {
        m_view->setLineFromDialog(sx->value(), sy->value(), ex->value(), ey->value());
        if (m_view->view() != VIEW_DLGLINE) onViewAction(VIEW_DLGLINE);
    } else if (r == 10) {
        if (m_view->view() != VIEW_DLGLINE) onViewAction(VIEW_DLGLINE);
        m_view->beginPick(1);
    } else if (r == 11) {
        if (m_view->view() != VIEW_DLGLINE) onViewAction(VIEW_DLGLINE);
        m_view->beginPick(2);
    }
    refreshStatus();
}

/* 三角形顶点（p1/Fill 的 CTrigle） */
void JstMainWindow::onTriDialog()
{
    QDialog d(this);
    d.setWindowTitle(QString::fromUtf8("输入三角形顶点（CTrigle）"));
    QFormLayout *fl = new QFormLayout(&d);
    QSpinBox *x1 = new QSpinBox(&d), *y1 = new QSpinBox(&d);
    QSpinBox *x2 = new QSpinBox(&d), *y2 = new QSpinBox(&d);
    QSpinBox *x3 = new QSpinBox(&d), *y3 = new QSpinBox(&d);
    for (QSpinBox *s : {x1, y1, x2, y2, x3, y3}) s->setRange(-4000, 4000);
    QPointF a = m_view->startPoint(), b = m_view->endPoint();
    x1->setValue((int)a.x()); y1->setValue((int)a.y());
    x2->setValue((int)b.x()); y2->setValue((int)b.y());
    x3->setValue(m_view->triX3()); y3->setValue(m_view->triY3());
    fl->addRow("X1：", x1); fl->addRow("Y1：", y1);
    fl->addRow("X2：", x2); fl->addRow("Y2：", y2);
    fl->addRow("X3：", x3); fl->addRow("Y3：", y3);
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &d);
    fl->addRow(bb);
    connect(bb, &QDialogButtonBox::accepted, &d, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        if (m_view->view() != VIEW_SCANFILL) onViewAction(VIEW_SCANFILL);
        m_view->setTriangleFromDialog(x1->value(), y1->value(),
                                      x2->value(), y2->value(),
                                      x3->value(), y3->value());
    }
    refreshStatus();
}

void JstMainWindow::onAbout()
{
    QMessageBox::about(this, QString::fromUtf8("关于"),
        QString::fromUtf8("图形交互技术-上机 Linux 版\n\n"
                          "框架：Qt6 Widgets + QPainter（替代 MFC CDC）\n"
                          "算法：DDA、Bresenham、中点圆、扫描线填充、\n"
                          "      Cohen-Sutherland、中点分割、Liang-Barsky\n\n"
                          "对应 p1（MFC 工程）与 zip（算法包）"));
}

void JstMainWindow::onKeys()
{
    QMessageBox::information(this, QString::fromUtf8("快捷键"),
        QString::fromUtf8("+ / -    改半径\n"
                          "1        切换直线/圆（视图4、5）\n"
                          "R        复位当前视图\n"
                          "右键     清空/复位"));
}
