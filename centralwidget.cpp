#include "centralwidget.h"
#include <QGraphicsDropShadowEffect>

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(200, 250);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 70));
    shadow->setBlurRadius(20);
    setGraphicsEffect(shadow);
}
