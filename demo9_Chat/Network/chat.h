#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

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

signals:
    void newMessage(QString);
    void showFirstRequested();

private slots:
    void on_button_clear_clicked();
    void on_button_send_clicked();
    void on_button_attachment_clicked();
    void on_lineEdit_chat_returnPressed();

    void readSocket();
    void discardSocket();

    void displayMessage(const QString& str);

    void setServerInfo(QTcpSocket *s);

    void on_button_return_clicked();

private:
    Ui::Chat *ui;
    QTcpSocket *socket;
};

#endif // CHAT_H
