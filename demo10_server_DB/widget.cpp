#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initUI();
    initDatabase();
    initServer();
}

Widget::~Widget()
{
    if (db.open()){
        db.close();
    }
    if (model){
        delete model;
    }

    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    server->close();
    server->deleteLater();

    delete ui;
}

// 三个初始化

void Widget::initDatabase(){
    //连接数据库 加载QSQLITE
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydatabase.db");

    if (db.open()) {// 在这行代码之后才打开数据库
        QMessageBox::information(
            this, "连接提示", "成功连接数据库"
        );

        if (!db.tables().contains("User")){
            create_Table_User();
        }
        if (!db.tables().contains("Friendship")){
            create_Table_Friendship();
        }
        if (!db.tables().contains("Friendmsg")){
            create_Table_Friendmsg();
        }
        if (!db.tables().contains("Groupinfo")){
            create_Table_Groupinfo();
        }
        if (!db.tables().contains("Groupship")){
            create_Table_Groupship();
        }
        if (!db.tables().contains("Groupmsg")){
            create_Table_Groupmsg();
        }

        model = new QSqlTableModel;
        model->setTable("User");
        ui->tableView->setModel(model);

        // 刷新初始的数据库界面
        model->select();
    } else {
        QMessageBox::information(
            this, "连接提示", "连接失败"
        );
    }


}

void Widget::initServer(){
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

void Widget::initUI(){
    this->setWindowTitle("Server_db");
    ui->lineEdit_port_set->setText("8000");
}

// server


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
        receiveAttachment(socket, header, buffer);
    }else if(fileType=="message"){
        receiveMessage(socket, buffer);
    }else if(fileType=="sendRegister"){
        receiveRegister(socket, buffer);
    }else if(fileType=="sendLogin"){
        receiveLogin(socket, buffer);
    }else if(fileType=="sendAddFriend"){
        receiveAddFriend(socket, buffer);
    }else if(fileType=="sendCreateGroup"){
        receiveCreateGroup(socket, buffer);
    }else if(fileType=="sendAddGroup"){
        receiveAddGroup(socket, buffer);
    }else if(fileType=="sendSearchFriend"){
        receiveSearchFriend(socket, buffer);
    }else if(fileType=="sendSearchGroup"){
        receiveSearchGroup(socket, buffer);
    }else if(fileType=="sendUserInfoChange"){
        receiveUserInfoChanged(socket, buffer);
    }else if(fileType=="sendP2PMessage"){
        receiveP2PMessage(socket, buffer);
    }else if(fileType=="sendGetUserInfo"){
        receiveGetUserInfo(socket, buffer);
    }else if(fileType=="forwardMessage"){
        receiveforwardMessage(socket, buffer);
    }
}

