#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#define PORT 8000

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void newClientHandler();
    void clientInfoSlot(); // 接受client信息处理


    void on_button_clear_clicked();

    void on_button_send_clicked();

private:
    Ui::Widget *ui; // UI界面对象
    QTcpServer *server; // 服务器对象
};
#endif // WIDGET_H
