#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QFrame>
#include <QMenu>
#include <QVBoxLayout>
#include <QToolButton>
#include <QListWidget>
#include <QTableWidgetItem>
#include <QKeyEvent>
#include <QDir>
#include <QTcpSocket>
#include <Network/socket.h>
#include <DB/user.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QTcpSocket *s,QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *target, QEvent *event) override;// 事件过滤器 换行使用
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // 聊天相关 朋友或群聊
    QString cur_friend_name;
    QString cur_friend_icon;


    Ui::MainWindow *ui;
    QTcpSocket *socket;
    Socket *mySocket;
    QFrame *currentframe;
    QMenu *ColorMenu; // 颜色列表
    QString color; //气泡颜色

    // 四个功能切换的排布
    QVBoxLayout *layer_friend;
    QVBoxLayout *layer_group;
    QVBoxLayout *layer_add;
    QVBoxLayout *layer_setting;

    //添加界面 三个按钮
    QToolButton *add_friend;
    QToolButton *add_group;
    QToolButton *create_group;

    //修改界面 2个按钮
    QToolButton *alter_name_button;
    QToolButton *alter_image_button;

    QPoint m_dragStartPosition; //页面拖动要用

    User* MYSELF;
    QList<User> curFriendList;
    QList<Groupinfo> curGroupList;

    //好友用户名和好友列表button关系
    QMap<QString, QToolButton*> map_username_button;

    //群聊用户名和群聊列表的关系
    QMap<QString, QToolButton*> map_groupname_button;


private slots:

    // 打开和好友的聊天界面
    void open_friendchat_window();
    // 打开群聊的聊天界面
    void open_groupchat_window();

    void getCurUserSlot(bool isSuc, User user);
    void getFriendListSlot(bool isSuc, QList<User> friendList);
    void getGroupListSlot(bool isSuc, QList<Groupinfo> grouplist);

    void initWindow();

    // 连接服务器获得信息
    void setServerInfo(QTcpSocket *s);

    // 截图
    void onCompleteCapture(QPixmap captureImage);

    // 关闭缩小
    void on_button_mini_clicked();
    void on_button_close_clicked();

    // buttons_layout点击切换
    void on_button_friendlist_clicked();
    void on_button_grouplist_clicked();
    void on_button_add_clicked();
    void on_button_setting_clicked();

    // buttons_tool 5个图标
    void on_button_emoji_clicked();
    void on_button_screenShot_clicked();
    void on_button_picture_clicked(); // 待接入
    void on_button_file_clicked(); // 待接入
    void on_button_style_clicked();

    // 消息发送
    void on_button_send_clicked();

    // 显示好友或者服务端的信息
    void displayFriendsMessage(QString sender, QString sender_icon,
                               QString receiver, QString message);

    // 窗口初始化
    void FriendList(); //获取好友列表
    void GroupList(); //获取群聊列表
    void AddFriend();   //添加界面
    void Settingframe(); //修改界面
    void EmojiList(); //表情包加载
    void initProfile(); // 待实现 加载个人信息

    // 点击表情包
    void onEmojiItemClicked(QTableWidgetItem *item);

    // 换气泡颜色
    void changeColor();

    //添加窗口
    void add_friend_widget();
    void add_group_widget();
    void create_group_widget();

    //修改窗口
    void alter_name_widget();
    void alter_image_widget();

};

#endif // MAINWINDOW_H
