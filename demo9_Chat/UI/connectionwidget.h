#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>

namespace Ui {
class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionWidget(QWidget *parent = nullptr);
    ~ConnectionWidget();

signals:
    void returnLoginWidget(QTcpSocket *s);
    void showFirstRequested();

private:
    Ui::ConnectionWidget *ui;
    QTcpSocket *socket;

private slots:
    void on_pushButton_connect_clicked();
    void on_pushButton_cancel_clicked();
    void on_lineEdit_port_returnPressed();
    void handleConnected();
    void handleDisConnected();
    void connectHostSuccess(bool flag);
};

#endif // CONNECTIONWIDGET_H
