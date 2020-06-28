// TITLE:   Optimization_Interface/src/window/menu_button.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/menu_button.h"

namespace optgui {

MenuButton::MenuButton(STATE button_type, QWidget *parent)
    : QLabel(parent) {
    this->button_type_ = button_type;

    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
}

STATE MenuButton::getButtonType() {
    // get toggle type
    return this->button_type_;
}

void MenuButton::mousePressEvent(QMouseEvent *event) {
    // show button as clicked
    this->setFrameShadow(QFrame::Sunken);
    // toggle state
    emit changeState(this->button_type_);
    QLabel::mousePressEvent(event);
}

}  // namespace optgui
