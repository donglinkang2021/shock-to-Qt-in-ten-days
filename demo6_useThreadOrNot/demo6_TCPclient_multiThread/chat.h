#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(
        QTcpSocket *s,
        QWidget *parent = nullptr
    ); // 默认参数只能放在后面
    ~Chat();

private slots:
    void on_button_clear_clicked();

    void on_button_send_clicked();

    void on_lineEdit_chat_returnPressed();

private:
    Ui::Chat *ui;
    QTcpSocket *socket;
};

#endif // CHAT_H
