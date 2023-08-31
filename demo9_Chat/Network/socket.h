#ifndef SOCKET_H
#define SOCKET_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <DB/user.h>
#include <DB/groupinfo.h>

class Socket : public QWidget
{
    Q_OBJECT
public:
    explicit Socket(
        QTcpSocket *s,
        QWidget *parent = nullptr
    );

    static Socket* instance; // 全局调用
    static QString username; // 当前用户

    // 发送
    void send_message(QString str);
    void send_attachment();

    // 数据库操作
    void sendGetUserInfo(QString username);

    void sendRegister(QString username, QString password);
    void sendLogin(QString username, QString password);
    void sendAddFriend(QString username, QString friendname);
    void sendCreateGroup(QString username, QString groupname);
    void sendAddGroup(QString username, QString groupname);

    void sendSearchGroup(QString username);
    void sendSearchFriend(QString username);

    // 转发信息 和其它都有区别 不存数据库
    void forwardMessage(QString sender, QString sender_icon, QString type,
                        QString receiver, QString msg);

    void sendUserInfoChange(QString username, QString nickname = "",
                            QString headFile= "", QString birthday = "",
                            QString region = "", QString age = "",
                            QString sex = "");

    // 发送数据给服务器 自定义类型
    void sendSocket(QString message, QString fileType);

signals:
    void newMessage(QString);
    void newForwardMessage(QString sender, QString sender_icon,
                           QString receiver, QString message);

    void RegisterOK(bool isSuc);// 注册ok信号
    void LoginOK(bool isSuc); //登录进入
    void AddFriendOK(bool isSuc); // 加好友
    void CreateGroupOK(bool isSuc); // 建群成功
    void AddGroupOK(bool isSuc); // 加群ok
    void ChangeInfoOK(bool isSuc); // 改变信息
    void SearchFriendOK(bool isSuc, QList<User> friendList); // 查找朋友
    void SearchGroupOK(bool isSuc, QList<Groupinfo> groupList); // 查找群聊
    void GetUserInfoOK(bool isSuc, User user); // 返回一个用户指针


private slots:
    // 读取
    void readSocket();
    void discardSocket();

private:
    QTcpSocket *socket;


};

#endif // SOCKET_H
