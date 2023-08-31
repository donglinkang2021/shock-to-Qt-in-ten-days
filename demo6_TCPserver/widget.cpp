#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("server");
    ui->lineEdit_port_set->setText("8000");

    server = new QTcpServer;

    if(
        server->listen(
            QHostAddress::AnyIPv4,
            ui->lineEdit_port_set->text().toUInt()
        )
    ){
        // 由于有新的socket加入 在readsocket中会马上抛出新的message
        // 这里我们用displayMessage去捕获
        connect(
            this,
            &Widget::newMessage,
            this,
            &Widget::displayMessage
        );

        // 客户端发起连接 QTcpServer类检测到了连接信号
        // 于是触发服务器的连接槽 将新的连接socket加入进来

        connect(
            server,
            &QTcpServer::newConnection,
            this,
            &Widget::newConnection
        );
    }else{
        QMessageBox::critical(
            this,"QTCPServer",
            QString("无法打开无服务器: %1.")
                .arg(server->errorString())
        );
        exit(EXIT_FAILURE);
    }

}

Widget::~Widget()
{
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    server->close();
    server->deleteLater();

    delete ui;
}


void Widget::newConnection()
{
    while (server->hasPendingConnections()){
        // 建立TCP连接
        QTcpSocket *socket = server->nextPendingConnection();
        ui->lineEdit_ip->setText(socket->peerAddress().toString());
        ui->lineEdit_port->setText(QString::number(socket->peerPort()));
        appendToSocketList(socket);
    }
}

void Widget::appendToSocketList(QTcpSocket* socket){
    connection_set.insert(socket);
    connect(
        socket,
        &QTcpSocket::readyRead,
        this,
        &Widget::readSocket
    );
    connect(
        socket,
        &QTcpSocket::disconnected,
        this,
        &Widget::discardSocket
    );
    ui->comboBox_receiver->addItem(
        QString::number(socket->socketDescriptor())
    );
    displayMessage(
        QString(
            "INFO :: 用户:%1 进入了房间✨"
        ).arg(socket->socketDescriptor())
    );

}

void Widget::readSocket(){
    // 将发送者强转为socket
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    // 建立缓冲区
    QByteArray buffer;

    // 建立某个socket的数据流
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_14);

    // 开始转移到buffer中
    socketStream.startTransaction();
    socketStream >> buffer;

    // 如果没有转移完则提示等待更多数据
    if(!socketStream.commitTransaction())
    {
        QString message = QString(
            "%1 :: Waiting for more data to come.."
        ).arg(socket->socketDescriptor());
        emit newMessage(message);
        return;
    }

    // 提取头部信息
    QString header = buffer.mid(0,128);
    QString fileType = header.split(",")[0].split(":")[1];

    buffer = buffer.mid(128);

    if(fileType=="attachment"){
        // 提取附件信息
        QString fileName = header.split(",")[1].split(":")[1];
        QString ext = fileName.split(".")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

        if (
            QMessageBox::Yes == QMessageBox::question(
                this, "QTCPServer",
                QString(
                    "您收到了一个附件 \n发送者:%1 \n文件大小: %2 bytes, \n文件名 %3. \n是否接收?"
                ).arg(socket->socketDescriptor()).arg(size).arg(fileName)
            )
        )
        {
            // 提示用户保存文件
            QString filePath = QFileDialog::getSaveFileName(
                this, tr("Save File"),
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/"+fileName,
                QString("File (*.%1)").arg(ext)
            );

            QFile file(filePath);
            if(file.open(QIODevice::WriteOnly)){
                // 写入文件内容
                file.write(buffer);
                QString message = QString(
                    "INFO :: 来自发送者:%1的附件成功保存到了磁盘路径中%2"
                ).arg(socket->socketDescriptor())
                .arg(QString(filePath));
                emit newMessage(message);
            }else
                QMessageBox::critical(
                    this,"QTCPServer",
                    "写入文件时发生了错误"
                );
        }else{
            // 用户拒绝保存附件
            QString message = QString(
                "INFO :: 来自发送者:%1的附件被拒绝了😥"
            ).arg(socket->socketDescriptor());
            emit newMessage(message);
        }
    }else if(
        fileType=="message"
    ){
        // 处理收到的文本消息
        QString message = QString("%1 :: %2")
            .arg(socket->socketDescriptor())
            .arg(QString::fromStdString(buffer.toStdString()));
        emit newMessage(message);
    }
}

