// TITLE:   Optimization_Interface/include/window/menu_panel.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Main menu panel

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

    QWidget *menu_;
    QVBoxLayout *menu_layout_;
    QToolButton *close_button_;

 private:
    bool is_right_;
    void initializeCloseButton();
    void initializeMenuwidget();
};

}  // namespace optgui

#endif  // MENU_PANEL_H_
