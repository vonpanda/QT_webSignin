#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
/*
 *注：关于config.info结构
 * Default_IP:登陆的IP，也是注销的IP
 * Default_Port:登陆的端口
 * pid&calg：取自Default_IP的/a41.js;
 * 每行以\r\n结尾，一整条信息以两个\r\n结尾。
 *
 *
*/


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    mSysTrayIcon=new QSystemTrayIcon(this);
    Default_port=80;
    pid="2";
    calg="12345678";
    fileInfo.setFileName("config.info");

    tcpsocket=new QTcpSocket;
}

MainWindow::~MainWindow()
{

    if(fileInfo.isOpen())
        fileInfo.close();
    delete mShowMainAction;
    delete mExitAppAction;
    delete mSysTrayIcon;
    delete mMenu;
    delete ui;
}




void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{

    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        mSysTrayIcon->showMessage(QObject::trUtf8("感谢支持！"),QObject::trUtf8("欢迎使用本程序"),QSystemTrayIcon::Information,1000);
        break;
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    default:
        break;
    }

}

void MainWindow::createAction()
{
    mShowMainAction=new QAction(QObject::trUtf8("显示主界面"),this);
    connect(mShowMainAction,SIGNAL(triggered()),this,SLOT(on_showMainAction()));

    mExitAppAction = new QAction(QObject::trUtf8("退出"),this);
    connect(mExitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));

}

void MainWindow::createMenu()
{
    mMenu=new QMenu(this);
    mMenu->addAction(mShowMainAction);
    mMenu->addSeparator();
    mMenu->addAction(mExitAppAction);
    mSysTrayIcon->setContextMenu(mMenu);
}


void MainWindow::on_showMainAction()
{

    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->showNormal();
    this->raise();
    this->activateWindow();
    //this->show();
    //ui->setupUi(this);
}
void MainWindow::on_exitAppAction()
{
    exit(0);
}


void MainWindow::changeEvent(QEvent *e)
{
    if ((e->type() == QEvent::WindowStateChange) && this->isMinimized())
    {
        this->hide();

        QIcon icon=QIcon(":/p.png");
        mSysTrayIcon->setIcon(icon);
        mSysTrayIcon->setToolTip(QObject::trUtf8("测试系统托盘图标"));

        ui->textBrowser->setText("yes.");
        connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

        createAction();
        createMenu();
        mSysTrayIcon->show();
    }
}
QString MainWindow::upass(QString pass,QString pid,QString calg)
{
    QString temchar;
    temchar.append(pid).append(pass).append(calg);
    QByteArray bb;
    bb=QCryptographicHash::hash(temchar.toLatin1(),QCryptographicHash::Md5);

    temchar.clear();
    temchar.append(bb.toHex());
    temchar.append(calg);
    temchar.append(pid);
    return temchar;
}


void MainWindow::sigin()
{
    user=ui->userEdit->text();
    pass=ui->passEdit->text();
    tcpsocket->connectToHost(Default_IP,Default_port);
    tcpsocket->waitForConnected(300);

    qDebug("State:%d\n",tcpsocket->state());
    QString *data=new QString;
    data->append(tr("POST /0.htm HTTP/1.1\r\nHost: "));
    data->append(Default_IP);
    data->append(tr("\r\nConnection: keep-alive\r\nContent-Length: 101\r\nCache-Control: max-age=0\r\nOrigin: http://"));
    data->append(Default_IP);
    data->append(tr("\r\nUpgrade-Insecure-Requests: 1\r\nContent-Type: application/x-www-form-urlencoded\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nReferer: http://"));
    data->append(Default_IP);
    data->append(tr("/0.htm\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.8\r\nCookie: drcom_login=153110023%7C12185179\r\n\r\nDDDDD="));
    data->append(user);
    data->append("&upass=");
    data->append(upass(pass,pid,calg));
    data->append("&R1=0&R2=1&para=00&0MKKey=123456&v6ip=");


    char *ch;
    QByteArray ba=data->toLatin1();
    ch=ba.data();

    tcpsocket->write(ch);
    tcpsocket->flush();
    //flush(out);
    ui->textBrowser->append(tcpsocket->readAll());
    ui->textBrowser->append(ch);
    //delete ch;
    ui->signin->setText(tr("已登陆"));
    tcpsocket->disconnectFromHost();
    tcpsocket->waitForDisconnected(300);
    delete data;

}