void Widget::discardSocket(){
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    if (it != connection_set.end()){
        displayMessage(
            QString("INFO :: 一位用户离开了房间")
                    // .arg(socket->socketDescriptor()) // 用户在离开之后就不能获取描述符了
        );
        connection_set.remove(*it);
    }
    //刷新那个连接socket组合框
    refreshComboBox();
    socket->deleteLater();
}

void Widget::displayMessage(const QString& str){
    // 在browser中显示各个socket的信息
    ui->textBrowser_receivedMessages->append(str);
}

void Widget::refreshComboBox(){
    // 刷新列表
    ui->comboBox_receiver->clear();
    ui->comboBox_receiver->addItem("Broadcast");
    foreach(QTcpSocket* socket, connection_set)
        ui->comboBox_receiver->addItem(
            QString::number(socket->socketDescriptor())
        );
}

void Widget::sendMessage(QTcpSocket* socket) {
    if (socket) {
        if (socket->isOpen()) {
            // 获取用户在界面上输入的消息
            QString str = ui->lineEdit_send->text();
            // 新加：显示自己的聊天信息
            displayMessage(
                QString("SELF :: %1").arg(str)
            );

            // 建立与套接字关联的数据流
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_14);

            // 创建消息头部，指定消息类型和大小等信息
            // append 是添加在末尾 prepend是添加在头部
            QByteArray header;
            header.prepend(
                QString("fileType:message,fileName:null,fileSize:%1;")
                    .arg(str.size()).toUtf8()
            );
            header.resize(128); // 头部固定为 128 字节

            // 将消息内容转换为字节数组
            QByteArray byteArray = str.toUtf8();

            // 在消息内容前附加消息头部
            byteArray.prepend(header);

            // 将消息内容通过套接字流发送出去
            socketStream.setVersion(QDataStream::Qt_5_14);
            socketStream << byteArray;
        } else {
            // 套接字未打开时的错误处理
            QMessageBox::critical(
                this, "QTCPServer",
                "Socket doesn't seem to be opened"
            );
        }
    } else {
        // 没有连接到服务器时的错误处理
        QMessageBox::critical(this, "QTCPServer", "Not connected");
    }
}

void Widget::sendAttachment(QTcpSocket* socket, QString filePath) {
    if (socket) {
        if (socket->isOpen()) {
            // 打开附件文件
            QFile m_file(filePath);
            if (m_file.open(QIODevice::ReadOnly)) {

                // 提取附件文件信息
                QFileInfo fileInfo(m_file.fileName());
                QString fileName(fileInfo.fileName());

                // 建立与套接字关联的数据流
                QDataStream socketStream(socket);
                socketStream.setVersion(QDataStream::Qt_5_14);

                // 创建附件消息头部，指定消息类型、文件名和文件大小等信息
                QByteArray header;
                header.prepend(
                    QString("fileType:attachment,fileName:%1,fileSize:%2;")
                    .arg(fileName)
                    .arg(m_file.size())
                    .toUtf8()
                );
                header.resize(128); // 头部固定为 128 字节

                // 将附件内容读取为字节数组
                QByteArray byteArray = m_file.readAll();

                // 在附件内容前附加消息头部
                byteArray.prepend(header);

                // 将整个附件消息通过套接字流发送给服务器
                socketStream << byteArray;
            } else {
                // 无法打开附件文件时的错误处理
                QMessageBox::critical(
                    this, "QTCPClient",
                    "Couldn't open the attachment!"
                );
            }
        } else {
            // 套接字未打开时的错误处理
            QMessageBox::critical(
                this, "QTCPServer",
                "Socket doesn't seem to be opened"
            );
        }
    } else {
        // 没有连接到服务器时的错误处理
        QMessageBox::critical(
            this, "QTCPServer",
            "Not connected"
        );
    }
}

