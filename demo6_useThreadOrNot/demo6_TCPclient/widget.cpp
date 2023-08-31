#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("client");
    socket = new QTcpSocket; // 创建socket对象

    // 设置默认ip和端口
    ui->lineEdit_ip->setText("127.0.0.1");
    ui->lineEdit_port->setText("8000");
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_connect_clicked()
{
    QString IP = ui->lineEdit_ip->text();
    QString port = ui->lineEdit_port->text();

    // 连接服务器
    socket->connectToHost(
        QHostAddress(IP),
        port.toShort()
    );

    // 连接成功
    connect(
        socket,
        &QTcpSocket::connected,
        [this](){
            QMessageBox::information(
                this,
                "连接提示",
                "连接服务器成功"
            );

            this->hide();

            Chat *c = new Chat(socket); // 在堆上创建对象
            c->show();
        }
    );

    // 连接断开
    connect(
        socket,
        &QTcpSocket::disconnected,
        [this](){
            QMessageBox::warning(
                this,
                "连接提示",
                "连接异常 网络断开"
            );
        }
    );

    // 可以输入自己的电脑端口试一试 127.0.0.1 8080

}

void Widget::on_pushButton_cancel_clicked()
{
    this->close();
}
