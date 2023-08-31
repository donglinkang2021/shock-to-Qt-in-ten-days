#include "warn.h"
#include "ui_warn.h"

Warn::Warn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Warn)
{
    ui->setupUi(this);
    this->setWindowFlags(
        Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint
    );

    ui->contentLabel->setWordWrap(true); //自动换行
    ui->contentLabel->setAlignment(
        Qt::AlignHCenter|Qt::AlignVCenter
    ); //水平垂直居中

    QTimer* timer = new QTimer(this);
    timer->start(WARN_TIMEOUT);  //显示时间2秒
    timer->setSingleShot(true); //仅触发一次
    connect(
        timer, SIGNAL(timeout()),
        this, SLOT(close_warning())
    ); //timeout() -> close()

}

Warn::~Warn()
{
    delete ui;
}

void Warn::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Warn::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}


//传入warning内容
void Warn::set_label(QString str)
{
    ui->contentLabel->setText(str);
}

void Warn::close_warning()
{
    this->close();
}
