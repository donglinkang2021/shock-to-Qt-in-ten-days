#ifndef ITEMCHATWIDGET_H
#define ITEMCHATWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QDateTime>

namespace Ui {
class ItemChatWidget;
}

class ItemChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemChatWidget(QWidget *parent = nullptr);
    ~ItemChatWidget();

    int SetMyData(QString username, QString picon, QString txt); //用户消息框
    int SetMyImg(QString imgname, QString picon ,QString imgpth);

    void setColor(QString c);

    int SetFriendsData(QString username, QString picon, QString txt); //用户消息框
    int SetFriendsImg(QString imgname, QString picon ,QString imgpth);

private:
    Ui::ItemChatWidget *ui;
    QString Splite(QString txt, QFontMetrics& font);
    int countLine;
    int longestWidth;
    QString color; // 气泡颜色
};

#endif // ITEMCHATWIDGET_H
