#include "linebutton.h"


LineButton::LineButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(*this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void LineButton::generateIcon() {
    QPixmap *pix = new QPixmap(50, 50);
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    painter.drawLine(QPointF(10, 40), QPointF(40, 10));

    painter.end();

    this->buttonIcon = pix;
}

void LineButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->buttonOff();
    } else {
        this->buttonOn();
    }

    QLabel::mousePressEvent(event);
}

void LineButton::buttonOn() {
    this->setFrameShadow(QFrame::Sunken);
    this->clicked = true;
    emit lineOn();
}

void LineButton::buttonOff() {
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
    emit lineOff();
}
