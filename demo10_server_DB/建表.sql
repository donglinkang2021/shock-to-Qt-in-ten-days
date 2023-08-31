-- Active: 1692950880119@@127.0.0.1@3306@mydatabase

/* 使用mydatabase */
USE mydatabase;

/* 查看表 */
SHOW TABLES;


/* 创建User表（用户注册表）*/
CREATE TABLE IF NOT EXISTS User (
      Username VARCHAR(20) PRIMARY KEY,
      Password VARCHAR(20) NOT NULL,
      Nickname VARCHAR(20),
      Profile_picture VARCHAR(100),
      Personal_signature VARCHAR(100),
      Sex VARCHAR(20),
      Age INTEGER,
      Birthday VARCHAR(20),
      Region VARCHAR(20)
);

/* 查看User表属性 */
DESC User;
      
/*创建Friendship表（好友关系表）*/
CREATE TABLE IF NOT EXISTS Friendship (
      Myname VARCHAR(20) ,
      Friendname VARCHAR(20) ,
      PRIMARY KEY(Myname,Friendname),
      FOREIGN KEY (Myname) REFERENCES User(Username),
      FOREIGN KEY (Friendname) REFERENCES User(Username)
);

/* 查看Friendship表属性 */
DESC Friendship;

/* 删除之前存在的Friendmsg表 */
DROP TABLE IF EXISTS Friendmsg;
		
/*创建Friendmsg表（好友信息表）*/
CREATE TABLE IF NOT EXISTS Friendmsg (
      Sender VARCHAR(20),
      Receiver VARCHAR(20),
      Msg VARCHAR(1024),
      DT datetime NOT NULL,
      FOREIGN KEY (Sender) REFERENCES User(Username),
      FOREIGN KEY (Receiver) REFERENCES User(Username)
);

/* 查看Friendmsg表属性 */
DESC Friendmsg;
		
/*创建Groupinfo表（群聊信息表）*/
CREATE TABLE IF NOT EXISTS Groupinfo (
      Groupname VARCHAR(20) PRIMARY KEY,
      Group_profile VARCHAR(100)
);

/* 查看Groupinfo表属性 */
DESC Groupinfo;
		
/*创建Groupship表（群聊关系表）*/
CREATE TABLE IF NOT EXISTS Groupship(
      Username VARCHAR(20),
      Groupname VARCHAR(20),
      PRIMARY KEY(Username,Groupname),
      FOREIGN KEY (Username) REFERENCES User(Username),
      FOREIGN KEY (Groupname) REFERENCES Groupinfo(Groupname)
);

/* 查看Groupship表属性 */
DESC Groupship;

/* 删除之前存在的Groupmsg表 */
DROP TABLE IF EXISTS Groupmsg;
		
/*创建Groupmsg表（消息列表）*/
CREATE TABLE IF NOT EXISTS Groupmsg (
      Sender VARCHAR(20),
      Receiver VARCHAR(20),
      Msg VARCHAR(1024),
      DT datetime NOT NULL,
      FOREIGN KEY (Sender) REFERENCES User(Username),
      FOREIGN KEY (Receiver) REFERENCES Groupinfo(Groupname)
);

/* 查看Groupmsg表属性 */
DESC Groupmsg;

-- 在Groupinfo表中插入数据
INSERT INTO Groupinfo(Groupname,Group_profile) VALUES('group1','hello');

-- 查看Groupinfo表中的数据
SELECT * FROM Groupinfo;

-- 在Groupmsg表中插入数据
INSERT INTO Groupmsg(Sender,Receiver,Msg,DT) VALUES('admin','group1','hello',NOW());

-- 查看Groupmsg表中的数据
SELECT * FROM Groupmsg;

/* 插入数据 */
INSERT INTO User(Username,Password) VALUES('admin','123456');
INSERT INTO User(Username,Password) VALUES('admin1','123456');

/* 查看数据 */
SELECT * FROM User;

/* 删除数据 */
DELETE FROM User WHERE Username='admin';



/* 查看表 */
SHOW TABLES;

/*

在Qt中建表

void create_Table_User();
void create_Table_Friendship();
void create_Table_Friendmsg();
void create_Table_Groupinfo();
void create_Table_Groupship();
void create_Table_Groupmsg();

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


void Widget::on_button_signup_clicked()
{
    // 向User表中插入新数据
    QString Username = ui->lineEdit_Username->text();
    QString Password = ui->lineEdit_Password->text();

    QString insert_sql = QString(
        "INSERT INTO User(Username,Password) VALUES('%1','%2');"
    ).arg(Username).arg(Password);

      QSqlQuery sql_query;
}

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
      } else {
            QMessageBox::information(
                  this, "连接提示", "连接失败"
            );
      }
}


*/