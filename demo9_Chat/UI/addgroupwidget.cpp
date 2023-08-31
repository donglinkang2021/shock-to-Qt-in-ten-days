#include "addgroupwidget.h"
#include "ui_addgroupwidget.h"
#include "Network/socket.h"

AddGroupWidget::AddGroupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddGroupWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
    connect(
        Socket::instance,&Socket::AddGroupOK,
        this,&AddGroupWidget::changeToW
    );//转移窗口
}

AddGroupWidget::~AddGroupWidget()
{
    delete ui;
}

void AddGroupWidget::on_minimizeButton_clicked()
{
    this->showMinimized();
}

void AddGroupWidget::on_closeButton_clicked()
{
    this->close();
}

void AddGroupWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void AddGroupWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void AddGroupWidget::changeToW(bool isSuc)
{
    //    qDebug()<<"成功收到登录返回包";
    //Warn *w = new Warn;  //建立warning窗口
    if(isSuc)
    {
        //如果登录成功,转到主界面
        QMessageBox::information(
            this,
            "添加群聊提示",
            "添加成功"
        );
        this->close();
    }else
    {
        QMessageBox::information(
            this,
            "添加群聊提示",
            "添加失败"
        );
    }
}


void AddGroupWidget::on_button_addGroup_clicked()
{
    // 添加群聊功能代码
    QString groupname = ui->lineEdit_searchGroupName->text();
    Socket::instance->sendAddGroup(Socket::username,groupname);
}

void AddGroupWidget::on_button_search_clicked()
{
    // 搜索群聊存在
}
