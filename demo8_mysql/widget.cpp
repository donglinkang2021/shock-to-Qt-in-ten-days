#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //    db = QSqlDatabase::addDatabase("QMYSQL");
    //    db.setHostName("localhost"); // MySQL server hostname
    //    db.setPort(3306); //appropriate port number
    //    db.setDatabaseName("mydatabase");
    //    db.setUserName("root"); // username
    //    db.setPassword("mysql"); // password

    //连接数据库 加载QSQLITE 在这里mysql不能用
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydatabase.db");

    if (db.open()) {// 在这行代码之后才打开数据库
        //        qDebug() << "Connected to the database!";
        QMessageBox::information(this, "连接提示", "成功连接数据库");

        //        qDebug() << db.tables();
        if (!db.tables().contains("student")){
            create_stuTable();
        }

        m = new QSqlTableModel;
        m->setTable("student");
        ui->tableView->setModel(m);
    } else {
        //        qDebug() << "Failed to connect to the database: " << db.lastError().text();
        QMessageBox::information(this, "连接提示", "连接失败");
    }
}

Widget::~Widget()
{
    db.close();
    delete ui;
}


void Widget::on_button_insert_clicked()
{
    QString id = ui->lineEdit_stuID->text();
    QString name = ui->lineEdit_stuName->text();
    QString birth = ui->lineEdit_stuBirth->text();

    QString sql = QString(
        "INSERT INTO student(id,name,birth) VALUES(%1,'%2','%3');"
    ).arg(id).arg(name).arg(birth);

    qDebug() << sql;

    QSqlQuery query;
    if (query.exec(sql)){
        QMessageBox::information(this, "插入提示", "插入成功");
    }else{
        QMessageBox::information(this, "插入提示", "插入失败");
    }
}


void Widget::on_button_query_clicked()
{
    //查询数据
    //    QSqlQuery query;
    //    if (query.exec("SELECT * FROM student;")) {
    //        while (query.next()) {
    //            qDebug() << query.value(0).toString();
    //            qDebug() << query.value(1).toString();
    //            qDebug() << query.value(2).toString();
    //        }
    //    } else {
    //        qDebug() << "Query failed: " << query.lastError().text();
    //    }
    m->select();
}


void Widget::create_stuTable(){
    // 创建学生表
    QSqlQuery sql_query;
    QString create_sql = "create table student (id INT UNSIGNED PRIMARY KEY, name VARCHAR(20) NOT NULL, birth DATE )";
    sql_query.prepare(create_sql);
    if(sql_query.exec()){
        qDebug() << "Table created!";
    }else{
        qDebug() << "Error: Fail to create table." << sql_query.lastError();
    }
}
