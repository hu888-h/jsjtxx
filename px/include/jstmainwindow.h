/* jstmainwindow.h —— 主窗口（对应 MFC 的 CFrameWnd + 菜单 + 状态栏）
 *
 * PDF「对话框技术」两类的 Qt 实现：
 *   有模式 -> QDialog::exec()      （关闭前阻塞主窗口）
 *   无模式 -> QDialog::show()      （主窗口仍可交互）
 */
#ifndef JSTMAINWINDOW_H
#define JSTMAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

class QLabel;
class QAction;
class QMenu;

#include "canvasview.h"

class JstMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit JstMainWindow(QWidget *parent = nullptr);
    void setViewId(int id);

private slots:
    void onViewAction(int id);
    void onModalDialog();
    void onModelessDialog();
    void onCoordDialog();
    void onTriDialog();
    void onAbout();
    void onKeys();
    void refreshStatus();

private:
    void buildMenus();
    void buildStatusBar();
    QAction *addViewAction(QMenu *m, int vid);

    CanvasView        *m_view = nullptr;
    QLabel            *m_lblView = nullptr;
    QLabel            *m_lblPos  = nullptr;
    QPointer<QDialog>  m_modeless;
};

#endif
