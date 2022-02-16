// TITLE:   Optimization_Interface/include/graphics/view.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Renders graphical representations of constraints and controls

#ifndef VIEW_H_
#define VIEW_H_

#include <QGraphicsView>
#include <QToolButton>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QTableWidget>
#include <QHeaderView>
#include <QGestureEvent>
#include <QDoubleSpinBox>

#include "algorithm.h"

#include "include/graphics/canvas.h"
#include "include/window/menu_panel.h"
#include "include/globals.h"
#include "include/controls/controller.h"

namespace optgui {

const qreal DOT_SIZE = 14;

class View : public QGraphicsView {
    Q_OBJECT

 public:
    explicit View(QWidget *parent);
    ~View();

 protected:
    // pinch zoom
    bool viewportEvent(QEvent *event) override;
    // expand canvas when view expands
    void resizeEvent(QResizeEvent *event) override;
    // handle mouse input for toggle mode
    void mousePressEvent(QMouseEvent *event) override;

 private slots:
    // open/close side menu
    void openMenu();
    void closeMenu();

    // open/close expert menu
    void openExpertMenu();
    void closeExpertMenu();

    // set zoom level
    void setZoom(qreal value);

    // set toggle mode
    void setState(STATE button_type);

    // open network configuration dialog
    void setPorts();

    // execute staged traj
    void execute();

    // stage/unstage traj
    void stageTraj();
    void unstageTraj();

    // save all params in expert panel
    void setSkyeFlyParams();

    // set final time
    void setFinaltime(qreal final_time);

    // duplicate currently selected item
    void duplicateSelected();

    // set clearance around ellipses (in meters)
    void setClearance(qreal clearance);

    // set upper/lower bounds on waypoint index
    // when K is changed
    void constrainWpIdx(int value);

    // set upper/lower bounds on acceleration
    // when a_max or a_min is changed
    void constrainAccel();

    // update user feedback box message
    void updateFeedbackMessage();
    void setCurrEndpoints();
    void toggleSim(int);
    void toggleTrajLock(int);
    void toggleFreeFinalTime(int);
    void toggleDataCapture(int);

  private:
    void initializeMenuPanel();
    // set up expert panel
    void initializeExpertPanel();
    // clear temporary plane or polygon markers
    void clearMarkers();
    // expand view to fill window
    void expandView();
    // pinch zoom gesture
    bool pinchZoom(QGestureEvent *event);
    qreal currentStepScaleFactor_;  // pinch zoom factor
    qreal initialZoom_;  // initial scale factor at beginning of pinch

    // controller
    Controller *controller_;

    // panel menu open button
    QToolButton *menu_button_;
    // panel menu
    MenuPanel *menu_panel_;
    // user message box
    QLabel *user_msg_label_;
    // expert panel open button
    QToolButton *expert_menu_button_;
    // expert panel menu
    MenuPanel *expert_panel_;
    // toggle button state
    STATE state_;

    // canvas to render
    Canvas *canvas_;

    // markers for defining planes or polygons
    QVector<QGraphicsItem*> temp_markers_;
    // graphical info for markers
    QPen dot_pen_;
    QBrush dot_brush_;

    // expert panel items
    QTableWidget *skyefly_params_table_;
    quint32 a_min_row;
    quint32 a_max_row;
    quint32 wp_idx_row;
    QTableWidget *model_params_table_;

    // keep track of all widgets to delete them
    QVector<QWidget *> panel_widgets_;
    QDoubleSpinBox *zoom_slider_;
    // keep track of all toggle buttons to set them to toggled
    // or untoggled, deleted with panel_widgets_
    QVector<MenuButton *> toggle_buttons_;

    // Create buttons for menu panels
    void initializeMessageBox(MenuPanel *panel);
    void initializeZoom(MenuPanel *panel);
    void initializeWaypointButton(MenuPanel *panel);
    void initializeEraserButton(MenuPanel *panel);
    void initializePlaneButton(MenuPanel *panel);
    void initializePolygonButton(MenuPanel *panel);
    void initializeFinalPointButton(MenuPanel *panel);
    void initializeDroneButton(MenuPanel *panel);
    void initializeEllipseButton(MenuPanel *panel);
    void initializeCylinderButton(MenuPanel *panel);
    void initializeFlipButton(MenuPanel *panel);
    void initializeExecButton(MenuPanel *panel);
    void initializeStageButton(MenuPanel *panel);
    void initializeFinaltime(MenuPanel *panel);
    void initializeDuplicateButton(MenuPanel *panel);
    void initializeSimToggle(MenuPanel *panel);
    void initializeTrajLockToggle(MenuPanel *panel);
    void initializeFreeFinalTimeToggle(MenuPanel *panel);
    // expert panel skyefly params
    void initializeSkyeFlyParamsTable(MenuPanel *panel);
    void initializeDataCaptureToggle(MenuPanel *panel);
    // expert panel constraint_model params not in skyefly
    void initializeModelParamsTable(MenuPanel *panel);
};

}  // namespace optgui

#endif  // VIEW_H_
