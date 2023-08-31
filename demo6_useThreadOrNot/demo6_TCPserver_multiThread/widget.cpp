#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("server");

    server = new QTcpServer;

    if(
        server->listen(
            QHostAddress::AnyIPv4,
            PORT
        )
    ){
        // 客户端发起连接
        connect(
            server,
            &QTcpServer::newConnection,
            this,
            &Widget::newClientHandler
        );
    }



}

Widget::~Widget()
{
    server->close();
    server->deleteLater();

    delete ui;
}


void Widget::newClientHandler(){

    // 建立TCP连接
    QTcpSocket *socket = server->nextPendingConnection();
    //    socket->peerAddress(); // 获取客户端地址
    //    socket->peerPort(); // 获取端口号

    ui->lineEdit_ip->setText(socket->peerAddress().toString());
    ui->lineEdit_port->setText(QString::number(socket->peerPort()));

    // 启动线程
    mulThread *t = new mulThread(socket); // 创建线程对象
    t->start();

    connect(
        t, &mulThread::sendToWidget,
        this, &Widget::readThread
    );

}

void Widget::readThread(QByteArray ba){
    ui->lineEdit_info->setText(QString(ba));
}

void Widget::on_button_clear_clicked()
{
    ui->lineEdit_send->clear();
}

void Widget::on_button_send_clicked()
{

}
