// TITLE:   Optimization_Interface/include/window/menu_panel.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Main menu panel

#ifndef MENU_PANEL_H_
#define MENU_PANEL_H_

#include <QFrame>
#include <QToolButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
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

    // TODO(bchasnov): link sliders to controller at initialization of window
    QSpinBox *opt_finaltime_;       // final time
    QSpinBox *opt_horizon_;         // num iterations permitted
    QDoubleSpinBox *zoom_;          // scales background map

    // Labels
    QLabel *opt_finaltime_label_;         // labels final time
    QLabel *opt_horizon_label_;           // labels number of iterations
    QLabel *user_msg_label_;              // labels user feedback message

    // Initialization
    uint32_t finaltime_init_ = 3;         // initializes final time
    uint32_t horizonlength_init_ = 32;    // initializes horizon length
    qreal zoom_init_ = 1.0;

    // Message box for user feedback
    QMessageBox message_box_;             // display feedback about feasibility

 private:
    void initialize();                  // initializes menu panel
    void initializeZoom();        // initializes zoom
    void initializeWaypointButton();    // initializes button for waypoints
    void initializeEraserButton();      // initializes eraser button
    void initializePlaneButton();       // initializes plane button
    void initializePolygonButton();     // initializes polygon button
    void initializePointButton();       // initializes point button
    void initializeEllipseButton();     // initializes ellipse button
    void initializeFlipButton();        // initializes flip button
    void initializeExecButton();        // initializes traj exec button
    void initializeSimButton();         // initializes traj sim button
    void initializeFinaltime();   // initializes final time
    void initializeHorizon();     // initializes num iterations permitted
    void initializeDuplicateButton();   // initializes duplicate selected item
//    void initializeFreezeButton();    // initializes freeze button
};

}  // namespace interface

#endif  // MENU_PANEL_H_
