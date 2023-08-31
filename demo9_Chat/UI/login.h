#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>
#include <QDebug>
#include <UI/register.h>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:

    void loginOK2MainWindow(bool isSuc);

    void on_button_login_clicked();

    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_key_lineedit_textEdited(const QString &arg1);

    void on_label_signup_rightnow_clicked();

    void on_button_connection_clicked();

    void connectionSlot(QTcpSocket *s);

private:
    Ui::Login *ui;
    QPoint m_dragStartPosition;
    QTcpSocket *socket;
    QString tempUserName;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // LOGIN_H