void Widget::sendSocket(QTcpSocket *socket, QString message, QString fileType){
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


void Widget::receiveAttachment(QTcpSocket* socket, QString header, QByteArray buffer){
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
}

void Widget::receiveMessage(QTcpSocket* socket, QByteArray buffer){
    // 处理收到的文本消息
    QString message = QString("%1 :: %2")
        .arg(socket->socketDescriptor())
        .arg(QString::fromStdString(buffer.toStdString()));
    emit newMessage(message);
}

void Widget::receiveRegister(QTcpSocket* socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject json_obj=QJsonDocument::fromJson(buffer).object();

    //数据库处理
    QString username = json_obj["username"].toString();
    QString password = json_obj["password"].toString();

    QString insert_sql = QString(
        "INSERT INTO User(Username,Password) VALUES('%1','%2');"
    ).arg(username).arg(password);

    QSqlQuery sql_query;
    if (sql_query.exec(insert_sql)){
        // qDebug() << "注册成功";
        // 在这里不能使用Qmessagebox 不然会阻塞导致客户端无法运行
        sendOK("sendRegister",true,socket);
    }else{
        // qDebug() << "注册失败";
        sendOK("sendRegister",false,socket);
    }
}

void Widget::receiveLogin(QTcpSocket* socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject json_obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString username = json_obj["username"].toString();
    QString password = json_obj["password"].toString();

    // 可以初始化user和socket了
    map_user_socket[username] = socket;

    QString select_sql = QString(
        "SELECT * FROM User WHERE Username='%1' AND Password='%2';"
    ).arg(username).arg(password);

    QSqlQuery sql_query;
    sql_query.prepare(select_sql);
    bool flag = false;
    if (sql_query.exec()){
        while (sql_query.next()){
            qDebug() << "登录成功";
            flag = true;
            break;
        }
    }else{
        qDebug() << "登录失败";
    }
    sendOK("sendLogin", flag, socket);
}

void Widget::receiveAddFriend(QTcpSocket* socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString username = obj["username"].toString();
    QString friendname = obj["friendname"].toString();

    QString insert_sql1 = QString(
        "INSERT INTO Friendship(Myname,Friendname) VALUES('%1','%2');"
    ).arg(username).arg(friendname);

    QString insert_sql2 = QString(
        "INSERT INTO Friendship(Myname,Friendname) VALUES('%1','%2');"
    ).arg(friendname).arg(username);

    QSqlQuery sql_query;

    if (sql_query.exec(insert_sql1) && sql_query.exec(insert_sql2)){
        // qDebug() << "添加好友成功";
        sendOK("sendAddFriend",true,socket);
    }else{
        // qDebug() << "添加好友失败";
        sendOK("sendAddFriend",false,socket);
    }
}

void Widget::receiveCreateGroup(QTcpSocket *socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString groupname = obj["groupname"].toString();
    QString username = obj["username"].toString();

    QString insert_sql1 = QString(
        "INSERT INTO Groupinfo(Groupname) VALUES('%1');"
    ).arg(groupname);

    QString insert_sql2 = QString(
        "INSERT INTO Groupship(Username,Groupname) VALUES('%1','%2');"
    ).arg(username).arg(groupname);


    QSqlQuery sql_query;

    if (sql_query.exec(insert_sql1) && sql_query.exec(insert_sql2)){
         qDebug() << "创建群成功";
        sendOK("sendCreateGroup",true,socket);
    }else{
         qDebug() << "创建群失败";
        sendOK("sendCreateGroup",false,socket);
    }
}

void Widget::receiveAddGroup(QTcpSocket *socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString username = obj["username"].toString();
    QString groupname = obj["groupname"].toString();

    QString insert_sql = QString(
        "INSERT INTO Groupship(Username,Groupname) VALUES('%1','%2');"
    ).arg(username).arg(groupname);

    QSqlQuery sql_query;

    if (sql_query.exec(insert_sql)){
        qDebug() << "添加群成功";
        sendOK("sendAddGroup",true,socket);
    }else{
        qDebug() << "添加群失败";
        sendOK("sendAddGroup",false,socket);
    }
}

void Widget::receiveSearchGroup(QTcpSocket *socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString username = obj["username"].toString();

    // 向客户端发送群列表QJsonArray
    QJsonArray groupList;
    QSqlQuery sql_query;

    // 下面错了
//    QString select_sql = QString(
//        "SELECT * FROM Groupship WHERE Username='%1';"
//    ).arg(username);

    // 正确的 SQL 查询语句
    QString select_sql = QString(
        "SELECT G.Groupname, GI.Group_profile FROM Groupship AS G "
        "JOIN Groupinfo AS GI ON G.Groupname = GI.Groupname "
        "WHERE G.Username = '%1';"
    ).arg(username);


    bool flag = false;

    if (sql_query.exec(select_sql)){
        while (sql_query.next()){
            QString groupname = sql_query.value("Groupname").toString();
            // 新👇
            QString groupprofile = sql_query.value("Group_profile").toString();
            QJsonObject groupInfo;
            groupInfo.insert("groupname",groupname);
            groupInfo.insert("groupprofile",groupprofile);
            groupList.append(groupInfo);
            // 新👆
            flag = true;
        }
    }else{
        qDebug() << "查询群失败";
    }

    QJsonObject obj2client;
    obj2client.insert("type", "sendSearchGroup");
    obj2client.insert("isSuc", flag?"true":"false");
    obj2client.insert("groupList", groupList);
    QString message;
    message = QJsonDocument(obj2client).toJson();

    sendSocket(socket, message, "sendOK");

}

void Widget::receiveSearchFriend(QTcpSocket *socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString username = obj["username"].toString();

    // 向客户端发送好友列表QJsonArray 
    // 向Friendship表查询朋友列表 同时向User表查询朋友的profile_picture nickname 添加到最后的QJsonArray中
    QJsonArray friendList;
    QSqlQuery sql_query;
    QString select_sql = QString(
        "SELECT Friendship.Friendname, User.Profile_picture, User.Nickname "
        "FROM Friendship "
        "INNER JOIN User ON Friendship.Friendname = User.Username "
        "WHERE Friendship.Myname='%1';"
    ).arg(username);

    bool flag = false;

    if (sql_query.exec(select_sql)) {
        while (sql_query.next()) {
            QString friendname = sql_query.value("Friendname").toString();
            QString profilePicture = sql_query.value("Profile_picture").toString();
            QString nickname = sql_query.value("Nickname").toString();

            QJsonObject friendInfo;
            friendInfo.insert("friendname", friendname);
            friendInfo.insert("profilePicture", profilePicture);
            friendInfo.insert("nickname", nickname);
            friendList.append(friendInfo);

            flag = true;
        }
    } else {
        qDebug() << "查询好友失败";
    }

    QJsonObject obj2client;
    obj2client.insert("type", "sendSearchFriend");
    obj2client.insert("isSuc", flag?"true":"false");
    obj2client.insert("friendList", friendList);
    QString message;
    message = QJsonDocument(obj2client).toJson();

    sendSocket(socket, message, "sendOK");
}


void Widget::receiveP2PMessage(QTcpSocket *socket, QByteArray buffer){
    qDebug() << "接收到P2P信息";
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理
    QString sender = obj["sender"].toString();
    QString receiver = obj["receiver"].toString();
    QString msg = obj["msg"].toString();
    QString type = obj["type"].toString(); // Friend or Group

    QString insert_sql = QString(
        "INSERT INTO %1msg(Sender,Receiver,Msg,DT) VALUES('%2','%3','%4',datetime('now','localtime'));"
    ).arg(type).arg(sender).arg(receiver).arg(msg);

    qDebug() << insert_sql;

    QSqlQuery sql_query;

    if (sql_query.exec(insert_sql)){
        qDebug() << "发送消息成功";
        sendOK("sendP2PMessage", true, socket);
    }else{
        qDebug() << "发送消息失败";
        sendOK("sendP2PMessage", false, socket);
    }
}

void Widget::receiveUserInfoChanged(QTcpSocket *socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    //数据库处理

    QString Username = obj["username"].toString();
    QString Nickname = obj["nickname"].toString();
    QString profile_picture = obj["headFile"].toString();
    QString Birthday = obj["birthday"].toString();
    QString Region = obj["region"].toString();
    QString Age = obj["age"].toString();
    QString Sex = obj["sex"].toString();

    // 更新数据库User表
    QString update_sql = "UPDATE User SET "
                         "Nickname = :Nickname, "
                         "Profile_picture = :Profile_picture, "
                         "Birthday = :Birthday, "
                         "Region = :Region, "
                         "Age = :Age, "
                         "Sex = :Sex "
                         "WHERE Username = :Username";

    QSqlQuery query;
    query.prepare(update_sql);
    query.bindValue(":Nickname", Nickname);
    query.bindValue(":Profile_picture", profile_picture);
    query.bindValue(":Birthday", Birthday);
    query.bindValue(":Region", Region);
    query.bindValue(":Age", Age.toInt()); // Assuming Age is stored as an integer in the database
    query.bindValue(":Sex", Sex);
    query.bindValue(":Username", Username);

    bool flag = false;

    if(query.exec()){
        qDebug() << "更新User表成功";
        flag = true;
    }else{
        qDebug() << "更新User表失败";
    }
    // 向客户端发送是否成功
    sendOK("sendUserInfoChange",flag,socket);
}

void Widget::receiveGetUserInfo(QTcpSocket *socket, QByteArray buffer){
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    QString Username = obj["username"].toString();

    // 数据库处理 查询user用户返回其基本信息
    QString select_sql = QString(
        "SELECT * FROM User WHERE Username='%1';"
    ).arg(Username);

    QSqlQuery sql_query;
    bool flag = false;

    QString nickname;
    QString image;
    QString motto;
    QString sex;
    quint32 age = 0;
    QString birthday;
    QString location;

    if (sql_query.exec(select_sql)) {
        if (sql_query.next()) {
            flag = true;
            nickname = sql_query.value("Nickname").toString();
            image = sql_query.value("Profile_picture").toString();
            motto = sql_query.value("Personal_signature").toString();
            sex = sql_query.value("Sex").toString();
            age = sql_query.value("Age").toUInt();
            birthday = sql_query.value("Birthday").toString();
            location = sql_query.value("Region").toString();
        }
    } else {
        qDebug() << "Query failed: " << sql_query.lastError();
    }

    QJsonObject userInfo;
    userInfo.insert("type", "sendGetUserInfo");
    userInfo.insert("isSuc", flag?"true":"false");
    userInfo["username"] = Username;
    userInfo["nickname"] = nickname;
    userInfo["profile_picture"] = image;
    userInfo["personal_signature"] = motto;
    userInfo["sex"] = sex;
    userInfo["age"] = static_cast<int>(age);
    userInfo["birthday"] = birthday;
    userInfo["location"] = location;

    QString message;
    message = QJsonDocument(userInfo).toJson();
    sendSocket(socket, message, "sendOK");

}

void Widget::receiveforwardMessage(QTcpSocket *socket, QByteArray buffer){
    Q_UNUSED(socket);
    // 处理收到的文本消息
    QJsonObject obj=QJsonDocument::fromJson(buffer).object();
    obj.insert("type", "forwardMessage");
    QString receiver = obj["receiver"].toString();
    QString str;
    str = QJsonDocument(obj).toJson();

    sendSocket(
        map_user_socket[receiver],
        str,
        "sendOK"
    );
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


// database的功能

void Widget::create_Table_User(){
    // 创建User表
    QSqlQuery sql_query;
    QString create_sql = "CREATE TABLE IF NOT EXISTS User (\
                              Username VARCHAR(20) PRIMARY KEY,\
                              Password VARCHAR(20) NOT NULL,\
                              Nickname VARCHAR(20),\
                              Profile_picture VARCHAR(100),\
                              Personal_signature VARCHAR(100),\
                              Sex VARCHAR(20),\
                              Age INTEGER,\
                              Birthday VARCHAR(20),\
                              Region VARCHAR(20)\
                              );";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "成功建User表！";
    }else{
        qDebug() << "Error: 创建User表失败" << sql_query.lastError();
    }
}

void Widget::create_Table_Friendship(){
    // 创建Friendship表
    QSqlQuery sql_query;
    QString create_sql = "CREATE TABLE IF NOT EXISTS Friendship (\
                              Myname VARCHAR(20) ,\
                              Friendname VARCHAR(20) ,\
                              PRIMARY KEY(Myname,Friendname),\
                              FOREIGN KEY (Myname) REFERENCES User(Username),\
                              FOREIGN KEY (Friendname) REFERENCES User(Username)\
                              );";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "成功建Friendship表！";
    }else{
        qDebug() << "Error: 创建Friendship表失败" << sql_query.lastError();
    }
}

