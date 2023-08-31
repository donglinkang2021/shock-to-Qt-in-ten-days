#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(
        ui->action_New,
        &QAction::triggered,
        this,
        &MainWindow::action_New_Slot
    );

    connect(
        ui->action_Open,
        &QAction::triggered,
        this,
        &MainWindow::action_Open_Slot
    );

    connect(
        ui->action_Save,
        &QAction::triggered,
        this,
        &MainWindow::action_Save_Slot
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::action_New_Slot(){
    ui->textEdit->clear();
    this->setWindowTitle("新建文档文件");
}

void MainWindow::action_Open_Slot(){
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择一个文件",
        QCoreApplication::applicationFilePath(),
        "*.*"
    );

    if (fileName.isEmpty()){
        QMessageBox::warning(this,"警告","请选择一个文件");
    }else{
        //        qDebug() << fileName;
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QByteArray ba = file.readAll();
        ui->textEdit->setText(QString(ba));
        file.close();
    }
}

void MainWindow::action_Save_Slot(){
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "选择一个文件",
        QCoreApplication::applicationFilePath()
    );

    if (fileName.isEmpty()){
        QMessageBox::warning(this,"警告","请选择一个文件");
    }else{
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QByteArray ba;
        ba.append(ui->textEdit->toPlainText());
        file.write(ba);
        file.close();
    }

}

void MainWindow::keyPressEvent(QKeyEvent *k){
    if(k->modifiers() == Qt::ControlModifier && k->key() == Qt::Key_S){
        action_Save_Slot();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *m){
    QPoint pos = m->pos();
    qDebug() << pos;

    if (m->button() == Qt::LeftButton){
        qDebug() << "左键被按下";
    }else if(m->button() == Qt::RightButton){
        qDebug() << "右键被按下";
    }
}

