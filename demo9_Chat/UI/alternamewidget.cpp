#include "alternamewidget.h"
#include "ui_alternamewidget.h"
#include "warn.h"

AlterNameWidget::AlterNameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlterNameWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
}

AlterNameWidget::~AlterNameWidget()
{
    delete ui;
}

void AlterNameWidget::on_minimizeButton_clicked()
{
    this->showMinimized();
}

void AlterNameWidget::on_closeButton_clicked()
{
    this->close();
}

void AlterNameWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void AlterNameWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}


void AlterNameWidget::on_button_renameUsername_clicked()
{
    Warn* w = new Warn;
    if(ui->lineEdit_renameUsername->text() != "")
    {
        QString newname = ui->lineEdit_renameUsername->text();
        //newname传入数据库


        w->set_label(
            QString("修改昵称成功\n新昵称为%1")
                .arg(newname)
        );
        w->show();
        //将修改的信息传入数据库
        this->close();
    }
    else
    {
        QString str = "修改失败\n昵称不能为空。";
        w->set_label(str);
        w->show();
    }
}