void Widget::create_Table_Friendmsg(){
    // 创建Friendmsg表
    QSqlQuery sql_query;
    QString create_sql = "CREATE TABLE IF NOT EXISTS Friendmsg (\
                              Sender VARCHAR(20),\
                              Receiver VARCHAR(20),\
                              Msg VARCHAR(1024),\
                              DT datetime NOT NULL,\
                              FOREIGN KEY (Sender) REFERENCES User(Username),\
                              FOREIGN KEY (Receiver) REFERENCES User(Username)\
                              );";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "成功建Friendmsg表！";
    }else{
        qDebug() << "Error: 创建Friendmsg表失败" << sql_query.lastError();
    }
}

void Widget::create_Table_Groupinfo(){
    // 创建Groupinfo表
    QSqlQuery sql_query;
    QString create_sql = "CREATE TABLE IF NOT EXISTS Groupinfo (\
                              Groupname VARCHAR(20) PRIMARY KEY,\
                              Group_profile VARCHAR(100)\
                              );";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "成功建Groupinfo表！";
    }else{
        qDebug() << "Error: 创建Groupinfo表失败" << sql_query.lastError();
    }
}

void Widget::create_Table_Groupship(){
    // 创建Groupship表
    QSqlQuery sql_query;
    QString create_sql = "CREATE TABLE IF NOT EXISTS Groupship(\
                              Username VARCHAR(20),\
                              Groupname VARCHAR(20),\
                              PRIMARY KEY(Username,Groupname),\
                              FOREIGN KEY (Username) REFERENCES User(Username),\
                              FOREIGN KEY (Groupname) REFERENCES Groupinfo(Groupname)\
                              );";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "成功建Groupship表！";
    }else{
        qDebug() << "Error: 创建Groupship表失败" << sql_query.lastError();
    }
}

