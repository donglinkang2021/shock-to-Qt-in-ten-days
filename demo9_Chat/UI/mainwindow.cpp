#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "itemchatwidget.h"
#include "addfriendwidget.h"
#include "addgroupwidget.h"
#include "creategroupwidget.h"
#include "alternamewidget.h"
#include "alterimagewidget.h"
#include "capturescreen.h"

MainWindow::MainWindow(QTcpSocket *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setServerInfo(s);
    this->setWindowFlags(Qt::FramelessWindowHint); //无窗口边框
    initWindow(); //初始化
}

MainWindow::~MainWindow()
{
    if (mySocket){
        delete mySocket;
    }
    delete ui;
}

void MainWindow::initWindow(){
    //信息初始化
    initProfile();
    FriendList(); //好友列表界面加载
    GroupList();  //群聊列表界面加载
    AddFriend();   //添加界面加载
    Settingframe();//修改界面加载
    EmojiList(); //Emoji列表加载


    //界面初始化
    ui->frame_friend->hide();
    ui->frame_group->hide();
    ui->frame_add->hide();
    ui->frame_setting->hide();
    currentframe = ui->frame_friend;
    currentframe->show();
    ui->frame_dialog->hide();
    ui->frame_background->hide();
    ui->frame_info->hide();

    // 输入ctrl+enter换行
    ui->textEdit_edit->installEventFilter(this);

    // 隐藏滚动条
    ui->listWidget_chat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget_chat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);



}

void MainWindow::setServerInfo(QTcpSocket *s){
    socket = s;
    mySocket = Socket::instance;
    // 服务器用的
//    connect(
//        mySocket, &Socket::newMessage,
//        this, &MainWindow::displayFriendsMessage
//    );
    connect(
        mySocket, &Socket::newForwardMessage,
        this, &MainWindow::displayFriendsMessage
    );

}

void MainWindow::on_button_mini_clicked()
{
    this->showMinimized();
}

void MainWindow::on_button_close_clicked()
{
    this->close();
}

void MainWindow::on_button_friendlist_clicked()
{
    currentframe->hide();
    FriendList();
    currentframe = ui->frame_friend;
    currentframe->show();
    ui->frame_dialog->show();
    ui->frame_background->hide();
    ui->frame_info->hide();
}

void MainWindow::on_button_grouplist_clicked()
{
    currentframe->hide();
    GroupList();
    currentframe = ui->frame_group;
    currentframe->show();
    ui->frame_dialog->show();
    ui->frame_background->hide();
    ui->frame_info->hide();
}

void MainWindow::on_button_add_clicked()
{
    currentframe->hide();
    currentframe = ui->frame_add;
    currentframe->show();
    ui->frame_dialog->hide();
    ui->frame_background->show();
    ui->frame_info->hide();
}

void MainWindow::on_button_setting_clicked()
{
    currentframe->hide();
    currentframe = ui->frame_setting;
    currentframe->show();
    ui->frame_dialog->hide();
    ui->frame_info->show();
}

void MainWindow::on_button_emoji_clicked()
{
    ui->tableWidget_emoji->setVisible(
        !ui->tableWidget_emoji->isVisible()
    );
}

void MainWindow::on_button_screenShot_clicked()
{
    CaptureScreen* captureHelper = new CaptureScreen();
    connect(
        captureHelper, SIGNAL(signalCompleteCature(QPixmap)),
        this, SLOT(onCompleteCapture(QPixmap))
    );
    captureHelper->show();
}

void MainWindow::on_button_picture_clicked()
{

}

void MainWindow::on_button_file_clicked()
{
    mySocket->send_attachment();
}

