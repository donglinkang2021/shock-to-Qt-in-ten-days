#ifndef ADDFRIENDWIDGET_H
#define ADDFRIENDWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class AddFriendWidget;
}

class AddFriendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriendWidget(QWidget *parent = nullptr);
    ~AddFriendWidget();

private slots:
    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_button_addFriend_clicked();

    void on_button_search_clicked();

    void changeToW(bool isSuc);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::AddFriendWidget *ui;
    QPoint m_dragStartPosition;
};

#endif // ADDFRIENDWIDGET_H
