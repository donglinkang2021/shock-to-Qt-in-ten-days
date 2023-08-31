#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // connect 谁发出信号 信号 谁接受信号 处理信号
    connect(
        ui->lineEdit,
        SIGNAL(returnPressed()),
        this,
        SLOT(on_commitButton_clicked())
    );

    connect(
        ui->cancelButton,
        &QPushButton::clicked,
        this,
        &MainWindow::on_cancelButton_clicked
    );

    connect(
        ui->browseButton,
        &QPushButton::clicked,
        [this](){
            QMessageBox::information(
                this, "信息", "点击浏览"
            );
        }
    );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_commitButton_clicked()
{
    // 获取line_edit数据
    QString program = ui->lineEdit->text();
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program);
}

void MainWindow::on_cancelButton_clicked()
{
    this->close();
}
