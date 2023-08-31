#ifndef ALTERNAMEWIDGET_H
#define ALTERNAMEWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class AlterNameWidget;
}

class AlterNameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlterNameWidget(QWidget *parent = nullptr);
    ~AlterNameWidget();

private slots:
    void on_minimizeButton_clicked();

    void on_closeButton_clicked();



    void on_button_renameUsername_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::AlterNameWidget *ui;
    QPoint m_dragStartPosition; // 拖动所用
};

#endif // ALTERNAMEWIDGET_H
