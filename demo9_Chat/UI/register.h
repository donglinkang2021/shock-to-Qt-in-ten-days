#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

private slots:

    void on_label_return_login_clicked();

    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_button_signUp_config_clicked();

    void on_lineEdit_pwd_textEdited(const QString &arg1);

    void on_lineEdit_pwd_again_textEdited(const QString &arg1);

    void registerOK2Login(bool isSuc);

private:
    Ui::Register *ui;
    QPoint m_dragStartPosition;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

};

#endif // REGISTER_H
