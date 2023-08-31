#include "addfriendwidget.h"
#include "ui_addfriendwidget.h"
#include "Network/socket.h"

AddFriendWidget::AddFriendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriendWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
    connect(
        Socket::instance, &Socket::AddFriendOK,
        this, &AddFriendWidget::changeToW
    );//转移窗口
}

AddFriendWidget::~AddFriendWidget()
{
    delete ui;
}

void AddFriendWidget::on_minimizeButton_clicked()
{
    this->showMinimized();
}



void AddFriendWidget::on_closeButton_clicked()
{
    this->close();
}

void AddFriendWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void AddFriendWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void AddFriendWidget::on_button_addFriend_clicked()
{


    // 获取朋友用户名
    QString friendName = ui->lineEdit_searchUserName->text();
    QString myName = Socket::username;
    Socket::instance->sendAddFriend(myName, friendName);
}

void AddFriendWidget::on_button_search_clicked()
{
    // 搜索朋友是否存在

}

void AddFriendWidget::changeToW(bool isSuc)
{
//    qDebug()<<"成功收到好友返回包";
    if(isSuc)
    {
        //如果登录成功,转到主界面
        QMessageBox::information(
            this,
            "添加好友提示",
            "添加成功"
        );
        this->close();
    }else
    {
        QMessageBox::information(
            this,
            "添加好友提示",
            "加好友失败！服务器拒绝。"
        );
    }
}
