#ifndef ADDGROUPWIDGET_H
#define ADDGROUPWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class AddGroupWidget;
}

class AddGroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddGroupWidget(QWidget *parent = nullptr);
    ~AddGroupWidget();

private slots:
    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_button_addGroup_clicked();

    void on_button_search_clicked();

    void changeToW(bool isSuc);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::AddGroupWidget *ui;
    QPoint m_dragStartPosition;
};

#endif // ADDGROUPWIDGET_H
