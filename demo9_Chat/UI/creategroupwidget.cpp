#include "creategroupwidget.h"
#include "ui_creategroupwidget.h"
#include "Network/socket.h"

CreateGroupWidget::CreateGroupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateGroupWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
    connect(
        Socket::instance, &Socket::CreateGroupOK,
        this, &CreateGroupWidget::changeToW
    );//转移窗口

}

CreateGroupWidget::~CreateGroupWidget()
{
    delete ui;
}

void CreateGroupWidget::on_minimizeButton_clicked()
{
    this->showMinimized();
}

void CreateGroupWidget::on_closeButton_clicked()
{
    this->close();
}

void CreateGroupWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void CreateGroupWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void CreateGroupWidget::changeToW(bool isSuc)
{
//    qDebug()<<"成功收到创建群聊返回包";
    //Warn *w = new Warn;  //建立warning窗口
    if(isSuc)
    {
        //如果登录成功,转到主界面
        //如果登录成功,转到主界面
        QMessageBox::information(
            this,
            "创建群聊提示",
            "创建成功"
        );
        this->close();
    }else
    {
        QMessageBox::information(
            this,
            "创建群聊提示",
            "创建失败"
        );
    }
}

void CreateGroupWidget::on_button_createGroup_clicked()
{
    // 创建群聊 往服务器中写入数据
    QString groupname = ui->lineEdit_searchGroupName->text();
    Socket::instance->sendCreateGroup(Socket::username,groupname);
}
