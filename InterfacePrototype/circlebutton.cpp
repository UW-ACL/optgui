#include "circlebutton.h"

CircleButton::CircleButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void CircleButton::generateIcon() {
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);
    painter.drawEllipse(1, 1, 48, 48);
    painter.end();

    this->buttonIcon = pix;
}

/*
void CircleButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->setFrameShadow(QFrame::Sunken);
    } else {
        this->setFrameShadow(QFrame::Raised);
    }
    this->clicked = !this->clicked;
}
*/

void CircleButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->setFrameShadow(QFrame::Sunken);
    } else {
        this->setFrameShadow(QFrame::Raised);
    }
    this->clicked = !this->clicked;

    QPoint hotSpot = event->pos();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << QPoint(hotSpot) << qreal(25);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("circle-obstacle", itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(hotSpot);
    drag->setPixmap(this->buttonIcon);
    drag->exec();
}
