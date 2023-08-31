#ifndef CREATEGROUPWIDGET_H
#define CREATEGROUPWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class CreateGroupWidget;
}

class CreateGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGroupWidget(QWidget *parent = nullptr);
    ~CreateGroupWidget();

private slots:
    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_button_createGroup_clicked();

    void changeToW(bool isSuc);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::CreateGroupWidget *ui;
    QPoint m_dragStartPosition; // 页面拖动
};

#endif // CREATEGROUPWIDGET_H
