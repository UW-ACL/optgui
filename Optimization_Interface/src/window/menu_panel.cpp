// TITLE:   Optimization_Interface/src/window/menu_panel.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/menu_panel.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QtMath>

namespace optgui {

MenuPanel::MenuPanel(QWidget *parent, bool isRight, int size,
                     Qt::WindowFlags flags)
    : QFrame(parent, flags) {
    this->is_right_ = isRight;
    this->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
    this->setFixedWidth(size);

    // Set color
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);

    // Set frame
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);

    // Set layout
    this->setLayout(new QHBoxLayout(this));
    this->layout()->setContentsMargins(1, 1, 1, 1);
    this->layout()->setSpacing(1);

    // Create menu close button and button
    // on correct side of screen
    if (this->is_right_) {
        this->initializeCloseButton();
        this->initializeMenuwidget();
    } else {
        this->initializeMenuwidget();
        this->initializeCloseButton();
    }
}

MenuPanel::~MenuPanel() {
    // delete menu panel components
    delete this->menu_->layout();
    delete this->menu_;
    delete this->close_button_;
    delete this->layout();
}

void MenuPanel::initializeCloseButton() {
    this->close_button_ = new QToolButton(this);
    if (this->is_right_) {
        this->close_button_->setArrowType(Qt::RightArrow);
    } else {
        this->close_button_->setArrowType(Qt::LeftArrow);
    }
    this->close_button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->close_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->close_button_);
    if (this->is_right_) {
        this->layout()->setAlignment(this->close_button_, Qt::AlignLeft);
    } else {
        this->layout()->setAlignment(this->close_button_, Qt::AlignRight);
    }
}

void MenuPanel::initializeMenuwidget() {
    this->menu_ = new QWidget(this);
    this->menu_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->menu_layout_ = new QVBoxLayout(this->menu_);
    this->menu_->setLayout(this->menu_layout_);
    this->menu_->layout()->setContentsMargins(0, 5, 0, 5);
    this->menu_->layout()->setSpacing(1);

    this->layout()->addWidget(this->menu_);
}

}  // namespace optgui
