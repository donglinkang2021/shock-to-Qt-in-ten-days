#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include "connectionwidget.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
}

Login::~Login()
{
    delete ui;
}


void Login::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Login::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void Login::loginOK2MainWindow(bool isSuc){
    //Warn *w = new Warn;  //建立warning窗口
    if(isSuc)
    {
        //如果登录成功,转到主界面
        Socket::username = tempUserName;
        MainWindow *mi = new MainWindow(socket);
        mi->show();
        this->close();
    }else
    {
        qDebug() << "登录失败！服务器拒绝。";
    }
}

void Login::on_button_login_clicked()
{
    connect(
        Socket::instance, &Socket::LoginOK,
        this, &Login::loginOK2MainWindow
    );//转移到MainWindow // 这个会有一个warn但暂时先不改了

    // 呜呜这个命名错了应该是login才对
    QString username = ui->account_lineedit->text(); //获取账号
    QString password = ui->key_lineedit->text();   //获取密码

    //直接转到主界面的方法 记得先连接服务器 不然没法通讯
//    MainWindow *mi = new MainWindow(socket);
//    mi->show();
//    this->close();

    tempUserName = username;
    Socket::instance->sendLogin(username,password);
}

void Login::on_minimizeButton_clicked()
{
    this->showMinimized();
}

void Login::on_closeButton_clicked()
{
    this->close();
}

// 隐藏密码
void Login::on_key_lineedit_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1); // 标记参数为未使用
    ui->key_lineedit->setEchoMode(QLineEdit::Password);
}


// 跳转注册窗口
void Login::on_label_signup_rightnow_clicked()
{
    Register *page = new Register;
    page->show();
    this->close();
}

void Login::on_button_connection_clicked()
{
    ConnectionWidget *cw = new ConnectionWidget;
    cw->show();
    this->hide();
    connect(
        cw, &ConnectionWidget::returnLoginWidget,
        this, &Login::connectionSlot
    );
    connect(
        cw, &ConnectionWidget::showFirstRequested,
        [this](){
            this->show();
    });
}

void Login::connectionSlot(QTcpSocket *s){
    socket = s;
}
