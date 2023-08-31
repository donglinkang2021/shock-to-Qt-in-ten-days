#include "labelwithclick.h"

LabelWithClick::LabelWithClick(QWidget *parent) :
    QLabel(parent),
    m_str("")
{
    setText(m_str);
}


void LabelWithClick::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    return QLabel::mouseReleaseEvent(event);
}

