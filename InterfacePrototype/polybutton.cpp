#include "polybutton.h"

PolyButton::PolyButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(*this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void PolyButton::generateIcon() {
    QPixmap *pix = new QPixmap(50, 50);
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QPolygonF poly;
    poly << QPointF(25, 1);
    poly << QPointF(49, 21);
    poly << QPointF(40, 49);
    poly << QPointF(10, 49);
    poly << QPointF(1, 21);
    poly << QPointF(25, 1);

    painter.drawPolygon(poly);

    painter.end();

    this->buttonIcon = pix;
}

void PolyButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->buttonOff();
    } else {
        this->buttonOn();
    }

    QLabel::mousePressEvent(event);
}

void PolyButton::buttonOn() {
    this->setFrameShadow(QFrame::Sunken);
    emit polygonOn();
    this->clicked = true;
}

void PolyButton::buttonOff() {
    this->setFrameShadow(QFrame::Raised);
    emit polygonOff();
    this->clicked = false;
}
