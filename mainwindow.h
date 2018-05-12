#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtNetwork/QTcpSocket>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDir>
#include <QRegExp>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QEvent>
#include <QMenu>
#include <QSettings>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_signin_clicked();
    QString upass(QString pass, QString pid, QString calg);
    void on_pushButton_2_clicked();

    void on_setButton_clicked();
    void sigin();
    void on_actiona_triggered();
    void createMenu();
    void on_actiona_2_triggered();
    void createAction();
    void changeEvent(QEvent *e);
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_showMainAction();
    void on_exitAppAction();

    void on_actiona_3_triggered();

    void on_actiona_4_triggered();

private:
    Ui::MainWindow *ui;
    QString Default_IP;
    int Default_port;
    QString pid;
    QString calg;
    QString user;
    QString pass;
    QTcpSocket *tcpsocket;
    QFile fileInfo;
    QSystemTrayIcon *mSysTrayIcon;
    QAction *mShowMainAction;
    QAction *mExitAppAction;
    QMenu *mMenu;
    QList<QString> IP_List;
    QList<QString> user_List;
    QList<QString> pass_List;
    QSettings *reg;
};

#endif // MAINWINDOW_H
