// TITLE:   Optimization_Interface/include/window/menu_panel.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// menu panel template

#ifndef MENU_PANEL_H_
#define MENU_PANEL_H_

#include <QFrame>
#include <QToolButton>
#include <QVBoxLayout>

#include "include/window/menu_button.h"

namespace optgui {

class MenuPanel : public QFrame {
 public:
    explicit MenuPanel(QWidget *parent, bool isRight, int size = 100,
                       Qt::WindowFlags flags = Qt::Widget);
    ~MenuPanel();

    // widget to hold all the buttons
    QWidget *menu_;
    // formatting layout
    QVBoxLayout *menu_layout_;
    // button to close window
    QToolButton *close_button_;

 private:
    // flag for panel on left or right side of window
    bool is_right_;
    // set up close menu button
    void initializeCloseButton();
    // set up layout and buttons
    void initializeMenuwidget();
};

}  // namespace optgui

#endif  // MENU_PANEL_H_
