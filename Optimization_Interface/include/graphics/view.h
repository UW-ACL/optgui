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
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

 private slots:
    void openMenu();
    void closeMenu();
    void setZoom(qreal value);
    void setState(STATE button_type);
    void setPorts();
    void execute();
    void setHorizon(int horizon_length);
    void setFinaltime(qreal final_time);
    void duplicateSelected();

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
};

}  // namespace optgui

#endif  // VIEW_H_