void MainWindow::on_button_style_clicked()
{
    ColorMenu = new QMenu(ui->frame_dialog);
    QString Colors[5] = {"橙色🧡", "红色💖", "蓝色💙", "绿色💚", "紫色💜"};
    for(int i=0;i<5;i++)
    {
        QAction* colorAction = new QAction(Colors[i]);
        colorAction->setObjectName(QString::number(i));
        connect(
            colorAction,SIGNAL(triggered()),
            this,SLOT(changeColor())
        );
        ColorMenu->addAction(colorAction);
    }

    // 设置样式图标
    ui->button_style->setPopupMode(QToolButton::InstantPopup);
    ui->button_style->setMenu(ColorMenu);
}



void MainWindow::on_button_send_clicked()
{


    // 发送信息 给服务器
//    mySocket->send_message(
//        ui->textEdit_edit->toPlainText()
//    );

    Socket::instance->forwardMessage(
        Socket::username, MYSELF->getImage(), "Friend",
        cur_friend_name, ui->textEdit_edit->toPlainText()
    );

    ItemChatWidget* itemw = new ItemChatWidget(this);
    itemw -> setColor(color);
    int height = itemw->SetMyData(
        MYSELF->getName(),MYSELF->getImage(),
        ui->textEdit_edit->toPlainText()
    );

    QListWidgetItem* line = new QListWidgetItem();
    line->setSizeHint(QSize(350, height));
    ui->listWidget_chat->addItem(line);
    ui->listWidget_chat->setItemWidget(line, itemw);
    ui->listWidget_chat->scrollToBottom();
    ui->textEdit_edit->clear();

}

void MainWindow::displayFriendsMessage(QString sender, QString sender_icon,
                                       QString receiver, QString message){
    Q_UNUSED(receiver);
    ItemChatWidget* itemw = new ItemChatWidget(this);
    itemw -> setColor(color);
    int height = itemw->SetFriendsData(
        sender,sender_icon,
        message
    );

    QListWidgetItem* line = new QListWidgetItem();
    line->setSizeHint(QSize(350, height));
    ui->listWidget_chat->addItem(line);
    ui->listWidget_chat->setItemWidget(line, itemw);
    ui->listWidget_chat->scrollToBottom();

    cur_friend_name = sender;
    cur_friend_icon = sender_icon; //可选择头像 以QString类型存储

    //在聊天框显示头像和用户名
    ui->button_temp_img->setIconSize(QSize(40,40));
    ui->button_temp_img->setIcon(QIcon(cur_friend_icon));
    ui->button_temp_img->setText(" " + cur_friend_name);
    ui->button_temp_img->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); //text在icon旁边
}

void MainWindow::FriendList()
{
    connect(
       mySocket, &Socket::SearchFriendOK,
       this, &MainWindow::getFriendListSlot
    );
//    layer_friend = new QVBoxLayout(ui->frame_friend);
    Socket::instance->sendSearchFriend(Socket::username);

}

void MainWindow::getFriendListSlot(bool isSuc, QList<User> friendList){
    if (isSuc){
        qDebug() << "接收到friendList信息";
        curFriendList = friendList;
    }else{
        qDebug() << "没有接收到friendList信息";
    }

    //layer_friend清空
    delete layer_friend;
    layer_friend = new QVBoxLayout(ui->frame_friend);

    for (int i=0; i<curFriendList.size();i++){

        //获取好友信息
        QString f_name = curFriendList[i].getName();

        //判断好友是否已创建button/listwidget 若存在，则不new btn/listwid
        if(map_username_button.contains(f_name)){
            QToolButton* toolbtn = map_username_button[f_name];
            layer_friend->addWidget(toolbtn);
            continue;
        }

        User* user_friend = &curFriendList[i];

        QString f_image = user_friend->getImage(); //可选择头像 以QString类型存储

        //好友列表button设置
        QToolButton* toolbtn = new QToolButton; //创建好友按钮
        toolbtn->setIconSize(QSize(40,40));
        toolbtn->setObjectName(f_name);  //按键名为id
        toolbtn->setStyleSheet(
            "QToolButton{color:rgb(0,0,0);border:none;font-size:20px;}"
        );
        toolbtn->setIcon(QIcon(f_image));
        toolbtn->setText(f_name);
        toolbtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); //text在icon旁边

        map_username_button[user_friend->getName()] = toolbtn;  //好友username和toolbtn对应

        connect(
            toolbtn, SIGNAL(clicked()),
            this, SLOT(open_friendchat_window())
        ); //实现通信

        layer_friend->addWidget(toolbtn);

    }
    layer_friend->addStretch(); //调整组件布局比例
}