void Widget::create_Table_Groupmsg(){
    // 创建Groupmsg表
    QSqlQuery sql_query;
    QString create_sql = "CREATE TABLE IF NOT EXISTS Groupmsg (\
                              Sender VARCHAR(20),\
                              Receiver VARCHAR(20),\
                              Msg VARCHAR(1024),\
                              DT datetime NOT NULL,\
                              FOREIGN KEY (Sender) REFERENCES User(Username),\
                              FOREIGN KEY (Receiver) REFERENCES Groupinfo(Groupname)\
                              );";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "成功建Groupmsg表！";
    }else{
        qDebug() << "Error: 创建Groupmsg表失败" << sql_query.lastError();
    }
}

void Widget::on_button_signup_clicked()
{   
    // 向User表中插入新数据
    QString Username = ui->lineEdit_Username->text();
    QString Password = ui->lineEdit_Password->text();

     QString insert_sql = QString(
         "INSERT INTO User(Username,Password) VALUES('%1','%2');"
     ).arg(Username).arg(Password);

     QSqlQuery sql_query;
     if (sql_query.exec(insert_sql)){
         QMessageBox::information(this, "注册提示", "注册成功");
     }else{
         QMessageBox::information(this, "注册提示", "注册失败");
     }

    // 注册成功与否都刷新一下model 选择User表
    model->setTable("User");
    ui->tableView->setModel(model);
    model->select();

}

