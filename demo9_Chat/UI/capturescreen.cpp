#include "capturescreen.h"

CaptureScreen::CaptureScreen(QWidget *parent) : QWidget(parent), m_isMousePress(false)
{
    initWindow();
    loadBackgroundPixmap();
}

CaptureScreen::~CaptureScreen()
{

}

void CaptureScreen::initWindow()
{
    this->setMouseTracking(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
}

void CaptureScreen::loadBackgroundPixmap()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    m_loadPixmap = screen->grabWindow(
        QApplication::desktop()->winId()
    );
    m_screenwidth = m_loadPixmap.width();
    m_screenheight = m_loadPixmap.height();
}

void CaptureScreen::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isMousePress = true;
        m_beginPoint = event->pos();
    }

    return QWidget::mousePressEvent(event);
}

void CaptureScreen::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isMousePress)
    {
        m_endPoint = event->pos();
        update();
    }
    return QWidget::mouseMoveEvent(event);
}

void CaptureScreen::mouseReleaseEvent(QMouseEvent *event)
{
    m_endPoint = event->pos();
    m_isMousePress = false;
    return QWidget::mouseReleaseEvent(event);
}

QRect CaptureScreen::getRect(const QPoint &beginPoint, const QPoint &endPoint)
{
    int x, y, width, height;
    width = qAbs(beginPoint.x() - endPoint.x());
    height = qAbs(beginPoint.y() - endPoint.y());
    x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
    y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

    QRect selectedRect = QRect(x, y, width, height);
    // 避免宽或高为零时拷贝截图有误;
    // 可以看QQ截图，当选取截图宽或高为零时默认为1;
    if (selectedRect.width() == 0)
    {
        selectedRect.setWidth(1);
    }
    if (selectedRect.height() == 0)
    {
        selectedRect.setHeight(1);
    }

    return selectedRect;
}

void CaptureScreen::keyPressEvent(QKeyEvent *event)
{
    // Esc 键退出截图;
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    // Eeter键完成截图;
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        emit signalCompleteCature(m_capturePixmap);
        close();
    }
}

void CaptureScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    m_painter.begin(this);			//进行重绘;

    QColor shadowColor = QColor(0, 0, 0, 100);						//阴影颜色设置;
    m_painter.setPen(QPen(Qt::green, 2, Qt::DotLine, Qt::FlatCap));	//设置画笔;
    m_painter.drawPixmap(0, 0, m_loadPixmap);						//将背景图片画到窗体上;
    m_painter.fillRect(m_loadPixmap.rect(), shadowColor);			//画影罩效果;

    if (m_isMousePress)
    {
        QRect selectedRect = getRect(m_beginPoint, m_endPoint);
        //        QScreen* screen = QGuiApplication::primaryScreen();
        //        m_capturePixmap = screen->grabWindow(0, selectedRect.left() ,selectedRect.top(),
        //                                             selectedRect.width() , selectedRect.height());
        m_capturePixmap = m_loadPixmap.copy(selectedRect);
        m_painter.drawPixmap(selectedRect.topLeft(), m_capturePixmap);
        m_painter.drawRect(selectedRect);
        //        qDebug()<<"Rect"<<selectedRect.left()<<selectedRect.top();
        //        qDebug()<<"Rect2"<<selectedRect.width()<<selectedRect.height();
        //        qDebug()<<"Pic"<<m_capturePixmap.width()<<m_capturePixmap.height();
        QPen pen;
        pen.setColor(Qt::yellow);
        m_painter.setPen(pen);
        QFont myfont("Arial", 12);
        m_painter.setFont(myfont);
        int x = selectedRect.left(), y = selectedRect.top(), w = selectedRect.width(), h =selectedRect.height();
        m_painter.drawText(
            x + 2, y - 8,
            tr("截图范围：( %1 x %2 ) - ( %3 x %4 )  图片大小：( %5 x %6 )")
                .arg(x).arg(y).arg(x + w).arg(y + h).arg(w).arg(h)
        );

    }

    m_painter.end();  //重绘结束;
}
