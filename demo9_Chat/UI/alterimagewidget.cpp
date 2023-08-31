#include "alterimagewidget.h"
#include "ui_alterimagewidget.h"

AlterImageWidget::AlterImageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlterImageWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
}

AlterImageWidget::~AlterImageWidget()
{
    delete ui;
}

void AlterImageWidget::on_minimizeButton_clicked()
{
    this->showMinimized();
}



void AlterImageWidget::on_closeButton_clicked()
{
    this->close();
}

void AlterImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (ui->comboBox_chosseImages->underMouse())
        {
            // 如果点击的是ComboBox，不执行拖动逻辑
            return;
        }
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void AlterImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (ui->comboBox_chosseImages->underMouse())
        {
            // 如果点击的是ComboBox，不执行拖动逻辑
            return;
        }
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void AlterImageWidget::on_button_alterImage_clicked()
{
    // 获取当前combobox的icon 将其设置为头像

}