void Widget::on_button_query_clicked()
{
    model->select();
}

void Widget::sendOK(QString type,bool isSuc, QTcpSocket* socket)
{
    QJsonObject obj;
    obj.insert("type", type);
    obj.insert("isSuc", isSuc?"true":"false"); // 一定true和false
    QString message;
    message = QJsonDocument(obj).toJson();
    sendSocket(
        socket, message, "sendOK"
    );
}

void Widget::on_comboBox_tables_currentTextChanged(const QString &arg1)
{
    model->setTable(arg1);
    ui->tableView->setModel(model);
    on_button_query_clicked();
}

void Widget::on_button_login_clicked()
{
    QString username =  ui->lineEdit_Username->text();
    QString password =  ui->lineEdit_Password->text();

    QString select_sql = QString(
        "SELECT * FROM User WHERE Username='%1' AND Password='%2';"
    ).arg(username).arg(password);

    QSqlQuery sql_query;
    sql_query.prepare(select_sql);
    if (sql_query.exec()){
        if (sql_query.next()){
            QMessageBox::information(this, "登录提示", "登录成功");
        }else{
            QMessageBox::information(this, "登录提示", "登录失败");
        }
    }else{
        QMessageBox::information(this, "登录提示", "登录失败");
    }

}

void Widget::on_button_create_group_clicked()
{
    QString groupName = ui->lineEdit_create_group->text();

    QString insert_sql = QString(
        "INSERT INTO Groupinfo(Groupname) VALUES('%1');"
    ).arg(groupName);

    QSqlQuery sql_query;
    if (sql_query.exec(insert_sql)){
        QMessageBox::information(this, "创建群提示", "创建群成功");
    }else{
        QMessageBox::information(this, "创建群提示", "创建群失败");
    }

    // 创建群成功与否都刷新一下model 选择Groupinfo表
    model->setTable("Groupinfo");
    ui->tableView->setModel(model);
    model->select();
}