void MainWindow::open_friendchat_window()
{
    QToolButton* Sender = qobject_cast<QToolButton*>(sender());

    cur_friend_name = Sender->text();

    foreach(User curfriend, curFriendList) {
        if(curfriend.getName() == cur_friend_name){
            cur_friend_icon = curfriend.getImage();
            break;
        }
    }

    //在聊天框显示头像和用户名
    ui->button_temp_img->setIconSize(QSize(40,40));
    ui->button_temp_img->setIcon(QIcon(cur_friend_icon));
    ui->button_temp_img->setText(" " + cur_friend_name);
    ui->button_temp_img->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); //text在icon旁边

}


void MainWindow::GroupList()
{
    connect(
       mySocket, &Socket::SearchGroupOK,
       this, &MainWindow::getGroupListSlot
    );
//    layer_group = new QVBoxLayout(ui->frame_group);
    Socket::instance->sendSearchGroup(Socket::username);
}

void MainWindow::getGroupListSlot(bool isSuc, QList<Groupinfo> grouplist){
    if (isSuc){
        qDebug() << "接收到GroupList信息";
        curGroupList = grouplist;
    }else{
        qDebug() << "没有接收到GroupList信息";
    }

    delete layer_group;
    layer_group = new QVBoxLayout(ui->frame_group);

    for (int i=0; i<curGroupList.size();i++){

        //获取好友信息
        QString g_name = curGroupList[i].getName();

        //判断好友是否已创建button/listwidget 若存在，则不new btn/listwid
        if(map_groupname_button.contains(g_name)){
            QToolButton* toolbtn = map_username_button[g_name];
            layer_group->addWidget(toolbtn);
            continue;
        }

        Groupinfo* user_group = &curGroupList[i];

        QString g_image = user_group->getGroupProfile(); //可选择头像 以QString类型存储

        //好友列表button设置
        QToolButton* toolbtn = new QToolButton; //创建好友按钮
        toolbtn->setIconSize(QSize(40,40));
        toolbtn->setObjectName(g_image);  //按键名为id
        toolbtn->setStyleSheet(
            "QToolButton{color:rgb(0,0,0);border:none;font-size:20px;}"
        );
        toolbtn->setIcon(QIcon(g_image));
        toolbtn->setText(g_name);
        toolbtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); //text在icon旁边

        map_groupname_button[user_group->getGroupName()] = toolbtn;  //好友username和toolbtn对应

        connect(
            toolbtn, SIGNAL(clicked()),
            this, SLOT(open_groupchat_window())
        ); //实现通信

        layer_group->addWidget(toolbtn);

    }
    layer_group->addStretch(); //调整组件布局比例
}

void MainWindow::open_groupchat_window(){
    QToolButton* Sender = qobject_cast<QToolButton*>(sender());

    cur_friend_name = Sender->text();

    foreach(Groupinfo curfriend, curGroupList) {
        if(curfriend.getGroupName() == cur_friend_name){
            cur_friend_icon = curfriend.getGroupProfile();
            break;
        }
    }

    //在聊天框显示头像和用户名
    ui->button_temp_img->setIconSize(QSize(40,40));
    ui->button_temp_img->setIcon(QIcon(cur_friend_icon));
    ui->button_temp_img->setText(" " + cur_friend_name);
    ui->button_temp_img->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); //text在icon旁边

}

