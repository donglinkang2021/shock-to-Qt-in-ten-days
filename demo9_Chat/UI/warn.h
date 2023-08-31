#ifndef WARN_H
#define WARN_H

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
const int WARN_TIMEOUT = 1 * 1000;

namespace Ui {
class Warn;
}

class Warn : public QWidget
{
    Q_OBJECT

public:
    explicit Warn(QWidget *parent = nullptr);
    ~Warn();
    void set_label(QString str);

private:
    Ui::Warn *ui;
    QPoint m_dragStartPosition;

private slots:
    void close_warning();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;


};

#endif // WARN_H