void Widget::on_button_query_group_clicked()
{
    QString groupName = ui->lineEdit_query_group->text();

    QString select_sql = QString(
        "SELECT * FROM Groupinfo WHERE Groupname='%1';"
    ).arg(groupName);

    QSqlQuery sql_query;
    sql_query.prepare(select_sql);
    if (sql_query.exec()){
        if (sql_query.next()){
            QMessageBox::information(this, "查询群提示", "查询群成功");
        }else{
            QMessageBox::information(this, "查询群提示", "查询群失败");
        }
    }else{
        QMessageBox::information(this, "查询群提示", "查询群失败");
    }    
}

void Widget::on_button_add_group_clicked()
{
    QString groupName = ui->lineEdit_add_group->text();
    QString username = ui->lineEdit_Username->text(); //当前登录的用户


    QString insert_sql = QString(
        "INSERT INTO Groupship(Username,Groupname) VALUES('%1','%2');"
    ).arg(username).arg(groupName);

    QSqlQuery sql_query;
    if (sql_query.exec(insert_sql)){
        QMessageBox::information(this, "加群提示", "加群成功");
    }else{
        QMessageBox::information(this, "加群提示", "加群失败");
    }

    // 加群成功与否都刷新一下model 选择Groupship表
    model->setTable("Groupship");
    ui->tableView->setModel(model);
    model->select();
}

void Widget::on_button_query_group_member_clicked()
{
    QString groupName = ui->lineEdit_query_group_member->text();

    QString select_sql = QString(
        "SELECT * FROM Groupship WHERE Groupname='%1';"
    ).arg(groupName);

    // 将查询结果显示在tableView上
    QSqlQueryModel *queryModel = new QSqlQueryModel;
    queryModel->setQuery(select_sql);
    ui->tableView->setModel(queryModel);

}

void Widget::on_button_send_group_clicked()
{
    QString sendGroupName = ui->lineEdit_send_group_name->text();
    QString sendGroupMessage = ui->lineEdit_send_group_message->text();
    QString username = ui->lineEdit_Username->text(); //当前登录的用户

    QString insert_sql = QString(
        "INSERT INTO Groupmsg(Sender,Receiver,Msg,DT) VALUES('%1','%2','%3',datetime('now','localtime'));"
    ).arg(username).arg(sendGroupName).arg(sendGroupMessage);

    QSqlQuery sql_query;
    if (sql_query.exec(insert_sql)){
        QMessageBox::information(this, "发送群消息提示", "发送群消息成功");
    }else{
        QMessageBox::information(this, "发送群消息提示", "发送群消息失败");
    }

    // 发送群消息成功与否都刷新一下model 选择Groupmsg表
    model->setTable("Groupmsg");
    ui->tableView->setModel(model);
    model->select();
}

void Widget::on_button_query_group_list_clicked()
{
    QString username = ui->lineEdit_Username->text(); //当前登录的用户

    QString select_sql = QString(
        "SELECT * FROM Groupship WHERE Username='%1';"
    ).arg(username);

    // 将查询结果显示在tableView上
    QSqlQueryModel *queryModel = new QSqlQueryModel;
    queryModel->setQuery(select_sql);
    ui->tableView->setModel(queryModel);
}

void Widget::on_button_query_friend_clicked()
{
    QString friendName = ui->lineEdit_query_friend->text();

    QString select_sql = QString(
        "SELECT * FROM User WHERE Username='%1';"
    ).arg(friendName);

    QSqlQuery sql_query;
    sql_query.prepare(select_sql);
    if (sql_query.exec()){
        if (sql_query.next()){
            QMessageBox::information(this, "查询好友提示", "查询好友成功");
        }else{
            QMessageBox::information(this, "查询好友提示", "查询好友失败");
        }
    }else{
        QMessageBox::information(this, "查询好友提示", "查询好友失败");
    }
}