void MainWindow::AddFriend()
{
    layer_friend = new QVBoxLayout(ui->frame_add);
    //添加好友
    add_friend = new QToolButton(ui->frame_add);
    add_friend->setText("添加好友");
    add_friend->setGeometry(50,30,150,50);
    add_friend->setStyleSheet(
        "QToolButton{\
        background-color:rgb(223,234,250);\
        color:rgb(6,50,115);\
        border:1px solid rgb(81,109,146);\
        border-radius:5px;}"
    );
    connect(
        add_friend,SIGNAL(clicked()),
        this,SLOT(add_friend_widget())
    );

    //添加群聊
    add_group = new QToolButton(ui->frame_add);
    add_group->setText("添加群聊");
    add_group->setGeometry(50,120,150,50);
    add_group->setStyleSheet(
        "QToolButton{\
        background-color:rgb(223,234,250);\
        color:rgb(6,50,115);\
        border:1px solid rgb(81,109,146);\
        border-radius:5px;}"
    );
    connect(
        add_group,SIGNAL(clicked()),
        this,SLOT(add_group_widget())
    );

    //创建群聊
    create_group = new QToolButton(ui->frame_add);
    create_group->setText("创建群聊");
    create_group->setGeometry(50,210,150,50);
    create_group->setStyleSheet(
        "QToolButton{\
        background-color:rgb(223,234,250);\
        color:rgb(6,50,115);\
        border:1px solid rgb(81,109,146);border-radius:5px;}"
    );
    connect(
        create_group,SIGNAL(clicked()),
        this,SLOT(create_group_widget())
    );

}

void MainWindow::Settingframe()
{
    layer_setting = new QVBoxLayout(ui->frame_setting);
    //修改昵称
    alter_name_button = new QToolButton(ui->frame_setting);
    alter_name_button->setText("修改昵称");
    alter_name_button->setGeometry(50,30,150,50);
    alter_name_button->setStyleSheet(
        "QToolButton{\
            background-color:rgb(223,234,250);\
            color:rgb(6,50,115);\
            border:1px solid rgb(81,109,146);\
            border-radius:5px;}"
    );
    connect(
        alter_name_button,SIGNAL(clicked()),
        this,SLOT(alter_name_widget())
    );

    //修改头像
    alter_image_button = new QToolButton(ui->frame_setting);
    alter_image_button->setText("修改头像");
    alter_image_button->setGeometry(50,120,150,50);
    alter_image_button->setStyleSheet(
        "QToolButton{\
            background-color:rgb(223,234,250);\
            color:rgb(6,50,115);\
            border:1px solid rgb(81,109,146);\
            border-radius:5px;}"
    );
    connect(
        alter_image_button,SIGNAL(clicked()),
        this,SLOT(alter_image_widget())
    );

}

void MainWindow::changeColor()
{
    QAction* Sender = qobject_cast<QAction*>(sender());
    QString colorRGB[5] = {
        "rgb(254, 208, 142)",
        "rgb(254, 147, 152)",
        "rgb(153, 224, 254)",
        "rgb(137, 214, 146)",
        "rgb(221, 188, 254)"
    };
    int i =Sender->objectName().toUInt();
    color = colorRGB[i];
}

void MainWindow::EmojiList(){
    //加载表情包列表
    ui->tableWidget_emoji->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_emoji->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_emoji->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int ROW_COUNT=9, COL_COUNT=10, HEIGHT=131, WIDTH= 551;

    ui->tableWidget_emoji->setRowCount(ROW_COUNT);
    ui->tableWidget_emoji->setColumnCount(COL_COUNT);
    ui->tableWidget_emoji->setWindowTitle("QTableWidget & Item");
    ui->tableWidget_emoji->resize(WIDTH, HEIGHT);  //设置表格
    ui->tableWidget_emoji->verticalHeader()->setVisible(false); // 隐藏行号
    ui->tableWidget_emoji->horizontalHeader()->setVisible(false);
    char32_t  t[] = {0x1F601,0x0};	//加上0x00是为了防止表情后面跟随乱码
    int cnt = 0;
    for(char32_t i = 0x1F601 ; i <=  0x1F64F ; ++i, ++cnt)
    {
        ui->tableWidget_emoji->setRowHeight(cnt/COL_COUNT, HEIGHT/ROW_COUNT);
        ui->tableWidget_emoji->setColumnWidth(cnt%COL_COUNT, WIDTH/COL_COUNT);
        t[0] = i ;
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromUcs4(t));
        ui->tableWidget_emoji->setItem(cnt/COL_COUNT,cnt%COL_COUNT,item);
    }