void Widget::on_button_clear_clicked()
{
    ui->lineEdit_send->clear();
}

void Widget::on_button_send_clicked()
{
    // 获取当前组合框的文本 接收者是广播的话就给所有人发消息
    QString receiver = ui->comboBox_receiver->currentText();

    if(receiver=="Broadcast")
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            sendMessage(socket);
        }
    }
    else
    {
        foreach (QTcpSocket* socket,connection_set)
        {
            if(socket->socketDescriptor() == receiver.toLongLong())
            {
                sendMessage(socket);
                break;
            }
        }
    }
    on_button_clear_clicked();
}

void Widget::on_button_attachment_clicked()
{
    QString receiver = ui->comboBox_receiver->currentText();

    // 弹出文件选择对话框以选择附件文件 只发送小文件 暂时不考虑发送大文件
    QString filePath = QFileDialog::getOpenFileName(
        this, ("Select an attachment"),
        QStandardPaths::writableLocation(
            QStandardPaths::DocumentsLocation
        ),
        ("File (*.json *.txt *.png *.jpg *.jpeg *.*)")
    );
    // 传30MB的视频还是够用的 但如果是200M的电子书就必须得用多线程来做了

    // 如果未选择附件文件则显示错误消息并返回
    if(filePath.isEmpty()){
        QMessageBox::critical(
            this,"QTCPClient",
            "You haven't selected any attachment!"
        );
        return;
    }

    if(receiver=="Broadcast")
    {   // 广播
        foreach (QTcpSocket* socket,connection_set)
        {
            sendAttachment(socket, filePath);
        }
    }
    else
    {   // 给个别发附件
        foreach (QTcpSocket* socket,connection_set)
        {
            if(socket->socketDescriptor() == receiver.toLongLong())
            {
                sendAttachment(socket, filePath);
                break;
            }
        }
    }
    ui->lineEdit_send->clear();
}

void Widget::on_lineEdit_send_returnPressed()
{
    on_button_send_clicked();
}

void Widget::on_button_config_port_clicked()
{
    if (server->isListening()) {
        // 如果有server在跑，就先关掉
        // 但server不会马上终止正在进行的连接 如果希望断联那就通过下面方式
        qDebug() << "关闭server";
        // 关闭已建立的连接
        QList<QTcpSocket*> connectedClients = server->findChildren<QTcpSocket*>();
        for (QTcpSocket* client : connectedClients) {
            client->disconnectFromHost();
            client->deleteLater(); // 可选：删除客户端socket对象
        }
        server->close();
    }

    // 添加监听端口
    if(
        server->listen(
            QHostAddress::AnyIPv4,
            ui->lineEdit_port_set->text().toUInt()
        )
    ){
        QMessageBox::information(
            this,
            "监听端口设置",
            "更改成功"
        );
    }else{
        QMessageBox::critical(
            this,"QTCPServer",
            QString("无法打开服务器: %1.")
                .arg(server->errorString())
        );
        exit(EXIT_FAILURE);
    }

}

void Widget::on_button_check_local_IP_clicked()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, interfaces) {
        if (interface.flags() & QNetworkInterface::IsUp &&
            interface.flags() & QNetworkInterface::IsRunning &&
            !(interface.flags() & QNetworkInterface::IsLoopBack)) {

            QList<QNetworkAddressEntry> entries = interface.addressEntries();

            // 获取自己计算机的公网ip
            foreach (QNetworkAddressEntry entry, entries) {
                if (
                    entry.ip().protocol() == QAbstractSocket::IPv4Protocol && // 符合ipv4协议
                    entry.ip() != QHostAddress::LocalHost &&    // 不是local host
                    !entry.ip().toString().startsWith("192.168.") // 不是内网
                ) {
                    //  qDebug() << "Local IP: " << entry.ip().toString();
                    QMessageBox::information(
                        this,
                        "连接提示",
                        "服务器外网ip为" + entry.ip().toString()
                    );
                }
            }
        }
    }
}
