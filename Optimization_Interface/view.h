// TITLE:   Optimization_Interface/view.h
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

#include "canvas.h"
#include "menu_panel.h"
#include "globals.h"
#include "controller.h"

namespace interface {

const qreal DOT_SIZE = 20;

class View : public QGraphicsView {
    Q_OBJECT

 public:
    explicit View(QWidget *parent);
    ~View();

 protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

 private slots:
    void openMenu();
    void closeMenu();
    void setZoom(int value);
    void setState(STATE button_type);
    void loadFile();
    void saveFile();
    void setPorts();
    void startServers();
    void closeServers();
//    void compute();
    void execute();
    void updatePath();
    void stepSim();
    void toggleSim();

 private:
    void initialize();
    void clearMarkers();
    void expandView();
    Controller *controller_;
    QToolButton *menu_button_;
    MenuPanel *menu_panel_;
    STATE state_;
    Canvas *canvas_;
    QVector<QGraphicsItem*> *temp_markers_;
    QPen dot_pen_;
    QBrush dot_brush_;
    uint64_t simulating_ = -1;

};

}  // namespace interface

#endif  // VIEW_H_