//    qDebug()<<cnt;
    ui->tableWidget_emoji->setVisible(false);
    connect(
        ui->tableWidget_emoji, &QTableWidget::itemClicked,
        this, &MainWindow::onEmojiItemClicked
    );
}

void MainWindow::getCurUserSlot(bool isSuc, User user){
    if (isSuc){
        qDebug() << "接收到user信息";
        MYSELF = new User(user);
        // 现在才开始初始化头像什么的
        //左上角信息显示
        ui->button_user_image->setIcon(QIcon(MYSELF->getImage()));
        ui->label_username_brief->setText(MYSELF->getName()); //用户名
        ui->label_userid_brief->setText(MYSELF->getNickname()); //昵称
    }else{
        qDebug() << "没有接收到user信息";
    }
}

void MainWindow::initProfile(){
    qDebug() << "开始initProfile";
    connect(
        mySocket, &Socket::GetUserInfoOK,
        this, &MainWindow::getCurUserSlot
    );

    // 向服务器请求发送数据 会获得一个信号 所以需要先connect
    Socket::instance->sendGetUserInfo(Socket::username);
}

//选择表情包后在输入框显示并关闭表情包列表
void MainWindow::onEmojiItemClicked(QTableWidgetItem *item)
{
    if (item) {
//        qDebug() << "Item Clicked:" << item->text();
        ui->textEdit_edit->insertPlainText(item->text());
        on_button_emoji_clicked();
    }
}

//打开窗口
//打开添加好友窗口
void MainWindow::add_friend_widget()
{
    AddFriendWidget *w = new AddFriendWidget;
    w->show();
}

//打开添加群聊窗口
void MainWindow::add_group_widget()
{
    AddGroupWidget *w = new AddGroupWidget;
    w->show();
}

//打开创建群聊窗口
void MainWindow::create_group_widget()
{
    CreateGroupWidget *w = new CreateGroupWidget;
    w->show();
}

//打开修改昵称窗口
void MainWindow::alter_name_widget()
{
    AlterNameWidget *w = new AlterNameWidget;
    w->show();
}

//打开修改头像窗口
void MainWindow::alter_image_widget()
{
    AlterImageWidget *w = new AlterImageWidget;
    w->show();
}

//事件过滤器实现 实现回车换行
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit_edit){
        if(event->type() == QEvent::KeyPress){
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if (k->key() == Qt::Key_Return){
                if(k->modifiers() == Qt::ControlModifier){
                    ui->textEdit_edit->insertPlainText("\n");  // Ctrl+enter换行
                }else{
                    on_button_send_clicked();
                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(target,event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
        event->accept();
    }
}

void MainWindow::onCompleteCapture(QPixmap captureImage)
{
    //保存图片到本地
    QDir* folder = new QDir;
    if (!folder->exists("./screenshots")) {
        folder->mkpath("./screenshots");
    }
    QString filePath = "./screenshots/" + \
            (QDateTime::currentDateTime())
                .toString("yyyy_MM_dd_hh_mm_ss_ms") + ".png";
    captureImage.save(filePath);

    //显示图片
    ItemChatWidget* itemw = new ItemChatWidget(this);
    itemw -> setColor("rgb(254,208,142)");
    //下面的1表示用户id
    int height=itemw->SetMyImg("小康在这！",":/img/defaultimg.png",filePath);
    QListWidgetItem* line = new QListWidgetItem();
    line->setSizeHint(QSize(350, height));
    ui->listWidget_chat->addItem(line);
    ui->listWidget_chat->setItemWidget(line, itemw);
    ui->listWidget_chat->scrollToBottom();

    //发送图片

}





