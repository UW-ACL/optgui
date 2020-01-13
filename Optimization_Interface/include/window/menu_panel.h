// TITLE:   Optimization_Interface/include/window/menu_panel.h
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
#include <QMessageBox>

#include "include/window/menu_button.h"

namespace interface {

class MenuPanel : public QFrame {
 public:
    explicit MenuPanel(QWidget *parent, Qt::WindowFlags flags = Qt::Widget);
    ~MenuPanel();
    QWidget *menu_;                       // menu widget

    // Buttons
    QToolButton *close_button_;           // closes window
    QPushButton *exec_button_;            // commands trajectory to vehicle
    QPushButton *sim_button_;             // simulates trajectory in GUI
    QPushButton *duplicate_button_;       // duplicates selected constraint

    QVector<MenuButton*> *menu_buttons_;   // creates menu buttoms

    // Sliders
    // TODO(bchasnov): link sliders to controller at initialization of window
    QSlider *opt_finaltime_slider_;       // final time slider
    QSlider *opt_horizon_slider_;         // num iterations permitted slider
    QSlider *zoom_slider_;                // scales background map;

    // Labels
    QLabel *opt_finaltime_label_;         // labels final time slider
    QLabel *opt_horizon_label_;           // labels number of iterations slider
    QLabel *user_msg_label_;              // labels user feedback message

    // Initialization
    // TODO(bchasnov): add initializations for other sliders
    uint32_t finaltime_init_ = 3;         // initializes final time
    uint32_t horizonlength_init_ = 32;    // initializes horizon length
    uint32_t zoom_init_ = 5;

    // Message box for user feedback
    QMessageBox message_box_;             // display feedback about feasibility

 private:
    void initialize();                  // initializes menu panel
    void initializeZoomSlider();        // initializes zoom slider
    void initializeWaypointButton();    // initializes button for waypoints
    void initializeEraserButton();      // initializes eraser button
    void initializePlaneButton();       // initializes plane button
    void initializePolygonButton();     // initializes polygon button
    void initializePointButton();       // initializes point button
    void initializeEllipseButton();     // initializes ellipse button
    void initializeFlipButton();        // initializes flip button
    void initializeExecButton();        // initializes traj exec button
    void initializeSimButton();         // initializes traj sim button
    void initializeFinaltimeSlider();   // initializes final time slider
    void initializeHorizonSlider();     // initializes num iterations permitted
    void initializeDuplicateButton();   // initializes duplicate selected item
//    void initializeFreezeButton();    // initializes freeze button
};

}  // namespace interface

#endif  // MENU_PANEL_H_