void MainWindow::on_signin_clicked()
{
    fileInfo.open(QIODevice::ReadWrite|QIODevice::Text);


    while(!(fileInfo.readLine().isEmpty()) ||!(fileInfo.readLine().isNull()))
    {
        ui->userEdit->clear();
        ui->passEdit->clear();
        ui->ipEdit->clear();
        ui->textBrowser->clear();
        Default_IP=fileInfo.readLine().replace("\r\n","").replace("\n","").replace("\r","");//iter
        fileInfo.readLine().replace("\r\n","");
        user=fileInfo.readLine().replace("\r\n","").replace("\n","").replace("\r","");
        pass=fileInfo.readLine().replace("\r\n","").replace("\n","").replace("\r","");
        IP_List.append(Default_IP);
        user_List.append(user);
        pass_List.append(pass);


        ui->userEdit->setText(user);
        ui->passEdit->setText(pass);
        ui->ipEdit->setText(Default_IP);
        sigin();

    }
    if(ui->userEdit->text().isEmpty() || ui->passEdit->text().isEmpty())
        QMessageBox::warning(this,tr("请输入账号密码"),tr("账号或密码错误!"),QMessageBox::Yes);
    if(fileInfo.isOpen())
        fileInfo.close();
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<"now,i am pushButton_2";
    QString *ch=new QString;
    QByteArray ba;
    for(int i=0;i<IP_List.size();i++)
    {
        ch->clear();
        ba.clear();
        Default_IP=IP_List[i];
        user=user_List[i];
        pass=pass_List[i];

        ui->ipEdit->setText(Default_IP);
        ui->userEdit->setText(user);
        ui->passEdit->setText(pass);

            tcpsocket->connectToHost(Default_IP,Default_port);
            tcpsocket->waitForConnected(300);
            ch->append("GET /F.htm HTTP/1.1\r\nHost: ");
            ch->append(Default_IP);
            ch->append("\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nReferer: ");
            ch->append("http://");
            ch->append(Default_IP);
            ch->append(":9002/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9\r\nCookie: drcom_login=");
            ch->append(user);
            ch->append("%7C");
            ch->append(pass);
            ch->append("\r\n\r\n");
            ba=ch->toLatin1();
            ui->textBrowser->append(ba.data());
            tcpsocket->write(ba.data());
            tcpsocket->flush();
            tcpsocket->flush();
            tcpsocket->disconnectFromHost();
            tcpsocket->waitForDisconnected(300);
    }


    //ui->textBrowser->setText(tcpsocket->readAll());

}

void MainWindow::on_setButton_clicked()
{

    if(ui->ipEdit->text().isEmpty() ||ui->userEdit->text().isEmpty()||ui->passEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("请输入数据"),tr("未输入IP或账号或密码!"),QMessageBox::Yes);
    }
    else
    {
        fileInfo.open(QIODevice::ReadWrite|QIODevice::Append);
        QByteArray ba=ui->ipEdit->text().append("\r\n").toLatin1();
        fileInfo.write("\r\n");
        //fileInfo.write("Default_IP:");
        fileInfo.write(ba.data());
        fileInfo.flush();

        //fileInfo.write("Default_port:");
        fileInfo.write("80");
        fileInfo.write("\r\n");

        //fileInfo.write("user:");
        ba=ui->userEdit->text().toLatin1();
        fileInfo.write(ba.data());
        fileInfo.write("\r\n");

        //fileInfo.write("pass:");
        ba=ui->passEdit->text().toLatin1();
        fileInfo.write(ba.data());
        fileInfo.write("\r\n");

        //fileInfo.write("upass:");
        //ba=upass(ui->userEdit->text(),ui->passEdit->text(),pid,calg).toLatin1();
        //fileInfo.write(ba.data());
        ui->setButton->setText(tr("已记录配置"));
        //fileInfo.write("\r\n\r\n");
        fileInfo.flush();
        if(fileInfo.isOpen())
            fileInfo.close();

    }
}

void MainWindow::on_actiona_triggered()
{
    QMessageBox::warning(this,tr("如何如何配置登录器？"),tr("    第一次使用时，请输入账号密码，及IP，然后点击发送即可，之后的登陆只要点击发送即可！"),QMessageBox::Yes);
}

void MainWindow::on_actiona_2_triggered()
{
    QMessageBox::warning(this,tr("作者:panda"),tr("Email:syc0d69@gmail.com\nblog:pandacode.pw\ngithub:xxx\n欢迎反馈意见，本软件开源！"),QMessageBox::Yes);
}

void MainWindow::on_actiona_3_triggered()
{
    reg=new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
    QString strPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    reg->setValue("wirtepad",strPath);
    delete reg;
}

void MainWindow::on_actiona_4_triggered()
{
     reg=new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
     reg->setValue("wirtepad","");
     delete reg;

}