void Widget::on_button_add_friend_clicked()
{
    QString myName = ui->lineEdit_Username->text();
    QString friendName = ui->lineEdit_add_friend->text();

    // 得添加两条记录
    QString insert_sql1 = QString(
        "INSERT INTO Friendship(Myname,Friendname) VALUES('%1','%2');"
    ).arg(myName).arg(friendName);

    QString insert_sql2 = QString(
        "INSERT INTO Friendship(Myname,Friendname) VALUES('%1','%2');"
    ).arg(friendName).arg(myName);

    QSqlQuery sql_query;
    if (sql_query.exec(insert_sql1) && sql_query.exec(insert_sql2)){
        QMessageBox::information(this, "添加好友提示", "添加好友成功");
    }else{
        QMessageBox::information(this, "添加好友提示", "添加好友失败");
    }

    // 添加好友成功与否都刷新一下model 选择Friendship表
    model->setTable("Friendship");
    ui->tableView->setModel(model);
    model->select();
    
}

void Widget::on_button_send_friend_clicked()
{
    QString sendFriendName = ui->lineEdit_send_friend_name->text();
    QString sendFriendMessage = ui->lineEdit_send_friend_message->text();
    QString username = ui->lineEdit_Username->text(); //当前登录的用户

    QString insert_sql = QString(
        "INSERT INTO Friendmsg(Sender,Receiver,Msg,DT) VALUES('%1','%2','%3',datetime('now','localtime'));"
    ).arg(username).arg(sendFriendName).arg(sendFriendMessage);

    QSqlQuery sql_query;
    if (sql_query.exec(insert_sql)){
        QMessageBox::information(this, "发送好友消息提示", "发送好友消息成功");
    }else{
        QMessageBox::information(this, "发送好友消息提示", "发送好友消息失败");
    }

    // 发送好友消息成功与否都刷新一下model 选择Friendmsg表
    model->setTable("Friendmsg");
    ui->tableView->setModel(model);
    model->select();
}

void Widget::on_button_query_friend_list_clicked()
{
    QString username = ui->lineEdit_Username->text(); //当前登录的用户

    QString select_sql = QString(
        "SELECT * FROM Friendship WHERE Myname='%1';"
    ).arg(username);

    // 将查询结果显示在tableView上
    QSqlQueryModel *queryModel = new QSqlQueryModel;
    queryModel->setQuery(select_sql);
    ui->tableView->setModel(queryModel);
}

void Widget::on_button_alter_nickname_clicked()
{
    QString nickname = ui->lineEdit_alter_nickname->text();
    QString username = ui->lineEdit_Username->text(); //当前登录的用户

    QString update_sql = QString(
        "UPDATE User SET Nickname='%1' WHERE Username='%2';"
    ).arg(nickname).arg(username);

    QSqlQuery sql_query;
    if (sql_query.exec(update_sql)){
        QMessageBox::information(this, "修改昵称提示", "修改昵称成功");
    }else{
        QMessageBox::information(this, "修改昵称提示", "修改昵称失败");
    }

    // 修改昵称成功与否都刷新一下model 选择User表
    model->setTable("User");
    ui->tableView->setModel(model);
    model->select();
}

void Widget::on_button_alter_profile_clicked()
{
    QString profile_FilePath = ui->lineEdit_alter_profile->text();
    QString username = ui->lineEdit_Username->text(); //当前登录的用户

    QString update_sql = QString(
        "UPDATE User SET Profile_picture='%1' WHERE Username='%2';"
    ).arg(profile_FilePath).arg(username);

    QSqlQuery sql_query;
    if (sql_query.exec(update_sql)){
        QMessageBox::information(this, "修改头像提示", "修改头像成功");
    }else{
        QMessageBox::information(this, "修改头像提示", "修改头像失败");
    }

    // 修改头像成功与否都刷新一下model 选择User表
    model->setTable("User");
    ui->tableView->setModel(model);
    model->select();
}
