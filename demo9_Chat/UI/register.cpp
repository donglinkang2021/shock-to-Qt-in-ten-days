#include "register.h"
#include "ui_register.h"
#include "login.h"
#include "warn.h"
#include "Network/socket.h"

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
    connect(
        Socket::instance, &Socket::RegisterOK,
        this, &Register::registerOK2Login
    );//转移窗口
}

Register::~Register()
{
    delete ui;
}

void Register::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Register::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void Register::on_label_return_login_clicked()
{
    Login *w = new Login;
    w->show();
    this->close();
}

void Register::on_minimizeButton_clicked()
{
    this->showMinimized();
}

void Register::on_closeButton_clicked()
{
    this->close();
}

void Register::on_button_signUp_config_clicked()
{
    QString username = ui->lineEdit_username->text(); //获取账号
    QString password = ui->lineEdit_pwd->text();   //获取密码
    QString password2 = ui->lineEdit_pwd_again->text();   //获取确认密码

    Warn *w = new Warn;  //建立warning窗口
    if(password == "" or password2 =="")
    {
        QString str = "注册失败！\n密码不能为空。";
        w->set_label(str);
        w->show();
    }
    else if(password != password2)
    {
        QString str = "注册失败！\n两次输入密码不同。";
        w->set_label(str);
        w->show();
    }
    else
    {
        Socket::instance->sendRegister(username, password);
    }
}

void Register::on_lineEdit_pwd_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1); // 标记参数为未使用
    ui->lineEdit_pwd->setEchoMode(QLineEdit::Password);
}

void Register::on_lineEdit_pwd_again_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1); // 标记参数为未使用
    ui->lineEdit_pwd_again->setEchoMode(QLineEdit::Password);
}

void Register::registerOK2Login(bool isSuc){
    Warn *w = new Warn;  //建立warning窗口
    if(isSuc)
    {
        QString str = "注册成功！\n请回到登录界面登录😘";
        w->set_label(str);
        w->show();
    }else
    {
        QString str = "注册失败！服务器拒绝。";
        w->set_label(str);
        w->show();
    }
}
