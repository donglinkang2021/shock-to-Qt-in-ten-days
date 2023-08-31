#include "socket.h"

Socket* Socket::instance = nullptr;
QString Socket::username = "";

Socket::Socket(QTcpSocket *s, QWidget *parent) : QWidget(parent)
{
    socket = s;

    // 读取信息
    connect(
        socket,
        &QTcpSocket::readyRead,
        this,
        &Socket::readSocket
    );
    connect(
        socket,
        &QTcpSocket::disconnected,
        this,
        &Socket::discardSocket
    );

    // 实例化
    instance = this;
}

void Socket::readSocket(){
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
        QString message = QString("%1")
//            .arg(socket->socketDescriptor()) // 注意这里的描述符也是一个数字 但不是port
            .arg(QString::fromStdString(buffer.toStdString()));
        emit newMessage(message);
    }else if(fileType=="sendOK"){
        // 处理收到的文本消息 buffer和之前所有信息都是一样的
        // 如果是sendOK那就是一个json
        QJsonObject obj = QJsonDocument::fromJson(buffer).object();
        bool isSuc = obj["isSuc"]=="true";
        if(obj["type"] == "sendRegister")
        {
            emit RegisterOK(isSuc);
        }else if(obj["type"] == "sendLogin")
        {
            emit LoginOK(isSuc);
        }else if(obj["type"] == "sendAddFriend")
        {
             emit AddFriendOK(isSuc);
        }else if(obj["type"] == "sendCreateGroup")
        {
             emit CreateGroupOK(isSuc);
        }else if(obj["type"] == "sendSearchFriend")
        {
            QList<User> friendList;
            QJsonArray array = obj["friendList"].toArray();
            for(int i=0;i<array.size();i++)
            {
                // 补全解包的代码
                QJsonObject friendInfo = array[i].toObject();
                QString friendname = friendInfo["friendname"].toString();
                QString profilePicture = friendInfo["profilePicture"].toString();
                QString nickname = friendInfo["nickname"].toString();
                User user = User(friendname,"",nickname, profilePicture);
                friendList.append(user);
            }
            emit SearchFriendOK(isSuc, friendList);
        }else if(obj["type"] == "sendSearchGroup")
        {
            QList<Groupinfo> groupList;
            QJsonArray array = obj["groupList"].toArray();
            for(int i=0;i<array.size();i++)
            {
                // 补全解包的代码
                QJsonObject groupInfo = array[i].toObject();
                QString groupname = groupInfo["groupname"].toString();
                QString profilePicture = groupInfo["groupprofile"].toString();
                Groupinfo group = Groupinfo(groupname, profilePicture);
                groupList.append(group);
            }
            emit SearchGroupOK(isSuc,groupList);
        }else if(obj["type"] == "sendAddGroup")
        {
             emit AddGroupOK(isSuc);
        }else if(obj["type"] == "sendUserInfoChange")
        {
            emit ChangeInfoOK(isSuc);
        }else if(obj["type"] == "sendGetUserInfo"){
            User newUser = User(
                    obj["username"].toString(),
                    "",
                    obj["nickname"].toString(),
                    obj["profile_picture"].toString(),
                    obj["personal_signature"].toString(),
                    obj["sex"].toString(),
                    obj["age"].toInt(),
                    obj["birthday"].toString()
            );
            emit GetUserInfoOK(isSuc, newUser);
        }else if(obj["type"] == "forwardMessage"){
            QString sender = obj["sender"].toString();
            QString sender_icon = obj["sender_icon"].toString();
            QString receiver = obj["receiver"].toString();
            QString msg = obj["msg"].toString();

            emit newForwardMessage(sender,sender_icon,receiver,msg);
        }
    }
}

void Socket::discardSocket()
{
    this->close();
    socket->deleteLater();
    socket=nullptr;
}

void Socket::sendSocket(QString message, QString fileType){
    // 传入message和fileType发送给socket
    if(socket)
    {
        if(socket->isOpen())
        {
            //传输
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_14);

            QByteArray header;

            header.prepend(
                QString(
                    "fileType:%1,fileName:null,fileSize:%2;"
                ).arg(fileType).arg(message.size()).toUtf8()
            );//直接在原有的fileType改类型 进行操作

            header.resize(128);

            QByteArray byteArray = message.toUtf8();
            byteArray.prepend(header);

            socketStream << byteArray;
        }
        else
            QMessageBox::critical(this,"QTCPClient","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPClient","Not connected");
}

void Socket::send_message(QString send_str){
    if (socket) {
        if (socket->isOpen()) {
            // 建立与套接字关联的数据流
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_14);

            // 创建消息头部，指定消息类型和大小等信息
            QByteArray header;
            header.prepend(
                QString("fileType:message,fileName:null,fileSize:%1;")
                    .arg(send_str.size()).toUtf8()
            );
            header.resize(128); // 头部固定为 128 字节

            // 将消息内容转换为字节数组
            QByteArray byteArray = send_str.toUtf8();

            // 在消息内容前附加消息头部
            byteArray.prepend(header);

            // 将消息内容通过套接字流发送给服务器
            socketStream << byteArray;
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

void Socket::send_attachment(){
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

void Socket::sendGetUserInfo(QString username){
    //封装信息
    QJsonObject json_obj;
    json_obj.insert("username", username);
    QString str;
    str = QJsonDocument(json_obj).toJson();
    sendSocket(str, "sendGetUserInfo");
}

void Socket::sendRegister(QString username, QString password)
{
    //封装信息
    QJsonObject json_obj;
    json_obj.insert("username", username);
    json_obj.insert("password", password);
    QString str;
    str = QJsonDocument(json_obj).toJson();
    sendSocket(str, "sendRegister");
}

void Socket::sendLogin(QString username, QString password)
{
    //封装信息
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("password", password);
    QString str;
    str = QJsonDocument(obj).toJson();
    sendSocket(str, "sendLogin");
}

void Socket::sendAddFriend(QString username, QString friendname){
    //封装信息
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("friendname", friendname);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "sendAddFriend");
}

void Socket::sendCreateGroup(QString username,QString groupname){
    //封装信息
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("groupname", groupname);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "sendCreateGroup");
}

void Socket::sendAddGroup(QString username, QString groupname){
    //封装信息
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("groupname", groupname);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "sendAddGroup");
}

void Socket::sendSearchGroup(QString username){
    //封装信息
    QJsonObject obj;
    obj.insert("username", username);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "sendSearchGroup");
}

void Socket::sendSearchFriend(QString username){
    //封装信息
    QJsonObject obj;
    obj.insert("username", username);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "sendSearchFriend");
}

void Socket::forwardMessage(QString sender, QString sender_icon, QString type, QString receiver, QString msg){
    QJsonObject obj;
    obj.insert("sender", sender);
    obj.insert("sender_icon", sender_icon);
    obj.insert("type", type);
    obj.insert("receiver", receiver);
    obj.insert("msg", msg);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "forwardMessage");
}


void Socket::sendUserInfoChange(
    QString username, QString nickname,
    QString headFile, QString birthday,
    QString region, QString age, QString sex
){
    //封装用户信息修改的信息
    QJsonObject obj;
    obj.insert("username", username);
    obj.insert("nickname", nickname);
    obj.insert("headFile", headFile);
    obj.insert("birthday", birthday);
    obj.insert("region", region);
    obj.insert("age", age);
    obj.insert("sex",sex);
    QString str;
    str=QJsonDocument(obj).toJson();
    sendSocket(str, "sendUserInfoChange");
}
