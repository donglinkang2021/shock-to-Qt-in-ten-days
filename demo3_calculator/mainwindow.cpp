#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("cal");

    QFont fangsong("仿宋", 14);
    ui->mainLineEdit->setFont(
        fangsong
    );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_button_1_clicked()
{
    expression += "1";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_2_clicked()
{
    expression += "2";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_3_clicked()
{
    expression += "3";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_4_clicked()
{
    expression += "4";
    ui->mainLineEdit->setText(expression);
}



void MainWindow::on_button_5_clicked()
{
    expression += "5";
    ui->mainLineEdit->setText(expression);
}



void MainWindow::on_button_6_clicked()
{
    expression += "6";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_7_clicked()
{
    expression += "7";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_8_clicked()
{
    expression += "8";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_9_clicked()
{
    expression += "9";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_zero_clicked()
{
    expression += "0";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_leftbrace_clicked()
{
    expression += "(";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_rightbrace_clicked()
{
    expression += ")";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_multiply_clicked()
{
    expression += "×";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_devide_clicked()
{
    expression += "÷";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_plus_clicked()
{
    expression += "+";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_minus_clicked()
{
    expression += "-";
    ui->mainLineEdit->setText(expression);
}

void MainWindow::on_button_clear_clicked()
{
    expression.clear();
    ui->mainLineEdit->clear();
}

void MainWindow::on_button_del_clicked()
{
    expression.chop(1);
    ui->mainLineEdit->setText(expression);
}
