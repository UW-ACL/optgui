// TITLE:   Optimization_Interface/menu_panel.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Main menu panel

#ifndef MENU_PANEL_H_
#define MENU_PANEL_H_

#include <QFrame>
#include <QToolButton>
#include <QSlider>
#include <QVector>
#include <QPushButton>
#include <QGroupBox>

#include "menu_button.h"

namespace interface {

class MenuPanel : public QFrame {
 public:
    explicit MenuPanel(QWidget *parent, Qt::WindowFlags flags = Qt::Widget);
    ~MenuPanel();
    QToolButton *close_button_;
    QWidget *menu_;
    QSlider *zoom_slider_;
    QPushButton *exec_button_;
    QPushButton *sim_button_;
    MenuButton *freeze_button_;
    QVector<MenuButton*> *menu_buttons_;

//    QGroupBox *opt_layout;
    QLabel *opt_finaltime_label_;
    QLabel *opt_horizon_label_;
//    QPushButton *opt_horizon_minus;
//    QPushButton *opt_horizon_plus;
//    QPushButton *opt_finaltime_minus;
//    QPushButton *opt_finaltime_plus;

    QSlider *opt_finaltime_slider_;
    QSlider *opt_horizon_slider_;

 private:
    void initialize();
    void initializeZoomSlider();
    void initializeWaypointButton();
    void initializeEraserButton();
    void initializePlaneButton();
    void initializePolygonButton();
    void initializePointButton();
    void initializeEllipseButton();
    void initializeFlipButton();
    void initializeExecButton();
    void initializeFreezeButton();
    void initializeSimButton();
    void initializeFinaltimeSlider();
    void initializeHorizonSlider();
};

}  // namespace interface

#endif  // MENU_PANEL_H_
