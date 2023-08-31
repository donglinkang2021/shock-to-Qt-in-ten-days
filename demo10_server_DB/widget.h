#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QList>
#include <QNetworkInterface>
#include <QHostAddress>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void initDatabase();
    void initServer();
    void initUI();

    void create_Table_User();
    void create_Table_Friendship();
    void create_Table_Friendmsg();
    void create_Table_Groupinfo();
    void create_Table_Groupship();
    void create_Table_Groupmsg();

    void on_button_query_clicked();

    // 发送成功则要给客户端发回sendOK fileType包
    void sendOK(QString type,bool isSuc, QTcpSocket* socket);

signals:
    void newMessage(QString);

private slots:

    // 数据库功能测试

    void on_button_signup_clicked();

    void on_comboBox_tables_currentTextChanged(const QString &arg1);

    void on_button_login_clicked();

    void on_button_create_group_clicked();

    void on_button_query_group_clicked();

    void on_button_add_group_clicked();

    void on_button_send_group_clicked();

    void on_button_query_group_member_clicked();

    void on_button_query_group_list_clicked();

    void on_button_query_friend_clicked();

    void on_button_add_friend_clicked();

    void on_button_send_friend_clicked();

    void on_button_query_friend_list_clicked();

    void on_button_alter_nickname_clicked();

    void on_button_alter_profile_clicked();

    // server

    void readSocket(); // 读取每个socket发送的信息
    void discardSocket(); // 丢弃该socket

    void newConnection(); // 每当有一个用户加进来就会有一个新连接，并调用`appendToSocketList`
    void appendToSocketList(QTcpSocket* socket); // 添加新的socket

    void displayMessage(const QString& str); // 在browser中显示消息
    void refreshComboBox(); // 刷新组合框

    void on_button_clear_clicked();
    void on_button_send_clicked();

    void sendMessage(QTcpSocket* socket); // fileType message
    void sendAttachment(QTcpSocket* socket, QString filePath); // fileType attachment

    void on_button_attachment_clicked();

    void on_lineEdit_send_returnPressed();

    void on_button_config_port_clicked();

    void on_button_check_local_IP_clicked();

private:
    Ui::Widget *ui;

    // db
    QSqlDatabase db;
    QSqlTableModel *model; // 总模型

    // server
    QTcpServer *server; // 服务器对象
    QSet<QTcpSocket*> connection_set; // 所有连接到server的socket

    QMap<QString, QTcpSocket*> map_user_socket;

    void sendSocket(QTcpSocket *socket, QString message, QString fileType); // any fileType

    void receiveAttachment(QTcpSocket* socket, QString header, QByteArray buffer);
    void receiveMessage(QTcpSocket* socket, QByteArray buffer);
    void receiveRegister(QTcpSocket* socket, QByteArray buffer);
    void receiveLogin(QTcpSocket* socket, QByteArray buffer);
    void receiveAddFriend(QTcpSocket* socket, QByteArray buffer);
    void receiveCreateGroup(QTcpSocket* socket, QByteArray buffer);
    void receiveAddGroup(QTcpSocket* socket, QByteArray buffer);
    void receiveSearchGroup(QTcpSocket* socket, QByteArray buffer);
    void receiveSearchFriend(QTcpSocket* socket, QByteArray buffer);
    void receiveP2PMessage(QTcpSocket* socket, QByteArray buffer);
    void receiveUserInfoChanged(QTcpSocket* socket, QByteArray buffer);

    void receiveforwardMessage(QTcpSocket* socket, QByteArray buffer);

    // 新
    void receiveGetUserInfo(QTcpSocket* socket, QByteArray buffer);

};
#endif // WIDGET_H
