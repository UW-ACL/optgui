#include "eraserbutton.h"

EraserButton::EraserButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(*this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void EraserButton::generateIcon() {
    QPixmap *pix = new QPixmap(":/images/erasericon.png");
    *pix = pix->scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->buttonIcon = pix;
}

void EraserButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->buttonOff();
    } else {
        this->buttonOn();
    }

    QLabel::mousePressEvent(event);
}

void EraserButton::buttonOn() {
    this->setFrameShadow(QFrame::Sunken);
    this->clicked = true;
    emit eraserOn();
}

void EraserButton::buttonOff() {
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
    emit eraserOff();
}
