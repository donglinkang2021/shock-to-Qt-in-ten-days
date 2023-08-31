#include "chat.h"
#include "ui_chat.h"

Chat::Chat(QTcpSocket *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    this->setWindowTitle("chat");
    this->setServerInfo(s);

    // 接收信息
    connect(
        this,
        &Chat::newMessage,
        this,
        &Chat::displayMessage
    );
    // 读取信息
    connect(
        socket,
        &QTcpSocket::readyRead,
        this,
        &Chat::readSocket
    );
    connect(
        socket,
        &QTcpSocket::disconnected,
        this,
        &Chat::discardSocket
    );

}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_button_clear_clicked()
{
    ui->lineEdit_chat->clear();
}

void Chat::on_button_send_clicked() {
    if (socket) {
        if (socket->isOpen()) {
            // 获取用户在界面上输入的聊天消息
            QString str = ui->lineEdit_chat->text();
            // 新加：显示自己的聊天信息
            displayMessage(
                QString("SELF :: %1").arg(str)
            );

            // 建立与套接字关联的数据流
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_14);

            // 创建消息头部，指定消息类型和大小等信息
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

            // 将消息内容通过套接字流发送给服务器
            socketStream << byteArray;

            // 清除输入框内容
            ui->lineEdit_chat->clear();
        } else {
            // 套接字未打开时的错误处理
            QMessageBox::critical(
                this, "QTCPClient",
                "Socket doesn't seem to be opened"
            );
        }
    } else {
        // 没有连接到服务器时的错误处理
        QMessageBox::critical(
            this, "QTCPClient",
            "Not connected"
        );
    }
}

void Chat::on_button_attachment_clicked() {
    if (socket) {
        if (socket->isOpen()) {
            // 弹出文件选择对话框以选择附件文件
            QString filePath = QFileDialog::getOpenFileName(
                this, ("Select an attachment"),
                QStandardPaths::writableLocation(
                    QStandardPaths::DocumentsLocation
                ),
                ("File (*.json *.txt *.png *.jpg *.jpeg)")
            );

            // 如果未选择附件文件则显示错误消息并返回
            if (filePath.isEmpty()) {
                QMessageBox::critical(
                    this, "QTCPClient",
                    "You haven't selected any attachment!"
                );
                return;
            }

            // 打开附件文件并读取内容
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
                    QString(
                        "fileType:attachment,fileName:%1,fileSize:%2;"
                    ).arg(fileName).arg(m_file.size()).toUtf8()
                );
                header.resize(128); // 头部固定为 128 字节

                // 将附件内容读取为字节数组
                QByteArray byteArray = m_file.readAll();

                // 在附件内容前附加消息头部
                byteArray.prepend(header);

                // 将整个附件消息通过套接字流发送给服务器
                socketStream.setVersion(QDataStream::Qt_5_14);
                socketStream << byteArray;
            } else {
                // 无法读取附件文件时的错误处理
                QMessageBox::critical(
                    this, "QTCPClient",
                    "Attachment is not readable!"
                );
            }
        } else {
            // 套接字未打开时的错误处理
            QMessageBox::critical(
                this, "QTCPClient",
                "Socket doesn't seem to be opened"
            );
        }
    } else {
        // 没有连接到服务器时的错误处理
        QMessageBox::critical(
            this, "QTCPClient",
            "Not connected"
        );
    }
}

void Chat::on_lineEdit_chat_returnPressed()
{
    on_button_send_clicked();
}

void Chat::readSocket(){
    // 唯一不同的是不需要获取socket列表这里，而是直接利用自己的socket
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
                ).arg(socket->socketDescriptor())
                .arg(size)
                .arg(fileName)
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
            .arg(socket->socketDescriptor()) // 注意这里的描述符也是一个数字 但不是port
            .arg(QString::fromStdString(buffer.toStdString()));
        emit newMessage(message);
    }
}

void Chat::displayMessage(const QString& str)
{
    ui->textBrowser_receivedMessages->append(str);
}

void Chat::discardSocket()
{
    this->close();
    socket->deleteLater();
    socket=nullptr;
}

void Chat::setServerInfo(QTcpSocket *s){
    socket = s;
    // 获取服务器的IP地址
    ui->lineEdit_ip->setText(s->peerName());
    // 获取服务器的端口号
    ui->lineEdit_port->setText(QString::number(s->peerPort()));
}


void Chat::on_button_return_clicked()
{
    this->close(); // 这句话执行完毕后没有马上执行析构函数 应该是做了优化
    // chat窗体随parent窗体销毁而销毁，所以此时我们要提早关闭socket
    // 所以我们不能在析构函数中写下面两行 而是点击就应该关闭
    if(socket->isOpen())
        socket->close();
    // 抛出请求打开第一个界面的信号
    emit showFirstRequested();
}
