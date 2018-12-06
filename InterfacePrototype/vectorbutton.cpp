#include "vectorbutton.h"

VectorButton::VectorButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(*this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void VectorButton::generateIcon() {
    QPixmap *pix = new QPixmap(50, 25);
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::blue);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QRectF rect(0, 0, 50, 25);
    painter.drawText(rect, Qt::AlignCenter, "Vector");

    painter.end();

    this->buttonIcon = pix;
}

void VectorButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->buttonOff();
    } else {
        this->buttonOn();
    }

    QLabel::mousePressEvent(event);
}

void VectorButton::buttonOn() {
    this->setFrameShadow(QFrame::Sunken);
    this->clicked = true;
    emit vectorOn();
}

void VectorButton::buttonOff() {
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
    emit vectorOff();
}

