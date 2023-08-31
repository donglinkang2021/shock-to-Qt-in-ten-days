#ifndef LABELWITHCLICK_H
#define LABELWITHCLICK_H

#include <QLabel>
#include <QWidget>
#include <QMenu>
#include <QMouseEvent>

class LabelWithClick : public QLabel
{
    Q_OBJECT
public:
    explicit LabelWithClick(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event);

private:
    //显示的文本
    QString m_str;

};

#endif // LABELWITHCLICK_H
