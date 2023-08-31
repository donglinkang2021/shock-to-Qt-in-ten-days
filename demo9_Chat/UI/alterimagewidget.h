#ifndef ALTERIMAGEWIDGET_H
#define ALTERIMAGEWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class AlterImageWidget;
}

class AlterImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlterImageWidget(QWidget *parent = nullptr);
    ~AlterImageWidget();

private slots:
    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_button_alterImage_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::AlterImageWidget *ui;
    QPoint m_dragStartPosition;
};

#endif // ALTERIMAGEWIDGET_H
