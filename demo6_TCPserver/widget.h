#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSet>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QList>
#include <QNetworkInterface>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void newMessage(QString);

private slots:

    //    void newClientHandler(); // 之前残留的代码，和上面`newConnection`差不多，到时候删除
    //    void clientInfoSlot();  // 之前残留的代码，接受client信息处理，和`readSocket`差不多，到时候删除
    void readSocket(); // 读取每个socket发送的信息
    void discardSocket(); // 丢弃该socket

    void newConnection(); // 每当有一个用户加进来就会有一个新连接，并调用`appendToSocketList`
    void appendToSocketList(QTcpSocket* socket); // 添加新的socket

    void displayMessage(const QString& str); // 在browser中显示消息
    void refreshComboBox(); // 刷新组合框

    void on_button_clear_clicked();
    void on_button_send_clicked();

    void sendMessage(QTcpSocket* socket);
    void sendAttachment(QTcpSocket* socket, QString filePath);

    void on_button_attachment_clicked();

    void on_lineEdit_send_returnPressed();

    void on_button_config_port_clicked();

    void on_button_check_local_IP_clicked();

private:
    Ui::Widget *ui; // UI界面对象
    QTcpServer *server; // 服务器对象
    QSet<QTcpSocket*> connection_set; // 所有连接到server的socket
};
#endif // WIDGET_H
