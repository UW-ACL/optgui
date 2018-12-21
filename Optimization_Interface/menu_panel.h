// TITLE:   Optimization_Interface/menu_panel.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved


#ifndef MENU_PANEL_H
#define MENU_PANEL_H

#include <QFrame>
#include <QToolButton>
#include <QSlider>
#include <QVector>

#include "menu_button.h"

namespace interface {

class MenuPanel : public QFrame
{

public:
    explicit MenuPanel(QWidget *parent, Qt::WindowFlags flags = Qt::Widget);
    ~MenuPanel();
    QToolButton *close_button_;
    QWidget *menu_;
    QSlider *zoom_slider_;
    QVector<MenuButton*> *menu_buttons_;
private:
    void initialize();
    void initializeZoomSlider();
    void initializePathButton();
    void initializeEraserButton();
    void initializePlaneButton();
    void initializePolygonButton();
    void initializeEllipseButton();
    void initializeFlipButton();
};

}  // namespace

#endif // MENU_PANEL_H
