/* main.cpp —— 程序入口（对应 MFC 的 CWinApp::InitInstance） */
#include <QApplication>
#include <QSurfaceFormat>
#include "jstmainwindow.h"
#include <QStringList>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QString::fromUtf8("px-qt"));

    JstMainWindow w;
    /* 与 X11 版一致的命令行约定：px -v N 直接进入第 N 个视图 */
    int vid = 0;
    QStringList args = app.arguments();
    for (int i = 1; i < args.size(); ++i) {
        if (args[i] == "-v" && i + 1 < args.size()) vid = args[++i].toInt();
    }
    w.setViewId(vid);
    w.show();
    return app.exec();
}
