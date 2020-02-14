// TITLE:   Optimization_Interface/include/window/menu_button.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Template for menu mode toggle buttons

#ifndef MENU_BUTTON_H_
#define MENU_BUTTON_H_

#include <QLabel>

#include "include/globals.h"

namespace optgui {

class MenuButton : public QLabel {
    Q_OBJECT
 public:
    explicit MenuButton(STATE button_type, QWidget *parent);
    STATE getButtonType();
 signals:
    void changeState(STATE button_type);
 protected:
    void mousePressEvent(QMouseEvent *event) override;
 private:
    void initialize();
    STATE button_type_;
};

}  // namespace optgui

#endif  // MENU_BUTTON_H_
