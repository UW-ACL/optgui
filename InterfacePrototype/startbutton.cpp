#include "startbutton.h"

StartButton::StartButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(*this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void StartButton::generateIcon() {
    QPixmap *pix = new QPixmap(50, 25);
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::green);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QRectF rect(0, 0, 50, 25);
    painter.drawText(rect, Qt::AlignLeft, "Start");
    pen.setColor(Qt::black);
    painter.drawText(rect, Qt::AlignCenter, "+");
    pen.setColor(Qt::red);
    painter.drawText(rect, Qt::AlignCenter, "End");

    painter.end();

    this->buttonIcon = pix;
}

void StartButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->buttonOff();
    } else {
        this->buttonOn();
    }

    QLabel::mousePressEvent(event);
}

void StartButton::buttonOn() {
    this->setFrameShadow(QFrame::Sunken);
    this->clicked = true;
    emit startOn();
}

void StartButton::buttonOff() {
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
    emit startOff();
}

