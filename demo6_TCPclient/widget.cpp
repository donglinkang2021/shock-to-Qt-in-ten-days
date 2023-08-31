#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("client");

    // 设置默认ip和端口
    ui->lineEdit_ip->setText("127.0.0.1");
    ui->lineEdit_port->setText("8000");
}

Widget::~Widget()
{
    if(socket->isOpen())
        socket->close();
    delete ui;
}

void Widget::on_pushButton_connect_clicked()
{
    socket = new QTcpSocket; // 创建socket对象

    QString IP = ui->lineEdit_ip->text();
    QString port = ui->lineEdit_port->text();

    // 后面结合多线程优化这个地方
    // 连接服务器
    socket->connectToHost(
        QHostAddress(IP),
        port.toShort()
    );

    //  启动线程来检测连接
    //    Thread *t = new Thread(socket); // 创建线程对象
    //    t->start();

    //    connect(
    //        t, &Thread::connectHostSignal,
    //        this, &Widget::connectHostSuccess
    //    );

    // 连接成功则建立下面连接
    // 打开chat窗口
    connect(
        socket,
        &QTcpSocket::connected,
        this,
        &Widget::handleConnected
    );

    //     连接时断开提示断开
    connect(
        socket,
        &QTcpSocket::disconnected,
        this,
        &Widget::handleDisConnected
    );

    // 可以输入自己的电脑端口试一试 127.0.0.1 8000

    // 等待连接成功
    connectHostSuccess(
        socket->waitForConnected(1000)
    );
}

void Widget::connectHostSuccess(bool flag){
    if(flag){
        QMessageBox::information(
            this,
            "连接提示",
            "连接服务器成功"
        );
    }else{
        QMessageBox::critical(
            this,"QTCPClient",
            QString("The following error occurred: %1.")
                .arg(socket->errorString())
        );
        //        exit(EXIT_FAILURE);
    }
}


void Widget::handleDisConnected(){
    QMessageBox::warning(
        this, "连接提示",
        "连接异常，网络断开\n用户已离开聊天室"
    );
    this->show();
}

void Widget::handleConnected(){
    this->hide();
    Chat *c = new Chat(socket); // 在堆上创建对象
    connect(c, &Chat::showFirstRequested, [this](){
        this->show();
    });
    c->show();
}

void Widget::on_pushButton_cancel_clicked()
{
    this->close();
}

void Widget::on_lineEdit_port_returnPressed()
{
    on_pushButton_connect_clicked();
}
