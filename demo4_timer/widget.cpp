#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    picID = 2;
    QPixmap pix(":/images/1.png");
    ui->label->setPixmap(pix);
    ui->label->setFixedSize(pix.size());
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_startButton_clicked()
{
    myTimerID = this->startTimer(TIMEOUT);
}

void Widget::timerEvent(QTimerEvent *event){
    if (event->timerId() != myTimerID)
        return;

    QString path(":/images/");
    path += QString::number(picID);
    path += ".png";

    QPixmap pix(path);
    ui->label->setPixmap(pix);
    ui->label->setFixedSize(pix.size());

    picID++;
    if (picID == 9)
        picID = 1;

}

void Widget::on_stopButton_clicked()
{
    this->killTimer(myTimerID);
}
