#include "mainwindow.h"
#include <QApplication>
#include "aboutdialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlags(w.windowFlags() | Qt::WindowStaysOnTopHint);
    w.show();
    return a.exec();
}
