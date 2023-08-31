#include "chat.h"
#include "ui_chat.h"

Chat::Chat(QTcpSocket *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    this->setWindowTitle("chat");
    socket = s;
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_button_clear_clicked()
{
    ui->lineEdit_chat->clear();
}

void Chat::on_button_send_clicked()
{
    QByteArray ba;
    ba.append(ui->lineEdit_chat->text());
    socket->write(ba);
}

void Chat::on_lineEdit_chat_returnPressed()
{
    on_button_send_clicked();
}
