#ifndef CAPTURESCREEN_H
#define CAPTURESCREEN_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QScreen>
#include <QApplication>
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QMouseEvent>

class CaptureScreen : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureScreen(QWidget *parent = nullptr);
    ~CaptureScreen();

signals:
    void signalCompleteCature(QPixmap captureImage);

private:
    void initWindow();// 初始化
    void loadBackgroundPixmap();// 加载背景图片

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);

    bool m_isMousePress; //鼠标是否按下
    QPixmap m_loadPixmap, m_capturePixmap;
    int m_screenwidth, m_screenheight;
    QPoint m_beginPoint, m_endPoint;
    QPainter m_painter;

};

#endif // CAPTURESCREEN_H
